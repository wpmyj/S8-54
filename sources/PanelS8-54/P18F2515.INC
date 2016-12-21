 //       LIST


 //       NOLIST


 //       IFNDEF __18F2515
 //          MESSG "Processor-header file mismatch.  Verify selected processor."
 //       ENDIF

/*;==========================================================================
;       18xxxx Family        EQUates
;==========================================================================*/
#define FSR0            = 0
#define FSR1            = 1
#define FSR2            = 2

#define FAST            = 1

#define W               = 0
#define A               = 0
#define ACCESS          = 0
#define BANKED          = 1
//;==========================================================================

//;==========================================================================
//;       16Cxxx/17Cxxx Substitutions
//;==========================================================================
  #define DDRA  TRISA      ; PIC17Cxxx SFR substitution
  #define DDRB  TRISB      ; PIC17Cxxx SFR substitution
  #define DDRC  TRISC      ; PIC17Cxxx SFR substitution
  #define DDRD  TRISD      ; PIC17Cxxx SFR substitution
  #define DDRE  TRISE      ; PIC17Cxxx SFR substitution

//;==========================================================================
//;
//;       Register Definitions
//;
//;==========================================================================

//;----- Register Files -----------------------------------------------------
#byte PORTA            =  0x0F80
#byte PORTB            =  0x0F81
#byte PORTC            =  0x0F82
#byte PORTE            =  0x0F84
#byte LATA             =  0x0F89
#byte LATB             =  0x0F8A
#byte LATC             =  0x0F8B
//#byte DDRA             =  0x0F92
#byte TRISA            =  0x0F92
//#byte DDRB             =  0x0F93
#byte TRISB            =  0x0F93
//#byte DDRC             =  0x0F94
#byte TRISC            =  0x0F94
#byte OSCTUNE          =  0x0F9B
#byte PIE1             =  0x0F9D
#byte PIR1             =  0x0F9E
#byte IPR1             =  0x0F9F
#byte PIE2             =  0x0FA0
#byte PIR2             =  0x0FA1
#byte IPR2             =  0x0FA2
#byte RCSTA            =  0x0FAB
#byte TXSTA            =  0x0FAC
#byte TXREG            =  0x0FAD
#byte RCREG            =  0x0FAE
#byte SPBRG            =  0x0FAF
#byte SPBRGH           =  0x0FB0
#byte T3CON            =  0x0FB1
#byte TMR3L            =  0x0FB2
#byte TMR3H            =  0x0FB3
#byte CMCON            =  0x0FB4
#byte CVRCON           =  0x0FB5
#byte PWM1CON          =  0x0FB7
#byte BAUDCON          =  0x0FB8
#byte BAUDCTL          =  0x0FB8
#byte CCP2CON          =  0x0FBA
#byte CCPR2            =  0x0FBB
#byte CCPR2L           =  0x0FBB
#byte CCPR2H           =  0x0FBC
#byte CCP1CON          =  0x0FBD
#byte CCPR1            =  0x0FBE
#byte CCPR1L           =  0x0FBE
#byte CCPR1H           =  0x0FBF
#byte ADCON2           =  0x0FC0
#byte ADCON1           =  0x0FC1
#byte ADCON0           =  0x0FC2
#byte ADRES            =  0x0FC3
#byte ADRESL           =  0x0FC3
#byte ADRESH           =  0x0FC4
#byte SSPCON2          =  0x0FC5
#byte SSPCON1          =  0x0FC6
#byte SSPSTAT          =  0x0FC7
#byte SSPADD           =  0x0FC8
#byte SSPBUF           =  0x0FC9
#byte T2CON            =  0x0FCA
#byte PR2              =  0x0FCB
#byte TMR2             =  0x0FCC
#byte T1CON            =  0x0FCD
#byte TMR1L            =  0x0FCE
#byte TMR1H            =  0x0FCF
#byte RCON             =  0x0FD0
#byte WDTCON           =  0x0FD1
#byte HLVDCON          =  0x0FD2
#byte LVDCON           =  0x0FD2
#byte OSCCON           =  0x0FD3
#byte T0CON            =  0x0FD5
#byte TMR0L            =  0x0FD6
#byte TMR0H            =  0x0FD7
#byte STATUS           =  0x0FD8
#byte FSR2L            =  0x0FD9
#byte FSR2H            =  0x0FDA
#byte PLUSW2           =  0x0FDB
#byte PREINC2          =  0x0FDC
#byte POSTDEC2         =  0x0FDD
#byte POSTINC2         =  0x0FDE
#byte INDF2            =  0x0FDF
#byte BSR              =  0x0FE0
#byte FSR1L            =  0x0FE1
#byte FSR1H            =  0x0FE2
#byte PLUSW1           =  0x0FE3
#byte PREINC1          =  0x0FE4
#byte POSTDEC1         =  0x0FE5
#byte POSTINC1         =  0x0FE6
#byte INDF1            =  0x0FE7
#byte WREG             =  0x0FE8
#byte FSR0L            =  0x0FE9
#byte FSR0H            =  0x0FEA
#byte PLUSW0           =  0x0FEB
#byte PREINC0          =  0x0FEC
#byte POSTDEC0         =  0x0FED
#byte POSTINC0         =  0x0FEE
#byte INDF0            =  0x0FEF
#byte INTCON3          =  0x0FF0
#byte INTCON2          =  0x0FF1
#byte INTCON           =  0x0FF2
#byte PROD             =  0x0FF3
#byte PRODL            =  0x0FF3
#byte PRODH            =  0x0FF4
#byte TABLAT           =  0x0FF5
#byte TBLPTR           =  0x0FF6
#byte TBLPTRL          =  0x0FF6
#byte TBLPTRH          =  0x0FF7
#byte TBLPTRU          =  0x0FF8
#byte PC               =  0x0FF9
#byte PCL              =  0x0FF9
#byte PCLATH           =  0x0FFA
#byte PCLATU           =  0x0FFB
#byte STKPTR           =  0x0FFC
#byte TOS              =  0x0FFD
#byte TOSL             =  0x0FFD
#byte TOSH             =  0x0FFE
#byte TOSU             =  0x0FFF
/*
//;----- PORTA Bits -----------------------------------------------------
#define RA0              =  0x0000
#define RA1              =  0x0001
#define RA2              =  0x0002
#define RA3              =  0x0003
#define RA4              =  0x0004
#define RA5              =  0x0005
#define RA6              =  0x0006
#define RA7              =  0x0007

#define T0CKI            =  0x0004
#define AN4              =  0x0005

#define SS               =  0x0005

#define NOT_SS           =  0x0005

#define LVDIN            =  0x0005

#define HLVDIN           =  0x0005


//;----- PORTB Bits -----------------------------------------------------
#define RB0              =  0x0000
#define RB1              =  0x0001
#define RB2              =  0x0002
#define RB3              =  0x0003
#define RB4              =  0x0004
#define RB5              =  0x0005
#define RB6              =  0x0006
#define RB7              =  0x0007

#define INT0             =  0x0000
#define INT1             =  0x0001
#define INT2             =  0x0002
#define CCP2_PORTB       =  0x0003
#define KBI0             =  0x0004
#define KBI1             =  0x0005
#define KBI2             =  0x0006
#define KBI3             =  0x0007

#define AN12             =  0x0000
#define AN10             =  0x0001
#define AN8              =  0x0002
#define AN9              =  0x0003
#define AN11             =  0x0004
#define PGM              =  0x0005
#define PGC              =  0x0006
#define PGD              =  0x0007

#define DAD5             =  0x0005
#define DAD6             =  0x0006
#define DAD7             =  0x0007


//;----- PORTC Bits -----------------------------------------------------
#define RC0              =  0x0000
#define RC1              =  0x0001
#define RC2              =  0x0002
#define RC3              =  0x0003
#define RC4              =  0x0004
#define RC5              =  0x0005
#define RC6              =  0x0006
#define RC7              =  0x0007

#define T1OSO            =  0x0000
#define T1OSI            =  0x0001
#define CCP1             =  0x0002
#define SCK              =  0x0003
#define SDI              =  0x0004
#define SDO              =  0x0005
#define TX               =  0x0006
#define RX               =  0x0007

#define T13CKI           =  0x0000
#define CCP2_PORTC       =  0x0001
#define SCL              =  0x0003
#define SDA              =  0x0004
#define CK               =  0x0006
//; DT is a reserved word
//; DT               EQU  H'0007'


//;----- PORTE Bits -----------------------------------------------------
#define RE3              =  0x0003


//;----- LATA Bits -----------------------------------------------------
#define LATA0            =  0x0000
#define LATA1            =  0x0001
#define LATA2            =  0x0002
#define LATA3            =  0x0003
#define LATA4            =  0x0004
#define LATA5            =  0x0005
#define LATA6            =  0x0006
#define LATA7            =  0x0007


//;----- LATB Bits -----------------------------------------------------
#define LATB0            =  0x0000
#define LATB1            =  0x0001
#define LATB2            =  0x0002
#define LATB3            =  0x0003
#define LATB4            =  0x0004
#define LATB5            =  0x0005
#define LATB6            =  0x0006
#define LATB7            =  0x0007


//;----- LATC Bits -----------------------------------------------------
#define LATC0            =  0x0000
#define LATC1            =  0x0001
#define LATC2            =  0x0002
#define LATC3            =  0x0003
#define LATC4            =  0x0004
#define LATC5            =  0x0005
#define LATC6            =  0x0006
#define LATC7            =  0x0007


//;----- DDRA Bits -----------------------------------------------------
#define RA0              =  0x0000
#define RA1              =  0x0001
#define RA2              =  0x0002
#define RA3              =  0x0003
#define RA4              =  0x0004
#define RA5              =  0x0005
#define RA6              =  0x0006
#define RA7              =  0x0007


//;----- TRISA Bits -----------------------------------------------------
#define TRISA0           =  0x0000
#define TRISA1           =  0x0001
#define TRISA2           =  0x0002
#define TRISA3           =  0x0003
#define TRISA4           =  0x0004
#define TRISA5           =  0x0005
#define TRISA6           =  0x0006
#define TRISA7           =  0x0007


//;----- DDRB Bits -----------------------------------------------------
#define RB0              =  0x0000
#define RB1              =  0x0001
#define RB2              =  0x0002
#define RB3              =  0x0003
#define RB4              =  0x0004
#define RB5              =  0x0005
#define RB6              =  0x0006
#define RB7              =  0x0007


//;----- TRISB Bits -----------------------------------------------------
#define TRISB0           =  0x0000
#define TRISB1           =  0x0001
#define TRISB2           =  0x0002
#define TRISB3           =  0x0003
#define TRISB4           =  0x0004
#define TRISB5           =  0x0005
#define TRISB6           =  0x0006
#define TRISB7           =  0x0007


//;----- DDRC Bits -----------------------------------------------------
#define RC0              =  0x0000
#define RC1              =  0x0001
#define RC2              =  0x0002
#define RC3              =  0x0003
#define RC4              =  0x0004
#define RC5              =  0x0005
#define RC6              =  0x0006
#define RC7              =  0x0007


//;----- TRISC Bits -----------------------------------------------------
#define TRISC0           =  0x0000
#define TRISC1           =  0x0001
#define TRISC2           =  0x0002
#define TRISC3           =  0x0003
#define TRISC4           =  0x0004
#define TRISC5           =  0x0005
#define TRISC6           =  0x0006
#define TRISC7           =  0x0007


//;----- OSCTUNE Bits -----------------------------------------------------
#define TUN0             =  0x0000
#define TUN1             =  0x0001
#define TUN2             =  0x0002
#define TUN3             =  0x0003
#define TUN4             =  0x0004
#define PLLEN            =  0x0006
#define INTSRC           =  0x0007


//;----- PIE1 Bits -----------------------------------------------------
#define TMR1IE           =  0x0000
#define TMR2IE           =  0x0001
#define CCP1IE           =  0x0002
#define SSPIE            =  0x0003
#define TXIE             =  0x0004
#define RCIE             =  0x0005
#define ADIE             =  0x0006


//;----- PIR1 Bits -----------------------------------------------------
#define TMR1IF           =  0x0000
#define TMR2IF           =  0x0001
#define CCP1IF           =  0x0002
#define SSPIF            =  0x0003
#define TXIF             =  0x0004
#define RCIF             =  0x0005
#define ADIF             =  0x0006


//;----- IPR1 Bits -----------------------------------------------------
#define TMR1IP           =  0x0000
#define TMR2IP           =  0x0001
#define CCP1IP           =  0x0002
#define SSPIP            =  0x0003
#define TXIP             =  0x0004
#define RCIP             =  0x0005
#define ADIP             =  0x0006


//;----- PIE2 Bits -----------------------------------------------------
#define CCP2IE           =  0x0000
#define TMR3IE           =  0x0001
#define LVDIE            =  0x0002
#define BCLIE            =  0x0003
#define CMIE             =  0x0006
#define OSCFIE           =  0x0007

#define HLVDIE           =  0x0002


//;----- PIR2 Bits -----------------------------------------------------
#define CCP2IF           =  0x0000
#define TMR3IF           =  0x0001
#define LVDIF            =  0x0002
#define BCLIF            =  0x0003
#define CMIF             =  0x0006
#define OSCFIF           =  0x0007

#define HLVDIF           =  0x0002


//;----- IPR2 Bits -----------------------------------------------------
#define CCP2IP           =  0x0000
#define TMR3IP           =  0x0001
#define LVDIP            =  0x0002
#define BCLIP            =  0x0003
#define CMIP             =  0x0006
#define OSCFIP           =  0x0007

#define HLVDIP           =  0x0002


//;----- RCSTA Bits -----------------------------------------------------
#define RX9D             =  0x0000
#define OERR             =  0x0001
#define FERR             =  0x0002
#define ADEN             =  0x0003
#define CREN             =  0x0004
#define SREN             =  0x0005
#define RX9              =  0x0006
#define SPEN             =  0x0007

#define ADDEN            =  0x0003


//;----- TXSTA Bits -----------------------------------------------------
#define TX9D             =  0x0000
#define TRMT             =  0x0001
#define BRGH             =  0x0002
#define SENDB            =  0x0003
#define SYNC             =  0x0004
#define TXEN             =  0x0005
#define TX9              =  0x0006
#define CSRC             =  0x0007


//;----- T3CON Bits -----------------------------------------------------
#define TMR3ON           =  0x0000
#define TMR3CS           =  0x0001
#define T3SYNC           =  0x0002
#define T3CCP1           =  0x0003
#define T3CKPS0          =  0x0004
#define T3CKPS1          =  0x0005
#define T3CCP2           =  0x0006
#define RD16             =  0x0007

#define NOT_T3SYNC       =  0x0002

//;----- CMCON Bits -----------------------------------------------------
#define CM0              =  0x0000
#define CM1              =  0x0001
#define CM2              =  0x0002
#define CIS              =  0x0003
#define C1INV            =  0x0004
#define C2INV            =  0x0005
#define C1OUT            =  0x0006
#define C2OUT            =  0x0007


//;----- CVRCON Bits -----------------------------------------------------
#define CVR0             =  0x0000
#define CVR1             =  0x0001
#define CVR2             =  0x0002
#define CVR3             =  0x0003
#define CVRSS            =  0x0004
#define CVRR             =  0x0005
#define CVROE            =  0x0006
#define CVREN            =  0x0007


//;----- PWM1CON Bits -----------------------------------------------------
#define PRSEN            =  0x0007


//;----- BAUDCON Bits -----------------------------------------------------
#define ABDEN            =  0x0000
#define WUE              =  0x0001
#define BRG16            =  0x0003
#define SCKP             =  0x0004
#define RCIDL            =  0x0006
#define ABDOVF           =  0x0007

#define TXCKP            =  0x0004
#define RXDTP            =  0x0005
#define RCMT             =  0x0006


//;----- BAUDCTL Bits -----------------------------------------------------
#define ABDEN            =  0x0000
#define WUE              =  0x0001
#define BRG16            =  0x0003
#define SCKP             =  0x0004
#define RCIDL            =  0x0006
#define ABDOVF           =  0x0007

#define TXCKP            =  0x0004
#define RXDTP            =  0x0005
#define RCMT             =  0x0006


//;----- CCP2CON Bits -----------------------------------------------------
#define CCP2M0           =  0x0000
#define CCP2M1           =  0x0001
#define CCP2M2           =  0x0002
#define CCP2M3           =  0x0003
#define CCP2Y            =  0x0004
#define CCP2X            =  0x0005

#define DC2B0            =  0x0004
#define DC2B1            =  0x0005


//;----- CCP1CON Bits -----------------------------------------------------
#define CCP1M0           =  0x0000
#define CCP1M1           =  0x0001
#define CCP1M2           =  0x0002
#define CCP1M3           =  0x0003
#define CCP1Y            =  0x0004
#define CCP1X            =  0x0005

#define DC1B0            =  0x0004
#define DC1B1            =  0x0005


//;----- ADCON2 Bits -----------------------------------------------------
#define ADCS0            =  0x0000
#define ADCS1            =  0x0001
#define ADCS2            =  0x0002
#define ACQT0            =  0x0003
#define ACQT1            =  0x0004
#define ACQT2            =  0x0005
#define ADFM             =  0x0007


//;----- ADCON1 Bits -----------------------------------------------------
#define PCFG0            =  0x0000
#define PCFG1            =  0x0001
#define PCFG2            =  0x0002
#define PCFG3            =  0x0003
#define VCFG0            =  0x0004
#define VCFG1            =  0x0005


//;----- ADCON0 Bits -----------------------------------------------------
#define ADON             =  0x0000
#define GO               =  0x0001
#define CHS0             =  0x0002
#define CHS1             =  0x0003
#define CHS2             =  0x0004
#define CHS3             =  0x0005

#define DONE             =  0x0001

#define NOT_DONE         =  0x0001

#define GO_DONE          =  0x0001


//;----- SSPCON2 Bits -----------------------------------------------------
#define SEN              =  0x0000
#define RSEN             =  0x0001
#define PEN              =  0x0002
#define RCEN             =  0x0003
#define ACKEN            =  0x0004
#define ACKDT            =  0x0005
#define ACKSTAT          =  0x0006
#define GCEN             =  0x0007


//;----- SSPCON1 Bits -----------------------------------------------------
#define SSPM0            =  0x0000
#define SSPM1            =  0x0001
#define SSPM2            =  0x0002
#define SSPM3            =  0x0003
#define CKP              =  0x0004
#define SSPEN            =  0x0005
#define SSPOV            =  0x0006
#define WCOL             =  0x0007


//;----- SSPSTAT Bits -----------------------------------------------------
#define BF               =  0x0000
#define UA               =  0x0001
#define R                =  0x0002
#define S                =  0x0003
#define P                =  0x0004
#define D                =  0x0005
#define CKE              =  0x0006
#define SMP              =  0x0007


#define NOT_W            =  0x0002
#define NOT_A            =  0x0005

#define R_W              =  0x0002
#define D_A              =  0x0005

#define NOT_WRITE        =  0x0002
#define NOT_ADDRESS      =  0x0005


//;----- T2CON Bits -----------------------------------------------------
#define T2CKPS0          =  0x0000
#define T2CKPS1          =  0x0001
#define TMR2ON           =  0x0002
#define T2OUTPS0         =  0x0003
#define T2OUTPS1         =  0x0004
#define T2OUTPS2         =  0x0005
#define T2OUTPS3         =  0x0006

#define TOUTPS0          =  0x0003
#define TOUTPS1          =  0x0004
#define TOUTPS2          =  0x0005
#define TOUTPS3          =  0x0006


//;----- T1CON Bits -----------------------------------------------------
#define TMR1ON           =  0x0000
#define TMR1CS           =  0x0001
#define T1SYNC           =  0x0002
#define T1OSCEN          =  0x0003
#define T1CKPS0          =  0x0004
#define T1CKPS1          =  0x0005
#define T1RUN            =  0x0006
#define RD16             =  0x0007

#define NOT_T1SYNC       =  0x0002


//;----- RCON Bits -----------------------------------------------------
#define BOR              =  0x0000
#define POR              =  0x0001
#define PD               =  0x0002
#define TO               =  0x0003
#define RI               =  0x0004
#define SBOREN           =  0x0006
#define IPEN             =  0x0007

#define NOT_BOR          =  0x0000
#define NOT_POR          =  0x0001
#define NOT_PD           =  0x0002
#define NOT_TO           =  0x0003
#define NOT_RI           =  0x0004


//;----- WDTCON Bits -----------------------------------------------------
#define SWDTEN           =  0x0000

#define SWDTE            =  0x0000


//;----- HLVDCON Bits -----------------------------------------------------
#define LVDL0            =  0x0000
#define LVDL1            =  0x0001
#define LVDL2            =  0x0002
#define LVDL3            =  0x0003
#define LVDEN            =  0x0004
#define IRVST            =  0x0005

#define LVV0             =  0x0000
#define LVV1             =  0x0001
#define LVV2             =  0x0002
#define LVV3             =  0x0003
#define BGST             =  0x0005

#define HLVDL0           =  0x0000
#define HLVDL1           =  0x0001
#define HLVDL2           =  0x0002
#define HLVDL3           =  0x0003
#define HLVDEN           =  0x0004
#define VDIRMAG          =  0x0007

#define IVRST            =  0x0005


//;----- LVDCON Bits -----------------------------------------------------
#define LVDL0            =  0x0000
#define LVDL1            =  0x0001
#define LVDL2            =  0x0002
#define LVDL3            =  0x0003
#define LVDEN            =  0x0004
#define IRVST            =  0x0005

#define LVV0             =  0x0000
#define LVV1             =  0x0001
#define LVV2             =  0x0002
#define LVV3             =  0x0003
#define BGST             =  0x0005

#define HLVDL0           =  0x0000
#define HLVDL1           =  0x0001
#define HLVDL2           =  0x0002
#define HLVDL3           =  0x0003
#define HLVDEN           =  0x0004
#define VDIRMAG          =  0x0007

#define IVRST            =  0x0005


//;----- OSCCON Bits -----------------------------------------------------
#define SCS0             =  0x0000
#define SCS1             =  0x0001
#define FLTS             =  0x0002
#define OSTS             =  0x0003
#define IRCF0            =  0x0004
#define IRCF1            =  0x0005
#define IRCF2            =  0x0006
#define IDLEN            =  0x0007

#define IOFS             =  0x0002


//;----- T0CON Bits -----------------------------------------------------
#define T0PS0            =  0x0000
#define T0PS1            =  0x0001
#define T0PS2            =  0x0002
#define T0PS3            =  0x0003
#define T0SE             =  0x0004
#define T0CS             =  0x0005
#define T016BIT          =  0x0006
#define TMR0ON           =  0x0007

#define PSA              =  0x0003
#define T08BIT           =  0x0006


//;----- STATUS Bits -----------------------------------------------------
#define C                =  0x0000
#define DC               =  0x0001
#define Z                =  0x0002
#define OV               =  0x0003
#define N                =  0x0004


//;----- INTCON3 Bits -----------------------------------------------------
#define INT1F            =  0x0000
#define INT2F            =  0x0001
#define INT1E            =  0x0003
#define INT2E            =  0x0004
#define INT1P            =  0x0006
#define INT2P            =  0x0007

#define INT1IF           =  0x0000
#define INT2IF           =  0x0001
#define INT1IE           =  0x0003
#define INT2IE           =  0x0004
#define INT1IP           =  0x0006
#define INT2IP           =  0x0007


//;----- INTCON2 Bits -----------------------------------------------------
#define RBIP             =  0x0000
#define TMR0IP           =  0x0002
#define INTEDG2          =  0x0004
#define INTEDG1          =  0x0005
#define INTEDG0          =  0x0006
#define RBPU             =  0x0007

#define NOT_RBPU         =  0x0007


//;----- INTCON Bits -----------------------------------------------------
#define RBIF             =  0x0000
#define INT0F            =  0x0001
#define TMR0IF           =  0x0002
#define RBIE             =  0x0003
#define INT0E            =  0x0004
#define TMR0IE           =  0x0005
#define PEIE             =  0x0006
#define GIE              =  0x0007

#define INT0IF           =  0x0001
#define T0IF             =  0x0002
#define INT0IE           =  0x0004
#define T0IE             =  0x0005
#define GIEL             =  0x0006
#define GIEH             =  0x0007


//;----- STKPTR Bits -----------------------------------------------------
#define SP0              =  0x0000
#define SP1              =  0x0001
#define SP2              =  0x0002
#define SP3              =  0x0003
#define SP4              =  0x0004
#define STKUNF           =  0x0006
#define STKFUL           =  0x0007

#define STKOVF           =  0x0007



/*;==========================================================================
;
;       RAM Definitions
;
;==========================================================================
       __MAXRAM  H'0FFF'
       __BADRAM  H'0F83'
       __BADRAM  H'0F85'-H'0F88'
       __BADRAM  H'0F8C'-H'0F91'
       __BADRAM  H'0F95'-H'0F9A'
       __BADRAM  H'0F9C'
       __BADRAM  H'0FA3'-H'0FAA'
       __BADRAM  H'0FB6'
       __BADRAM  H'0FB9'
       __BADRAM  H'0FD4'

;==========================================================================
;
;   IMPORTANT: For the PIC18 devices, the __CONFIG directive has been
;              superseded by the CONFIG directive.  The following settings
;              are available for this device.
;
;   Oscillator Selection bits:
;     OSC = LP             LP oscillator
;     OSC = XT             XT oscillator
;     OSC = HS             HS oscillator
;     OSC = RC             External RC oscillator, CLKO function on RA6
;     OSC = EC             EC oscillator, CLKO function on RA6
;     OSC = ECIO6          EC oscillator, port function on RA6
;     OSC = HSPLL          HS oscillator, PLL enabled (Clock Frequency = 4 x FOSC1)
;     OSC = RCIO6          External RC oscillator, port function on RA6
;     OSC = INTIO67        Internal oscillator block, port function on RA6 and RA7
;     OSC = INTIO7         Internal oscillator block, CLKO function on RA6, port function on RA7
;
;   Fail-Safe Clock Monitor Enable bit:
;     FCMEN = OFF          Fail-Safe Clock Monitor disabled
;     FCMEN = ON           Fail-Safe Clock Monitor enabled
;
;   Internal/External Oscillator Switchover bit:
;     IESO = OFF           Oscillator Switchover mode disabled
;     IESO = ON            Oscillator Switchover mode enabled
;
;   Power-up Timer Enable bit:
;     PWRT = ON            PWRT enabled
;     PWRT = OFF           PWRT disabled
;
;   Brown-out Reset Enable bits:
;     BOREN = OFF          Brown-out Reset disabled in hardware and software
;     BOREN = ON           Brown-out Reset enabled and controlled by software (SBOREN is enabled)
;     BOREN = NOSLP        Brown-out Reset enabled in hardware only and disabled in Sleep mode (SBOREN is disabled)
;     BOREN = SBORDIS      Brown-out Reset enabled in hardware only (SBOREN is disabled)
;
;   Brown-out Voltage bits:
;     BORV = 0             Maximum setting
;     BORV = 1             
;     BORV = 2             
;     BORV = 3             Minimum setting
;
;   Watchdog Timer Enable bit:
;     WDT = OFF            WDT disabled (control is placed on the SWDTEN bit)
;     WDT = ON             WDT enabled
;
;   Watchdog Timer Postscale Select bits:
;     WDTPS = 1            1:1
;     WDTPS = 2            1:2
;     WDTPS = 4            1:4
;     WDTPS = 8            1:8
;     WDTPS = 16           1:16
;     WDTPS = 32           1:32
;     WDTPS = 64           1:64
;     WDTPS = 128          1:128
;     WDTPS = 256          1:256
;     WDTPS = 512          1:512
;     WDTPS = 1024         1:1024
;     WDTPS = 2048         1:2048
;     WDTPS = 4096         1:4096
;     WDTPS = 8192         1:8192
;     WDTPS = 16384        1:16384
;     WDTPS = 32768        1:32768
;
;   MCLR Pin Enable bit:
;     MCLRE = OFF          RE3 input pin enabled; MCLR disabled
;     MCLRE = ON           MCLR pin enabled; RE3 input pin disabled
;
;   Low-Power Timer1 Oscillator Enable bit:
;     LPT1OSC = OFF        Timer1 configured for higher power operation
;     LPT1OSC = ON         Timer1 configured for low-power operation
;
;   PORTB A/D Enable bit:
;     PBADEN = OFF         PORTB<4:0> pins are configured as digital I/O on Reset
;     PBADEN = ON          PORTB<4:0> pins are configured as analog input channels on Reset
;
;   CCP2 MUX bit:
;     CCP2MX = PORTBE      CCP2 input/output is multiplexed with RB3
;     CCP2MX = PORTC       CCP2 input/output is multiplexed with RC1
;
;   Stack Full/Underflow Reset Enable bit:
;     STVREN = OFF         Stack full/underflow will not cause Reset
;     STVREN = ON          Stack full/underflow will cause Reset
;
;   Single-Supply ICSP Enable bit:
;     LVP = OFF            Single-Supply ICSP disabled
;     LVP = ON             Single-Supply ICSP enabled
;
;   Extended Instruction Set Enable bit:
;     XINST = OFF          Instruction set extension and Indexed Addressing mode disabled (Legacy mode)
;     XINST = ON           Instruction set extension and Indexed Addressing mode enabled
;
;   Background Debugger Enable bit:
;     DEBUG = ON           Background debugger enabled, RB6 and RB7 are dedicated to In-Circuit Debug
;     DEBUG = OFF          Background debugger disabled, RB6 and RB7 configured as general purpose I/O pins
;
;   Code Protection bit Block 0:
;     CP0 = ON             Block 0 (000800-003FFFh) code-protected
;     CP0 = OFF            Block 0 (000800-003FFFh) not code-protected
;
;   Code Protection bit Block 1:
;     CP1 = ON             Block 1 (004000-007FFFh) code-protected
;     CP1 = OFF            Block 1 (004000-007FFFh) not code-protected
;
;   Code Protection bit Block 2:
;     CP2 = ON             Block 2 (008000-00BFFFh) code-protected
;     CP2 = OFF            Block 2 (008000-00BFFFh) not code-protected
;
;   Boot Block Code Protection bit:
;     CPB = ON             Boot block (000000-0007FFh) code-protected
;     CPB = OFF            Boot block (000000-0007FFh) not code-protected
;
;   Write Protection bit Block 0:
;     WRT0 = ON            Block 0 (000800-003FFFh) write-protected
;     WRT0 = OFF           Block 0 (000800-003FFFh) not write-protected
;
;   Write Protection bit Block 1:
;     WRT1 = ON            Block 1 (004000-007FFFh) write-protected
;     WRT1 = OFF           Block 1 (004000-007FFFh) not write-protected
;
;   Write Protection bit Block 2:
;     WRT2 = ON            Block 2 (008000-00BFFFh) write-protected
;     WRT2 = OFF           Block 2 (008000-00BFFFh) not write-protected
;
;   Boot Block Write Protection bit:
;     WRTB = ON            Boot block (000000-0007FFh) write-protected
;     WRTB = OFF           Boot block (000000-0007FFh) not write-protected
;
;   Configuration Register Write Protection bit:
;     WRTC = ON            Configuration registers (300000-3000FFh) write-protected
;     WRTC = OFF           Configuration registers (300000-3000FFh) not write-protected
;
;   Table Read Protection bit Block 0:
;     EBTR0 = ON           Block 0 (000800-003FFFh) protected from table reads executed in other blocks
;     EBTR0 = OFF          Block 0 (000800-003FFFh) not protected from table reads executed in other blocks
;
;   Table Read Protection bit Block 1:
;     EBTR1 = ON           Block 1 (004000-007FFFh) protected from table reads executed in other blocks
;     EBTR1 = OFF          Block 1 (004000-007FFFh) not protected from table reads executed in other blocks
;
;   Table Read Protection bit Block 2:
;     EBTR2 = ON           Block 2 (008000-00BFFFh) protected from table reads executed in other blocks
;     EBTR2 = OFF          Block 2 (008000-00BFFFh) not protected from table reads executed in other blocks
;
;   Boot Block Table Read Protection bit:
;     EBTRB = ON           Boot block (000000-0007FFh) protected from table reads executed in other blocks
;     EBTRB = OFF          Boot block (000000-0007FFh) not protected from table reads executed in other blocks
;
;==========================================================================
;==========================================================================
;
;       Configuration Bits
;
;   NAME            Address
;   CONFIG1H        300001h
;   CONFIG2L        300002h
;   CONFIG2H        300003h
;   CONFIG3H        300005h
;   CONFIG4L        300006h
;   CONFIG5L        300008h
;   CONFIG5H        300009h
;   CONFIG6L        30000Ah
;   CONFIG6H        30000Bh
;   CONFIG7L        30000Ch
;   CONFIG7H        30000Dh
;
;==========================================================================*/

//; The following is an assignment of address values for all of the
//; configuration registers for the purpose of table reads
//_CONFIG1H        =  0x300001
//_CONFIG2L        =  0x300002
//_CONFIG2H        =  0x300003
//_CONFIG3H        =  0x300005
//_CONFIG4L        =  0x300006
//_CONFIG5L        =  0x300008
//_CONFIG5H        =  0x300009
//_CONFIG6L        =  0x30000A
//_CONFIG6H        =  0x30000B
//_CONFIG7L        =  0x30000C
//_CONFIG7H        =  0x30000D

//;----- CONFIG1H Options --------------------------------------------------
//_OSC_LP_1H           =  0xF0    //; LP oscillator
//_OSC_XT_1H           =  0xF1    //; XT oscillator
//_OSC_HS_1H           =  0xF2    //; HS oscillator
//_OSC_RC_1H           =  0xF3    //; External RC oscillator, CLKO function on RA6
//_OSC_EC_1H           =  0xF4    //; EC oscillator, CLKO function on RA6
//_OSC_ECIO6_1H        =  0xF5    //; EC oscillator, port function on RA6
//_OSC_HSPLL_1H        =  0xF6    //; HS oscillator, PLL enabled (Clock Frequency = 4 x FOSC1)
//_OSC_RCIO6_1H        =  0xF7    //; External RC oscillator, port function on RA6
//_OSC_INTIO67_1H      =  0xF8    //; Internal oscillator block, port function on RA6 and RA7
//_OSC_INTIO7_1H       =  0xF9    //; Internal oscillator block, CLKO function on RA6, port function on RA7

//_FCMEN_OFF_1H        =  0xBF    //; Fail-Safe Clock Monitor disabled
//_FCMEN_ON_1H         =  0xFF    //; Fail-Safe Clock Monitor enabled

//_IESO_OFF_1H         =  0x7F    //; Oscillator Switchover mode disabled
//_IESO_ON_1H          =  0xFF    //; Oscillator Switchover mode enabled

//;----- CONFIG2L Options --------------------------------------------------
//_PWRT_ON_2L          =  0xFE    //; PWRT enabled
//_PWRT_OFF_2L         =  0xFF    //; PWRT disabled

//_BOREN_OFF_2L        =  0xF9    //; Brown-out Reset disabled in hardware and software
//_BOREN_ON_2L         =  0xFB    //; Brown-out Reset enabled and controlled by software (SBOREN is enabled)
//_BOREN_NOSLP_2L      =  0xFD    //; Brown-out Reset enabled in hardware only and disabled in Sleep mode (SBOREN is disabled)
//_BOREN_SBORDIS_2L    =  0xFF    //; Brown-out Reset enabled in hardware only (SBOREN is disabled)

//_BORV_0_2L           =  0xE7    //; Maximum setting
//_BORV_1_2L           =  0xEF    //; 
//_BORV_2_2L           =  0xF7    //; 
//_BORV_3_2L           =  0xFF    //; Minimum setting

//;----- CONFIG2H Options --------------------------------------------------
//_WDT_OFF_2H          =  0xFE    //; WDT disabled (control is placed on the SWDTEN bit)
//_WDT_ON_2H           =  0xFF    //; WDT enabled

//_WDTPS_1_2H          =  0xE1    //; 1:1
//_WDTPS_2_2H          =  0xE3    //; 1:2
//_WDTPS_4_2H          =  0xE5    //; 1:4
//_WDTPS_8_2H          =  0xE7    //; 1:8
//_WDTPS_16_2H         =  0xE9    //; 1:16
//_WDTPS_32_2H         =  0xEB    //; 1:32
//_WDTPS_64_2H         =  0xED    //; 1:64
//_WDTPS_128_2H        =  0xEF    //; 1:128
//_WDTPS_256_2H        =  0xF1    //; 1:256
//_WDTPS_512_2H        =  0xF3    //; 1:512
//_WDTPS_1024_2H       =  0xF5    //; 1:1024
//_WDTPS_2048_2H       =  0xF7    //; 1:2048
//_WDTPS_4096_2H       =  0xF9    //; 1:4096
//_WDTPS_8192_2H       =  0xFB    //; 1:8192
//_WDTPS_16384_2H      =  0xFD    //; 1:16384
//_WDTPS_32768_2H      =  0xFF    //; 1:32768

//;----- CONFIG3H Options --------------------------------------------------
//_MCLRE_OFF_3H        =  0x7F    //; RE3 input pin enabled; MCLR disabled
//_MCLRE_ON_3H         =  0xFF    //; MCLR pin enabled; RE3 input pin disabled

//_LPT1OSC_OFF_3H      =  0xFB    //; Timer1 configured for higher power operation
//_LPT1OSC_ON_3H       =  0xFF    //; Timer1 configured for low-power operation

//_PBADEN_OFF_3H       =  0xFD    //; PORTB<4:0> pins are configured as digital I/O on Reset
//_PBADEN_ON_3H        =  0xFF    //; PORTB<4:0> pins are configured as analog input channels on Reset

//_CCP2MX_PORTBE_3H    =  0xFE    //; CCP2 input/output is multiplexed with RB3
//_CCP2MX_PORTC_3H     =  0xFF    //; CCP2 input/output is multiplexed with RC1

//;----- CONFIG4L Options --------------------------------------------------
//_STVREN_OFF_4L       =  0xFE    //; Stack full/underflow will not cause Reset
//_STVREN_ON_4L        =  0xFF    //; Stack full/underflow will cause Reset

//_LVP_OFF_4L          =  0xFB    //; Single-Supply ICSP disabled
//_LVP_ON_4L           =  0xFF    //; Single-Supply ICSP enabled

//_XINST_OFF_4L        =  0xBF    //; Instruction set extension and Indexed Addressing mode disabled (Legacy mode)
//_XINST_ON_4L         =  0xFF    //; Instruction set extension and Indexed Addressing mode enabled

//_DEBUG_ON_4L         =  0x7F    //; Background debugger enabled, RB6 and RB7 are dedicated to In-Circuit Debug
//_DEBUG_OFF_4L        =  0xFF    //; Background debugger disabled, RB6 and RB7 configured as general purpose I/O pins

//;----- CONFIG5L Options --------------------------------------------------
//_CP0_ON_5L           =  0xFE    //; Block 0 (000800-003FFFh) code-protected
//_CP0_OFF_5L          =  0xFF    //; Block 0 (000800-003FFFh) not code-protected

//_CP1_ON_5L           =  0xFD    //; Block 1 (004000-007FFFh) code-protected
//_CP1_OFF_5L          =  0xFF    //; Block 1 (004000-007FFFh) not code-protected

//_CP2_ON_5L           =  0xFB    //; Block 2 (008000-00BFFFh) code-protected
//_CP2_OFF_5L          =  0xFF    //; Block 2 (008000-00BFFFh) not code-protected

//;----- CONFIG5H Options --------------------------------------------------
//_CPB_ON_5H           =  0xBF    //; Boot block (000000-0007FFh) code-protected
//_CPB_OFF_5H          =  0xFF    //; Boot block (000000-0007FFh) not code-protected

//;----- CONFIG6L Options --------------------------------------------------
//_WRT0_ON_6L          =  0xFE    //; Block 0 (000800-003FFFh) write-protected
//_WRT0_OFF_6L         =  0xFF    //; Block 0 (000800-003FFFh) not write-protected

//_WRT1_ON_6L          =  0xFD    //; Block 1 (004000-007FFFh) write-protected
//_WRT1_OFF_6L         =  0xFF    //; Block 1 (004000-007FFFh) not write-protected

//_WRT2_ON_6L          =  0xFB    //; Block 2 (008000-00BFFFh) write-protected
//_WRT2_OFF_6L         =  0xFF    //; Block 2 (008000-00BFFFh) not write-protected

//;----- CONFIG6H Options --------------------------------------------------
//_WRTB_ON_6H          =  0xBF    //; Boot block (000000-0007FFh) write-protected
//_WRTB_OFF_6H         =  0xFF    //; Boot block (000000-0007FFh) not write-protected

//_WRTC_ON_6H          =  0xDF    //; Configuration registers (300000-3000FFh) write-protected
//_WRTC_OFF_6H         =  0xFF    //; Configuration registers (300000-3000FFh) not write-protected

//;----- CONFIG7L Options --------------------------------------------------
//_EBTR0_ON_7L         =  0xFE    //; Block 0 (000800-003FFFh) protected from table reads executed in other blocks
//_EBTR0_OFF_7L        =  0xFF    //; Block 0 (000800-003FFFh) not protected from table reads executed in other blocks

//_EBTR1_ON_7L         =  0xFD    //; Block 1 (004000-007FFFh) protected from table reads executed in other blocks
//_EBTR1_OFF_7L        =  0xFF    //; Block 1 (004000-007FFFh) not protected from table reads executed in other blocks

//_EBTR2_ON_7L         =  0xFB    //; Block 2 (008000-00BFFFh) protected from table reads executed in other blocks
//_EBTR2_OFF_7L        =  0xFF    //; Block 2 (008000-00BFFFh) not protected from table reads executed in other blocks

//;----- CONFIG7H Options --------------------------------------------------
//_EBTRB_ON_7H         =  0xBF    //; Boot block (000000-0007FFh) protected from table reads executed in other blocks
//_EBTRB_OFF_7H        =  0xFF    //; Boot block (000000-0007FFh) not protected from table reads executed in other blocks


//_DEVID1          =  0x3FFFFE
//_DEVID2          =  0x3FFFFF

//_IDLOC0          =  0x200000
//_IDLOC1          =  0x200001
//_IDLOC2          =  0x200002
//_IDLOC3          =  0x200003
//_IDLOC4          =  0x200004
//_IDLOC5          =  0x200005
//_IDLOC6          =  0x200006
//_IDLOC7          =  0x200007

 //       LIST
