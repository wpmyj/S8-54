// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "TcpSocket.h"
#include <lwip/tcp.h>
#include <string.h>
#include <stdarg.h>


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct tcp_pcb *pcbClient = 0;      // 0, если клиент не приконнекчен

enum States
{
    S_ACCEPTED,
    S_RECEIVED,
    S_CLOSING
};

struct State
{
    struct pbuf *p;     // pbuf (chain) to recycle
    uchar state;
    int numPort;
};

static void(*SocketFuncConnect)(void) = 0;                                 // this function will be called every time a new connection
static void(*SocketFuncReceiver)(const char *buffer, uint length) = 0;     // this function will be called when a message is received from any client

bool gEthIsConnected = false;                                       // Если true, то подсоединён клиент

static err_t CallbackOnAccept(void *arg, struct tcp_pcb *newPCB, err_t err);
static void Send(struct tcp_pcb *tpcb, struct State *ss);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool TCPSocket_Init(void(*funcConnect)(void), void(*funcReceiver)(const char *buffer, uint length))
{
    struct tcp_pcb *pcb = tcp_new();
    if (pcb != NULL)
    {
        err_t err = tcp_bind(pcb, IP_ADDR_ANY, (u16_t)DEFAULT_PORT);
        if (err == ERR_OK)
        {
            pcb = tcp_listen(pcb);
            SocketFuncReceiver = funcReceiver;
            SocketFuncConnect = funcConnect;
            tcp_accept(pcb, CallbackOnAccept);
        }
        else
        {
            // abort? output diagnostic?
        }
    }
    else
    {
        // abort? output diagonstic?
    }

    pcbClient = 0;

    return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool TCPSocket_Send(const char *buffer, uint length)
{
    if (pcbClient)
    {
        struct pbuf *tcpBuffer = pbuf_alloc(PBUF_RAW, (u16_t)length, PBUF_POOL);
        tcpBuffer->flags = 1;
        pbuf_take(tcpBuffer, buffer, (u16_t)length);
        struct State *ss = (struct State*)mem_malloc(sizeof(struct State));
        ss->p = tcpBuffer;
        Send(pcbClient, ss);
        mem_free(ss);
    }
    return pcbClient != 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void TCPSocket_SendFormatString(char *format, ...)
{
#undef SIZE_BUFFER
#define SIZE_BUFFER 200
    static char buffer[SIZE_BUFFER];
    __va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);
    strcat(buffer, "\r\n");
    TCPSocket_Send(buffer, strlen(buffer));
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void ETH_SendFormatString(char *format, ...)
{

}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void CloseConnection(struct tcp_pcb *tpcb, struct State *ss)
{
    gEthIsConnected = false;
    tcp_arg(tpcb, NULL);
    tcp_sent(tpcb, NULL);
    tcp_recv(tpcb, NULL);
    tcp_err(tpcb, NULL);
    tcp_poll(tpcb, NULL, 0);

    pcbClient = 0;

    if (ss)
    {
        mem_free(ss);
    }
    tcp_close(tpcb);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void Send(struct tcp_pcb *tpcb, struct State *ss)
{
    struct pbuf *ptr;
    err_t wr_err = ERR_OK;

    while ((wr_err == ERR_OK) && (ss->p != NULL) && (ss->p->len <= tcp_sndbuf(tpcb)))
    {
        ptr = ss->p;
        // enqueue data for transmittion
        wr_err = tcp_write(tpcb, ptr->payload, ptr->len, 1);
        if (wr_err == ERR_OK)
        {
            u16_t plen;
            u8_t freed;

            plen = ptr->len;
            // continue with new pbuf in chain (if any) 
            ss->p = ptr->next;
            if (ss->p != NULL)
            {
                // new reference!
                pbuf_ref(ss->p);
            }
            // chop first pbuf from chain
            do
            {
                // try hard to free pbuf 
                freed = pbuf_free(ptr);
            } while (freed == 0);
            // we can read more data now
            tcp_recved(tpcb, plen);
        }
        else if (wr_err == ERR_MEM)
        {
            // we are low on memory, try later / harder, defer to poll
            ss->p = ptr;
        }
        else
        {
            // other probler
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static err_t CallbackOnSent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
    struct State *ss;
    LWIP_UNUSED_ARG(len);
    ss = (struct State*)arg;

    if (ss->p != NULL)
    {
        Send(tpcb, ss);
    }
    else
    {
        // no more pbufs to send
        if (ss->state == S_CLOSING)
        {
            CloseConnection(tpcb, ss);
        }
    }
    return ERR_OK;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void SendAnswer(void *arg, struct tcp_pcb *tpcb)
{
    static const char policy[] = "<?xml version=\"1.0\"?>"                                                  \
        "<!DOCTYPE cross-domain-policy SYSTEM \"http://www.adobe.com/xml/dtds/cross-domain-policy.dtd\">"   \
        "<cross-domain-policy>"                                                                             \
        "<allow-access-from domain=\"*\" to-ports=\"9999\" />"                                                 \
        "</cross-domain-policy>"                                                                            \
        "\0";
    struct pbuf *tcpBuffer = pbuf_alloc(PBUF_RAW, (u16_t)strlen(policy), PBUF_POOL);
    tcpBuffer->flags = 1;
    pbuf_take(tcpBuffer, policy, (u16_t)strlen(policy));
    struct State *s = (struct State *)arg;
    s->p = tcpBuffer;
    Send(tpcb, s);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static err_t CallbackOnReceive(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    err_t ret_err;
    LWIP_ASSERT("arg != NULL", arg != NULL);
    struct State *ss = (struct State*)arg;

    static int number = 0;

    if (number )
    {
        number = number;
    }
    
    number++;

    if (p == NULL)
    {
        // remote host closed connection
        ss->state = S_CLOSING;
        if (ss->p == NULL)
        {
            // we're done sending, close it
            CloseConnection(tpcb, ss);
        }
        else
        {
            // we're not done yet
            //tcp_sent(tpcb, CallbackOnSent);
        }
        ret_err = ERR_OK;
    }
    else if (err != ERR_OK)
    {
        // cleanup, for unkown reason
        if (p != NULL)
        {
            ss->p = NULL;
            pbuf_free(p);
        }
        ret_err = err;
    }
    else if (ss->state == S_ACCEPTED)
    {
        if (ss->numPort == POLICY_PORT)
        {
            pbuf_free(p);
            ss->state = S_RECEIVED;
            SendAnswer(ss, tpcb);
            ss->state = S_CLOSING;
            ret_err = ERR_OK;
        }
        else
        {
            // first data chunk in p->payload
            ss->state = S_RECEIVED;
            // store reference to incoming pbuf (chain)
            ss->p = p;
            Send(tpcb, ss);
            ret_err = ERR_OK;
        }
    }
    else if (ss->state == S_RECEIVED)
    {
        // read some more data
        if (ss->p == NULL)
        {
            //ss->p = p;
            //tcp_sent(tpcb, CallbackOnSent);
            //Send(tpcb, ss);
            SocketFuncReceiver((char *)p->payload, p->len);

            u8_t freed = 0;
            do
            {
                // try hard to free pbuf 
                freed = pbuf_free(p);
            } while (freed == 0);

        }
        else
        {
            struct pbuf *ptr;
            // chain pbufs to the end of what we recv'ed previously
            ptr = ss->p;
            pbuf_chain(ptr, p);
        }
        ret_err = ERR_OK;
    }
    else if (ss->state == S_CLOSING)
    {
        // odd case, remote side closing twice, trash data
        tcp_recved(tpcb, p->tot_len);
        ss->p = NULL;
        pbuf_free(p);
        ret_err = ERR_OK;
    }
    else
    {
        // unknown ss->state, trash data
        tcp_recved(tpcb, p->tot_len);
        ss->p = NULL;
        pbuf_free(p);
        ret_err = ERR_OK;
    }
    return ret_err;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void CallbackOnError(void *arg, err_t err)
{
    struct State *ss;
    LWIP_UNUSED_ARG(err);
    ss = (struct State *)arg;
    if (ss != NULL)
    {
        mem_free(ss);
    }
    gEthIsConnected = false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static err_t CallbackOnPoll(void *arg, struct tcp_pcb *tpcb)
{
    err_t ret_err;
    struct State *ss = (struct State *)arg;
    if (ss != NULL)
    {
        if (ss->p != NULL)
        {
            // there is a remaining pbuf (chain)
            //tcp_sent(tpcb, CallbackOnSent);
            Send(tpcb, ss);
        }
        else
        {
            // no remaining pbuf (chain)
            if (ss->state == S_CLOSING)
            {
                CloseConnection(tpcb, ss);
            }
        }
        ret_err = ERR_OK;
    }
    else
    {
        // nothing to be done
        tcp_abort(tpcb);
        ret_err = ERR_ABRT;
    }
    return ret_err;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static err_t CallbackOnAccept(void *arg, struct tcp_pcb *newPCB, err_t err)
{
    err_t ret_err;

    struct State *s;
    LWIP_UNUSED_ARG(arg);
    LWIP_UNUSED_ARG(err);
    

    /* Unless this pcb should have NORMAL priority, set its priority now.
        When running out of pcbs, low priority pcbs can be aborted to create
        new pcbs of higher priority. */
    tcp_setprio(newPCB, TCP_PRIO_MIN);

    s = (struct State*)mem_malloc(sizeof(struct State));

    if (s)
    {
        s->state = S_ACCEPTED;
        s->numPort = ((unsigned short)POLICY_PORT == newPCB->local_port) ? POLICY_PORT : DEFAULT_PORT;
        s->p = NULL;
        /* pass newly allocated s to our callbacks */
        tcp_arg(newPCB, s);
        tcp_recv(newPCB, CallbackOnReceive);
        tcp_err(newPCB, CallbackOnError);
        tcp_poll(newPCB, CallbackOnPoll, 0);
        tcp_sent(newPCB, CallbackOnSent);
        ret_err = ERR_OK;

        if (s->numPort == DEFAULT_PORT)
        {
            if (pcbClient == 0)
            {
                pcbClient = newPCB;
                SocketFuncConnect();
                gEthIsConnected = true;
            }
        }

    }
    else
    {
        ret_err = ERR_MEM;
    }

    return ret_err;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
/*
static err_t CallbackOnAcceptPolicyPort(void *arg, struct tcp_pcb *newPCB, err_t err)
{
    return CallbackOnAccept(arg, newPCB, err);
}
*/
