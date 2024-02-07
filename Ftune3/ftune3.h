/*
===============================================================================

 Ftune3 is SH7305 CPG&BSC tuning utility for fx-9860GII-2      v2.00

 copyright(c)2014,2015,2016,2017,2018,2019 by sentaro21
 e-mail sentaro21@pm.matrix.jp

===============================================================================
*/

#include	"SH7305_CPG_BSC.h"

//---------------------------------------------------------------------------------------------

#define FLF_default  900
#define FLFMAX      2048-1

#define PLL_64x 63		//0b111111 
#define PLL_36x 35		//0b100011 
#define PLL_32x 31		//0b011111 
#define PLL_26x 25		//0b011001 
#define PLL_16x 15		//0b001111 
#define PLLMAX  64-1

#define DIV_2 0 		//0b0000	// 1/2
#define DIV_4 1 		//0b0001	// 1/4
#define DIV_8 2 		//0b0010	// 1/8
#define DIV16 3 		//0b0011	// 1/16
#define DIV32 4 		//0b0100	// 1/32
#define DIV64 5 		//0b0101	// 1/64

#define WAIT_0 0 		//0b0000
#define WAIT_1 1 		//0b0001
#define WAIT_2 2 		//0b0010	// cs2wcr default 
#define WAIT_3 3 		//0b0011
#define WAIT_4 4 		//0b0100
#define WAIT_5 5 		//0b0101
#define WAIT_6 6 		//0b0110
#define WAIT_8 7 		//0b0111
#define WAIT10 8 		//0b1000
#define WAIT12 9 		//0b1001
#define WAIT14 10		//0b1010
#define WAIT18 11		//0b1011	// cs0wcr default 
#define WAIT24 12		//0b1100

#define WAITW0 1		//0b0001
#define WAITW1 2		//0b0010
#define WAITW2 3		//0b0011
#define WAITW3 4		//0b0100
#define WAITW4 5		//0b0101
#define WAITW5 6		//0b0110
#define WAITW6 7		//0b0111

#define	Max_PLL		99900
#define	Max_IFC		35000
#define	Max_SFC		25000
#define	Max_BFC		25000
#define	Max_PFC		 4500

#define	Max_PLLdef	80000
#define	Max_IFCdef	27500
#define	Max_SFCdef	18000
#define	Max_BFCdef	13400
#define	Max_PFCdef	 3000

#define FLLFRQ_default  0x00004384
#define FRQCRA_default  0x0F202203

#define CS0BCR_default  0x24920400
#define CS2BCR_default  0x24923400
#define CS3BCR_default  0x24924400
#define CS4BCR_default  0x36DB0400
#define CS5aBCR_default 0x224A0200
#define CS5bBCR_default 0x224A0200
#define CS6aBCR_default 0x34D30200
#define CS6bBCR_default 0x34D30200

#define CS0WCR_default  0x000005C0
#define CS2WCR_default  0x00000140
#define CS3WCR_default  0x000024D0
#define CS4WCR_default  0x00000540
#define CS5aWCR_default 0x00000D41
#define CS5bWCR_default 0x00000D41
#define CS6aWCR_default 0x000302C0
#define CS6bWCR_default 0x000302C0

#define FLLFRQ_default2  0x00004384		// 35+EII
#define FRQCRA_default2  0x0F112213		// 35+EII

#define CS0BCR_default2  0x24920400		// 35+EII
#define CS2BCR_default2  0x24923400		// 35+EII
#define CS3BCR_default2  0x24924400		// 35+EII
#define CS4BCR_default2  0x36DB0400		// 35+EII
#define CS5aBCR_default2 0x224A0200		// 35+EII
#define CS5bBCR_default2 0x224A0200		// 35+EII
#define CS6aBCR_default2 0x34D30200		// 35+EII
#define CS6bBCR_default2 0x34D30200		// 35+EII

#define CS0WCR_default2  0x000001C0		// 35+EII
#define CS2WCR_default2  0x000001C0		// 35+EII
#define CS3WCR_default2  0x000024D0		// 35+EII
#define CS4WCR_default2  0x00000540		// 35+EII
#define CS5aWCR_default2 0x00031340		// 35+EII
#define CS5bWCR_default2 0x00000D41		// 35+EII
#define CS6aWCR_default2 0x000302C0		// 35+EII
#define CS6bWCR_default2 0x000302C0		// 35+EII

//	precision 10KHz   ex. 14.7456MHz => 1474
//	flash max speed			-margin									mx0%	mx0%USB				35+EII
#define	MaxROMwait_0 	 2300-400			//MX29LV320EBTI-70G		 2452	2540		16x+24	21.58
#define	MaxROMwait_1 	 3600-600			//(S29JL032H70TFI02)	 4084	4227				35.96
#define	MaxROMwait_2 	 5200-600			//						 5704	5901				50.06
#define	MaxROMwait_3 	 6600-600			//						 7335	7585				64.29
#define	MaxROMwait_4 	 7800-600			//						 8968	9271				78.30
#define	MaxROMwait_5 	 9200-800			//						10589  10944				92.61
#define	MaxROMwait_6 	10800-800			//						12198  12623				107.04
#define	MaxROMwait_8 	13000-1000			//						15456  15982				135.51
#define	MaxROMwait10 	17500-2500			//						18686  19433				163.83
#define	MaxROMwait12 	21500-4500			//						21088  21746				192.16
#define	MaxROMwait14 	24800-4800			//						
#define	MaxROMwait18 	27800-7800			//						

//	SRAM max speed			-margin									mx0%	mx0%USB
#define	MaxRAMwait_0 	 2400-360			// 60ns? SRAM			 2479	2532		24x+24	25.05
#define	MaxRAMwait_1 	 4900-740			//						 4959	5059				49.69
#define	MaxRAMwait_2 	 7400-740			//		default			 7417	7585				73.97
#define	MaxRAMwait_3 	 9900-1000			//						 9893  10101				98.64
#define	MaxRAMwait_4 	11400-1240			//						12372  12637				112.19
#define	MaxRAMwait_5 	13900-1490			//						14851  15155				132.12
#define	MaxRAMwait_6 	16300-1730			//						17304  17699				152.66
#define	MaxRAMwait_8 	20000-3000			//						21316  21510				205.16
#define	MaxRAMwait10 	23000-3000			//						

#define	MaxRAMwaitW_0 	 5000-500			//						 5358						55.33
#define	MaxRAMwaitW_1 	10000-1000			//						10719						102.60
#define	MaxRAMwaitW_2 	14000-2000			//						16120						128.86
#define	MaxRAMwaitW_3 	20000-5000			//						21040						171.09
#define	MaxRAMwaitW_4 	20000-5000			//						21176						187.99
#define	MaxRAMwaitW_5 	20000-5000			//						21221						187.99
#define	MaxRAMwaitW_6 	20000-5000			//						21243						187.99

#define ROMmarginDefault	3	// ROM Wait margin default
#define RAMmarginDefault	3	// RAM Wait margin default

//---------------------------------------------------------------------------------------------
#define ROMbase (unsigned int*)0xA0000000	// RAM top address (uncacheable area)
#define RAMbase (unsigned int*)0xA8000000	// RAM top address (uncacheable area)

int MainBatteryPercentage();

