/*
===============================================================================

 Ftune is SH7337/SH7355 CPG&BSC tuning utility for fx-9860G/GII series  v1.01

 copyright(c)2014,2015 by sentaro21
 e-mail sentaro21@pm.matrix.jp

===============================================================================
*/

#include	"SH7355_CPG_BSC.h"

//---------------------------------------------------------------------------------------------
#define PLL_1x 0 //0b0000	// x1
#define PLL_2x 1 //0b0001	// x2
#define PLL_3x 2 //0b0010	// x3
#define PLL_4x 3 //0b0011	// x4

#define DIV_1 0 //0b0000	// 1/1
#define DIV_2 1 //0b0001	// 1/2
#define DIV_3 2 //0b0010	// 1/3
#define DIV_4 3 //0b0011	// 1/4

#define WAIT_0 0 //0b0000
#define WAIT_1 1 //0b0001
#define WAIT_2 2 //0b0010	// default 
#define WAIT_3 3 //0b0011
#define WAIT_4 4 //0b0100
#define WAIT_5 5 //0b0101
#define WAIT_6 6 //0b0110
#define WAIT_8 7 //0b0111
#define WAIT10 8 //0b1000
#define WAIT12 9 //0b1001
#define WAIT14 10 //0b1010
#define WAIT18 11 //0b1011
#define WAIT24 12 //0b1100

#define WAITW0 1		//0b0001
#define WAITW1 2		//0b0010
#define WAITW2 3		//0b0011
#define WAITW3 4		//0b0100
#define WAITW4 5		//0b0101
#define WAITW5 6		//0b0110
#define WAITW6 7		//0b0111

#define FRQCR_default   0x1001

#define CS0BCR_default   0x02480400
#define CS2BCR_default   0x02483400
#define CS3BCR_default   0x36DB0600
#define CS4BCR_default   0x02480400
#define CS5aBCR_default  0x224A0200
#define CS5bBCR_default  0x224A0200
#define CS6aBCR_default  0x36DB0600
#define CS6bBCR_default  0x36DB0600

#define CS0WCR_default   0x00000140
#define CS2WCR_default   0x00000140
#define CS3WCR_default   0x00000500
#define CS4WCR_default   0x00000140
#define CS5aWCR_default  0x00000D41
#define CS5bWCR_default  0x00000D41
#define CS6aWCR_default  0x00000500
#define CS6bWCR_default  0x00000500

//---------------------------------------------------------------------------------------------
#define ROMbase (unsigned int*)0xA0000000	// RAM top address (uncacheable area)
#define RAMbase (unsigned int*)0xA8000000	// RAM top address (uncacheable area)

