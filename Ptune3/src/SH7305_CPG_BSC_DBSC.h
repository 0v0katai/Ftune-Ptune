/*---------------------------------------------------------------------------------------------

    SH7305 CPG&BSC&DBSC Register.

    for fx-9860GII-2, fx-CG10/20/50   v2.10
    
    copyright(c)2014,2015,2017,2019 by sentaro21
    e-mail sentaro21@pm.matrix.jp

-------------------------------------------------------------------------------
CPG(Clock Pulse Generator)                               quote: SH7724 datasheet
-------------------------------------------------------------------------------
    FLL: FLL circuit Multiplication Ratio.
         The FLL circuit multiples the clock frequency(32.768KHz) input from the RTC_CLK.
         default  multiplication rate is 900.
         FLL circuit output  32768Hz*900/2=14.7456MHz
         this is same as old SH3 model oscillatory frequency.

    PLL: PLL circuit Multiplication Ratio.
         base frequency 14.7456MHz at FLL output
         Multiplication Ratio is 1-64
         It becomes half speed when about over 800MHz.
         (PLL Clock is up to about 750MHz)

    IFC: CPU Clock Division Ratio.
    SFC: SHW Clock Division Ratio.
    BFC: bus Clock Division Ratio.
    PFC: I/O Clock Division Ratio.
        Division Ratio  (not same to  SH7724)
        0000: 1/2
        0001: 1/4   IFC dafault
        0010: 1/8   SFC,BFC default
        0011: 1/16  PFC default
        0100: 1/32
        0101: 1/64
    Note.
    IFC >= SFC >= BFC >= PFC
    IFC:SFC  only  1:1 1:2
    this tool adjust automatically.


-------------------------------------------------------------------------------
BSC(Bus State Controller)                                quote: SH7724 datasheet
-------------------------------------------------------------------------------
    register structure for the BCR/WCR similar to SH7724.

    CS0BCR, CS0WCR  : FLASH ROM area
    CS2BCR, CS2WCR  : main  RAM area
    CS5ABCR,CS5AWCR : LCDC
    others unknown.


    CSn Space Bus Contorl Register (CSnBCR) (n=0,2,3,4,5A,5B,6A,6B)

    IWW: Specification for Idol Cycles between Write-Read/Write-Write Cycles.
        000: no idol cycle
        001: 1 idol cycle inserted
        010: 2 idol cycles inserted
        011: 4 idol cycles inserted
        100: 6 idol cycles inserted
        101: 8 idol cycles inserted
        110: 10 idol cycles inserted
        111: 12idol cycles inserted

        (CS0BCR,CS2BCR)@SH7305 default value 2 idol cycles.
        lower frequency can modify more lower setteing.
        it work well effectively.

    IWRWD: Specification for Idol Cycles between Read-Write Cycles in Different Spaces.
        000: no idol cycle
        001: 1 idol cycle inserted
        010: 2 idol cycles inserted
        011: 4 idol cycles inserted
        100: 6 idol cycles inserted
        101: 8 idol cycles inserted
        110: 10 idol cycles inserted
        111: 12idol cycles inserted

        (CS0BCR,CS2BCR)@SH7305 default value 2 idol cycles.
        lower frequency can modify more lower setteing.
        it work well. but an effect is not felt.

    IWRWS: Specification for Idol Cycles between Read-Write Cycles in the Same Spaces.
        000: no idol cycle
        001: 1 idol cycle inserted
        010: 2 idol cycles inserted
        011: 4 idol cycles inserted
        100: 6 idol cycles inserted
        101: 8 idol cycles inserted
        110: 10 idol cycles inserted
        111: 12idol cycles inserted

        (CS0BCR,CS2BCR)@SH7305 default value 2 idol cycles.
        lower frequency can modify more lower setteing.
        it work well. but an effect is not felt.

    IWRRD: Specification for Idol Cycles between Read-Read Cycles in Different Spaces.
        000: no idol cycle
        001: 1 idol cycle inserted
        010: 2 idol cycles inserted
        011: 4 idol cycles inserted
        100: 6 idol cycles inserted
        101: 8 idol cycles inserted
        110: 10 idol cycles inserted
        111: 12idol cycles inserted

        (CS0BCR,CS2BCR)@SH7305 default value 2 idol cycles.
        lower frequency can modify more lower setteing.
        it work well. but an effect is not felt.

    IWRRS:  Specification for Idol Cycles between Read-Read Cycles in the Same Spaces.
        000: no idol cycle
        001: 1 idol cycle inserted
        010: 2 idol cycles inserted
        011: 4 idol cycles inserted
        100: 6 idol cycles inserted
        101: 8 idol cycles inserted
        110: 10 idol cycles inserted
        111: 12idol cycles inserted

        (CS0BCR,CS2BCR)@SH7305 default value 2 idol cycles.
        lower frequency can modify more lower setteing.
        it work well effectively.



    CSn Space Wait Contorl Register (CSnWCR) (n=0,2,3,4,5A,5B,6A,6B)

    WW: Number of Wait Cycles in Write Access
        000: The same cycles as WR settings
        001: 0 cycle
        010: 1 cycle
        011: 2 cycles
        100: 3 cycles
        101: 4 cycles
        110: 5 cycles
        111: 6 cycles

        (CS0WCR,CS2WCR)@SH7305 default value is the same cycles as WR settings.
        I think that it is not necessary to change it.
        but,CS5AWCR work well effectively. (When abnormality happens to LCD)

    WR: Number of Wait Cycles in Read Access
        0000: 0 cycle       1000: 10 cycles
        0001: 1 cycle       1001: 12 cycles
        0010: 2 cycle       1010: 14 cycles
        0011: 3 cycles      1011: 18 cycles
        0100: 4 cycles      1100: 24 cycles
        0101: 5 cycles
        0110: 6 cycles
        0111: 8 cycles

        CS0WCR@SH7305 default value is 3 cycles
        if increment it, and more bus frequency.

        CS2WCR@SH7305 default value is 2 cycles
        if increment it, and more bus frequency.


    SW: Numer of Delay Cycles from Address,CSn Assertion to RD,WE Assertion.
        00: 0.5cycle
        01: 1.5cycles
        10: 2.5cycles
        11: 3.5cycles
        (CS0WCR,CS2WCR)@SH7305 default value is 0.5cycle.


    HW: Delay Cycles from RD WEn Negation to Address,CSn Negation.
        00: 0.5cycle
        01: 1.5cycles
        10: 2.5cycles
        11: 3.5cycles
        (CS0WCR,CS2WCR)@SH7305 default value is 0.5cycle.

---------------------------------------------------------------------------------------------*/

struct st_cpg {
    union {                           // struct FRQCRA   similar to SH7724
        unsigned long LONG;           // long Word Access
        struct {                      // Bit  Access
            unsigned long KICK :1;    //    KICK      
            unsigned long      :1;    //              
            unsigned long STC  :6;    //    STC       
            unsigned long IFC  :4;    //    IFC       
            unsigned long      :4;    //              
            unsigned long SFC  :4;    //    SFC       
            unsigned long BFC  :4;    //    BFC       
            unsigned long      :4;    //              
            unsigned long PFC  :4;    //    PFC       
            }   BIT;
        }   FRQCRA;

    unsigned long FRQCRB;       // 0xA41500004
    unsigned long FCLKACR;      // 0xA41500008
    unsigned long FCLKBCR;      // 0xA4150000C
    unsigned long unknownA41500010;
    unsigned long unknownA41500014;
    unsigned long IRDACLKCR;    // 0xA41500018
    unsigned long unknownA4150001C;
    unsigned long unknownA41500020;
    unsigned long PLLCR;        // 0xA41500024
    unsigned long unknownA41500028;
    unsigned long unknownA4150002C;
    unsigned long unknownA41500030;
    unsigned long unknownA41500034;
    unsigned long unknownA41500038;
    unsigned long SPUCLKCR;     // 0xA4150003C
    unsigned long unknownA41500040;
    unsigned long unknownA41500044;
    unsigned long VCLKCR;       // 0xA41500048
    unsigned long unknownA4150004C;
        
    union {                           // struct FLLFRQ   similar to SH7724
        unsigned long LONG;           // long Word Access
        struct {                      // Bit  Access
            unsigned long      :16;   //              
            unsigned long SELXM:2;    //   SELXM      
            unsigned long      :3;    //              
            unsigned long FLF  :11;   //    FLF       
            }   BIT;
        }   FLLFRQ;
        
    unsigned long unknownA41500054;
    unsigned long unknownA41500058;
    unsigned long unknownA4150005C;
    unsigned long LSTATS;
};

struct st_bsc {
    unsigned long CMNCR;
    union {                           // struct CS0BCR   similar to SH7724
        unsigned long LONG;           // long Word Access
        struct {                      // Bit  Access
            unsigned long      :1;    //              
            unsigned long IWW  :3;    //   IWW        
            unsigned long IWRWD:3;    //   IWRWD      
            unsigned long IWRWS:3;    //   IWRWS      
            unsigned long IWRRD:3;    //   IWRRD      
            unsigned long IWRRS:3;    //              
            unsigned long TYPE :4;    //   TYPE       
            unsigned long      :1;    //              
            unsigned long BSZ  :2;    //   BSZ        
            unsigned long      :9;    //              
            }   BIT;
        }   CS0BCR;
    union {                           // struct CS2BCR 
        unsigned long LONG;           // long Word Access
        struct {                      // Bit  Access    similar to SH7724 SRAM
            unsigned long      :1;    //             
            unsigned long IWW  :3;    //   IWW        
            unsigned long IWRWD:3;    //   IWRWD      
            unsigned long IWRWS:3;    //   IWRWS      
            unsigned long IWRRD:3;    //   IWRRD      
            unsigned long IWRRS:3;    //              
            unsigned long TYPE :4;    //   TYPE       
            unsigned long      :1;    //              
            unsigned long BSZ  :2;    //   BSZ        
            unsigned long      :9;    //              
            }   BIT;
        }   CS2BCR;
    union {                           // struct CS3BCR
        unsigned long LONG;           // long Word Access
        struct {                      // Bit  Access    similar to SH7724 SRAM
            unsigned long      :1;    //              
            unsigned long IWW  :3;    //   IWW        
            unsigned long IWRWD:3;    //   IWRWD      
            unsigned long IWRWS:3;    //   IWRWS      
            unsigned long IWRRD:3;    //   IWRRD      
            unsigned long IWRRS:3;    //              
            unsigned long TYPE :4;    //   TYPE       
            unsigned long      :1;    //              
            unsigned long BSZ  :2;    //   BSZ        
            unsigned long      :9;    //              
            }   BIT;
        }   CS3BCR;
    union {                           // struct CS4BCR   similar to SH7724
        unsigned long LONG;           // long Word Access
        struct {                      // Bit  Access
            unsigned long      :1;    //              
            unsigned long IWW  :3;    //   IWW        
            unsigned long IWRWD:3;    //   IWRWD      
            unsigned long IWRWS:3;    //   IWRWS      
            unsigned long IWRRD:3;    //   IWRRD      
            unsigned long IWRRS:3;    //              
            unsigned long TYPE :4;    //   TYPE       
            unsigned long      :1;    //              
            unsigned long BSZ  :2;    //   BSZ        
            unsigned long      :9;    //              
            }   BIT;
        }   CS4BCR;
    union {                           // struct CS5ABCR   similar to SH7724
        unsigned long LONG;           // long Word Access
        struct {                      // Bit  Access
            unsigned long      :1;    //              
            unsigned long IWW  :3;    //   IWW        
            unsigned long IWRWD:3;    //   IWRWD      
            unsigned long IWRWS:3;    //   IWRWS      
            unsigned long IWRRD:3;    //   IWRRD      
            unsigned long IWRRS:3;    //              
            unsigned long TYPE :4;    //   TYPE       
            unsigned long      :1;    //              
            unsigned long BSZ  :2;    //   BSZ        
            unsigned long      :9;    //              
            }   BIT;
        }   CS5ABCR;
    union {                           // struct CS5BBCR   similar to SH7724
        unsigned long LONG;           // long Word Access
        struct {                      // Bit  Access
            unsigned long      :1;    //              
            unsigned long IWW  :3;    //   IWW        
            unsigned long IWRWD:3;    //   IWRWD      
            unsigned long IWRWS:3;    //   IWRWS      
            unsigned long IWRRD:3;    //   IWRRD      
            unsigned long IWRRS:3;    //              
            unsigned long TYPE :4;    //   TYPE       
            unsigned long      :1;    //              
            unsigned long BSZ  :2;    //   BSZ        
            unsigned long      :9;    //              
            }   BIT;
        }   CS5BBCR;
    union {                           // struct CS6ABCR   similar to SH7724
        unsigned long LONG;           // long Word Access
        struct {                      // Bit  Access
            unsigned long      :1;    //              
            unsigned long IWW  :3;    //   IWW        
            unsigned long IWRWD:3;    //   IWRWD      
            unsigned long IWRWS:3;    //   IWRWS      
            unsigned long IWRRD:3;    //   IWRRD      
            unsigned long IWRRS:3;    //              
            unsigned long TYPE :4;    //   TYPE       
            unsigned long      :1;    //              
            unsigned long BSZ  :2;    //   BSZ        
            unsigned long      :9;    //              
            }   BIT;
        }   CS6ABCR;
    union {                           // struct CS6BBCR   similar to SH7724
        unsigned long LONG;           // long Word Access
        struct {                      // Bit  Access
            unsigned long      :1;    //              
            unsigned long IWW  :3;    //   IWW        
            unsigned long IWRWD:3;    //   IWRWD      
            unsigned long IWRWS:3;    //   IWRWS      
            unsigned long IWRRD:3;    //   IWRRD      
            unsigned long IWRRS:3;    //              
            unsigned long TYPE :4;    //   TYPE       
            unsigned long      :1;    //              
            unsigned long BSZ  :2;    //   BSZ        
            unsigned long      :9;    //              
            }   BIT;
        }   CS6BBCR;
        
    union {                           // struct CS0WCR   similar to SH7724
        unsigned long LONG;           // long Word Access
        struct {                      // Bit  Access
            unsigned long      :11;   //              
            unsigned long BAS  :1;    //   BAS        
            unsigned long      :1;    //              
            unsigned long WW   :3;    //    WW  
            unsigned long ADRS :1;    //    ADRSFIX   
            unsigned long      :2;    //              
            unsigned long SW   :2;    //    SW        
            unsigned long WR   :4;    //    WR  
            unsigned long WM   :1;    //    WM        
            unsigned long      :4;    //              
            unsigned long HW   :2;    //    HW        
            }   BIT;
        }   CS0WCR;
    union {                           // struct CS2WCR   similar to SH7724
        unsigned long LONG;           // long Word Access
        struct {                      // Bit  Access
            unsigned long      :11;   //              
            unsigned long BAS  :1;    //   BAS        
            unsigned long      :1;    //              
            unsigned long WW   :3;    //    WW  
            unsigned long ADRS :1;    //    ADRSFIX   
            unsigned long      :2;    //              
            unsigned long SW   :2;    //    SW        
            unsigned long WR   :4;    //    WR  
            unsigned long WM   :1;    //    WM        
            unsigned long      :4;    //              
            unsigned long HW   :2;    //    HW        
            }   BIT;
//        struct {                      // Bit  Access    similar to SH7730 SDRAM
//            unsigned long      :23;   //              
//            unsigned long A2CL :2;    //   A2CL       
//            unsigned long      :7;    //              
//            }   BIT;
        }   CS2WCR;
    union {                           // struct CS3WCR   similar to SH7724
        unsigned long LONG;           // long Word Access
//        struct {                      // Bit  Access
//            unsigned long      :11;   //              
//            unsigned long BAS  :1;    //   BAS        
//            unsigned long      :1;    //              
//            unsigned long WW   :3;    //    WW  
//            unsigned long ADRS :1;    //    ADRSFIX   
//            unsigned long      :2;    //              
//            unsigned long SW   :2;    //    SW        
//            unsigned long WR   :4;    //    WR  
//            unsigned long WM   :1;    //    WM        
//            unsigned long      :4;    //              
//            unsigned long HW   :2;    //    HW        
//            }   BIT;
        struct {                      // Bit  Access    similar to SH7730 SDRAM
            unsigned long      :17;   //              
            unsigned long TRP  :2;    //   TRP        
            unsigned long      :1;    //              
            unsigned long TRCD :2;    //   TRCD       
            unsigned long      :1;    //              
            unsigned long A3CL :2;    //   A3CL       
            unsigned long      :2;    //              
            unsigned long TRWL :2;    //   TRCD       
            unsigned long      :1;    //              
            unsigned long TRC  :2;    //   TRC       
            }   BIT;
        }   CS3WCR;
    union {                           // struct CS4WCR   similar to SH7724
        unsigned long LONG;           // long Word Access
        struct {                      // Bit  Access
            unsigned long      :11;   //              
            unsigned long BAS  :1;    //   BAS        
            unsigned long      :1;    //              
            unsigned long WW   :3;    //    WW  
            unsigned long ADRS :1;    //    ADRSFIX   
            unsigned long      :2;    //              
            unsigned long SW   :2;    //    SW        
            unsigned long WR   :4;    //    WR  
            unsigned long WM   :1;    //    WM        
            unsigned long      :4;    //              
            unsigned long HW   :2;    //    HW        
            }   BIT;
        }   CS4WCR;
    union {                           // struct CS5AWCR   similar to SH7724
        unsigned long LONG;           // long Word Access
        struct {                      // Bit  Access
            unsigned long      :11;   //              
            unsigned long BAS  :1;    //   BAS        
            unsigned long      :1;    //              
            unsigned long WW   :3;    //    WW  
            unsigned long ADRS :1;    //    ADRSFIX   
            unsigned long      :2;    //              
            unsigned long SW   :2;    //    SW        
            unsigned long WR   :4;    //    WR  
            unsigned long WM   :1;    //    WM        
            unsigned long      :4;    //              
            unsigned long HW   :2;    //    HW        
            }   BIT;
        }   CS5AWCR;
    union {                           // struct CS5BWCR   similar to SH7724
        unsigned long LONG;           // long Word Access
        struct {                      // Bit  Access
            unsigned long      :11;   //              
            unsigned long BAS  :1;    //   BAS        
            unsigned long      :1;    //              
            unsigned long WW   :3;    //    WW  
            unsigned long ADRS :1;    //    ADRSFIX   
            unsigned long      :2;    //              
            unsigned long SW   :2;    //    SW        
            unsigned long WR   :4;    //    WR  
            unsigned long WM   :1;    //    WM        
            unsigned long      :4;    //              
            unsigned long HW   :2;    //    HW        
            }   BIT;
        }   CS5BWCR;
    union {                           // struct CS6AWCR   similar to SH7724
        unsigned long LONG;           // long Word Access
        struct {                      // Bit  Access
            unsigned long      :11;   //              
            unsigned long BAS  :1;    //   BAS        
            unsigned long      :1;    //              
            unsigned long WW   :3;    //    WW  
            unsigned long ADRS :1;    //    ADRSFIX   
            unsigned long      :2;    //              
            unsigned long SW   :2;    //    SW        
            unsigned long WR   :4;    //    WR  
            unsigned long WM   :1;    //    WM        
            unsigned long      :4;    //              
            unsigned long HW   :2;    //    HW        
            }   BIT;
        }   CS6AWCR;
    union {                           // struct CS6BWCR   similar to SH7724
        unsigned long LONG;           // long Word Access
        struct {                      // Bit  Access
            unsigned long      :11;   //              
            unsigned long BAS  :1;    //   BAS        
            unsigned long      :1;    //              
            unsigned long WW   :3;    //    WW  
            unsigned long ADRS :1;    //    ADRSFIX   
            unsigned long      :2;    //              
            unsigned long SW   :2;    //    SW        
            unsigned long WR   :4;    //    WR  
            unsigned long WM   :1;    //    WM        
            unsigned long      :4;    //              
            unsigned long HW   :2;    //    HW        
            }   BIT;
        }   CS6BWCR;
    union {                           // struct SDCR   similar to SH7730
        unsigned long LONG;           // long Word Access
        struct {                      // Bit  Access
            unsigned long      :11;   //              
            unsigned long A2ROW:2;    //    A2ROW     
            unsigned long      :1;    //              
            unsigned long A2COL:2;    //    A2COL     
            unsigned long      :4;    //              
            unsigned long RFRS :1;    //    RFRS      
            unsigned long RMODE:1;    //    RMODE     
            unsigned long PDOWN:1;    //    PDOWN     
            unsigned long BACTV:1;    //    BACTV     
            unsigned long      :3;    //              
            unsigned long A3ROW:2;    //    A3ROW     
            unsigned long      :1;    //              
            unsigned long A3COL:2;    //    A3COL     
            }   BIT;
        }   SDCR;
    
};


struct st_dbsc {
    union {                           // struct DBKIND   similar to SH7724
        unsigned long LONG;           // long Word Access
        struct {                      // Bit  Access
            unsigned long      :29;   //              
            unsigned long DDCG :3;    //   DDCG       
            }   BIT;
        }   DBKIND;
    union {                           // struct DBSTATE   similar to SH7724
        unsigned long LONG;           // long Word Access
        struct {                      // Bit  Access
            unsigned long      :23;   //              
            unsigned long ENDN :1;    //   ENDN       
            unsigned long      :8;    //              
            }   BIT;
        }   DBSTATE;
    union {                           // struct DBEN   similar to SH7724
        unsigned long LONG;           // long Word Access
        struct {                      // Bit  Access
            unsigned long      :31;   //              
            unsigned long ACEN :1;    //   ACEN       
            }   BIT;
        }   DBEN;
    union {                           // struct DBCMDCNT   similar to SH7724
        unsigned long LONG;           // long Word Access
        struct {                      // Bit  Access
            unsigned long      :29;   //              
            unsigned long CMD  :3;    //   CMD        
            }   BIT;
        }   DBCMDCNT;
    union {                           // struct DBCKECNT   similar to SH7724
        unsigned long LONG;           // long Word Access
        struct {                      // Bit  Access
            unsigned long      :31;   //              
            unsigned long CKEEN:1;    //   CKEEN      
            }   BIT;
        }   DBCKECNT;
    unsigned long unknownFD00001C;
    union {                           // struct DBCONF   similar to SH7724
        unsigned long LONG;           // long Word Access
        struct {                      // Bit  Access
            unsigned long      :7;    //              
            unsigned long SPLIT:9;    //   SPLIT      
            unsigned long BKADM:2;    //   BKADM      
            unsigned long BKADP:6;    //   BKADP      
            unsigned long BKADB:6;    //   BKADB      
            unsigned long BWIDTH:2;   //   BWIDTH     
            }   BIT;
        }   DBCONF;
    unsigned long unknownFD000024;
    unsigned long unknownFD000028;
    unsigned long unknownFD00002C;
    union {                           // struct DBTR0   similar to SH7724
        unsigned long LONG;           // long Word Access
        struct {                      // Bit  Access
            unsigned long      :5;    //              
            unsigned long CL   :3;    //   CL         
            unsigned long      :3;    //              
            unsigned long TRAS :5;    //   TRAS       
            unsigned long      :1;    //              
            unsigned long TRFC :7;    //   TRFC       
            unsigned long      :5;    //              
            unsigned long TRCD :3;    //   TRCD       
            }   BIT;
        }   DBTR0;
    union {                           // struct DBTR1   similar to SH7724
        unsigned long LONG;           // long Word Access
        struct {                      // Bit  Access
            unsigned long      :5;    //              
            unsigned long CKEH :3;    //   CKEH       
            unsigned long      :5;    //              
            unsigned long TRP  :3;    //   TRP        
            unsigned long      :5;    //              
            unsigned long TRRD :3;    //   TRRD       
            unsigned long      :5;    //              
            unsigned long TWR  :3;    //   TWR        
            }   BIT;
        }   DBTR1;
        
    union {                           // struct DBTR2   similar to SH7724
        unsigned long LONG;           // long Word Access
        struct {                      // Bit  Access
            unsigned long      :6;    //              
            unsigned long TRTP :2;    //   TRTP       
            unsigned long      :3;    //              
            unsigned long TRC  :5;    //   TRC        
            unsigned long      :4;    //              
            unsigned long RDWR :4;    //   RDWR       
            unsigned long      :4;    //              
            unsigned long WRRD :4;    //   WRRD       
            }   BIT;
        }   DBTR2;
    union {                           // struct DBTR3   similar to SH7724
        unsigned long LONG;           // long Word Access
        struct {                      // Bit  Access
            unsigned long      :29;   //              
            unsigned long ODTL :3;    //   ODTL       
            }   BIT;
        }   DBTR3;
    union {                           // struct DBRFPDN0   similar to SH7724
        unsigned long LONG;           // long Word Access
        struct {                      // Bit  Access
            unsigned long      :15;   //              
            unsigned long ARFEN:1;    //   ARFEN      
            unsigned long      :13;   //              
            unsigned long PDN  :1;    //   PDN        
            unsigned long DPDN :1;    //   DPDN       
            unsigned long SRFEN:1;    //   SREFN      
            }   BIT;
        }   DBRFPDN0;
    union {                           // struct DBRFPDN1   similar to SH7724
        unsigned long LONG;           // long Word Access
        struct {                      // Bit  Access
            unsigned long      :19;   //              
            unsigned long TREFI:13;   //   TREFI      
            }   BIT;
        }   DBRFPDN1;
    union {                           // struct DBRFPDN2   similar to SH7724
        unsigned long LONG;           // long Word Access
        struct {                      // Bit  Access
            unsigned long      :1;    //              
            unsigned long LV1TH:15;   //   LV1TH      
            unsigned long      :6;    //              
            unsigned long LV0TH:10;   //   LV0TH      
            }   BIT;
        }   DBRFPDN2;
    union {                           // struct DBRFSTS   similar to SH7724
        unsigned long LONG;           // long Word Access
        struct {                      // Bit  Access
            unsigned long      :31;   //              
            unsigned long RDUDF:1;    //   RFUDF      
            }   BIT;
        }   DBRFSTS;
    unsigned long unknownFD000050;
    unsigned long unknownFD000054;
    unsigned long unknownFD000058;
    unsigned long unknownFD00005C;
    union {                           // struct DBMRCNT   similar to SH7724
        unsigned long LONG;           // long Word Access
        struct {                      // Bit  Access
            unsigned long      :13;   //              
            unsigned long BA   :3;    //   BA         
            unsigned long      :1;    //              
            unsigned long MA   :15;   //   MA         
            }   BIT;
        }   DBMRCNT;
    unsigned long unknownFD000064;
    unsigned long unknownFD000068;
    unsigned long unknownFD00006C;
    unsigned long unknownFD000070;
    unsigned long unknownFD000074;
    unsigned long unknownFD000078;
    unsigned long unknownFD00007C;
    unsigned long unknownFD000080;
    unsigned long unknownFD000084;
    unsigned long unknownFD000088;
    unsigned long unknownFD00008C;
    unsigned long unknownFD000090;
    unsigned long unknownFD000094;
    unsigned long unknownFD000098;
    unsigned long unknownFD00009C;
    unsigned long unknownFD0000A0;
    unsigned long unknownFD0000A4;
    unsigned long unknownFD0000A8;
    unsigned long unknownFD0000AC;
    unsigned long unknownFD0000B0;
    unsigned long unknownFD0000B4;
    unsigned long unknownFD0000B8;
    unsigned long unknownFD0000BC;
    unsigned long unknownFD0000C0;
    unsigned long unknownFD0000C4;
    unsigned long unknownFD0000C8;
    unsigned long unknownFD0000CC;
    unsigned long unknownFD0000D0;
    unsigned long unknownFD0000D4;
    unsigned long unknownFD0000D8;
    unsigned long unknownFD0000DC;
    unsigned long unknownFD0000E0;
    unsigned long unknownFD0000E4;
    unsigned long unknownFD0000E8;
    unsigned long unknownFD0000EC;
    unsigned long unknownFD0000F0;
    unsigned long unknownFD0000F4;
    unsigned long unknownFD0000F8;
    unsigned long unknownFD0000FC;
    unsigned long unknownFD000100;
    unsigned long unknownFD000104;
    union {                           // struct DBPDCNT0   similar to SH7724
        unsigned long LONG;           // long Word Access
        struct {                      // Bit  Access
            unsigned long OFFSET:2;   //   OFFSET     
            unsigned long      :14;   //              
            unsigned long      :6;    //              
            unsigned long DQS  :1;    //   DQS        
            unsigned long ODTEN:1;    //   ODTEN      
            unsigned long ODT  :1;    //   ODT        
            unsigned long      :6;    //              
            unsigned long PLUP :1;    //   PLUP       
            }   BIT;
        }   DBPDCNT0;
    
};

#define CPG     (*(volatile struct st_cpg  *)0xA4150000)    // FRQCRA    Address
#define BSC     (*(volatile struct st_bsc  *)0xFEC10000)    // CMNCR     Address
#define DBSC    (*(volatile struct st_dbsc *)0xFD000008)    // DBKIND    Address

#define SDMR3_CL2    *(volatile short *)0xFEC15040    // SDMR2    Address
#define SDMR3_CL3    *(volatile short *)0xFEC15060    // SDMR2    Address

#define SSCGCR	(*(volatile unsigned int *)0xA4150044)    // SSCGCR (Spread spectrum control) Address 
