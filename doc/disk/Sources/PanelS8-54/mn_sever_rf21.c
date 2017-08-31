//Опрос состояния кнопок и ручек.
//Состояние кнопок и ручек передается по SPI(ведущий режим) и считывается состояние светодиодов каждые 10мс 
//Исходный Sever-S (поменялись все входы кнопок и ручек, добавились 8 шт.)

#include <18F2515.h>
#include <P18F2515.INC>
#device ADC=10
//#include "stdio.h"
//#include "stdlib.h"
//#include "math.h"

// Fuses: LP,XT,HS,EC_IO,INTRC_IO,INTRC,RC_IO,RC,NOWDT,WDT,PUT,NOPUT
// Fuses: NOMCLR,MCLR,NOBROWNOUT,BROWNOUT,PROTECT,NOPROTECT,CPD,NOCPD
//#fuses INTRC_IO, NOWDT, NOPROTECT, MCLR, BROWNOUT, PUT
#use delay(clock=8000000)
//#use fast_io(C)
//#use fixed_io(c_outputs=PIN_C0,PIN_C1,PIN_C2)

#zero_ram

#define SL0 PIN_B0
#define SL1 PIN_B1
#define SL2 PIN_B2
#define SL3 PIN_B3
#define SL4 PIN_B4
#define SL5 PIN_B5

#define sw_K1 PIN_C0
#define sw_K2 PIN_C1
#define sw_Sx PIN_C2

//#define SL0_H	OUTPUT_HIGH(SL0)

const char mask[6] = {0x3e,0x3d,0x3b,0x37,0x2f,0x1f};
//const char mask_kn[6] = {0x21,0x21,0x21,0x39,0x21,0x3f};
const char mask_kn[6] = {0x81,0x81,0x81,0x91,0xbf,0x81};

#bit RC6=PORTC.6
#bit RC0=PORTC.0
#bit RA0=PORTA.0
void pit_bkl(void);    
void pit_bykl(void);


char tek_state_rb[6]; //

char tek1_state_rb,tek2_state_rb,tek3_state_rb;
char pr_state_rb[6]; //, pr_state_ruch[5];
char ii,ib,pr_ruk;
char kod,du_pb;
char buf_per,buf_prm,pr_wkl_pit; 
char buff[100],tt,i,pr_wykl;
char pr_stab;
char tek1_ruch,tek2_ruch,tek3_ruch,tek4_ruch,tek5_ruch;
//char i_dr,mas[100],mas_prd[500],kk;

/*
#INT_TIMER0  //Приходят каждые 500 mks
void sch_wr500(void)
{ 
  set_timer0(64536);     //184  256-(0,0005/(4/1000000))=65036 -- (500 mks)       
 // RC6=~RC6;  	
  if (RC6==0)   RC6=1;  //RC6=~RC6;
	else  RC6=0; 
  //  set_timer0(131);     //184  65536-(0,0005/(4/1000000))=65036 -- (500 mks)      
}//@
*/

#INT_TIMER1 //Приходят каждые 10ms
void Prd_kn(void)  //По SPI перед. сост. ПП и прин сообщения от STM
{
// set_timer1(63036);  //65536-(0,01/(4/1000000))=65536-10000=63036 -- (10ms)  
//if (spi_data_is_in())  //true-если в порту SPI приняты данные
//  buf_prm=spi_read(); 
 while (!(sspstat&0x01))  {} // ждем  завершения приема
  buf_prm=SSPBUF;            // чтение буфера SPI
  kod=buf_prm&0x7f;

  if (( kod > 0)&&( kod <4))  //св.: Kan1,Kan2,Sinchr,
     {
      //зажигаем или гасим светодиод:
      if  (buf_prm &(1 << 7))
           //зажигаем - (0)
           switch(kod)
			{
             case 1: 
               {
                 output_low(sw_K1); 
                 break; 
               }
              case 2: 
               {
                 output_low(sw_K2); 
                 break; 
               }
              case 3: 
               {
                 output_low(sw_Sx); 
                 break; 
               }   
            default:
              break;  
            }
      else    //гасим - (1)
          switch(kod)
			{
             case 1: 
               {
                 output_high(sw_K1); 
                 break; 
               }
              case 2: 
               {
                 output_high(sw_K2); 
                 break; 
               }
              case 3: 
               {
                 output_high(sw_Sx); 
                 break; 
               }   
            default:
              break;  
            }
     }
 else
   {
     if ((kod==4)&&(pr_wkl_pit==1))
           {  
             pr_wykl=1;
             pit_bykl();
           }
    }
     
  sspcon1=sspcon1&0x7f;   //сброс бита конфликта
    //Буфер готов к передаче:   
     
    output_high(pin_C6); 
   // spi_write(buf_per); //запись байта в SPI
    SSPBUF=buf_per;
    buf_per=0;
    output_low(pin_C6);
 
 set_timer1(45536);  //65536-(0,01/(4/8000000))=20000 -- (10ms)  

}//@


void main ()
{
disable_interrupts(GLOBAL);   //общее запрещение прерываний
setup_oscillator(OSC_8MHZ);  //|OSC_INTRC
 
set_tris_a(0xff);   //выходы
set_tris_b(0x00);   //входы
set_tris_c(0x10);   //выходы, RC4-вход(данные от STM)

//output_c(0x4f);
portc=0xcf;
porta=0x7f;
portb=0xff;

//set_tris_c(0x10);   //выходы, RC4-вход(данные от STM)
//output_a(0x7f);
//output_b(0xff);

pit_bykl();
pr_wkl_pit=0;   //пр. вкл. питания (исп. для 1-го нажатия кн. "Питание")

//delay_ms(1000);  //для настройки STM

//Инициализация модуля MSSP(режим SPI)
SSPCON1=0x31;
SSPSTAT=0;    //0x80;
SSPBUF=0x55;

//иниц. таймеров:
//t0con=0xc8; //8-ми разр
t0con=0x88; //16-ми разр счетчик
T1CON=0x85;
setup_timer_1(T1_INTERNAL|T1_DIV_BY_1);  

set_timer1(62286);
//set_timer0(64536); 

//enable_interrupts(INT_TIMER2);
enable_interrupts(INT_TIMER1);
//enable_interrupts(INT_TIMER0);
enable_interrupts(GLOBAL);

//СОСТОЯНИЕ кнопок и ручек по ВКЛЮЧЕНИЮ:
//----------------------------------------
for (ii=0; ii<6;ii++) 
 {
   du_pb=portb&0xc0|mask[ii];
   output_b(du_pb);   
   pr_state_rb[ii] = input_a();
 }

while(1)
{
  for (ii=0; ii<6;ii++) 
 {   pr_ruk=0;
  // output_a(mask[ii]);              //SL... -> PORTB
   du_pb=portb&0xc0|mask[ii];
   output_b(du_pb); 
   tek_state_rb[ii] = input_a();
   if (pr_state_rb[ii] ^ tek_state_rb[ii])
       //возможно  нажатие или вращение ?
    {
    if ((tek_state_rb[ii]&mask_kn[ii])!=mask_kn[ii])  //если это кнопка, то отрабат. дребезг
       { //кнопка
	    delay_ms(1);
	    tek1_state_rb = input_a();
	    delay_ms(1);
	    tek2_state_rb = input_a();
	    delay_ms(1);
	    tek3_state_rb = input_a();
       if ((tek1_state_rb==tek2_state_rb)&&(tek2_state_rb==tek3_state_rb))
         pr_stab=1;
       }	
   else  //ручка
       {
      //   tek3_state_rb = input_b();
      //   pr_stab=1;
         tek1_ruch=input_a();
         delay_us(200);
         tek2_ruch=input_a();
         delay_us(200); 
         if (tek1_ruch==tek2_ruch)
          {
          tek3_ruch=input_a();
          delay_us(200);
            if (tek2_ruch==tek3_ruch)
               {
                tek4_ruch=input_a();
                delay_us(200);
                 if (tek3_ruch==tek4_ruch)
                  {
	                   tek5_ruch=input_a();
	                   delay_us(200);
	                  if (tek4_ruch==tek5_ruch)
	                     //есть стабильное изменение
                         {
	                      pr_stab=1;
                          tek3_state_rb=tek5_ruch;
                         }
                      else  
                          pr_stab=0;	                  
                  }
                else  
                 pr_stab=0;
               }
             else
	           pr_stab=0;            
          }
         else
	        pr_stab=0;
   
       }  //ручка
 
 if (pr_stab==1)
    {
      switch(ii)   //номер сигнала SL0,SL1,SL2,SL3,SL4,SL5
			{
              case 0:  //SL0
                //Кнопки K1, МЕНЮ :
				if (!(tek3_state_rb & (1 << 0)))  { buf_per=0x81;  //№1-наж.
                                                    do
                                                     tek3_state_rb = input_a(); 
                                                    while (!(tek3_state_rb & (1 << 0)));
                                                    //отжатие произошло
                                                    while (buf_per!=0) {}
                                                     buf_per=0x01;  //№1-отж.
                                                    break; 
                                                  }
                  if (!(tek3_state_rb & (1 << 7))) { buf_per=0x8d;  //№13-наж.
                                                    do
                                                     tek3_state_rb = input_a(); 
                                                    while (!(tek3_state_rb & (1 << 7)));
                                                    //отжатие произошло
                                                    while (buf_per!=0) {}
                                                     buf_per=0x0d;  //№13-отж.
                                                    break; 
                                                  }

                //Ручки -B_K1, +B_K1, V? -Sm_K1, +Sm_K1, V? :
                //**********************************************
                
              if  ((tek3_state_rb & 0x02)&&(tek3_state_rb & 0x04))  //RB1 & RB2==1
                    { 
                     for (ib=0; ib<2;ib++) 
                     {
                       if ((tek3_state_rb & (0x02 << ib)) && (!(pr_state_rb[ii] & (0x02 << ib))))
                            
                            switch(ib)
			                {
                               case 0: 
                                pr_ruk=1;
                                buf_per=0x14;  //№20-влево 
                               break;

                               case 1:
                                pr_ruk=1;  
                                buf_per=0x94;  //№20-вправо
                               break;
                            }//switch 
                     } //for  ib    
                   } //if

          //    else
          //     {                 
               if  ((tek3_state_rb & 0x10)&&(tek3_state_rb & 0x20)&& (!pr_ruk))   //RB4 & RB5==1 & pr_ruk=0 
                 {  
                     for (ib=0; ib<2;ib++) 
                     {
                      if ((tek3_state_rb & (0x10 << ib)) && (!(pr_state_rb[ii] & (0x10 << ib))))
                             
                            switch(ib)
			                {
                               case 0: 
                               {
                                buf_per=0x15;  //№21-влево 
                                break;
                               }

                               case 1: 
                               {
                                buf_per=0x95;  //№21-вправо
                               break;
                               }
                            }//switch ib
                     } //for
                  
                   } //if
             
              pr_state_rb[ii]=tek3_state_rb;  
              pr_ruk=0;
			  break;
                               
              case 1:    //SL1
               //Кнопки K2,  1  // DISP:
				if (!(tek3_state_rb & (1 << 0))) { buf_per=0x83;  //№3-наж.
                                                    do
                                                     tek3_state_rb = input_a(); 
                                                    while (!(tek3_state_rb & (1 << 0)));
                                                    //отжатие произошло
                                                    while (buf_per!=0) {}
                                                     buf_per=0x03;  //№3-отж.
                                                    break; 
                                                  }
                  if (!(tek3_state_rb & (1 << 7))) { buf_per=0x8e;  //№14-наж.
                                                    do
                                                     tek3_state_rb = input_a(); 
                                                    while (!(tek3_state_rb & (1 << 7)));
                                                    //отжатие произошло
                                                    while (buf_per!=0) {}
                                                     buf_per=0x0e;  //№14-отж.
                                                    break; 
                                                  }

                //Ручки -B_K2, +B_K2 -Sm_K2, +Sm_K2:
                //***********************************
                
              if  ((tek3_state_rb & 0x02)&&(tek3_state_rb & 0x04))  //RB1 & RB2==1
                    { 
                     for (ib=0; ib<2;ib++) 
                     {
                       if ((tek3_state_rb & (0x02 << ib)) && (!(pr_state_rb[ii] & (0x02 << ib))))
                             
                            switch(ib)
			                {
                               case 0: 
                                pr_ruk=1;
                                buf_per=0x16;  //№22-влево
                               break;

                               case 1: 
                                pr_ruk=1;
                                buf_per=0x96;  //№22-вправо
                               break;
                            }//switch 
                     } //for      
                   } //if
                
               if  ((tek3_state_rb & 0x10)&&(tek3_state_rb & 0x20) && (!pr_ruk))   //RB4 & RB5==1 & (!pr_ruk)
                 {
                     for (ib=0; ib<2;ib++) 
                   {

                     if ((tek3_state_rb & (0x10 << ib)) && (!(pr_state_rb[ii] & (0x10 << ib))))
                             
                            switch(ib)
			                {
                               case 0: 
                               {
                                buf_per=0x17;  //№23-влево
                                break;
                               }

                               case 1: 
                               {
                               buf_per=0x97;  //№23-вправо
                               break;
                               }
                            }//switch 
                     } //for
                   
                   } //if
           
              pr_state_rb[ii]=tek3_state_rb;
              pr_ruk=0;
						 		                        
			  break;
      
              case 2: //SL2
               //Кнопки Raz, 2 // Pam:
				if (!(tek3_state_rb & (1 << 0))) { buf_per=0x85;  //№5-наж.
                                                    do
                                                     tek3_state_rb = input_a(); 
                                                    while (!(tek3_state_rb & (1 << 0)));
                                                    //отжатие произошло
                                                    while (buf_per!=0) {}
                                                     buf_per=0x05;  //№5-отж.
                                                    break; 
                                                  }
                  if (!(tek3_state_rb & (1 << 7))) { buf_per=0x8f;  //№15-наж.
                                                    do
                                                     tek3_state_rb = input_a(); 
                                                    while (!(tek3_state_rb & (1 << 7)));
                                                    //отжатие произошло
                                                    while (buf_per!=0) {}
                                                     buf_per=0x0f;  //№15-отж.
                                                    break; 
                                                  }
                //Ручки -T, +T V& -Del, +Del V&:
                //***********************************
                
              if  ((tek3_state_rb & 0x02)&&(tek3_state_rb & 0x04))  //RB1 & RB2==1
                    { 
                     for (ib=0; ib<2;ib++) 
                     {
                       if ((tek3_state_rb & (0x02 << ib)) && (!(pr_state_rb[ii] & (0x02 << ib))))
                             
                            switch(ib)
			                {
                               case 0: 
                                pr_ruk=1;
                                buf_per=0x18;  //№24-влево
                               break;

                               case 1: 
                                pr_ruk=1;
                                buf_per=0x98;  //№24-вправо
                               break;
                            }//switch 
                     } //for      
                   } //if
               
               if  ((tek3_state_rb & 0x10)&&(tek3_state_rb & 0x20) && (!pr_ruk))   //RB4 & RB5==1 & (!pr_ruk)
                 {
                     for (ib=0; ib<2;ib++) 
                   {

                     if ((tek3_state_rb & (0x10 << ib)) && (!(pr_state_rb[ii] & (0x10 << ib))))
                             
                            switch(ib)
			                {
                               case 0: 
                               {
                                buf_per=0x19;  //№25-влево
                                break;
                               }

                               case 1: 
                               {
                               buf_per=0x99;  //№25-вправо
                               break;
                               }
                            }//switch 
                     } //for
                   
                   } //if
              pr_state_rb[ii]=tek3_state_rb; 
			  pr_ruk=0;			 		                        
			  break; 

              case 3:  //SL3

               //Кнопки Sinch, Pusk, 3 //Kurs, Izm:
				if (!(tek3_state_rb & (1 << 0))) { buf_per=0x87;  //№7-наж.
                                                    do
                                                     tek3_state_rb = input_a(); 
                                                    while (!(tek3_state_rb & (1 << 0)));
                                                    //отжатие произошло
                                                    while (buf_per!=0) {}
                                                     buf_per=0x07;  //№7-отж.
                                                    break; 
                                                  }
                if (!(tek3_state_rb & (1 << 4))) { buf_per=0x88;  //№8-наж.
                                                    do
                                                     tek3_state_rb = input_a(); 
                                                    while (!(tek3_state_rb & (1 << 4)));
                                                    //отжатие произошло
                                                    while (buf_per!=0) {}
                                                     buf_per=0x08;  //№8-отж.
                                                    break; 
                                                  }
                if (!(tek3_state_rb & (1 << 7))) { buf_per=0x90;  //№16-наж.
                                                    do
                                                     tek3_state_rb = input_a(); 
                                                    while (!(tek3_state_rb & (1 << 7)));
                                                    //отжатие произошло
                                                    while (buf_per!=0) {}
                                                     buf_per=0x10;  //№16-отж.
                                                    break; 
                                                  }

                //Ручки -Ur, +Ur :
                //****************
                
              if  ((tek3_state_rb & 0x02)&&(tek3_state_rb & 0x04))  //RB1 & RB2==1
                    { 
                     for (ib=0; ib<2;ib++) 
                     {
                       if ((tek3_state_rb & (0x02 << ib)) && (!(pr_state_rb[ii] & (0x02 << ib))))
                             
                            switch(ib)
			                {
                               case 0: 
                                buf_per=0x1A;  //№26-влево
                               break;

                               case 1:
                               buf_per=0x9A;  //№26-вправо
                               break;
                            }//switch 
                     } //for      
                   } //if
			    pr_state_rb[ii]=tek3_state_rb; 			 		                        
			  break;

              case 4: // SL4
              //Кнопки Kurs, Izm ,DISP, Sbor,Pam, Serv,4:
			      if (!(tek3_state_rb & (1 << 0))) { buf_per=0x89;  //№9-наж.
                                                    do
                                                     tek3_state_rb = input_a(); 
                                                    while (!(tek3_state_rb & (1 << 0)));
                                                    //отжатие произошло
                                                    while (buf_per!=0) {}
                                                     buf_per=0x09;  //№9-отж.
                                                    break; 
                                                  }
                  if (!(tek3_state_rb & (1 << 1))) { buf_per=0x8A;  //№10-наж.
                                                    do
                                                     tek3_state_rb = input_a(); 
                                                    while (!(tek3_state_rb & (1 << 1)));
                                                    //отжатие произошло
                                                    while (buf_per!=0) {}
                                                     buf_per=0x0A;  //№10-отж.
                                                    break; 
                                                  }
                  if (!(tek3_state_rb & (1 << 2))) { buf_per=0x87;  //№4-наж.
                                                    do
                                                     tek3_state_rb = input_a(); 
                                                    while (!(tek3_state_rb & (1 << 2)));
                                                    //отжатие произошло
                                                    while (buf_per!=0) {}
                                                     buf_per=0x04;  //№4-отж.
                                                    break; 
                                                  }
                 if (!(tek3_state_rb & (1 << 3))) { buf_per=0x8c;  //№12-наж.
                                                    do
                                                     tek3_state_rb = input_a(); 
                                                    while (!(tek3_state_rb & (1 << 3)));
                                                    //отжатие произошло
                                                    while (buf_per!=0) {}
                                                     buf_per=0x0c;  //№12-отж.
                                                    break; 
                                                  }
                if (!(tek3_state_rb & (1 << 4))) { buf_per=0x86;  //№6-наж.
                                                    do
                                                     tek3_state_rb = input_a(); 
                                                    while (!(tek3_state_rb & (1 << 4)));
                                                    //отжатие произошло
                                                    while (buf_per!=0) {}
                                                     buf_per=0x06;  //№6-отж.
                                                    break; 
                                                  }
               if (!(tek3_state_rb & (1 << 5))) { buf_per=0x82;  //№2-наж.
                                                    do
                                                     tek3_state_rb = input_a(); 
                                                    while (!(tek3_state_rb & (1 << 5)));
                                                    //отжатие произошло
                                                    while (buf_per!=0) {}
                                                     buf_per=0x02;  //№2-отж.
                                                    break; 
                                                  }

                if (!(tek3_state_rb & (1 << 7))) { buf_per=0x91;  //№17-наж.
                                                    do
                                                     tek3_state_rb = input_a(); 
                                                    while (!(tek3_state_rb & (1 << 7)));
                                                    //отжатие произошло
                                                    while (buf_per!=0) {}
                                                     buf_per=0x11;  //№17-отж.
                                                    break; 
                                                  }

                break;

              case 5: //SL5 Pit
                   
                 if (!(tek3_state_rb & (1 << 0)))  { 
                                                    if (pr_wkl_pit==0)
                                                      { 
                                                       pit_bkl();
                                                       delay_ms(1);  //для настройки STM
                                                       do
                                                        tek3_state_rb = input_a(); 
                                                        while (!(tek3_state_rb & (1 << 0)));
                                                        //отжатие произошло
                                                      }
                                                    else
                                                      {
                                                       while (buf_per!=0) {}
                                                       buf_per=0x8B;  //№11-наж.
                                                      } 
                                                    break; 
                                                  }
               //Кнопки Men "5":
			
                if (!(tek3_state_rb & (1 << 7))) { buf_per=0x92;  //№18-наж.
                                                    do
                                                     tek3_state_rb = input_a(); 
                                                    while (!(tek3_state_rb & (1 << 7)));
                                                    //отжатие произошло
                                                    while (buf_per!=0) {}
                                                     buf_per=0x12;  //№18-отж.
                                                    break; 
                                                  }

                 //Ручки -Ust, +Ust :
                //********************
                
              if  ((tek3_state_rb & 0x02)&&(tek3_state_rb & 0x04))  //RB1 & RB2==1
                    { 
                     for (ib=0; ib<2;ib++) 
                     {
                       if ((tek3_state_rb & (0x02 << ib)) && (!(pr_state_rb[ii] & (0x02 << ib))))
                             
                            switch(ib)
			                {
                               case 0: 
                                buf_per=0x1B;  //№27-влево
                               break;

                               case 1: 
                                buf_per=0x9B;  //№27-вправо
                               break;
                            }//switch 
                     } //for      
                   } //if
			     pr_state_rb[ii]=tek3_state_rb; 			 		                        
			  break; 

              default:
              break;  

            }//switch(ii) 
      pr_state_rb[ii]=tek3_state_rb; 
      pr_stab=0;
  } //pr_stab=1
       
 }//if

    if  (pr_wykl==1) 
   {
  //   output_low(pin_A6);
  //    output_high(pin_A7);
      output_high(pin_C7);
      pr_wykl=0;   
   }
  
     
 }//for (ii)
 
 

}//while(1)
//output_bit(PIN_C3,1);
//BUF_CAP[i-1]=input(PIN_C4);
//output_bit( PIN_B2, 0);  //Ldac  в 0

//level = input_state(pin_A3);

}  //main


void pit_bkl(void)
{
//  output_high(pin_A6);
//  output_low(pin_A7);
  output_low(pin_C7);
  pr_wkl_pit=1;
  
}
    
void pit_bykl(void)
{
//   output_low(pin_A6);
//   output_high(pin_A7);
   output_high(sw_K1); 
   output_high(sw_K2); 
   output_high(sw_Sx);
   output_high(pin_C7);
   pr_wkl_pit=0; 

}



/*  !!!!!!!!!!!!!!!!!!!!
void pit_bkl(void)
{
  porta=porta & 0xbf;
  porta=porta | 0x80;
  portc=portc | 0x90;
  pr_wkl_pit=1;
}
    
void pit_bykl(void)
{
  porta=porta | 0x40;
  porta=porta & 0x7f;
  portc=portc & 0x6f;
  pr_wkl_pit=0; 
}


//прием от STM

  buf_prm=SSPBUF; 
  kod=buf_prm&0x7f;

  if (( kod !=0)&&( kod <4))  //Kan1,Kan2,Sinchr,
     {
      if  (buf_prm & (1 << 7))
          portc=portc | (1 << (kod -1));  //bit=1
      else 
          portc=portc ^ (1 << (kod -1));  //bit=0
     }
  if ( kod ==4)
     if  (buf_prm & (1 << 7))
          pit_bkl();
     else
          pit_bykl();



    if ((sspstat&0x01)==1)
   {
    SSPBUF=0x55;
   }
   set_timer1(63036);
*/ 






/*



void obr_ruk_Bp_K1(void)
{ 
 buf[ik]=tek3_state_rb;
 ik=ik+1;

  buf_per[kk]=2;
 kk=kk+1; 
  buf_per[kk]=0x0a;
  kk=kk+1;
 //putc('2');
} 
*/


  //возможно  нажатие или вращение ?
/*   {
   if ((tek_state_rb[ii]&mask_kn[ii])!=mask_kn[ii])  //если это кнопка, то отрабат. дребезг
      { //кнопка
	    delay_ms(1);
	    tek1_state_rb = input_b();
	    delay_ms(1);
	    tek2_state_rb = input_b();
	    delay_ms(1);
	    tek3_state_rb = input_b();
	
	//    if (tek_state_rb[ii] == tek3_state_rb)  //дребезг зак.-есть нажатие/отжатие или вращение!
      }
     else  //ручка

      tek1_ruch=tek_state_rb[ii];  
*/