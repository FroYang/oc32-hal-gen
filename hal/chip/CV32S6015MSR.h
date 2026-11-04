//---------------------------------------------------------------;
// CV32S6015MSR.h                                                ;
// Header file for CV32S6015 Mapped(memory space) SR             ;
//---------------------------------------------------------------;

#ifndef CV32S6015MSR_H
#define CV32S6015MSR_H

//---------------------------------------------------------------;
// Register Struct                                               ;
//---------------------------------------------------------------;
// ---- Core Part (starts from 0x3FF00000) ----

// ---- Process Identifier (0x3FF00000) ----
#define PID (*(volatile uint32_t *)0x3FF00000)

typedef volatile struct
{
    uint32_t UWSRP : 1;
    uint32_t HISE : 1;
    uint32_t SRME : 1;
    uint32_t SFBR : 1;
    uint32_t HIAS : 4;
    uint32_t : 24;
} CMODE_TypeDef; // 0x0004
#define CMODE ((CMODE_TypeDef *)0x3FF00004)

// ---- Hardware Interrupt (0x3FF00040) ----
typedef volatile struct
{
    uint32_t V0 : 1;
    uint32_t V1 : 1;
    uint32_t V2 : 1;
    uint32_t V3 : 1;
    uint32_t V4 : 1;
    uint32_t V5 : 1;
    uint32_t V6 : 1;
    uint32_t V7 : 1;
    uint32_t V8 : 1;
    uint32_t V9 : 1;
    uint32_t V10 : 1;
    uint32_t V11 : 1;
    uint32_t V12 : 1;
    uint32_t V13 : 1;
    uint32_t V14 : 1;
    uint32_t V15 : 1;
    uint32_t V16 : 1;
    uint32_t V17 : 1;
    uint32_t V18 : 1;
    uint32_t V19 : 1;
    uint32_t V20 : 1;
    uint32_t V21 : 1;
    uint32_t V22 : 1;
    uint32_t V23 : 1;
    uint32_t V24 : 1;
    uint32_t V25 : 1;
    uint32_t V26 : 1;
    uint32_t V27 : 1;
    uint32_t V28 : 1;
    uint32_t V29 : 1;
    uint32_t V30 : 1;
    uint32_t V31 : 1;
} HIC_TypeDef; // 0x3FF00040; 
#define HIE ((HIC_TypeDef *)0x3FF00040)
#define HIPRM ((HIC_TypeDef *)0x3FF00044)
#define HIPL0 ((HIC_TypeDef *)0x3FF00048)
#define HIPL1 ((HIC_TypeDef *)0x3FF0004C)


// ---- Peripheral Part (starts from 0x3FF08000) ----

// ---- Hardware Reset Flag (0x3FF08000) ----
typedef volatile struct
{
    uint32_t   : 14;
    uint32_t VTSAFCLR : 1;
    uint32_t VTSAF : 1;
    uint32_t   : 2;
    uint32_t WDT0RFCLR : 1;
    uint32_t WDT0RF : 1;
    uint32_t WDT1RFCLR : 1;
    uint32_t WDT1RF : 1;
    uint32_t LVDRFCLR : 1;
    uint32_t LVDRF : 1;
    uint32_t DBGRFCLR : 1;
    uint32_t DBGRF : 1;
    uint32_t IERFCLR : 1;
    uint32_t IERF : 1;
    uint32_t   : 2;
    uint32_t PORFCLR : 1;
    uint32_t PORF : 1;
} HRF_TypeDef; // 0x8000
#define HRF ((HRF_TypeDef *)0x3FF08000)


// ---- Alarm (0x3FF08004) ----
typedef volatile struct
{
    uint32_t   : 12;
    uint32_t UXARE : 1;
    uint32_t VTSARE : 1;
    uint32_t   : 18;
} ALMCON_TypeDef; // 0x8004
#define ALMCON ((ALMCON_TypeDef *)0x3FF08004)


// ---- Bandgap & Power (0x3FF08008) ----
typedef volatile struct
{
    uint32_t BTC : 4;
    uint32_t  : 4;
    uint32_t BVC : 4;
    uint32_t  : 4;
    uint32_t REG15C : 5;
    uint32_t  : 3;
    uint32_t REG33C : 5;
    uint32_t  : 1;
    uint32_t HOLD : 1;
    uint32_t SLEEP : 1;
} BPCON_TypeDef; // 0x8008
#define BPCON ((BPCON_TypeDef *)0x3FF08008)


// ---- DMA (0x3FF0800C) ----
typedef volatile struct
{
    uint32_t DMACH0 : 4;
    uint32_t DMACH1 : 4;
    uint32_t DMACH2 : 4;
    uint32_t DMACH3 : 4;
    uint32_t  : 16;
} DMACON_TypeDef; // 0x800C
#define DMACON ((DMACON_TypeDef *)0x3FF0800C)


// ---- LVD (0x3FF08010) ----
typedef volatile struct
{
    uint32_t VTSE : 1;
    uint32_t  : 3;
    uint32_t VTST : 5;
    uint32_t  : 23;
} VTSCON_TypeDef; // 0x8010
#define VTSCON ((VTSCON_TypeDef *)0x3FF08010)

typedef volatile struct
{
    uint32_t LVDE : 1;
    uint32_t LVDRE : 1;
    uint32_t LVDWE : 1;
    uint32_t LVDIE : 1;
    uint32_t LVDVP : 4;
    uint32_t  : 22;
    uint32_t LVDECLR : 1;
    uint32_t LVDEF : 1;
} LVDCON_TypeDef; // 0x8014
#define LVDCON ((LVDCON_TypeDef *)0x3FF08014)


// ---- RCM (0x3FF08018) ----
typedef volatile struct
{
    uint32_t IOSCH : 1;
    uint32_t IOSCUTE : 1;
    uint32_t IOSCUTL : 1;
    uint32_t UXCDRE : 1;
    uint32_t PLLE : 1;
    uint32_t PLLM : 1;
    uint32_t XHOSCE : 1;
    uint32_t CSF : 1;
    uint32_t CSWT : 7;
    uint32_t FLMCG : 1;
    uint32_t CRCMCG : 1;
    uint32_t USBMCG : 1;
    uint32_t ADCMCG : 1;
    uint32_t  : 1;
    uint32_t DACMCG : 1;
    uint32_t  : 11;
} CLKCON_TypeDef; // 0x8018
#define CLKCON ((CLKCON_TypeDef *)0x3FF08018)
#define CLKCON_DEFAULT (0x00001F01U)
#define CSWT_MAX (0x7FU)

typedef volatile struct
{
    uint32_t IOSCHFT : 10;
    uint32_t  : 2;
    uint32_t IOSCHRT : 2;
    uint32_t  : 2;
    uint32_t IOSCLFT : 10;
    uint32_t  : 2;
    uint32_t IOSCLRT : 2;
    uint32_t  : 2;
} IOSCCON_TypeDef; // 0x801C
#define IOSCCON ((IOSCCON_TypeDef *)0x3FF0801C)

typedef volatile struct
{
    uint32_t PFM : 8;
    uint32_t PFD : 3;
    uint32_t  : 5;
    uint32_t PFCNT : 10;
    uint32_t  : 3;
    uint32_t PFOM : 1;
    uint32_t PFOCLR : 1;
    uint32_t PFO : 1;
} PLLCON_TypeDef; // 0x82FC
#define PLLCON ((PLLCON_TypeDef *)0x3FF082FC)
#define PLLCON_DEFAULT (0x00000028U)
#define PFCNT_MAX (0x3FFU)


// ---- Flashloader (0x3FF08020) ----
typedef volatile struct
{
    uint32_t FLCI : 8;
    uint32_t FLCC : 2;
    uint32_t FLCDL : 2;
    uint32_t FLCLC : 4;
    uint32_t FLTSHSL : 4;
    uint32_t FLCAC : 5;
    uint32_t  : 3;
    uint32_t FLBANK0V : 1;
    uint32_t FLBANK1V : 1;
    uint32_t FLBANK2V : 1;
    uint32_t FLBANK3V : 1;
} FLCCON_TypeDef; // 0x8020
#define FLCCON ((FLCCON_TypeDef *)0x3FF08020)

typedef volatile struct
{
    uint32_t FLUI : 8;
    uint32_t FLUC : 2;
    uint32_t FLUDC : 2;
    uint32_t FLUT : 3;
    uint32_t FLUKS : 1;
    uint32_t FLULC : 4;
    uint32_t FLUWIPC : 1;
    uint32_t FLUSC : 1;
    uint32_t FLUSS : 1;
    uint32_t FLURUCS : 1;
    uint32_t FLURRC : 3;
    uint32_t FLURRE : 1;
    uint32_t FLDCTP : 1;
    uint32_t FLUIE : 1;
    uint32_t FLUOCLR : 1;
    uint32_t FLUOPF : 1;
} FLUCON_TypeDef; // 0x8024
#define FLUCON ((FLUCON_TypeDef *)0x3FF08024)

#define FLUADR (*(volatile uint32_t *)0x3FF08028)
#define FLUDATA0 (*(volatile uint32_t *)0x3FF0802C)
#define FLUDATA1 (*(volatile uint32_t *)0x3FF08030)
#define FLUDATA2 (*(volatile uint32_t *)0x3FF08034)
#define FLUDATA3 (*(volatile uint32_t *)0x3FF08038)
#define FLURRDA (*(volatile uint32_t *)0x3FF0803C)


// ---- Port (0x3FF08040/0x3FF08080) ----
typedef volatile struct
{
    uint32_t P0 : 1;
    uint32_t P1 : 1;
    uint32_t P2 : 1;
    uint32_t P3 : 1;
    uint32_t P4 : 1;
    uint32_t P5 : 1;
    uint32_t P6 : 1;
    uint32_t P7 : 1;
    uint32_t P8 : 1;
    uint32_t P9 : 1;
    uint32_t P10 : 1;
    uint32_t P11 : 1;
    uint32_t P12 : 1;
    uint32_t P13 : 1;
    uint32_t P14 : 1;
    uint32_t P15 : 1;
    uint32_t P16 : 1;
    uint32_t P17 : 1;
    uint32_t P18 : 1;
    uint32_t P19 : 1;
    uint32_t P20 : 1;
    uint32_t P21 : 1;
    uint32_t P22 : 1;
    uint32_t P23 : 1;
    uint32_t P24 : 1;
    uint32_t P25 : 1;
    uint32_t P26 : 1;
    uint32_t P27 : 1;
    uint32_t P28 : 1;
    uint32_t P29 : 1;
    uint32_t P30 : 1;
    uint32_t P31 : 1;
} PORT_TypeDef; // 0x8040; Port Data
#define PA ((PORT_TypeDef *)0x3FF08040)
#define PAOE ((PORT_TypeDef *)0x3FF08044)
#define PAIE ((PORT_TypeDef *)0x3FF08048)
#define PAPU ((PORT_TypeDef *)0x3FF0804C)
#define PARIE ((PORT_TypeDef *)0x3FF08050)
#define PAFIE ((PORT_TypeDef *)0x3FF08054)
#define PAIF ((PORT_TypeDef *)0x3FF08058)
#define PAIFC ((PORT_TypeDef *)0x3FF0805C)
#define PAFS0 ((PORT_TypeDef *)0x3FF08060)
#define PAFS1 ((PORT_TypeDef *)0x3FF08064)
#define PALS ((PORT_TypeDef *)0x3FF08068)
#define PAPE ((PORT_TypeDef *)0x3FF0806C)
#define PA0  (PA->P0)
#define PA1  (PA->P1)
#define PA2  (PA->P2)
#define PA3  (PA->P3)
#define PA4  (PA->P4)
#define PA5  (PA->P5)
#define PA6  (PA->P6)
#define PA7  (PA->P7)
#define PA8  (PA->P8)
#define PA9  (PA->P9)
#define PA10 (PA->P10)
#define PA11 (PA->P11)
#define PA12 (PA->P12)
#define PA13 (PA->P13)
#define PA14 (PA->P14)
#define PA15 (PA->P15)
#define PA16 (PA->P16)
#define PA17 (PA->P17)
#define PA18 (PA->P18)
#define PA19 (PA->P19)
#define PA20 (PA->P20)
#define PA21 (PA->P21)
#define PA22 (PA->P22)
#define PA23 (PA->P23)
#define PA24 (PA->P24)
#define PA25 (PA->P25)
#define PA26 (PA->P26)
#define PA27 (PA->P27)
#define PA28 (PA->P28)
#define PA29 (PA->P29)
#define PA30 (PA->P30)
#define PA31 (PA->P31)


#define PB ((PORT_TypeDef *)0x3FF08080)
#define PBOE ((PORT_TypeDef *)0x3FF08084)
#define PBIE ((PORT_TypeDef *)0x3FF08088)
#define PBPU ((PORT_TypeDef *)0x3FF0808C)
#define PBRIE ((PORT_TypeDef *)0x3FF08090)
#define PBFIE ((PORT_TypeDef *)0x3FF08094)
#define PBIF ((PORT_TypeDef *)0x3FF08098)
#define PBIFC ((PORT_TypeDef *)0x3FF0809C)
#define PBFS0 ((PORT_TypeDef *)0x3FF080A0)
#define PBFS1 ((PORT_TypeDef *)0x3FF080A4)
#define PBLS ((PORT_TypeDef *)0x3FF080A8)
#define PBPE ((PORT_TypeDef *)0x3FF080AC)
#define PB0  (PB->P0)
#define PB1  (PB->P1)
#define PB2  (PB->P2)
#define PB3  (PB->P3)
#define PB4  (PB->P4)
#define PB5  (PB->P5)
#define PB6  (PB->P6)
#define PB7  (PB->P7)
#define PB8  (PB->P8)
#define PB9  (PB->P9)
#define PB10 (PB->P10)
#define PB11 (PB->P11)
#define PB12 (PB->P12)
#define PB13 (PB->P13)
#define PB14 (PB->P14)
#define PB15 (PB->P15)
#define PB16 (PB->P16)
#define PB17 (PB->P17)
#define PB18 (PB->P18)
#define PB19 (PB->P19)
#define PB20 (PB->P20)
#define PB21 (PB->P21)
#define PB22 (PB->P22)
#define PB23 (PB->P23)
#define PB24 (PB->P24)
#define PB25 (PB->P25)
#define PB26 (PB->P26)
#define PB27 (PB->P27)
#define PB28 (PB->P28)
#define PB29 (PB->P29)
#define PB30 (PB->P30)
#define PB31 (PB->P31)


typedef volatile struct
{
    uint32_t D;
    uint32_t OE;
    uint32_t IE;
    uint32_t PU;
    uint32_t RIE;
    uint32_t FIE;
    uint32_t IF;
    uint32_t IFC;
    uint32_t FS0;
    uint32_t FS1;
    uint32_t LS;
    uint32_t PE;
} MODULE_PORT_TypeDef;

#define PORT_A ((MODULE_PORT_TypeDef *)0x3FF08040)
#define PORT_B ((MODULE_PORT_TypeDef *)0x3FF08080)


// ---- ADC (0x3FF08140) ----
typedef volatile struct
{
    uint32_t ADCE : 1;
    uint32_t ADCDMADA : 1;
    uint32_t ADCDMABM : 2;
    uint32_t ADCCCD : 3;
    uint32_t ADCUCS : 1;
    uint32_t ADCRS : 4;
    uint32_t ADCP0ES : 5;
    uint32_t ADCP0REE : 1;
    uint32_t ADCP0FEE : 1;
    uint32_t ADCP1ES : 5;
    uint32_t ADCP1REE : 1;
    uint32_t ADCP1FEE : 1;
    uint32_t  : 1;
    uint32_t ADCESDLYP : 1;
    uint32_t ADCESDLYU : 4;
} ADCCON_TypeDef; // 0x8140
#define ADC0CON ((ADCCON_TypeDef *)0x3FF08140)
#define ADC1CON ((ADCCON_TypeDef *)0x3FF08180)

typedef volatile struct
{
    uint32_t ADCED : 12;
    uint32_t ADCEIS : 4;
    uint32_t ADCEES : 4;
    uint32_t ADCESDLY : 4;
    uint32_t ADCEPL : 3;
    uint32_t ADCEST : 3;
    uint32_t ADCECCLR : 1;
    uint32_t ADCECF : 1;
} ADCE0CC_TypeDef; // 0x8144
#define ADC0E0CC ((ADCE0CC_TypeDef *)0x3FF088144)
#define ADC1E0CC ((ADCE0CC_TypeDef *)0x3FF088184)

typedef volatile struct
{
    uint32_t ADCED : 12;
    uint32_t ADCEIS : 4;
    uint32_t ADCEES : 4;
    uint32_t ADCESDLY : 4;
    uint32_t ADCEPL : 3;
    uint32_t ADCEST : 3;
    uint32_t ADCECCLR : 1;
    uint32_t ADCECF : 1;
} ADCE1CC_TypeDef; // 0x8148
#define ADC0E1CC ((ADCE1CC_TypeDef *)0x3FF08148)
#define ADC1E1CC ((ADCE1CC_TypeDef *)0x3FF08188)

typedef volatile struct
{
    uint32_t ADCED : 12;
    uint32_t ADCEIS : 4;
    uint32_t ADCEES : 4;
    uint32_t ADCESDLY : 4;
    uint32_t ADCEPL : 3;
    uint32_t ADCEST : 3;
    uint32_t ADCECCLR : 1;
    uint32_t ADCECF : 1;
} ADCE2CC_TypeDef; // 0x814C
#define ADC0E2CC ((ADCE2CC_TypeDef *)0x3FF0814C)
#define ADC1E2CC ((ADCE2CC_TypeDef *)0x3FF0818C)

typedef volatile struct
{
    uint32_t ADCED : 12;
    uint32_t ADCEIS : 4;
    uint32_t ADCEES : 4;
    uint32_t ADCESDLY : 4;
    uint32_t ADCEPL : 3;
    uint32_t ADCEST : 3;
    uint32_t ADCECCLR : 1;
    uint32_t ADCECF : 1;
} ADCE3CC_TypeDef; // 0x8150
#define ADC0E3CC ((ADCE3CC_TypeDef *)0x3FF08150)
#define ADC1E3CC ((ADCE3CC_TypeDef *)0x3FF08190)

typedef volatile struct
{
    uint32_t ADCED : 12;
    uint32_t ADCEIS : 4;
    uint32_t ADCEES : 4;
    uint32_t ADCESDLY : 4;
    uint32_t ADCEPL : 3;
    uint32_t ADCEST : 3;
    uint32_t ADCECCLR : 1;
    uint32_t ADCECF : 1;
} ADCE4CC_TypeDef; // 0x8154
#define ADC0E4CC ((ADCE4CC_TypeDef *)0x3FF08154)
#define ADC1E4CC ((ADCE4CC_TypeDef *)0x3FF08194)

typedef volatile struct
{
    uint32_t ADCED : 12;
    uint32_t ADCEIS : 4;
    uint32_t ADCEES : 4;
    uint32_t ADCESDLY : 4;
    uint32_t ADCEPL : 3;
    uint32_t ADCEST : 3;
    uint32_t ADCECCLR : 1;
    uint32_t ADCECF : 1;
} ADCE5CC_TypeDef; // 0x8158
#define ADC0E5CC ((ADCE5CC_TypeDef *)0x3FF08158)
#define ADC1E5CC ((ADCE5CC_TypeDef *)0x3FF08198)

typedef volatile struct
{
    uint32_t ADCED : 12;
    uint32_t ADCEIS : 4;
    uint32_t ADCEES : 4;
    uint32_t ADCESDLY : 4;
    uint32_t ADCEPL : 3;
    uint32_t ADCEST : 3;
    uint32_t ADCECCLR : 1;
    uint32_t ADCECF : 1;
} ADCE6CC_TypeDef; // 0x815C
#define ADC0E6CC ((ADCE6CC_TypeDef *)0x3FF0815C)
#define ADC1E6CC ((ADCE6CC_TypeDef *)0x3FF0819C)

typedef volatile struct
{
    uint32_t ADCED : 12;
    uint32_t ADCEIS : 4;
    uint32_t ADCEES : 4;
    uint32_t ADCESDLY : 4;
    uint32_t ADCEPL : 3;
    uint32_t ADCEST : 3;
    uint32_t ADCECCLR : 1;
    uint32_t ADCECF : 1;
} ADCE7CC_TypeDef; // 0x8160
#define ADC0E7CC ((ADCE7CC_TypeDef *)0x3FF08160)
#define ADC1E7CC ((ADCE7CC_TypeDef *)0x3FF081A0)

#define ADC0DA (*(volatile uint32_t *)0x3FF08164)
#define ADC1DA (*(volatile uint32_t *)0x3FF081A4)

typedef volatile struct
{
    uint32_t ADCE0DC : 7;
    uint32_t ADCE0DMAE : 1;
    uint32_t ADCE1DC : 7;
    uint32_t ADCE1DMAE : 1;
    uint32_t ADCE2DC : 7;
    uint32_t ADCE2DMAE : 1;
    uint32_t ADCE3DC : 7;
    uint32_t ADCE3DMAE : 1;
} ADCDC0_TypeDef; // 0x8168
#define ADC0DC0 ((ADCDC0_TypeDef *)0x3FF08168)
#define ADC1DC0 ((ADCDC0_TypeDef *)0x3FF081A8)

typedef volatile struct
{
    uint32_t ADCE4DC : 7;
    uint32_t ADCE4DMAE : 1;
    uint32_t ADCE5DC : 7;
    uint32_t ADCE5DMAE : 1;
    uint32_t ADCE6DC : 7;
    uint32_t ADCE6DMAE : 1;
    uint32_t ADCE7DC : 7;
    uint32_t ADCE7DMAE : 1;
} ADCDC1_TypeDef; // 0x816C
#define ADC0DC1 ((ADCDC1_TypeDef *)0x3FF0816C)
#define ADC1DC1 ((ADCDC1_TypeDef *)0x3FF081AC)

typedef volatile struct
{
    uint32_t ADCW0E0E : 1;
    uint32_t ADCW0E1E : 1;
    uint32_t ADCW0E2E : 1;
    uint32_t ADCW0E3E : 1;
    uint32_t ADCW0E4E : 1;
    uint32_t ADCW0E5E : 1;
    uint32_t ADCW0E6E : 1;
    uint32_t ADCW0E7E : 1;
    uint32_t ADCE0OV : 1;
    uint32_t ADCE1OV : 1;
    uint32_t ADCE2OV : 1;
    uint32_t ADCE3OV : 1;
    uint32_t ADCE4OV : 1;
    uint32_t ADCE5OV : 1;
    uint32_t ADCE6OV : 1;
    uint32_t ADCE7OV : 1;
    uint32_t ADCW0E0CLR : 1;
    uint32_t ADCW0E0F : 1;
    uint32_t ADCW0E1CLR : 1;
    uint32_t ADCW0E1F : 1;
    uint32_t ADCW0E2CLR : 1;
    uint32_t ADCW0E2F : 1;
    uint32_t ADCW0E3CLR : 1;
    uint32_t ADCW0E3F : 1;
    uint32_t ADCW0E4CLR : 1;
    uint32_t ADCW0E4F : 1;
    uint32_t ADCW0E5CLR : 1;
    uint32_t ADCW0E5F : 1;
    uint32_t ADCW0E6CLR : 1;
    uint32_t ADCW0E6F : 1;
    uint32_t ADCW0E7CLR : 1;
    uint32_t ADCW0E7F : 1;
} ADCWC0_TypeDef; // 0x8170
#define ADC0WC0 ((ADCWC0_TypeDef *)0x3FF08170)
#define ADC1WC0 ((ADCWC0_TypeDef *)0x3FF081B0)

typedef volatile struct
{
    uint32_t ADCWW0T : 12;
    uint32_t ADCE0IB : 1;
    uint32_t ADCE1IB : 1;
    uint32_t ADCE2IB : 1;
    uint32_t ADCE3IB : 1;
    uint32_t ADCWW0B : 12;
    uint32_t ADCE4IB : 1;
    uint32_t ADCE5IB : 1;
    uint32_t ADCE6IB : 1;
    uint32_t ADCE7IB : 1;
} ADCWW0_TypeDef; // 0x8174
#define ADC0WW0 ((ADCWW0_TypeDef *)0x3FF08174)
#define ADC1WW0 ((ADCWW0_TypeDef *)0x3FF081B4)

typedef volatile struct
{
    uint32_t ADCW1E0E : 1;
    uint32_t ADCW1E1E : 1;
    uint32_t ADCW1E2E : 1;
    uint32_t ADCW1E3E : 1;
    uint32_t ADCW1E4E : 1;
    uint32_t ADCW1E5E : 1;
    uint32_t ADCW1E6E : 1;
    uint32_t ADCW1E7E : 1;
    uint32_t ADCE0IE : 1;
    uint32_t ADCE1IE : 1;
    uint32_t ADCE2IE : 1;
    uint32_t ADCE3IE : 1;
    uint32_t ADCE4IE : 1;
    uint32_t ADCE5IE : 1;
    uint32_t ADCE6IE : 1;
    uint32_t ADCE7IE : 1;
    uint32_t ADCW1E0CLR : 1;
    uint32_t ADCW1E0F : 1;
    uint32_t ADCW1E1CLR : 1;
    uint32_t ADCW1E1F : 1;
    uint32_t ADCW1E2CLR : 1;
    uint32_t ADCW1E2F : 1;
    uint32_t ADCW1E3CLR : 1;
    uint32_t ADCW1E3F : 1;
    uint32_t ADCW1E4CLR : 1;
    uint32_t ADCW1E4F : 1;
    uint32_t ADCW1E5CLR : 1;
    uint32_t ADCW1E5F : 1;
    uint32_t ADCW1E6CLR : 1;
    uint32_t ADCW1E6F : 1;
    uint32_t ADCW1E7CLR : 1;
    uint32_t ADCW1E7F : 1;
} ADCWC1_TypeDef; // 0x8178
#define ADC0WC1 ((ADCWC1_TypeDef *)0x3FF08178)
#define ADC1WC1 ((ADCWC1_TypeDef *)0x3FF081B8)

typedef volatile struct
{
    uint32_t ADCWW1T : 12;
    uint32_t  : 4;
    uint32_t ADCWW1B : 12;
    uint32_t  : 2;
    uint32_t ADCRC : 2;
} ADCWW1_TypeDef; // 0x817C
#define ADC0WW1 ((ADCWW1_TypeDef *)0x3FF0817C)
#define ADC1WW1 ((ADCWW1_TypeDef *)0x3FF081BC)

// ---- CCP (0x3FF081C0) ----
typedef volatile struct
{
    uint32_t CCPE : 1;
    uint32_t CCPUT : 1;
    uint32_t CCPUM : 2;
    uint32_t CCPM : 2;
    uint32_t CCPCIC : 2;
    uint32_t CCPCI : 3;
    uint32_t CCPVFE : 1;
    uint32_t CCPT : 3;
    uint32_t CCPIE : 1;
    uint32_t CCPFC0I : 2;
    uint32_t CCPFC1I : 2;
    uint32_t CCPFC0L : 1;
    uint32_t CCPFC1L : 1;
    uint32_t CCPFC0E : 1;
    uint32_t CCPFC1E : 1;
    uint32_t CCPFC : 1;
    uint32_t CCPCOV : 1;
    uint32_t CCPFCLR : 1;
    uint32_t CCPFF : 1;
    uint32_t CCPGCLR : 1;
    uint32_t CCPGF : 1;
    uint32_t CCPCCLR : 1;
    uint32_t CCPCF : 1;
} CCPCON_TypeDef; // 0x81C0
#define CCP0CON ((CCPCON_TypeDef *)0x3FF081C0)
#define CCP1CON ((CCPCON_TypeDef *)0x3FF081D0)
#define CCP2CON ((CCPCON_TypeDef *)0x3FF081E0)
#define CCP3CON ((CCPCON_TypeDef *)0x3FF081F0)
#define CCP4CON ((CCPCON_TypeDef *)0x3FF08200)
#define CCP5CON ((CCPCON_TypeDef *)0x3FF08210)

typedef volatile struct
{
    uint32_t CCPCV : 16;
    uint32_t CCPPSC : 8;
    uint32_t CCPDT : 8;
} CCPVAL_TypeDef; // 0x81C4
#define CCP0VAL ((CCPVAL_TypeDef *)0x3FF081C4)
#define CCP1VAL ((CCPVAL_TypeDef *)0x3FF081D4)
#define CCP2VAL ((CCPVAL_TypeDef *)0x3FF081E4)
#define CCP3VAL ((CCPVAL_TypeDef *)0x3FF081F4)
#define CCP4VAL ((CCPVAL_TypeDef *)0x3FF08204)
#define CCP5VAL ((CCPVAL_TypeDef *)0x3FF08214)

typedef volatile struct
{
    uint32_t CCPPOE : 1;
    uint32_t CCPNOE : 1;
    uint32_t CCPFPO : 1;
    uint32_t CCPFNO : 1;
    uint32_t CCPPIO : 1;
    uint32_t CCPNIO : 1;
    uint32_t CCPROC : 2;
    uint32_t CCPGM : 2;
    uint32_t CCPGUE : 1;
    uint32_t CCPGDE : 1;
    uint32_t CCPGINV : 1;
    uint32_t CCPPFM : 1;
    uint32_t  : 2;
    uint32_t CCPDCD : 3;
    uint32_t CCPSBR : 1;
    uint32_t CCPCOL : 2;
    uint32_t CCPGFM : 1;
    uint32_t  : 9;
} CCPOUT_TypeDef; // 0x81C8
#define CCP0OUT ((CCPOUT_TypeDef *)0x3FF081C8)
#define CCP1OUT ((CCPOUT_TypeDef *)0x3FF081D8)
#define CCP2OUT ((CCPOUT_TypeDef *)0x3FF081E8)
#define CCP3OUT ((CCPOUT_TypeDef *)0x3FF081F8)
#define CCP4OUT ((CCPOUT_TypeDef *)0x3FF08208)
#define CCP5OUT ((CCPOUT_TypeDef *)0x3FF08218)

typedef volatile struct
{
    uint32_t CCPGSV : 16;
    uint32_t CCPGEV : 16;
} CCPGAT_TypeDef; // 0x81CC
#define CCP0GAT ((CCPGAT_TypeDef *)0x3FF081CC)
#define CCP1GAT ((CCPGAT_TypeDef *)0x3FF081DC)
#define CCP2GAT ((CCPGAT_TypeDef *)0x3FF081EC)
#define CCP3GAT ((CCPGAT_TypeDef *)0x3FF081FC)
#define CCP4GAT ((CCPGAT_TypeDef *)0x3FF0820C)
#define CCP5GAT ((CCPGAT_TypeDef *)0x3FF0821C)


typedef volatile struct
{
    CCPCON_TypeDef CCPCON;
    CCPVAL_TypeDef CCPVAL;
    CCPOUT_TypeDef CCPOUT;
    CCPGAT_TypeDef CCPGAT;
} MODULE_CCP_TypeDef;

#define CCP_0 ((MODULE_CCP_TypeDef *)0x3FF081C0)
#define CCP_1 ((MODULE_CCP_TypeDef *)0x3FF081D0)
#define CCP_2 ((MODULE_CCP_TypeDef *)0x3FF081E0)
#define CCP_3 ((MODULE_CCP_TypeDef *)0x3FF081F0)
#define CCP_4 ((MODULE_CCP_TypeDef *)0x3FF08200)
#define CCP_5 ((MODULE_CCP_TypeDef *)0x3FF08210)


// ---- MPWM (0x3FF08220) ----
typedef volatile struct
{
    uint32_t MPW0E : 1;
    uint32_t MPW1E : 1;
    uint32_t MPW2E : 1;
    uint32_t MPW3E : 1;
    uint32_t MPW4E : 1;
    uint32_t MPW5E : 1;
    uint32_t MPW6E : 1;
    uint32_t MPW7E : 1;
    uint32_t MPT : 3;
    uint32_t MPCO : 1;
    uint32_t MPLSM : 1;
    uint32_t MPLSKS : 1;
    uint32_t MPIE : 1;
    uint32_t  : 1;
    uint32_t MPW0CLR : 1;
    uint32_t MPW0F : 1;
    uint32_t MPW1CLR : 1;
    uint32_t MPW1F : 1;
    uint32_t MPW2CLR : 1;
    uint32_t MPW2F : 1;
    uint32_t MPW3CLR : 1;
    uint32_t MPW3F : 1;
    uint32_t MPW4CLR : 1;
    uint32_t MPW4F : 1;
    uint32_t MPW5CLR : 1;
    uint32_t MPW5F : 1;
    uint32_t MPW6CLR : 1;
    uint32_t MPW6F : 1;
    uint32_t MPW7CLR : 1;
    uint32_t MPW7F : 1;
} MPCON_TypeDef; // 0x8220
#define MP0CON ((MPCON_TypeDef *)0x3FF08220)
#define MP1CON ((MPCON_TypeDef *)0x3FF0823C)
#define MP2CON ((MPCON_TypeDef *)0x3FF08258)

typedef volatile struct
{
    uint32_t MPW0DTY : 16;
    uint32_t MPW1DTY : 16;
} MPDTY01_TypeDef; // 0x8224
#define MP0DTY01 ((MPDTY01_TypeDef *)0x3FF08224)
#define MP1DTY01 ((MPDTY01_TypeDef *)0x3FF08240)
#define MP2DTY01 ((MPDTY01_TypeDef *)0x3FF0825C)

typedef volatile struct
{
    uint32_t MPW2DTY : 16;
    uint32_t MPW3DTY : 16;
} MPDTY23_TypeDef; // 0x8228
#define MP0DTY23 ((MPDTY23_TypeDef *)0x3FF08228)
#define MP1DTY23 ((MPDTY23_TypeDef *)0x3FF08244)
#define MP2DTY23 ((MPDTY23_TypeDef *)0x3FF08260)

typedef volatile struct
{
    uint32_t MPW4DTY : 16;
    uint32_t MPW5DTY : 16;
} MPDTY45_TypeDef; // 0x822C
#define MP0DTY45 ((MPDTY45_TypeDef *)0x3FF0822C)
#define MP1DTY45 ((MPDTY45_TypeDef *)0x3FF08248)
#define MP2DTY45 ((MPDTY45_TypeDef *)0x3FF08264)

typedef volatile struct
{
    uint32_t MPW6DTY : 16;
    uint32_t MPW7DTY : 16;
} MPDTY67_TypeDef; // 0x8230
#define MP0DTY67 ((MPDTY67_TypeDef *)0x3FF08230)
#define MP1DTY67 ((MPDTY67_TypeDef *)0x3FF0824C)
#define MP2DTY67 ((MPDTY67_TypeDef *)0x3FF08268)

#define MP0LDA (*(volatile uint32_t *)0x3FF08234)
#define MP1LDA (*(volatile uint32_t *)0x3FF08250)
#define MP2LDA (*(volatile uint32_t *)0x3FF0826C)

#define MP0LDC (*(volatile uint32_t *)0x3FF08238)
#define MP1LDC (*(volatile uint32_t *)0x3FF08254)
#define MP2LDC (*(volatile uint32_t *)0x3FF08270)


// ---- Watchdog (0x3FF08278) ----
typedef volatile struct
{
    uint32_t WDTE : 1;
    uint32_t WDTRE : 1;
    uint32_t WDTWE : 1;
    uint32_t WDTIE : 1;
    uint32_t WDTCD : 3;
    uint32_t  : 1;
    uint32_t WDTCLR : 8;
    uint32_t WDTRLV : 8;
    uint32_t  : 7;
    uint32_t WDTTO : 1;
} WDTCON_TypeDef; // 0x8278
#define WDT0CON ((WDTCON_TypeDef *)0x3FF08278)
#define WDT1CON ((WDTCON_TypeDef *)0x3FF0827C)

typedef volatile struct
{
    WDTCON_TypeDef WDTCON;
} MODULE_WDT_TypeDef;

#define WDT_0 ((MODULE_WDT_TypeDef *)0x3FF08278)
#define WDT_1 ((MODULE_WDT_TypeDef *)0x3FF0827C)


// ---- SPI (0x3FF08280 / 0x3FF082A0) ----
typedef volatile struct
{
    uint32_t SPIE : 1;
    uint32_t SPIKS : 1;
    uint32_t SPICIH : 1;
    uint32_t SPIDSP : 1;
    uint32_t SPIMCD : 4;
    uint32_t SPIS : 1;
    uint32_t SPIHD : 1;
    uint32_t SPIHDR : 1;
    uint32_t SPILF : 1;
    uint32_t SPIDMAE : 1;
    uint32_t SPIMSOE : 1;
    uint32_t SPISIOP : 1;
    uint32_t SPIPORTS : 1;
    uint32_t SPIIE : 1;
    uint32_t  : 13;
    uint32_t SPITCLR : 1;
    uint32_t SPITBV : 1;
} SPICON_TypeDef; // 0x8280
#define SPI0CON ((SPICON_TypeDef *)0x3FF08280)
#define SPI1CON ((SPICON_TypeDef *)0x3FF082A0)

#define SPI0TBUF (*(volatile uint32_t *)0x3FF08284)
#define SPI0RBUF (*(volatile uint32_t *)0x3FF08288)
#define SPI0TDA  (*(volatile uint32_t *)0x3FF0828C)
#define SPI0RDA  (*(volatile uint32_t *)0x3FF08290)
#define SPI0TBC  (*(volatile uint32_t *)0x3FF08294)
#define SPI1TBUF (*(volatile uint32_t *)0x3FF082A4)
#define SPI1RBUF (*(volatile uint32_t *)0x3FF082A8)
#define SPI1TDA  (*(volatile uint32_t *)0x3FF082AC)
#define SPI1RDA  (*(volatile uint32_t *)0x3FF082B0)
#define SPI1TBC  (*(volatile uint32_t *)0x3FF082B4)

// ---- UART (0x3FF082C0) ----
typedef volatile struct
{
    uint32_t UTE : 1;
    uint32_t UTTKS : 1;
    uint32_t UTM : 2;
    uint32_t UTBRA : 3;
    uint32_t UTRE : 1;
    uint32_t UTBR : 12;
    uint32_t UTHD : 1;
    uint32_t UTTDMAE : 1;
    uint32_t UTRDMAE : 1;
    uint32_t UTPORTS : 1;
    uint32_t UTIE : 1;
    uint32_t UTPAR : 2;
    uint32_t UTRERR : 1;
    uint32_t UTTCLR : 1;
    uint32_t UTTBV : 1;
    uint32_t UTRCLR : 1;
    uint32_t UTRBV : 1;
} UTCON_TypeDef; // 0x82C0
#define UT0CON ((UTCON_TypeDef *)0x3FF082C0)
#define UT1CON ((UTCON_TypeDef *)0x3FF082E0)

#define UT0TBUF  (*(volatile uint32_t *)0x3FF082C4)
#define UT0RBUF  (*(volatile uint32_t *)0x3FF082C8)
#define UT0TDA   (*(volatile uint32_t *)0x3FF082CC)
#define UT0RDA   (*(volatile uint32_t *)0x3FF082D0)
#define UT0TBC   (*(volatile uint32_t *)0x3FF082D4)
#define UT0RBC   (*(volatile uint32_t *)0x3FF082D8)
#define UT1TBUF  (*(volatile uint32_t *)0x3FF082E4)
#define UT1RBUF  (*(volatile uint32_t *)0x3FF082E8)
#define UT1TDA   (*(volatile uint32_t *)0x3FF082EC)
#define UT1RDA   (*(volatile uint32_t *)0x3FF082F0)
#define UT1TBC   (*(volatile uint32_t *)0x3FF082F4)
#define UT1RBC   (*(volatile uint32_t *)0x3FF082F8)


typedef volatile struct
{
    UTCON_TypeDef UTCON;
    uint32_t UTTBUF;
    uint32_t UTRBUF;
    uint32_t UTTDA;
    uint32_t UTRDA;
    uint32_t UTTBC;
    uint32_t UTRBC;
} MODULE_UART_TypeDef;

#define UART_0 ((MODULE_UART_TypeDef *)0x3FF082C0)
#define UART_1 ((MODULE_UART_TypeDef *)0x3FF082E0)


// ---- Analog
typedef volatile struct
{
    uint32_t ABUFIS : 5;
    uint32_t ABUFE : 1;
    uint32_t ABUFM : 2;
    uint32_t ABUFOT : 4;
    uint32_t  : 4;
    uint32_t ADCTM : 8;
    uint32_t  : 8;
} ABUFCON_TypeDef; // 0x8274
#define ABUFCON ((ABUFCON_TypeDef *)0x3FF08274)

typedef volatile struct
{
    uint32_t CMPTWS : 16;
    uint32_t CMPTWE : 16;
} CMPTW_TypeDef; // 0x82B8
#define CMP0TW ((CMPTW_TypeDef *)0x3FF082B8)
#define CMP1TW ((CMPTW_TypeDef *)0x3FF082BC)
#define CMP2TW ((CMPTW_TypeDef *)0x3FF082DC)

typedef volatile struct
{
    uint32_t CMPS : 1;
    uint32_t CMPE : 1;
    uint32_t CMPHS : 2;
    uint32_t CMPFS : 2;
    uint32_t CMPRFE : 1;
    uint32_t CMPFFE : 1;
    uint32_t CMPIS : 3;
    uint32_t CMPOE : 1;
    uint32_t CMPRS : 3;
    uint32_t CMPIE : 1;
    uint32_t CMPOT : 4;
    uint32_t CMPTM : 3;
    uint32_t  : 7;
    uint32_t CMPECLR : 1;
    uint32_t CMPEF : 1;
} CMPCON_TypeDef; // 0x8318
#define CMP0CON ((CMPCON_TypeDef *)0x3FF08318)
#define CMP1CON ((CMPCON_TypeDef *)0x3FF0831C)
#define CMP2CON ((CMPCON_TypeDef *)0x3FF083DC)

typedef volatile struct
{
    uint32_t AMP0GC : 3;
    uint32_t AMP0E : 1;
    uint32_t AMP0OT : 4;
    uint32_t AMP1GC : 3;
    uint32_t AMP1E : 1;
    uint32_t AMP1OT : 4;
    uint32_t AMP2GC : 3;
    uint32_t AMP2E : 1;
    uint32_t AMP2OT : 4;
    uint32_t AMP3GC : 3;
    uint32_t AMP3E : 1;
    uint32_t AMP3OT : 4;
} AMPCON_TypeDef; // 0x833C
#define AMPCON ((AMPCON_TypeDef *)0x3FF0833C)


// ---- IIC (0x3FF08300) ----
typedef volatile struct
{
    uint32_t IICE : 1;
    uint32_t IICKS : 1;
    uint32_t IICS : 1;
    uint32_t IICTX : 1;
    uint32_t IICTXC : 2;
    uint32_t IICHE : 1;
    uint32_t IICSAM : 1;
    uint32_t IICMCD : 8;
    uint32_t IICSCL : 1;
    uint32_t IICDMAE : 1;
    uint32_t IICPORTS : 1;
    uint32_t IICIE : 1;
    uint32_t IICBSTA : 1;
    uint32_t IICSLE : 1;
    uint32_t IICBSTART : 1;
    uint32_t IICBSTOP : 1;
    uint32_t IICCCLR : 1;
    uint32_t IICCLTO : 1;
    uint32_t IICHCLR : 1;
    uint32_t IICHNACK : 1;
    uint32_t IICTCLR : 1;
    uint32_t IICTBV : 1;
    uint32_t IICRCLR : 1;
    uint32_t IICRBV : 1;
} IICCON_TypeDef; // 0x8300
#define IICCON ((IICCON_TypeDef *)0x3FF08300)

#define IICTBUF  (*(volatile uint32_t *)0x3FF08304)
#define IICRBUF  (*(volatile uint32_t *)0x3FF08308)
#define IICADR   (*(volatile uint32_t *)0x3FF0830C)
#define IICDA    (*(volatile uint32_t *)0x3FF08310)
#define IICBC    (*(volatile uint32_t *)0x3FF08314)


// ---- CRC (0x3FF08320) ----
typedef volatile struct
{
    uint32_t CRCREGL : 5;
    uint32_t CRCREFI : 1;
    uint32_t CRCREFO : 1;
    uint32_t CRCCPLO : 1;
    uint32_t CRCIDM : 2;
    uint32_t CRCPDC : 2;
    uint32_t CRCKS : 1;
    uint32_t CRCIE : 1;
    uint32_t CRCRVB : 1;
    uint32_t  : 18;
    uint32_t CRCCCLR : 1;
    uint32_t CRCCF : 1;
} CRCCON_TypeDef; // 0x8320
#define CRCCON ((CRCCON_TypeDef *)0x3FF08320)

#define CRCPOLY  (*(volatile uint32_t *)0x3FF08324)
#define CRCIN    (*(volatile uint32_t *)0x3FF08328)
#define CRCREG   (*(volatile uint32_t *)0x3FF0832C)
#define CRCOUT   (*(volatile uint32_t *)0x3FF08330)
#define CRCDA    (*(volatile uint32_t *)0x3FF08334)
#define CRCBC    (*(volatile uint32_t *)0x3FF08338)


// ---- Timer (0x3FF08340 / 0x3FF08350 / ... / 0x3FF083B0) ----
typedef volatile struct
{
    uint32_t TE : 1;
    uint32_t TUT : 1;
    uint32_t TUM : 2;
    uint32_t TCM : 2;
    uint32_t TECC : 2;
    uint32_t TCS : 3;
    uint32_t TOIE : 1;
    uint32_t TERS : 4;
    uint32_t TERC : 2;
    uint32_t TRS : 1;
    uint32_t TRGE : 1;
    uint32_t TENC0 : 2;
    uint32_t TENC1 : 2;
    uint32_t TENCM : 2;
    uint32_t TENCP : 1;
    uint32_t TOFM : 2;
    uint32_t TCD : 1;
    uint32_t TOCLR : 1;
    uint32_t TOV : 1;
} TCON_TypeDef; // 0x8340
#define T0CON ((TCON_TypeDef *)0x3FF08340)
#define T1CON ((TCON_TypeDef *)0x3FF08350)
#define T2CON ((TCON_TypeDef *)0x3FF08360)
#define T3CON ((TCON_TypeDef *)0x3FF08370)
#define T4CON ((TCON_TypeDef *)0x3FF08380)
#define T5CON ((TCON_TypeDef *)0x3FF08390)
#define T6CON ((TCON_TypeDef *)0x3FF083A0)
#define T7CON ((TCON_TypeDef *)0x3FF083B0)

#define T0CNT    (*(volatile uint32_t *)0x3FF08344)
#define T1CNT    (*(volatile uint32_t *)0x3FF08354)
#define T2CNT    (*(volatile uint32_t *)0x3FF08364)
#define T3CNT    (*(volatile uint32_t *)0x3FF08374)
#define T4CNT    (*(volatile uint32_t *)0x3FF08384)
#define T5CNT    (*(volatile uint32_t *)0x3FF08394)
#define T6CNT    (*(volatile uint32_t *)0x3FF083A4)
#define T7CNT    (*(volatile uint32_t *)0x3FF083B4)


#define T0ARL (*(volatile uint32_t *)0x3FF08348)
#define T1ARL (*(volatile uint32_t *)0x3FF08358)
#define T2ARL (*(volatile uint32_t *)0x3FF08368)
#define T3ARL (*(volatile uint32_t *)0x3FF08378)
#define T4ARL (*(volatile uint32_t *)0x3FF08388)
#define T5ARL (*(volatile uint32_t *)0x3FF08398)
#define T6ARL (*(volatile uint32_t *)0x3FF083A8)
#define T7ARL (*(volatile uint32_t *)0x3FF083B8)

typedef volatile struct
{
    uint32_t TPEC : 8;
    uint32_t TETS : 4;
    uint32_t TETC : 1;
    uint32_t TET : 1;
    uint32_t TACT : 2;
    uint32_t TEPS : 3;
    uint32_t TEPC : 1;
    uint32_t  : 1; /* uint32_t TPS : 1; BUG in hardware, thus we disable it */
    uint32_t TPGE : 1;
    uint32_t TCSC : 2;
    uint32_t TSBR : 1;
    uint32_t TEIE : 1;
    uint32_t  : 4;
    uint32_t TECLR : 1;
    uint32_t TEF : 1;
} TECON_TypeDef; // 0x834C
#define T0ECON ((TECON_TypeDef *)0x3FF0834C)
#define T1ECON ((TECON_TypeDef *)0x3FF0835C)
#define T2ECON ((TECON_TypeDef *)0x3FF0836C)
#define T3ECON ((TECON_TypeDef *)0x3FF0837C)
#define T4ECON ((TECON_TypeDef *)0x3FF0838C)
#define T5ECON ((TECON_TypeDef *)0x3FF0839C)
#define T6ECON ((TECON_TypeDef *)0x3FF083AC)
#define T7ECON ((TECON_TypeDef *)0x3FF083BC)


typedef volatile struct
{
    TCON_TypeDef TCON;
    uint32_t TCNT;
    uint32_t TARL;
    TECON_TypeDef TECON;
} MODULE_TIMER_TypeDef;

#define TIMER_0 ((MODULE_TIMER_TypeDef *)0x3FF08340)
#define TIMER_1 ((MODULE_TIMER_TypeDef *)0x3FF08350)
#define TIMER_2 ((MODULE_TIMER_TypeDef *)0x3FF08360)
#define TIMER_3 ((MODULE_TIMER_TypeDef *)0x3FF08370)
#define TIMER_4 ((MODULE_TIMER_TypeDef *)0x3FF08380)
#define TIMER_5 ((MODULE_TIMER_TypeDef *)0x3FF08390)
#define TIMER_6 ((MODULE_TIMER_TypeDef *)0x3FF083A0)
#define TIMER_7 ((MODULE_TIMER_TypeDef *)0x3FF083B0)


// ---- USB (0x3FF083C0) ----
typedef volatile struct
{
    uint32_t UE : 1;
    uint32_t USRSM : 1;
    uint32_t UFS : 1;
    uint32_t  : 1;
    uint32_t ULT : 1;
    uint32_t  : 1;
    uint32_t UDPPU : 1;
    uint32_t UDNPU : 1;
    uint32_t UDADR : 7;
    uint32_t  : 1;
    uint32_t UXM : 1;
    uint32_t UIE : 1;
    uint32_t URSBF : 1;
    uint32_t  : 8;
    uint32_t UBRSM : 1;
    uint32_t  : 1;
    uint32_t UBSPD : 1;
    uint32_t UBCLR : 1;
    uint32_t UBRST : 1;
} UBCON_TypeDef; // 0x83C0
#define UBCON ((UBCON_TypeDef *)0x3FF083C0)

typedef volatile struct
{
    uint32_t UFNUM : 11;
    uint32_t UFCRC : 5;
    uint32_t USOFE : 1;
    uint32_t  : 12;
    uint32_t UMISS : 1;
    uint32_t USCLR : 1;
    uint32_t USOF : 1;
} UFCON_TypeDef; // 0x83C4
#define UFCON ((UFCON_TypeDef *)0x3FF083C4)

typedef volatile struct
{
    uint32_t UEOPS : 8;
    uint32_t UEIPS : 8;
    uint32_t UE0DOS : 1;
    uint32_t UE0DIS : 1;
    uint32_t UE0SOS : 1;
    uint32_t UE0SIS : 1;
    uint32_t UE0STALL : 1;
    uint32_t ULTS : 1;
    uint32_t UECERRH : 1;
    uint32_t UECTP : 1;
    uint32_t UENCLR : 1;
    uint32_t UEINACK : 1;
    uint32_t UECCLR : 1;
    uint32_t UECERR : 1;
    uint32_t UEOCLR : 1;
    uint32_t UEOPF : 1;
    uint32_t UEICLR : 1;
    uint32_t UEIPF : 1;
} UE0CON_TypeDef; // 0x83C8
#define UE0CON ((UE0CON_TypeDef *)0x3FF083C8)

typedef volatile struct
{
    uint32_t UEOPS : 8;
    uint32_t UEIPS : 8;
    uint32_t UEOS : 1;
    uint32_t UEIS : 1;
    uint32_t UEODATA0 : 1;
    uint32_t UEIDATA0 : 1;
    uint32_t UEOSTALL : 1;
    uint32_t UEISTALL : 1;
    uint32_t UECERRH : 1;
    uint32_t UENOHS : 1;
    uint32_t UENCLR : 1;
    uint32_t UEINACK : 1;
    uint32_t UECCLR : 1;
    uint32_t UECERR : 1;
    uint32_t UEOCLR : 1;
    uint32_t UEOPF : 1;
    uint32_t UEICLR : 1;
    uint32_t UEIPF : 1;
} UEXCON_TypeDef; // 0x83CC
#define UE1CON ((UEXCON_TypeDef *)0x3FF083CC)
#define UE2CON ((UEXCON_TypeDef *)0x3FF083D0)
#define UE3CON ((UEXCON_TypeDef *)0x3FF083D4)
#define UE4CON ((UEXCON_TypeDef *)0x3FF083D8)

// ---- DAC (0x3FF083E0 / 0x3FF083F0) ----
typedef volatile struct
{
    uint32_t DACCCD : 16;
    uint32_t DACE : 1;
    uint32_t DACIE : 1;
    uint32_t DACDMAE : 1;
    uint32_t DACDMAL : 1;
    uint32_t DACCS : 1;
    uint32_t DACIM : 1;
    uint32_t DACTE : 1;
    uint32_t  : 7;
    uint32_t DACCCLR : 1;
    uint32_t DACCF : 1;
} DACCON_TypeDef; // 0x83E0
#define DAC0CON ((DACCON_TypeDef *)0x3FF083E0)
#define DAC1CON ((DACCON_TypeDef *)0x3FF083F0)

#define DAC0BUF  (*(volatile uint32_t *)0x3FF083E4)
#define DAC0DA   (*(volatile uint32_t *)0x3FF083E8)
#define DAC0DC   (*(volatile uint32_t *)0x3FF083EC)
#define DAC1BUF  (*(volatile uint32_t *)0x3FF083F4)
#define DAC1DA   (*(volatile uint32_t *)0x3FF083F8)
#define DAC1DC   (*(volatile uint32_t *)0x3FF083FC)

/*
 * 统一寄存器读写宏 (GCC 内建)
 * 兼容两种寄存器定义风格:
 *   指针型:   #define HRF     ((HRF_TypeDef *)0x3FF08000)
 *   解引用型: #define DAC0BUF (*(volatile uint32_t *)0x3FF083E4)
 *
 * 用法:
 *   uint32_t val = READ_SR(HRF);          // 指针型
 *   uint32_t val = READ_SR(DAC0BUF);      // 解引用型
 *   WRITE_SR(HRF, 0x1234);                // 指针型
 *   WRITE_SR(DAC0BUF, 0x1234);            // 解引用型
 */
#define READ_SR(REG) _Generic((REG), \
    uint32_t: (REG), \
    default: *(volatile uint32_t *)(REG) \
)

#define WRITE_SR(REG, VAL) \
    _Generic((REG), \
        uint32_t: (REG), \
        default: *(volatile uint32_t *)(REG) \
    ) = (VAL)

#endif // CV32S6015MSR_H
