/*
===============================================================================

 Ftune2 is SH7305 CPG&BSC tuning utility for fx-9860GII-2      v1.10

 copyright(c)2014,2015,2016 by sentaro21
 e-mail sentaro21@pm.matrix.jp

===============================================================================
*/

#include "fxlib.h"
#include "timer.h"
#include "stdio.h"
#include "string.h"

#include "common.h"
#include "ftune2.h"
#include "ftune2_sub.h"

//---------------------------------------------------------------------------------------------

int SetupData= 0xFFFF0000 + RAMmarginDefault*0x1000 + ROMmarginDefault*0x100 +(0<<7)+(0<<6)+(1<<5)+(1<<4)+(1<<3)+(1<<2)+(0<<1)+1;
							//	bit 0		Startup Auto memcheck on:1 /off:0	on
							//	bit 1		F1   yes/no check     on:1 /off:0	off
							//	bit 2		Wait Auto setting     on:1 /off:0	on
							//	bit 3		RAM WW  Auto setting  on:1 /off:0	on
							//	bit 4		ROM IWW Auto setting  on:1 /off:0	on
							//	bit 5		PFC  Auto up          on:1 /off:0	on
							//	bit 6		FLL  disp             on:1 /off:0	off
							//	bit 7		BATT disp             on:1 /off:0	off
							//  bit 8-11	ROM Wait margin       0-15%  default 5%
							//  bit12-15	RAM Wait margin       0-15%  default 5%
							//  bit16-31	reserved


//---------------------------------------------------------------------------------------------
void init_menu(void){
    char *onoff[]   ={" off      "," on       "};
	char buffer[22];
	int y,k;
	int freq;
	int	flag=1;
	int selectb=0;
	unsigned int key;
	int scrl=0;
	
	while (flag) {
		Bdisp_AllClr_VRAM();

		if ( scrl <=0 ) {
			PrintXYR( 0*6,(0-scrl)*8, "ROM margin : ",     selectb==0?1:0);
			sprintf(buffer," %d%%       ",ROMmargin());
			PrintXYR(12*6,(0-scrl)*8, buffer,              selectb==0?1:0);
		}
		if ( scrl <=1 ) {
			PrintXYR( 0*6,(1-scrl)*8, "RAM margin : ",     selectb==1?1:0);
			sprintf(buffer," %d%%       ",RAMmargin());
			PrintXYR(12*6,(1-scrl)*8, buffer,              selectb==1?1:0);
		}
		if ( scrl <=2 ) {
			PrintXYR( 0*6,(2-scrl)*8, "PLL CLK Max: ",     selectb==2?1:0);
			sprintf(buffer,"%6.2fMHz ",(float)MaxFreq_PLL/100);
			PrintXYR(12*6,(2-scrl)*8, buffer,              selectb==2?1:0);
		}
		if ( scrl <=3 ) {
			PrintXYR( 0*6,(3-scrl)*8, "CPU CLK Max: ",     selectb==3?1:0);
			sprintf(buffer,"%6.2fMHz ",(float)MaxFreq_IFC/100);
			PrintXYR(12*6,(3-scrl)*8, buffer,              selectb==3?1:0);
		}
		if ( scrl <=4 ) {
			PrintXYR( 0*6,(4-scrl)*8, "Shw CLK Max: ",     selectb==4?1:0);
			sprintf(buffer,"%6.2fMHz ",(float)MaxFreq_SFC/100);
			PrintXYR(12*6,(4-scrl)*8, buffer,              selectb==4?1:0);
		}
		if ( scrl <=5 ) {
			PrintXYR( 0*6,(5-scrl)*8, "Bus CLK Max: ",     selectb==5?1:0);
			sprintf(buffer,"%6.2fMHz ",(float)MaxFreq_BFC/100);
			PrintXYR(12*6,(5-scrl)*8, buffer,              selectb==5?1:0);
		}
		if ( scrl <=6 ) {
			PrintXYR( 0*6,(6-scrl)*8, "I/O CLK Max: ",     selectb==6?1:0);
			sprintf(buffer,"%6.2fMHz ",(float)MaxFreq_PFC/100);
			PrintXYR(12*6,(6-scrl)*8, buffer,              selectb==6?1:0);
		}
		if ( scrl >=1 ) {
			PrintXYR( 0*6,(7-scrl)*8, "Start MemCK: ",     selectb==7?1:0);
			PrintXYR(12*6,(7-scrl)*8,onoff[(SetupData>>0)&1],selectb==7?1:0);
		}
		if ( scrl >=2 ) {
			PrintXYR( 0*6,(8-scrl)*8, "F1  Yes/No : ",     selectb==8?1:0);
			PrintXYR(12*6,(8-scrl)*8,onoff[(SetupData>>1)&1],selectb==8?1:0);
		}
		if ( scrl >=3 ) {
			PrintXYR( 0*6,(9-scrl)*8, "Wait Auto -: ",     selectb==9?1:0);
			PrintXYR(12*6,(9-scrl)*8,onoff[(SetupData>>2)&1],selectb==9?1:0);
		}
		if ( scrl >=4 ) {
			PrintXYR( 0*6,(10-scrl)*8, "RAM WW Auto: ",     selectb==10?1:0);
			PrintXYR(12*6,(10-scrl)*8,onoff[(SetupData>>3)&1],selectb==10?1:0);
		}
		if ( scrl >=5 ) {
			PrintXYR( 0*6,(11-scrl)*8, "ROM IWW At-: ",     selectb==11?1:0);
			PrintXYR(12*6,(11-scrl)*8,onoff[(SetupData>>4)&1],selectb==11?1:0);
		}
		if ( scrl >=6 ) {
			PrintXYR( 0*6,(12-scrl)*8, "PFC Auto Up: ",     selectb==12?1:0);
			PrintXYR(12*6,(12-scrl)*8,onoff[(SetupData>>5)&1],selectb==12?1:0);
		}
		if ( scrl >=7 ) {
			PrintXYR( 0*6,(13-scrl)*8, "FLL   Disp : ",     selectb==13?1:0);
			PrintXYR(12*6,(13-scrl)*8,onoff[(SetupData>>6)&1],selectb==13?1:0);
		}
		if ( scrl >=8 ) {
			PrintXYR( 0*6,(14-scrl)*8, "Batt  Disp : ",     selectb==14?1:0);
			PrintXYR(12*6,(14-scrl)*8,onoff[(SetupData>>7)&1],selectb==14?1:0);
		}
        if ( selectb <= 6 ) {
			Fkey_dispNP(0);
			Fkey_dispNM(1);
			Fkey_dispNInit(3);
		}
		if ( selectb >= 7  ) {
			Fkey_dispN(0," on ");
			Fkey_dispN(1," off");
		}
		
		Fkey_dispN(5,"Ver.");	

		Bdisp_PutDisp_DD();

		GetKey( &key );
		switch (key) {
			case KEY_CTRL_AC:
				flag=0;
				break;
			case KEY_CTRL_EXIT:
				flag=0;
				break;
		
			case KEY_CTRL_UP:
				selectb-=1;
				if ( selectb < 0 ) {selectb=14; scrl=8;}
				if ( selectb < scrl ) scrl-=1;
				if ( scrl < 0 ) scrl=0;
				break;
			case KEY_CTRL_DOWN:
				selectb+=1;
				if ( selectb > 14 ) {selectb=0; scrl=0;}
				if ((selectb - scrl) > 6 ) scrl+=1;
				if ( scrl > 8 ) scrl=8;
				break;
				
			case KEY_CHAR_PLUS:
			case KEY_CTRL_F1:		//	+
				switch (selectb) {
					case 0: // ROM wait margin +
						if ( ROMmargin() < 15 )  SetupData += 0x0100;
						SetMaxFreq();;
						break;
					case 1: // RAM wait margin +
						if ( RAMmargin() < 15 )  SetupData += 0x1000;
						SetMaxFreq();;
						break;
					case 2:	// PLL max +
						MaxFreq_PLL+=500;
						if ( MaxFreq_PLL > Max_PLL ) MaxFreq_PLL=Max_PLL;
						break;
					case 3:	// IFC max +
						MaxFreq_IFC+=100;
						if ( MaxFreq_IFC > Max_IFC ) MaxFreq_IFC=Max_IFC;
						break;
					case 4:	// SFC max +
						MaxFreq_SFC+=100;
						if ( MaxFreq_SFC > Max_SFC ) MaxFreq_SFC=Max_SFC;
						break;
					case 5:	// BFC max +
						MaxFreq_BFC+=100;
						if ( MaxFreq_BFC > Max_BFC ) MaxFreq_BFC=Max_BFC;
						break;
					case 6:	// PFC max +
						MaxFreq_PFC+=100;
						if ( MaxFreq_PFC > Max_PFC ) MaxFreq_PFC=Max_PFC;
						break;
					case 7: // Startup Auto memcheck on
						SetupData=( SetupData | 1 ) ;
						break;
					case 8: // F1   Yes/No  on
						SetupData=( SetupData | 2 ) ;
						break;
					case 9: // wait Auto   on
						SetupData=( SetupData | 4 ) ;
						break;
					case 10: // RAM WW Auto  on
						SetupData=( SetupData | 8 ) ;
						break;
					case 11: // ROM IWW Auto  on
						SetupData=( SetupData | 16 ) ;
						break;
					case 12: // PFC Auto up
						SetupData=( SetupData | 32 ) ;
						break;
					case 13: // FLL Display on
						SetupData=( SetupData | 64 ) ;
						break;
					case 14: // Batt Display on
						SetupData=( SetupData | 128 ) ;
						break;
				}
				break;
				
			case KEY_CTRL_RIGHT:		//	+
				switch (selectb) {
					case 2:	// PLL max +
						MaxFreq_PLL+=50;
						if ( MaxFreq_PLL > Max_PLL ) MaxFreq_PLL=Max_PLL;
						break;
					case 3:	// IFC max +
						MaxFreq_IFC+=10;
						if ( MaxFreq_IFC > Max_IFC ) MaxFreq_IFC=Max_IFC;
						break;
					case 4:	// SFC max +
						MaxFreq_SFC+=10;
						if ( MaxFreq_SFC > Max_SFC ) MaxFreq_SFC=Max_SFC;
						break;
					case 5:	// BFC max +
						MaxFreq_BFC+=10;
						if ( MaxFreq_BFC > Max_BFC ) MaxFreq_BFC=Max_BFC;
						break;
					case 6:	// PFC max +
						MaxFreq_PFC+=10;
						if ( MaxFreq_PFC > Max_PFC ) MaxFreq_PFC=Max_PFC;
						break;
				}
				break;
				
			case KEY_CHAR_MINUS:
			case KEY_CTRL_F2:		//		-
				switch (selectb) {
					case 0: // ROM wait margin -
						if ( ROMmargin() > 0 )  SetupData -= 0x0100;
						SetMaxFreq();;
						break;
					case 1: // RAM wait margin -
						if ( RAMmargin() > 0 )  SetupData -= 0x1000;
						SetMaxFreq();;
						break;
					case 2:	// PLL max -
						MaxFreq_PLL-=500;
						if ( MaxFreq_PLL < 0 ) MaxFreq_PLL=0;
						break;
					case 3:	// IFC max -
						MaxFreq_IFC-=100;
						if ( MaxFreq_IFC < 0 ) MaxFreq_IFC=0;
						break;
					case 4:	// SFC max -
						MaxFreq_SFC-=100;
						if ( MaxFreq_SFC < 0 ) MaxFreq_SFC=0;
						break;
					case 5:	// BFC max -
						MaxFreq_BFC-=100;
						if ( MaxFreq_BFC < 0 ) MaxFreq_BFC=0;
						break;
					case 6:	// PFC max -
						MaxFreq_PFC-=100;
						if ( MaxFreq_PFC < 0 ) MaxFreq_PFC=0;
						break;
					case 7: // Startup Auto memcheck off
						SetupData=( SetupData & 0xFFFFFFFE ) ;
						break;
					case 8: // F1   Yes/No off
						SetupData=( SetupData & 0xFFFFFFFD ) ;
						break;
					case 9: // wait Auto   off
						SetupData=( SetupData & 0xFFFFFFFB ) ;
						break;
					case 10: // RAM WW Auto  off
						SetupData=( SetupData & 0xFFFFFFF7 ) ;
						break;
					case 11: // ROM IWW Auto  off
						SetupData=( SetupData & 0xFFFFFFEF ) ;
						break;
					case 12: // PFC Auto up
						SetupData=( SetupData & 0xFFFFFFDF ) ;
						break;
					case 13: // FLL Display off
						SetupData=( SetupData & 0xFFFFFFBF ) ;
						break;
					case 14: // Batt Display off
						SetupData=( SetupData & 0xFFFFFF7F ) ;
						break;
				}
				break;
				
			case KEY_CTRL_LEFT:		//		-
				switch (selectb) {
					case 2:	// PLL max -
						MaxFreq_PLL-=50;
						if ( MaxFreq_PLL < 0 ) MaxFreq_PLL=0;
						break;
					case 3:	// IFC max -
						MaxFreq_IFC-=10;
						if ( MaxFreq_IFC < 0 ) MaxFreq_IFC=0;
						break;
					case 4:	// SFC max -
						MaxFreq_SFC-=10;
						if ( MaxFreq_SFC < 0 ) MaxFreq_SFC=0;
						break;
					case 5:	// BFC max -
						MaxFreq_BFC-=10;
						if ( MaxFreq_BFC < 0 ) MaxFreq_BFC=0;
						break;
					case 6:	// PFC max -
						MaxFreq_PFC-=10;
						if ( MaxFreq_PFC < 0 ) MaxFreq_PFC=0;
						break;
				}
				break;

			case KEY_CTRL_F4:		// initialize
				switch (selectb) {
					case 0: // ROM wait margin init
						SetupData = ( SetupData & 0xFFFFF0FF ) | ( ROMmarginDefault * 0x0100 ) ;
						SetMaxFreq();;
						break;
					case 1: // RAM wait margin init
						SetupData = ( SetupData & 0xFFFF0FFF ) | ( RAMmarginDefault * 0x1000 ) ;
						SetMaxFreq();;
						break;
					case 2:		// PLL max init
						MaxFreq_PLL=Max_PLLdef;
						break;
					case 3:		// IFC max init
						MaxFreq_IFC=Max_IFCdef;
						break;
					case 4:		// SFC max init
						MaxFreq_SFC=Max_SFCdef;
						break;
					case 5:		// BFC max init
						MaxFreq_BFC=Max_BFCdef;
						break;
					case 6:	// PFC max init
						MaxFreq_PFC=Max_PFCdef;
						break;
				}
				break;
			case KEY_CTRL_F6:		// about
				about();
				break;
        }
    }
}

