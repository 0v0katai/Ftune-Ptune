/*
===============================================================================

 Ptune3 is SH7305 CPG&BSC tuning utility for PRIZM fx-CG50/Graph 90+E  v0.24

 copyright(c)2014,2015,2016,2017 by sentaro21
 e-mail sentaro21@pm.matrix.jp

===============================================================================
*/
#define VERSION "v0.24"

#include	"SH7305_CPG_BSC_DBSC.h"

//---------------------------------------------------------------------------------------------

#define FLF_default  900
#define FLFMAX      2048-1

#define PLL_64x 0b111111 //
#define PLL_36x 0b100011 //
#define PLL_32x 0b011111 //
#define PLL_26x 0b011001 //
#define PLL_16x 0b001111 // default
#define PLLMAX  32-1

#define DIV_2 0b0000	// 1/2
#define DIV_4 0b0001	// 1/4
#define DIV_8 0b0010	// 1/8
#define DIV16 0b0011	// 1/16
#define DIV32 0b0100	// 1/32
#define DIV64 0b0101	// 1/64

#define WAIT_0 0b0000
#define WAIT_1 0b0001
#define WAIT_2 0b0010	// cs2wcr default
#define WAIT_3 0b0011	// cs0wcr default
#define WAIT_4 0b0100
#define WAIT_5 0b0101
#define WAIT_6 0b0110
#define WAIT_8 0b0111
#define WAIT10 0b1000
#define WAIT12 0b1001
#define WAIT14 0b1010
#define WAIT18 0b1011
#define WAIT24 0b1100

#define WAITW0 0b0001
#define WAITW1 0b0010
#define WAITW2 0b0011
#define WAITW3 0b0100
#define WAITW4 0b0101
#define WAITW5 0b0110
#define WAITW6 0b0111

#define	Max_PLL		99900
#define	Max_IFC		35000
#define	Max_SFC		20000
#define	Max_BFC		20000
#define	Max_PFC		 6000

#define	Max_PLLdef	80000
#define	Max_IFCdef	27500
#define	Max_SFCdef	15000
#define	Max_BFCdef	10000
#define	Max_PFCdef	 5000

#define FLLFRQ_default	0x00004384
//#define FRQCR_default	0x0F102203	// CG10/20
#define FRQCR_default	0x0F011112	// CG50

//#define CS0BCR_default	0x24920400	// CG10/20
#define CS0BCR_default	0x36DA0400	// CG50
#define CS2BCR_default	0x36DA3400	// CG50
#define CS3BCR_default  0x36DB4400	// CG50
#define CS4BCR_default  0x36DB0400
#define CS5aBCR_default 0x17DF0400
#define CS5bBCR_default 0x17DF0400
#define CS6aBCR_default 0x34D30200
#define CS6bBCR_default 0x34D30200

//#define CS0WCR_default	0x000001C0	// CG10/20
#define CS0WCR_default	0x000003C0	// CG50
#define CS2WCR_default	0x000003C0	// CG50
#define CS3WCR_default  0x000024D1	// CG50
#define CS4WCR_default  0x00000540
#define CS5aWCR_default 0x000203C1
#define CS5bWCR_default 0x000203C1
#define CS6aWCR_default 0x000302C0
#define CS6bWCR_default 0x000302C0
#define SDCR_default    0x00000A08


//	precision 10KHz   ex. 14.7456MHz => 1474
//	flash max speed				-margin		//		11x+16			spa0%	spa0%+USB
#define	MaxROMwait_0 	 1600-200		//spanision 100ns			1648	1700
#define	MaxROMwait_1 	 2700-300		//					2756	2747	2835
#define	MaxROMwait_2 	 3800-200		//			*IWW0	3854	3818	3941
#define	MaxROMwait_3 	 4900-250  		//	dafault	4891	4944	4911	5067
#define	MaxROMwait_4 	 6000-300		//					6049	5998	6194
#define	MaxROMwait_5 	 7100-350		//					7141	7100	7333
#define	MaxROMwait_6 	 8200-400		//			*IWW1	8227	8185	8464
#define	MaxROMwait_8 	10600-500		//						   10349   10696
#define	MaxROMwait10 	12500-600		//						   12547   12951
#define	MaxROMwait12 	14600-700		//			*IWW2		   14733   15210
#define	MaxROMwait14 	16600-800		//
#define	MaxROMwait18 	20000-900		//
/*
//	flash max speed				-margin				12x+18			mx0%	spa0%USB
#define	MaxROMwait_0 	 1800-180		//	MX 90ns					1822	1852
#define	MaxROMwait_1 	 3000-300		//							3041	3084
#define	MaxROMwait_2 	 4200-210		//							4255	4326
#define	MaxROMwait_3 	 5400-270		//	dafault	*IWW0			5466	5557
#define	MaxROMwait_4 	 6600-330		//							6683	6792
#define	MaxROMwait_5 	 7800-390		//							7899	8036
#define	MaxROMwait_6 	 9000-450		//			*IWW1			9136	9271
#define	MaxROMwait_8 	11400-570		//						   11547   11724
#define	MaxROMwait10 	13800-690		//						   13932   14216
#define	MaxROMwait12 	16200-760		//			*IWW2		   16389   16696
#define	MaxROMwait14 	18600-880		//
#define	MaxROMwait18 	23000-990		//
*/

//	SRAM max speed				-margin				19x+19			spa0%	spa0%USB	mx0%	mx0%USB
#define	MaxRAMwait_0 	 1900-400		// 80ns?					1912	1970		2004	2043
#define	MaxRAMwait_1 	 3800-800		//							3802	3916		4005	4082
#define	MaxRAMwait_2 	 5700-500		//		default				5703	5855		6011	6112
#define	MaxRAMwait_3 	 7600-600		//							7594	7813		7999	8150
#define	MaxRAMwait_4 	 9500-900		//							9488	9780	   10013   10200
#define	MaxRAMwait_5 	11400-900		//						   11407   11712	   12024   12238
#define	MaxRAMwait_6 	13300-900		//						   13278   13677	   14000   14263
#define	MaxRAMwait_8 	10100-0000		//					SDRAM
#define	MaxRAMwait10 	10000-0000		//					no use

#define	MaxRAMwaitW_0 	 3500-500		//							3497				3549
#define	MaxRAMwaitW_1 	 7000-1000		//							6974				7114
#define	MaxRAMwaitW_2 	10500-1500		//						   10467			   10673
#define	MaxRAMwaitW_3 	14000-2000		//						   13964			   14291
#define	MaxRAMwaitW_4 	17500-2500		//						   17491			   17936
#define	MaxRAMwaitW_5 	18000-2500		//						   18251			   20745
#define	MaxRAMwaitW_6 	 7000-0000		//					SDRAM write

#define ROMmarginDefault	5	// ROM Wait margin default
#define RAMmarginDefault	5	// RAM Wait margin default

//	SDRAM max speed				-margin
//  unknown

//---------------------------------------------------------------------------------------------
#define ROMbase (unsigned int*)0xA0000000	// RAM top address (uncacheable area)
//#define RAMbase (unsigned int*)0xA8000000	// RAM top address (uncacheable area)
#define RAMbase (unsigned int*)0xAC000000	// SDRAM top address (uncacheable area)

