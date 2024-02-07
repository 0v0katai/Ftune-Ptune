/*
===============================================================================

 Ptune3 is SH7305 CPG&BSC tuning utility for PRIZM fx-CG50/Graph 90+E  v0.21

 copyright(c)2014,2015,2016,2017 by sentaro21
 e-mail sentaro21@pm.matrix.jp

===============================================================================
*/

#include "color.h"
#include "display.h"
#include "display_syscalls.h"
#include "keyboard.hpp"
#include "string.h"
#include "keyboard_syscalls.h"
#include "system.h"
#include "stdio.h"
#include "stdlib.h"
#include "disp_tools.hpp"
/*#include "BFILE_syscalls.h"*/
#include "rtc.h"
#include "CONVERT_syscalls.h"
#include "SYSTEM_syscalls.h"

#include "Ptune3.h"
#include "Ptune3_sub.h"
#include "Ptune3_common.h"

//---------------------------------------------------------------------------------------------

int SetupData= 0xFFFF0000 + RAMmarginDefault*0x1000 + ROMmarginDefault*0x100 +(1<<7)+(0<<6)+(1<<5)+(1<<4)+(0<<3)+(1<<2)+(0<<1)+0;
							//	bit 0		Startup Auto memcheck on:1 /off:0	off
							//	bit 1		F1   yes/no check     on:1 /off:0	off
							//	bit 2		Wait Auto setting     on:1 /off:0	on
							//	bit 3		RAM WW  Auto setting  on:1 /off:0	off
							//	bit 4		ROM IWW Auto setting  on:1 /off:0	on
							//	bit 5		PFC  Auto up          on:1 /off:0	on
							//	bit 6		FLL  disp             on:1 /off:0	off
							//	bit 7		BATT disp             on:1 /off:0	on
							//  bit 8-11	ROM Wait margin       0-15%  default 5%
							//  bit12-15	RAM Wait margin       0-15%  default 5%
							//  bit16-30	reserved
							//  bit31		adjust actual frequency  on:1 /off:0	on

//---------------------------------------------------------------------------------------------
void init_menu(void){
    char *onoff[]   ={"   off      ","   on       "};
	char buffer[22]="   ";
	int y,k;
	int freq;
	int	flag=1;
	int selectb=0;
	unsigned int key;
	int scrl=0;
	unsigned int* iresult;	
	
	while (flag) {
		StatusArea_sub();

		if ( scrl <=0 ) {
			PrintXY(1,   (1-scrl), "  ROM margin : ",   selectb==0?TEXT_MODE_INVERT:TEXT_MODE_NORMAL,TEXT_COLOR_BLACK);
			itoa(ROMmargin(),buffer+2); strcat(buffer,"%        ");
			PrintXY(14  ,(1-scrl), buffer,              selectb==0?TEXT_MODE_INVERT:TEXT_MODE_NORMAL,TEXT_COLOR_BLACK);
		}
		if ( scrl <=1 ) {
			PrintXY(1,   (2-scrl), "  RAM margin : ",   selectb==1?TEXT_MODE_INVERT:TEXT_MODE_NORMAL,TEXT_COLOR_BLACK);
			itoa(RAMmargin(),buffer+2); strcat(buffer,"%        ");
			PrintXY(14  ,(2-scrl), buffer,              selectb==1?TEXT_MODE_INVERT:TEXT_MODE_NORMAL,TEXT_COLOR_BLACK);
		}
		if ( scrl <=2 ) {
			PrintXY(1,   (3-scrl), "  PLL CLK Max: ",   selectb==2?TEXT_MODE_INVERT:TEXT_MODE_NORMAL,TEXT_COLOR_BLACK);
			k=sprintf62(buffer+2,MaxFreq_PLL,"MHz ");
			PrintXY(13+k,(3-scrl), buffer,              selectb==2?TEXT_MODE_INVERT:TEXT_MODE_NORMAL,TEXT_COLOR_BLACK);
		}
		if ( scrl <=3 ) {
			PrintXY(1,   (4-scrl), "  CPU CLK Max: ",   selectb==3?TEXT_MODE_INVERT:TEXT_MODE_NORMAL,TEXT_COLOR_BLACK);
			k=sprintf62(buffer+2,MaxFreq_IFC,"MHz ");
			PrintXY(13+k,(4-scrl), buffer,              selectb==3?TEXT_MODE_INVERT:TEXT_MODE_NORMAL,TEXT_COLOR_BLACK);
		}
		if ( scrl <=4 ) {
			PrintXY(1,   (5-scrl), "  Shw CLK Max: ",   selectb==4?TEXT_MODE_INVERT:TEXT_MODE_NORMAL,TEXT_COLOR_BLACK);
			k=sprintf62(buffer+2,MaxFreq_SFC,"MHz ");
			PrintXY(13+k,(5-scrl), buffer,              selectb==4?TEXT_MODE_INVERT:TEXT_MODE_NORMAL,TEXT_COLOR_BLACK);
		}
		if ( scrl <=5 ) {
			PrintXY(1,   (6-scrl), "  Bus CLK Max: ",   selectb==5?TEXT_MODE_INVERT:TEXT_MODE_NORMAL,TEXT_COLOR_BLACK);
			k=sprintf62(buffer+2,MaxFreq_BFC,"MHz ");
			PrintXY(13+k,(6-scrl), buffer,              selectb==5?TEXT_MODE_INVERT:TEXT_MODE_NORMAL,TEXT_COLOR_BLACK);
		}
		if ( scrl <=6 ) {
			PrintXY(1,   (7-scrl), "  I/O CLK Max: ",   selectb==6?TEXT_MODE_INVERT:TEXT_MODE_NORMAL,TEXT_COLOR_BLACK);
			k=sprintf62(buffer+2,MaxFreq_PFC,"MHz ");
			PrintXY(13+k,(7-scrl), buffer,              selectb==6?TEXT_MODE_INVERT:TEXT_MODE_NORMAL,TEXT_COLOR_BLACK);
		}
		if ( scrl >=1 ) {
			PrintXY(1,   (8-scrl), "  Start MemCK: ",   selectb==7?TEXT_MODE_INVERT:TEXT_MODE_NORMAL,TEXT_COLOR_BLACK);
			PrintXY(13,  (8-scrl),onoff[(SetupData>>0)&1],selectb==7?TEXT_MODE_INVERT:TEXT_MODE_NORMAL,TEXT_COLOR_BLACK);
		}
		if ( scrl >=2 ) {
			PrintXY(1,   (9-scrl), "  F1  Yes/No : ",   selectb==8?TEXT_MODE_INVERT:TEXT_MODE_NORMAL,TEXT_COLOR_BLACK);
			PrintXY(13,  (9-scrl),onoff[(SetupData>>1)&1],selectb==8?TEXT_MODE_INVERT:TEXT_MODE_NORMAL,TEXT_COLOR_BLACK);
		}
		if ( scrl >=3 ) {
			PrintXY(1,   (10-scrl), "  Wait Auto -: ",   selectb==9?TEXT_MODE_INVERT:TEXT_MODE_NORMAL,TEXT_COLOR_BLACK);
			PrintXY(13,  (10-scrl),onoff[(SetupData>>2)&1],selectb==9?TEXT_MODE_INVERT:TEXT_MODE_NORMAL,TEXT_COLOR_BLACK);
		}
		if ( scrl >=4 ) {
			PrintXY(1,   (11-scrl), "  RAM WW Auto: ",   selectb==10?TEXT_MODE_INVERT:TEXT_MODE_NORMAL,TEXT_COLOR_BLACK);
			PrintXY(13,  (11-scrl),onoff[(SetupData>>3)&1],selectb==10?TEXT_MODE_INVERT:TEXT_MODE_NORMAL,TEXT_COLOR_BLACK);
		}
		if ( scrl >=5 ) {
			PrintXY(1,   (12-scrl), "  ROM IWW At-: ",   selectb==11?TEXT_MODE_INVERT:TEXT_MODE_NORMAL,TEXT_COLOR_BLACK);
			PrintXY(13,  (12-scrl),onoff[(SetupData>>4)&1],selectb==11?TEXT_MODE_INVERT:TEXT_MODE_NORMAL,TEXT_COLOR_BLACK);
		}
		if ( scrl >=6 ) {
			PrintXY(1,   (13-scrl), "  PFC Auto Up: ",   selectb==12?TEXT_MODE_INVERT:TEXT_MODE_NORMAL,TEXT_COLOR_BLACK);
			PrintXY(13,  (13-scrl),onoff[(SetupData>>5)&1],selectb==12?TEXT_MODE_INVERT:TEXT_MODE_NORMAL,TEXT_COLOR_BLACK);
		}
		if ( scrl >=7 ) {
			PrintXY(1,   (14-scrl), "  FLL   Disp : ",   selectb==13?TEXT_MODE_INVERT:TEXT_MODE_NORMAL,TEXT_COLOR_BLACK);
			PrintXY(13,  (14-scrl),onoff[(SetupData>>6)&1],selectb==13?TEXT_MODE_INVERT:TEXT_MODE_NORMAL,TEXT_COLOR_BLACK);
		}
		if ( scrl >=8 ) {
			PrintXY(1,   (15-scrl), "  Batt  Disp : ",   selectb==14?TEXT_MODE_INVERT:TEXT_MODE_NORMAL,TEXT_COLOR_BLACK);
			PrintXY(13,  (15-scrl),onoff[(SetupData>>7)&1],selectb==14?TEXT_MODE_INVERT:TEXT_MODE_NORMAL,TEXT_COLOR_BLACK);
		}
		if ( scrl >=9 ) {
			PrintXY(1,   (16-scrl), "  Actual Freq: ",   selectb==15?TEXT_MODE_INVERT:TEXT_MODE_NORMAL,TEXT_COLOR_BLACK);
			PrintXY(13,  (16-scrl),onoff[(SetupData>>31)&1],selectb==15?TEXT_MODE_INVERT:TEXT_MODE_NORMAL,TEXT_COLOR_BLACK);
		}
//		if ( scrl >=10 ) {
//			PrintXY(1,   (17-scrl), "  Spread Spct: ",   selectb==16?TEXT_MODE_INVERT:TEXT_MODE_NORMAL,TEXT_COLOR_BLACK);
//			PrintXY(13,  (17-scrl),onoff[SSCGCR!=0],selectb==16?TEXT_MODE_INVERT:TEXT_MODE_NORMAL,TEXT_COLOR_BLACK);
//		}

        if ( selectb <= 6 ) {
			Fkey_dispNP(0);
			Fkey_dispNM(1);
			GetFKeyPtr(0x005f, &iresult);	//INITIAL 	,white
			FKey_Display(3, iresult);
		}
		if ( selectb >= 7  ) {
			GetFKeyPtr(0x0011, &iresult);	//on 		,white
			FKey_Display(0, iresult);
			GetFKeyPtr(0x0012, &iresult);	//off 		,white
			FKey_Display(1, iresult);
		}
				
		GetFKeyPtr(0x03df, &iresult);	//version 	,black
		FKey_Display(5, iresult);

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
				if ( selectb < 0 ) {selectb=15; scrl=selectb-6;}
				if ( selectb < scrl ) scrl-=1;
				if ( scrl < 0 ) scrl=0;
				break;
			case KEY_CTRL_DOWN:
				selectb+=1;
				if ( selectb > 15 ) {selectb=0; scrl=0;}
				if ((selectb - scrl) > 6 ) scrl+=1;
				if ( scrl > 15 ) scrl=15-6;
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
					case 15: // Actual Freq Display on
						SetupData=( SetupData | 0x80000000 ) ;
						break;
//					case 16: // Spread spectrum control on
//						SSCGCR = (unsigned int)0x80000000;
//						break;
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
					case 15: // Actual Freq Display off
						SetupData=( SetupData & 0x7FFFFFFF ) ;
						break;
//					case 16: // Spread spectrum control off
//						SSCGCR = (unsigned int)0x00000000;
//						break;
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

