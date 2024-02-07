/*
===============================================================================

 Ptune2 is SH7305 CPG&BSC tuning utility for PRIZM fx-CG10/20  v1.24

 copyright(c)2014,2015,2016-2020 by sentaro21
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

#include "Ptune2.h"
#include "Ptune2_sub.h"
#include "Ptune2_common.h"
#include "Ptune2_memcheck.h"
#include "Ptune2_benchmark.h"
#include "Ptune2_bscmod.h"
#include "Ptune2_data.h"
#include "Ptune2_setup.h"

//---------------------------------------------------------------------------------------------

void about(){
	int key;
		MsgBoxPush(5);
    	PrintXY(3,2,"  Ptune2 v1.24 ",TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
    	PrintXY(3,3,"  Copyright(C)2017 ",TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
    	PrintXY(3,4,"       by sentaro21",TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
    	PrintXY(3,6,"     Press:[EXIT]",TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
		Bdisp_PutDisp_DD();
		GetKey_DisableCatalog(&key);	
		MsgBoxPop();
		Bdisp_PutDisp_DD();
}

//---------------------------------------------------------------------------------------------
void cls7(void){
	locate_OS( 1, 1 );	Print(cls);
	locate_OS( 1, 2 );	Print(cls);
	locate_OS( 1, 3 );	Print(cls);
	locate_OS( 1, 4 );	Print(cls);
	locate_OS( 1, 5 );	Print(cls);
	locate_OS( 1, 6 );	Print(cls);
	locate_OS( 1, 7 );	Print(cls);
}

void dsp_FRQCR(){
	cls7();	
	HexPrintXY(1,1,"FRQCR    :",CPG.FRQCRA.LONG);
	HexPrintXY(1,2,"FCLKACR  :",CPG.FCLKACR);
	HexPrintXY(1,3,"SPUCLKCR :",CPG.SPUCLKCR);
	
	HexPrintXY(1,4,"PLLCR    :",CPG.PLLCR);
	HexPrintXY(1,5,"FLLFRQ   :",CPG.FLLFRQ.LONG);
	
	HexPrintXY(1,7,"CCR      :",*(unsigned int*)0xFF00001C);
	Bdisp_PutDisp_DD();
}
void dsp_BCR(){
	cls7();
	HexPrintXY(1,1,"CS0BCR   :",BSC.CS0BCR.LONG);
	HexPrintXY(1,2,"CS2BCR   :",BSC.CS2BCR.LONG);
	HexPrintXY(1,3,"CS3BCR   :",BSC.CS3BCR.LONG);
	HexPrintXY(1,4,"CS4BCR   :",BSC.CS4BCR.LONG);
	HexPrintXY(1,5,"CS5ABCR  :",BSC.CS5ABCR.LONG);
	HexPrintXY(1,6,"CS6ABCR  :",BSC.CS6ABCR.LONG);
	Bdisp_PutDisp_DD();
}
void dsp_WCR(){
	cls7();
	HexPrintXY(1,1,"CS0WCR   :",BSC.CS0WCR.LONG);
	HexPrintXY(1,2,"CS2WCR   :",BSC.CS2WCR.LONG);
	HexPrintXY(1,3,"CS3WCR   :",BSC.CS3WCR.LONG);
	HexPrintXY(1,4,"CS4WCR   :",BSC.CS4WCR.LONG);
	HexPrintXY(1,5,"CS5AWCR  :",BSC.CS5AWCR.LONG);
	HexPrintXY(1,6,"CS6AWCR  :",BSC.CS6AWCR.LONG);
	Bdisp_PutDisp_DD();
}
/*
void dsp_TLB(){
	cls7();
	HexPrintXY(1,1,"PTEH   :",*(unsigned int*)0xFF000000);
	HexPrintXY(1,2,"PTEL   :",*(unsigned int*)0xFF000004);
	HexPrintXY(1,3,"TTB    :",*(unsigned int*)0xFF000008);
	HexPrintXY(1,4,"TEA    :",*(unsigned int*)0xFF00000C);
	HexPrintXY(1,5,"MMUCR  :",*(unsigned int*)0xFF000010);
	HexPrintXY(1,6,"PTEA   :",*(unsigned int*)0xFF000034);
	HexPrintXY(1,7,"PASCR  :",*(unsigned int*)0xFF000070);
	HexPrintXY(1,8,"IRMCR  :",*(unsigned int*)0xFF000078);
	Bdisp_PutDisp_DD();
}
*/

//---------------------------------------------------------------------------------------------
int	mes_flag=0;		// under message select

void update(int select,int select_func,int modify) {
	int COLOR_ROM,COLOR2_ROM,COLOR1_RAM,COLOR2_RAM,COLOR3_RAM,COLOR4_RAM;;
	char buffer[20];
    char *divs[]   ={"  1/2","  1/4","  1/8","  /16","  /32","  /64"};
    char *waits[]  ={"   0","   1","   2","   3","   4","   5","   6","   8","  10","  12","  14","  18","  24"};
	char *waitW[]  ={"  =R","   0","   1","   2","   3","   4","   5","   6"};
    char *selects[]={
		" FLL mult Ratio       (up to x1023)          ",
		" PLL mult Ratio       (up to 750MHz)         ",
		" CPU core Clock       (up to 250 MHz)        ",
		" SuperHyway Clock   (up to 150 MHz)      ",
		" Bus Clock               (up to 120 MHz)       ",
		" Peripheral Clock     (up to 25 MHz)       "};
	
	int	_FRQCR,FRQCR_STC,FRQCR_IFC,FRQCR_SFC,FRQCR_BFC,FRQCR_PFC;
	int	CS0WCR_WR,CS2WCR_WR,CS2WCR_WW;
	int	FLF;
	int freq,k,bat;
	
	FLF       = CPG.FLLFRQ.BIT.FLF ;
	FRQCR_STC = CPG.FRQCRA.BIT.STC ;
	FRQCR_IFC = CPG.FRQCRA.BIT.IFC ;
	FRQCR_SFC = CPG.FRQCRA.BIT.SFC ;
	FRQCR_BFC = CPG.FRQCRA.BIT.BFC ;
	FRQCR_PFC = CPG.FRQCRA.BIT.PFC ;
	CS0WCR_WR = BSC.CS0WCR.BIT.WR ;
	CS2WCR_WR = BSC.CS2WCR.BIT.WR ;
	CS2WCR_WW = BSC.CS2WCR.BIT.WW ;

	if (select==-1) {
		PrintXY(1, 1, cls,TEXT_MODE_NORMAL,TEXT_COLOR_BLACK);
		PrintMiniXY(380, 0*24+4, "  ", COLOR_BLACK, COLOR_WHITE);
		PrintXY(1, 1, "   FLL:", select==-1?TEXT_MODE_INVERT:TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
	}
	else {
		PrintXY(1, 1, cls,TEXT_MODE_NORMAL,TEXT_COLOR_BLACK);
		PrintMiniXY(0, 0*24+4, " Operating Frequency *:Normal              ", COLOR_WHITE, COLOR_BLACK);
		if (SetupData & 128) {
					bat=GetMainBatteryVoltage(1);k=sprintf62(buffer,bat,"v ");
					PrintMiniXY(316+4, 0*24+4,buffer,  COLOR_WHITE, COLOR_BLACK);
		} else		PrintMiniXY(316+4, 0*24+4,"v1.24", COLOR_WHITE, COLOR_BLACK);
	}
	PrintXY(1, 2, cls,TEXT_MODE_NORMAL,TEXT_COLOR_BLACK);
	PrintXY(1, 3, cls,TEXT_MODE_NORMAL,TEXT_COLOR_BLACK);
	PrintXY(1, 4, cls,TEXT_MODE_NORMAL,TEXT_COLOR_BLACK);
	PrintXY(1, 5, cls,TEXT_MODE_NORMAL,TEXT_COLOR_BLACK);
	PrintXY(1, 6, cls,TEXT_MODE_NORMAL,TEXT_COLOR_BLACK);
	PrintXY(1, 2, "   PLL:", select==0?TEXT_MODE_INVERT:TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
	PrintXY(1, 3, "   IFC:", select==1?TEXT_MODE_INVERT:TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
	PrintXY(1, 4, "   SFC:", select==2?TEXT_MODE_INVERT:TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
	PrintXY(1, 5, "   BFC:", select==3?TEXT_MODE_INVERT:TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
	PrintXY(1, 6, "   PFC:", select==4?TEXT_MODE_INVERT:TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
	PrintXY(1,select+2,pointss, TEXT_MODE_NORMAL, TEXT_COLOR_RED);
	PrintXY(1, 8, cls, TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);

	switch (select_func) {	
		case 0:
			func_menu1();
				break;
		case 1:
			func_menu2();
				break;
		case 2:
			func_menu3();
				break;
#ifdef OPTION_REG
		case 3:
			func_menu4();
				break;
#endif
	}

	if (select == -1) {
		itoa(FLF,(unsigned char*)buffer);	
		PrintMiniXY(5*18+4,0*24+4, "\xA9  ", FLF>=1024?COLOR_RED:COLOR_BLACK,COLOR_WHITE);
		PrintMiniXY(6*18+0,0*24+4, buffer, FLF>=1024?COLOR_RED:COLOR_BLACK,COLOR_WHITE);

		freq=(FLF*32768/100/2+50)/100;															// FLL freq
		k=sprintf62(buffer,freq," MHz    ");
		PrintMiniXY(13*18+3+k*12,0*24+4, buffer, FLF>=1024?COLOR_RED:COLOR_BLACK,COLOR_WHITE);
		if (FLF==FLF_default)
			PrintMiniXY(12*18+3, 0*24+4, "*", COLOR_BLACK,COLOR_WHITE);	
	}

	itoa(FRQCR_STC+1,(unsigned char*)buffer+2);	
	freq=freq_sub(FLF,FRQCR_STC,-1);
	PrintXY(6,  2, "  \xA9",  TEXT_MODE_NORMAL, freq>=75000?TEXT_COLOR_RED:TEXT_COLOR_BLACK);
	PrintXY(7,  2, buffer, TEXT_MODE_NORMAL, freq>=75000?TEXT_COLOR_RED:TEXT_COLOR_BLACK);

	k=sprintf62(buffer,freq," MHz    ");												// PLL freq
	PrintMiniXY(13*18+3+k*12,1*24+3, buffer, freq>=75000?COLOR_RED:COLOR_BLACK,COLOR_WHITE);
	if ( (FRQCR_STC==PLL_16x) && ( FLF == FLF_default ) )
		PrintMiniXY(12*18+3, 1*24+3, "*", COLOR_BLACK,COLOR_WHITE);
	
	freq=freq_sub(FLF,FRQCR_STC,FRQCR_IFC);												// CPU freq
	k=sprintf62(buffer+2,freq,"MHz ");
	PrintXY( 6,3, divs[FRQCR_IFC], TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
	PrintXY( 9,3, "  CPU   ", TEXT_MODE_INVERT, freq>25100?TEXT_COLOR_RED:TEXT_COLOR_BLUE);
	PrintXY(13+k,3, buffer,   TEXT_MODE_INVERT, freq>25100?TEXT_COLOR_RED:TEXT_COLOR_BLUE);
	if ( (freq==freq_sub(FLF,PLL_16x,DIV_4)) && ( FLF == FLF_default ) ) 
		PrintMiniXY(12*18+3, 2*24+3, "*", COLOR_BLACK,COLOR_WHITE);
	

	freq=freq_sub(FLF,FRQCR_STC,FRQCR_SFC);												// SHW freq
	k=sprintf62(buffer,freq," MHz    ");
	PrintXY( 6,4, divs[FRQCR_SFC], TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
	PrintMiniXY(13*18+3+k*12,3*24+4, buffer, freq>15000?COLOR_RED:COLOR_BLACK,COLOR_WHITE);
	if ( (freq==freq_sub(FLF,PLL_16x,DIV_8)) && ( FLF == FLF_default ) ) 
		PrintMiniXY(12*18+3, 3*24+4, "*", COLOR_BLACK,COLOR_WHITE);


	freq=freq_sub(FLF,FRQCR_STC,FRQCR_BFC);												// bus freq
	k=sprintf62(buffer,freq," MHz    ");
	PrintXY( 6,5, divs[FRQCR_BFC],  TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
	PrintMiniXY(13*18+3+k*12,4*24+4, buffer,freq>12000?COLOR_RED:COLOR_BLACK,COLOR_WHITE);
	if ( (freq==freq_sub(FLF_default,PLL_16x,DIV_8)) && (BSC.CS0BCR.LONG==CS0BCR_default) && (BSC.CS2BCR.LONG==CS2BCR_default)
	   						&& (FLF==FLF_default)    && (BSC.CS0WCR.LONG==CS0WCR_default) && (BSC.CS2WCR.LONG==CS2WCR_default) )
		PrintMiniXY(12*18+3, 4*24+4, "*", COLOR_BLACK,COLOR_WHITE);

	COLOR_ROM=COLOR_WHITE; COLOR2_ROM=COLOR_BLACK;
	if ( ROMselect != 0 ) {
		if (( MaxFreq_ROMwait[CS0WCR_WR-1] <=  freq )
			&& ( freq <= MaxFreq_ROMwait[CS0WCR_WR-1]*105/100 ))	{ COLOR_ROM=COLOR_GREEN;  COLOR2_ROM=COLOR_BLACK; }
		if (( MaxFreq_ROMwait0[CS0WCR_WR]*(90+ROMmargin())/100 < freq )
			&& ( freq < MaxFreq_ROMwait[CS0WCR_WR+1] ))				{ COLOR_ROM=COLOR_YELLOW; COLOR2_ROM=COLOR_BLACK; }
		if (( MaxFreq_ROMwait0[CS0WCR_WR]*(95+ROMmargin())/100 < freq )
			&& ( freq < MaxFreq_ROMwait[CS0WCR_WR+1] ))				{ COLOR_ROM=COLOR_WHITE;  COLOR2_ROM=COLOR_RED; }
	}
	PrintMiniXY(10*18-6, 4*24-19, waits[CS0WCR_WR],COLOR_ROM,COLOR2_ROM);
	PrintMiniXY( 8*18+3, 4*24-19, "roR",           COLOR_ROM,COLOR2_ROM);

	COLOR1_RAM=COLOR_WHITE; COLOR2_RAM=COLOR_BLACK;
	COLOR3_RAM=COLOR_WHITE; COLOR4_RAM=COLOR_BLACK;
	if ( RAMselect != 0 ) {
		if (( MaxFreq_RAMwait[CS2WCR_WR-1] <=  freq )
			&& ( freq <= MaxFreq_RAMwait[CS2WCR_WR-1]*105/100 ))	{ COLOR1_RAM=COLOR_GREEN;  COLOR2_RAM=COLOR_BLACK; }
		if (( MaxFreq_RAMwait0[CS2WCR_WR]*(90+RAMmargin())/100 < freq )
			&& ( freq < MaxFreq_RAMwait[CS2WCR_WR+1] ))				{ COLOR1_RAM=COLOR_YELLOW; COLOR2_RAM=COLOR_BLACK; }
		if (( MaxFreq_RAMwait0[CS2WCR_WR]*(95+RAMmargin())/100 < freq )
			&& ( freq < MaxFreq_RAMwait[CS2WCR_WR+1] ))				{ COLOR1_RAM=COLOR_WHITE;  COLOR2_RAM=COLOR_RED; }

		if (( MaxFreq_RAMwaitW0[CS2WCR_WW]*(90+RAMmargin())/100 < freq ) && ( CS2WCR_WW != 0 )
			&& ( freq < MaxFreq_RAMwaitW[CS2WCR_WW+1] ))			{ COLOR3_RAM=COLOR_YELLOW; COLOR4_RAM=COLOR_BLACK; }
		if (( MaxFreq_RAMwaitW0[CS2WCR_WW]*(95+RAMmargin())/100 < freq ) && ( CS2WCR_WW != 0 )
			&& ( freq < MaxFreq_RAMwaitW[CS2WCR_WW+1] ))			{ COLOR3_RAM=COLOR_WHITE;  COLOR4_RAM=COLOR_RED; }
	}
	PrintMiniXY(10*18-6, 4*24+ 5, waits[CS2WCR_WR],COLOR1_RAM,COLOR2_RAM);
	PrintMiniXY( 8*18+3, 4*24+ 5, "raR",           COLOR1_RAM,COLOR2_RAM);

	PrintMiniXY(10*18-6, 4*24+26, waitW[CS2WCR_WW],COLOR3_RAM,COLOR4_RAM);
	PrintMiniXY( 8*18+3, 4*24+26, "raW",           COLOR3_RAM,COLOR4_RAM);


	freq=freq_sub(FLF,FRQCR_STC,FRQCR_PFC);												// I/O freq
	k=sprintf62(buffer,freq," MHz    ");
	PrintXY( 6,6, divs[FRQCR_PFC],  TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
	PrintMiniXY(13*18+3+k*12,5*24+4, buffer, freq>2500?COLOR_RED:COLOR_BLACK,COLOR_WHITE);
	if ( (freq==freq_sub(FLF,PLL_16x,DIV16)) && ( FLF == FLF_default ) ) 
		PrintMiniXY(12*18+3, 5*24+4, "*", COLOR_BLACK,COLOR_WHITE);


	switch	(mes_flag) {
		case 0:
			PrintXY(1, 7, cls,TEXT_MODE_NORMAL,TEXT_COLOR_BLACK);
			PrintMiniXY(0,6*24+2, selects[select+1], COLOR_WHITE, COLOR_BLACK);
			break;
		case 1:
			if ( modify ) {
				Bench(100,1);
				Benchmem(50);
			}
			break;
		case 2:
			if ( modify ) {
				Bench(100,1);
				BenchDD(1000);
			}
			break;
		default:
			break;
	}
	
	Bdisp_PutDisp_DD();
}


//---------------------------------------------------------------------------------------------

int main(void) {
	int	FRQCR_STC,FRQCR_IFC,FRQCR_SFC,FRQCR_BFC,FRQCR_PFC;
	int	CS0WCR_WR,CS2WCR_WR,CS2WCR_WW;
	int	FLF;
	
	unsigned int key;
	int select=0;
	int select_func=0;
	int freq;
	int i;
	int BCRWCR=0;

	SetMaxFreq();
	LoadFile(0);	// Startup auto load
	SetMaxFreq0();
	StatusArea_sub();
	if ( (ROMselect==0) && (SetupData&1) ) if (YesNo("Check Memory")) test_wait_mem();	// startup memory check
	update(select,select_func,1);

	while(1) {
		FLF       = CPG.FLLFRQ.BIT.FLF ;
		FRQCR_STC = CPG.FRQCRA.BIT.STC ;
		FRQCR_IFC = CPG.FRQCRA.BIT.IFC ;
		FRQCR_SFC = CPG.FRQCRA.BIT.SFC ;
		FRQCR_BFC = CPG.FRQCRA.BIT.BFC ;
		FRQCR_PFC = CPG.FRQCRA.BIT.PFC ;
		CS0WCR_WR = BSC.CS0WCR.BIT.WR ;
		CS2WCR_WR = BSC.CS2WCR.BIT.WR ;
		CS2WCR_WW = BSC.CS2WCR.BIT.WW ;
		
		freq=freq_sub(FLF,FRQCR_STC,FRQCR_BFC);	// Bus freq

		GetKey_DisableCatalog(&key);
		switch (key) {
			case KEY_CTRL_SHIFT:		// -------------------------shift key
					select_func=1;		// Current setting Save&load
					func_menu2();
				GetKey_DisableCatalog(&key);
				switch (key) {
					case KEY_CTRL_F1:
						SaveFile();
						select_func=0;
						break;
					case KEY_CTRL_F2:
						SaveDataF2();
						select_func=0;
						break;
					case KEY_CTRL_F3:
						SaveDataF3();
						select_func=0;
						break;
					case KEY_CTRL_F4:
						SaveDataF4();
						select_func=0;
						break;
					case KEY_CTRL_F5:
						SaveDataF5();
						select_func=0;
						break;
					case KEY_CTRL_F6:
						LoadFile(1);
						select_func=0;
						break;
					case KEY_CTRL_SETUP:		// setup
						init_menu();
						StatusArea_sub();
						update(select,select_func,1);
						break;
#ifdef OPTION_REG
					case KEY_CTRL_PRGM:
						select_func=3;			// BCR WCR edit
						func_menu4();
						break;
#endif
					case KEY_CTRL_PAGEUP:
						select=-1;
						update(select,select_func,0);
						break;

					case KEY_CHAR_LBRCKT:	// 	shift + "+" 					//	RAM WW wait +
						if( CS2WCR_WW == 0 ) CS2WCR_WW=CheckRAMwaitW(freq)-1 ;
						CS2WCR_WW+=1;
						if( CS2WCR_WW > 7 ) CS2WCR_WW=0;
						BSC.CS2WCR.BIT.WW = CS2WCR_WW ;
						update(select,select_func,1);
						break;
					case KEY_CHAR_RBRCKT:	// 	shift + "-" 					//	RAM WW wait -
						if( CS2WCR_WW == 0 ) CS2WCR_WW=CheckRAMwaitW(freq)+1 ;
						if( CS2WCR_WW <= CheckRAMwaitW(freq) ) CS2WCR_WW=1; // =WR
						CS2WCR_WW-=1;
						BSC.CS2WCR.BIT.WW = CS2WCR_WW ;
						update(select,select_func,1);
						break;
				}
				if ( select_func == 1 ) select_func=0;
				if ( select_func == 0 ) func_menu1();
				break;
				
			case KEY_CTRL_AC:
				mes_flag=0;
				select_func=0;
				StatusArea_sub();
				update(select,select_func,0);
				break;
            case KEY_CTRL_EXIT:
				mes_flag=0;
				select_func=0;
				StatusArea_sub();
				update(select,select_func,0);
                break;
			case KEY_CTRL_EXE:			// update +bench
				update(select,select_func,1);
				break;

			case KEY_CTRL_OPTN:
				select_func=wait_menu(select_func);		// wait menu
				StatusArea_sub();
				update(select,select_func,1);
				break;
			case KEY_CTRL_VARS:			//
				select_func=2;			// Register check&edit
				func_menu3();
				break;

			case KEY_CTRL_F1:
				switch (select_func) {
					case 0:
						if ((SetupData>>1)&1) if ( YesNo("Load Default")==0 ) break;
						LoadDataF1();	// nomal  default
						update(select,select_func,1);
						break;
					case 2:
						dsp_FRQCR();
						break;
#ifdef OPTION_REG
					case 3:
						dsp_BCR34_mod(freq);
						update(select,select_func,1);
						break;
#endif
				}
				break;
			case KEY_CTRL_F2:
				switch (select_func) {
					case 0:
						LoadDataF2();
						update(select,select_func,1);
						break;
					case 2:
						if ( BCRWCR ) dsp_WCR(); else dsp_BCR();
						BCRWCR=1-BCRWCR;
						break;
#ifdef OPTION_REG
					case 3:
						dsp_WCR34_mod(freq);
						update(select,select_func,1);
						break;
#endif
				}
				break;
			case KEY_CTRL_F3:
				switch (select_func) {
					case 0:
						LoadDataF3();
						update(select,select_func,1);
						break;
					case 2:
						dsp_BCR_mod(freq);
						update(select,select_func,1);
						break;
#ifdef OPTION_REG
					case 3:
						dsp_BCR5_mod(freq);
						update(select,select_func,1);
						break;
#endif
				}
				break;
			case KEY_CTRL_F4:
				switch (select_func) {
					case 0:
						LoadDataF4();
						update(select,select_func,1);
						break;
					case 2:
						dsp_WCR_mod(freq);
						update(select,select_func,1);
						break;
#ifdef OPTION_REG
					case 3:
						dsp_WCR5_mod(freq);
						update(select,select_func,1);
						break;
#endif
				}
				break;
			case KEY_CTRL_F5:
				switch (select_func) {
					case 0:
						LoadDataF5();
						update(select,select_func,1);
						break;
					case 2:
						dsp_BCR5_mod(freq);
						update(select,select_func,1);
						break;
#ifdef OPTION_REG
					case 3:
						dsp_BCR6_mod(freq);
						update(select,select_func,1);
						break;
#endif
				}
				break;
			case KEY_CTRL_F6:
				switch (select_func) {
					case 0:
				        Bench(100,1);			// bench
						Bdisp_PutDisp_DD();
						mes_flag+=1; if ( mes_flag == 3 ) mes_flag=1;
				        if ( mes_flag == 1 ) Benchmem(50);
		        		if ( mes_flag == 2 ) BenchDD(1000);
						break;
					case 2:
						dsp_WCR5_mod(freq);
						update(select,select_func,1);
						break;
#ifdef OPTION_REG
					case 3:
						dsp_WCR6_mod(freq);
						update(select,select_func,1);
						break;
#endif
				}
				break;
				
			case KEY_CTRL_UP:
				select-=1;
				if ( (select < -1) ) select+=1;
				if ( (((SetupData>>6)&1)==1) && (select < -1) ) select+=1;
				if ( (((SetupData>>6)&1)==0) && (select <  0) ) select+=1;
				update(select,select_func,0);
				break;
			case KEY_CTRL_DOWN:
				select+=1;
				if (select > 4) select=4;
				update(select,select_func,0);
				break;
				
			case KEY_CTRL_LEFT:
				switch (select) {
					case -1:	// FLL
						if( FLF <= 450 ) break;
						FLF-=1;
						CPG.FLLFRQ.BIT.FLF = FLF ;
						SetWaitm(FLF-1,FRQCR_STC,FRQCR_BFC,CS0WCR_WR,CS2WCR_WR);
						PFC_auto_up(FLF-1,FRQCR_STC,FRQCR_BFC,FRQCR_PFC);
						update(select,select_func,1);
						break;
					case 0:		// PLL
						if( FRQCR_STC <= 0 ) break;
						change_STC(FRQCR_STC-1);
						SetWaitm(FLF,FRQCR_STC-1,FRQCR_BFC,CS0WCR_WR,CS2WCR_WR);
						PFC_auto_up(FLF,FRQCR_STC-1,FRQCR_BFC,FRQCR_PFC);
						update(select,select_func,1);
						break;
					case 1:	// IFC
						if( FRQCR_IFC >= 5 ) break;
						if( FRQCR_IFC == FRQCR_PFC ) change_PFC(FRQCR_PFC+1);
						if( FRQCR_IFC == FRQCR_BFC ){change_BFC(FRQCR_BFC+1);
													 SetWaitm(FLF,FRQCR_STC,FRQCR_BFC+1,CS0WCR_WR,CS2WCR_WR);}
						if( FRQCR_IFC == FRQCR_SFC ) change_SFC(FRQCR_SFC+1);
						change_IFC(FRQCR_IFC+1);
						update(select,select_func,1);
						break;
					case 2:	// SFC
						if( FRQCR_SFC >= 5 ) break;
						if( FRQCR_SFC == FRQCR_PFC ) change_PFC(FRQCR_PFC+1);
						if( FRQCR_SFC == FRQCR_BFC ){change_BFC(FRQCR_BFC+1);
													 SetWaitm(FLF,FRQCR_STC,FRQCR_BFC+1,CS0WCR_WR,CS2WCR_WR);}
						change_SFC(FRQCR_SFC+1);
						if((FRQCR_SFC - FRQCR_IFC) >= 1 ) change_IFC(FRQCR_IFC+1);
						update(select,select_func,1);
						break;
					case 3:	// BFC
						if( FRQCR_BFC >= 5 ) break;
						if( FRQCR_BFC == FRQCR_PFC ) change_PFC(FRQCR_PFC+1);
						change_BFC(FRQCR_BFC+1);
						SetWaitm(FLF,FRQCR_STC,FRQCR_BFC+1,CS0WCR_WR,CS2WCR_WR);
						if((FRQCR_BFC - FRQCR_SFC) >= 2 ) change_SFC(FRQCR_SFC+1);
						if((FRQCR_BFC - FRQCR_IFC) >= 3 ) change_IFC(FRQCR_IFC+1);
						update(select,select_func,1);
						break;
					case 4:	// PFC
						if( FRQCR_PFC >= 5 ) break;
						change_PFC(FRQCR_PFC+1);
						if((FRQCR_PFC - FRQCR_BFC) >= 3 ){change_BFC(FRQCR_BFC+1);
														  SetWaitm(FLF,FRQCR_STC,FRQCR_BFC+1,CS0WCR_WR,CS2WCR_WR);}
						if((FRQCR_PFC - FRQCR_SFC) >= 3 ) change_SFC(FRQCR_SFC+1);
						if((FRQCR_PFC - FRQCR_IFC) >= 4 ) change_IFC(FRQCR_IFC+1);
						update(select,select_func,1);
						break;
				}
				break;


			case KEY_CTRL_RIGHT:
				switch (select) {
					case -1:	// FLL
						if(FLF < FLFMAX) {
							if  (CheckFC(FLF+1,FRQCR_STC,-1       ,MaxFreq_PLL) == 0) break;		// PLL freq
							if  (CheckFC(FLF+1,FRQCR_STC,FRQCR_IFC,MaxFreq_IFC) == 0) break;		// IFC freq
							if  (CheckFC(FLF+1,FRQCR_STC,FRQCR_SFC,MaxFreq_SFC) == 0) break;		// SFC freq
							freq=CheckFC(FLF+1,FRQCR_STC,FRQCR_BFC,MaxFreq_BFC);					// BFC freq
							if (freq == 0) break;
							SetROMwait(freq,CS0WCR_WR);
							SetRAMwait(freq,CS2WCR_WR);
							PFC_auto_down(FLF+1,FRQCR_STC,FRQCR_PFC);
							FLF+=1;
							CPG.FLLFRQ.BIT.FLF = FLF ;
							update(select,select_func,1);
							break;
						}
						break;
						
					case 0:	// PLL
						if(FRQCR_STC < PLLMAX) {
							if ( FRQCR_STC < 1+FRQCR_BFC ) { change_STC(FRQCR_STC+1); update(select,select_func,1); break; }
							if (CheckFC(FLF,FRQCR_STC+1,-1       ,MaxFreq_PLL) == 0) break;			// PLL freq
							if (CheckFC(FLF,FRQCR_STC+1,FRQCR_IFC,MaxFreq_IFC) == 0) break;			// IFC freq
							if (CheckFC(FLF,FRQCR_STC+1,FRQCR_SFC,MaxFreq_SFC) == 0) break;			// SFC freq
							freq=CheckFC(FLF,FRQCR_STC+1,FRQCR_BFC,MaxFreq_BFC);					// BFC freq
							if (freq == 0) break;
							SetROMwait(freq,CS0WCR_WR);
							SetRAMwait(freq,CS2WCR_WR);
							PFC_auto_down(FLF,FRQCR_STC+1,FRQCR_PFC);
							change_STC(FRQCR_STC+1);
							update(select,select_func,1);
							break;
						}
						break;
					case 1:	// IFC
						if ( FRQCR_IFC == 0 ) break;
						if ( CheckFC(FLF,FRQCR_STC,FRQCR_IFC-1,MaxFreq_IFC) == 0) break;			// IFC freq
						if ( FRQCR_IFC <=  FRQCR_SFC ) {
							change_IFC(FRQCR_IFC-1);
							if ( FRQCR_IFC < FRQCR_SFC ) change_SFC(FRQCR_SFC-1);
							if ((FRQCR_BFC - FRQCR_IFC) >= 3) UpBFC(FLF,FRQCR_STC,FRQCR_BFC-1,CS0WCR_WR,CS2WCR_WR);
							if ((FRQCR_PFC - FRQCR_IFC) >= 4) UpPFC(FLF,FRQCR_STC,FRQCR_PFC-1);
							update(select,select_func,1);
							break;
						}
						break;
					case 2:	// SFC
						if ( FRQCR_SFC == 0 ) break; 
						if ( CheckFC(FLF,FRQCR_STC,FRQCR_SFC-1,MaxFreq_SFC) == 0) break;			// SFC freq
						if ( FRQCR_SFC >=  FRQCR_IFC ) {
							if( FRQCR_SFC == FRQCR_IFC ) change_IFC(FRQCR_IFC-1);
							change_SFC(FRQCR_SFC-1);
							if((FRQCR_BFC - FRQCR_SFC) >= 2) UpBFC(FLF,FRQCR_STC,FRQCR_BFC-1,CS0WCR_WR,CS2WCR_WR);
							if((FRQCR_PFC - FRQCR_SFC) >= 3) UpPFC(FLF,FRQCR_STC,FRQCR_PFC-1);
							update(select,select_func,1);
							break;
						}
						break;
					case 3:	// BFC
						if( FRQCR_BFC == 0 ) break;
						if( CheckFC(FLF,FRQCR_STC,FRQCR_BFC-1,MaxFreq_BFC) == 0) break;			// BFC freq
						if( FRQCR_BFC == FRQCR_SFC ) {
							if( FRQCR_BFC == FRQCR_IFC ) change_IFC(FRQCR_IFC-1);
							change_SFC(FRQCR_SFC-1);
						}	
						UpBFC(FLF,FRQCR_STC,FRQCR_BFC-1,CS0WCR_WR,CS2WCR_WR);
						if((FRQCR_PFC - FRQCR_BFC) >= 3) UpPFC(FLF,FRQCR_STC,FRQCR_PFC-1);
						update(select,select_func,1);
						break;
					case 4:	// PFC
						if ( FRQCR_PFC == 0 ) break;
						if ( CheckFC(FLF,FRQCR_STC,FRQCR_PFC-1,MaxFreq_PFC) == 0) break;			// PFC freq
						if ( FRQCR_PFC == FRQCR_BFC ) {
							if( FRQCR_PFC == FRQCR_SFC ) {
								if( FRQCR_PFC == FRQCR_IFC ) change_IFC(FRQCR_IFC-1);
								change_SFC(FRQCR_SFC-1);
							}
							UpBFC(FLF,FRQCR_STC,FRQCR_BFC-1,CS0WCR_WR,CS2WCR_WR);
						}
						change_PFC(FRQCR_PFC-1);
						update(select,select_func,1);
						break;
				}
				break;


			case KEY_CHAR_MULT:	//		"*"								//	ROM wait +
				if(CS0WCR_WR < WAIT24) {
					BSC.CS0WCR.BIT.WR= (CS0WCR_WR+1) ;
					update(select,select_func,1);
					break;
				}
				break;	
			case KEY_CHAR_DIV:	// 		"/" 							//	ROM wait -
				if(CS0WCR_WR > WAIT_0) {
					if ( CheckROMwait(freq) > (CS0WCR_WR) ) break;
					if ( CheckROMwait(freq) > (CS0WCR_WR-1) ) {
						if ( freq > MaxFreq_ROMwait[CS0WCR_WR-1]*105/100 ) break;
//						if (YesNo("ROM wait - dec")==0) break;
						MSG1("ROM Check ");
						i=ROMtestn(FLF,CS0WCR_WR);
						MSGpop();
						if ( i ) { 		// error
							ErrorADRS("ROM READ Error",i);
							break;
						}
						MaxFreq_ROMwait[CS0WCR_WR-1]  =freq;
						MaxFreq_ROMwait0[CS2WCR_WR-1] =freq*100/(100-ROMmargin());
					}
					BSC.CS0WCR.BIT.WR = (CS0WCR_WR-1);
					update(select,select_func,1);
					break;
				}
				break;				
				
			case KEY_CHAR_PLUS:		//		"+"							//	RAM wait +
				if(CS2WCR_WR < WAIT24) {
					BSC.CS2WCR.BIT.WR = (CS2WCR_WR+1);
					update(select,select_func,1);
					break;
				}
				break;
			case KEY_CHAR_MINUS:	//		"-"							//	RAM wait -
				if(CS2WCR_WR > WAIT_0) {
					if ( CheckRAMwait(freq) > (CS2WCR_WR) ) break;
					if ( CheckRAMwait(freq) > (CS2WCR_WR-1) ) {
						if ( freq > MaxFreq_RAMwait[CS2WCR_WR-1]*105/100 ) break;
//						if (YesNo("RAM wait - dec")==0) break;
						MSG1("RAM Check ");
						i=RAMtestn(FLF,CS2WCR_WR);
						MSGpop();
						if ( i ) { 		// error
							ErrorADRS("RAM READ Error",i);
							break;
						}
						MaxFreq_RAMwait[CS2WCR_WR-1]  =freq;
						MaxFreq_RAMwait0[CS2WCR_WR-1] =freq*100/(100-RAMmargin());
					}
					BSC.CS2WCR.BIT.WR = (CS2WCR_WR-1);
					update(select,select_func,1);
					break;
				}
				break;			

			default:
				break;
		}
	}
 
	return 0;
}



