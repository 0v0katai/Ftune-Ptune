/*
===============================================================================

 Ftune is SH7337/SH7355 CPG&BSC tuning utility for fx-9860G/GII series  v1.40

 copyright(c)2014,2015,2019 by sentaro21
 e-mail sentaro21@pm.matrix.jp

===============================================================================
*/

#include "fxlib.h"
#include "timer.h"
#include "stdio.h"
#include "string.h"

#include "common.h"
#include "ftune.h"
#include "ftune_sub.h"
#include "ftune_benchmark.h"
#include "ftune_memcheck.h"
#include "ftune_bscmod.h"
#include "ftune_data.h"

//---------------------------------------------------------------------------------------------

void about(){
	unsigned int key;
	SaveDisp(SAVEDISP_PAGE1);
	PopUpWin(5);
	locate(3,2);Print((unsigned char *)"Ftune v1.40 ");
	locate(3,3);Print((unsigned char *)"Copyright(C)2019");
	locate(3,4);Print((unsigned char *)"     by sentaro21");
	locate(3,6);Print((unsigned char *)"   Press:[EXIT]");
	Bdisp_PutDisp_DD();
	GetKey(&key);	
	RestoreDisp(SAVEDISP_PAGE1);
	Bdisp_PutDisp_DD();
}

//---------------------------------------------------------------------------------------------

void cls7(void){
	locate( 1, 1 );PrintLine((unsigned char *) " ", 21);
	locate( 1, 2 );PrintLine((unsigned char *) " ", 21);
	locate( 1, 3 );PrintLine((unsigned char *) " ", 21);
	locate( 1, 4 );PrintLine((unsigned char *) " ", 21);
	locate( 1, 5 );PrintLine((unsigned char *) " ", 21);
	locate( 1, 6 );PrintLine((unsigned char *) " ", 21);
	locate( 1, 7 );PrintLine((unsigned char *) " ", 21);
}

void dsp_FRQCR(){
	cls7();	
	HexPrintXY(1,1,"FRQCR    :",CPG.FRQCR.WORD);
	
	HexPrintXY(1,5,"CCR1     :",*(unsigned int*)0xFFFFFFEC);
	HexPrintXY(1,6,"CCR2     :",*(unsigned int*)0xA40000B0);
	HexPrintXY(1,7,"CCR3     :",*(unsigned int*)0xA40000B4);
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
	HexPrintXY(1,1,"PTEH   :%08X",*(unsigned int*)0xFF000000);
	HexPrintXY(1,2,"PTEL   :%08X",*(unsigned int*)0xFF000004);
	HexPrintXY(1,3,"TTB    :%08X",*(unsigned int*)0xFF000008);
	HexPrintXY(1,4,"TEA    :%08X",*(unsigned int*)0xFF00000C);
	HexPrintXY(1,5,"MMUCR  :%08X",*(unsigned int*)0xFF000010);
	HexPrintXY(1,6,"PTEA   :%08X",*(unsigned int*)0xFF000034);
	HexPrintXY(1,7,"PASCR  :%08X",*(unsigned int*)0xFF000070);
	HexPrintXY(1,8,"IRMCR  :%08X",*(unsigned int*)0xFF000078);
	Bdisp_PutDisp_DD();
}
*/

//---------------------------------------------------------------------------------------------
int	mes_flag=0;		// message select

void update(int select,int select_func,int modify) {
	int	FRQCR_STC,FRQCR_IFC,FRQCR_BFC,FRQCR_PFC;
	int	CS0WCR_WR,CS2WCR_WR,CS2WCR_WW;

    int count;
	char buf[20];
    char *divs[] ={"1/1  ","1/2  ","1/3  ","1/4  "};
    char *waits[]={"0 ","1 ","2 ","3 ","4 ","5 ","6 ","8 ","10","12","14","18","24"};
	char *waitW[]={"=R","0 ","1 ","2 ","3 ","4 ","5 ","6 "};
    char *selects[]={
		" PLL multiplication Ratio        ",
		" CPU core       upto 118 MHz   ",
		" ",
		" Bus Clock      upto  60 MHz   ",
		" Peripheral     upto  30 MHz   "};

	int freq;
	
	FRQCR_STC = CPG.FRQCR.BIT.STC ;
	FRQCR_IFC = CPG.FRQCR.BIT.IFC ;
	FRQCR_BFC = 0 ;
	FRQCR_PFC = CPG.FRQCR.BIT.PFC ;
	CS0WCR_WR = BSC.CS0WCR.BIT.WR ;
	CS2WCR_WR = BSC.CS2WCR.BIT.WR ;
	CS2WCR_WW = BSC.CS2WCR.BIT.WW ;

//	Bdisp_AllClr_VRAM();
	locate(1,1); PrintRLine((unsigned char *)" Ftune for SH3         ",21);
	sprintf(buf," %4.2fv",(float)BatteryStatus()/100);
	PrintMini(17*6-4,2,(unsigned char *)buf, MINI_REV);
//	PrintMini(17*6,2,(unsigned char *)"v1.40", MINI_REV);

	locate(1,2); PrintLine((unsigned char *) " ", 21);
	locate(1,3); PrintLine((unsigned char *) " ", 21);
	locate(1,4); PrintLine((unsigned char *) " ", 21);
	locate(1,5); PrintLine((unsigned char *) " ", 21);
	locate(1,6); PrintLine((unsigned char *) " ", 21);
	PrintXY(0*6,1*8,(unsigned char *)" PLL:",select==0?1:0);
	PrintXY(0*6,2*8,(unsigned char *)" IFC:",select==1?1:0);

	PrintXY(0*6,4*8,(unsigned char *)" BFC:",select==3?1:0);
	PrintXY(0*6,5*8,(unsigned char *)" PFC:",select==4?1:0);
	locate(1,select+2);Print((unsigned char *)points);
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
	
	sprintf(buf,"\xA9%d ",FRQCR_STC+1);
	PrintXY(5*6-1,1*8,(unsigned char *)buf,0);
	freq=freq_sub(FRQCR_STC,0); sprintf(buf,"%7.2f MHz",(float)freq/100);
	PrintMini(14*6-8,1*8+1,(unsigned char *)buf, MINI_OVER);
	if (FRQCR_STC==PLL_1x)
		PrintMini(12*6+1,1*8+1,(unsigned char *)"*", MINI_OVER);
	
	PrintXY(5*6-1,2*8,(unsigned char *)divs[FRQCR_IFC],0);
	freq=freq_sub(FRQCR_STC,FRQCR_IFC); sprintf(buf,"%6.2fMHz",(float)freq/100);
	locate( 9,3); if ( freq>26666 ) PrintRev((unsigned char *)"CPU ");
							else	Print((unsigned char *)"CPU ");
	locate(13,3); if ( freq>26666 ) PrintRev((unsigned char *)buf);
							else	Print((unsigned char *)buf);
	if (freq==freq_sub(PLL_1x,DIV_1)) 
		PrintMini(12*6+1,2*8+1,(unsigned char *)"*", MINI_OVER);


	freq=freq_sub(0,FRQCR_BFC); sprintf(buf,"%7.2f MHz",(float)freq/100);
	PrintXY(5*6-1,4*8,(unsigned char *)divs[FRQCR_STC],0);
	PrintMini(14*6-8,4*8+2,(unsigned char *)buf,  freq>6666?MINI_REV:MINI_OVER);
	if ( (freq==freq_sub(PLL_1x,DIV_1)) && (BSC.CS0BCR.LONG==CS0BCR_default) && (BSC.CS2BCR.LONG==CS2BCR_default)
	  									&& (BSC.CS0WCR.LONG==CS0WCR_default) && (BSC.CS2WCR.LONG==CS2WCR_default) )
		PrintMini(12*6+1,4*8+2,(unsigned char *)"*", MINI_OVER);
	

	freq=freq_sub(FRQCR_STC,FRQCR_PFC); sprintf(buf,"%7.2f MHz",(float)freq/100);
	PrintXY(5*6-1,5*8,(unsigned char *)divs[FRQCR_PFC],0);
	PrintMini(14*6-8,5*8+2,(unsigned char *)buf, freq>3333?MINI_REV:MINI_OVER);
	if (freq==freq_sub(PLL_1x,DIV_2))
		PrintMini(12*6+1,5*8+2,(unsigned char *)"*", MINI_OVER);


	PrintMini( 8*6+0,4*8-6,(unsigned char *)"    ", CS0WCR_WR!=WAIT_2?MINI_REV:MINI_OVER);
	PrintMini( 8*6+1,4*8-6,(unsigned char *)"RoR ", CS0WCR_WR!=WAIT_2?MINI_REV:MINI_OVER);
	PrintMini(10*6+4,4*8-6,(unsigned char *)waits[CS0WCR_WR],CS0WCR_WR!=WAIT_2?MINI_REV:MINI_OVER);
	PrintMini( 8*6+0,4*8+2,(unsigned char *)"    ", CS2WCR_WR!=WAIT_2?MINI_REV:MINI_OVER);
	PrintMini( 8*6+1,4*8+2,(unsigned char *)"RaR ", CS2WCR_WR!=WAIT_2?MINI_REV:MINI_OVER);
	PrintMini(10*6+4,4*8+2,(unsigned char *)waits[CS2WCR_WR],CS2WCR_WR!=WAIT_2?MINI_REV:MINI_OVER);
	PrintMini( 8*6+0,4*8+9,(unsigned char *)"    ", CS2WCR_WW!=0?MINI_REV:MINI_OVER);
	PrintMini( 8*6+1,4*8+9,(unsigned char *)"RaW ", CS2WCR_WW!=0?MINI_REV:MINI_OVER);
	PrintMini(10*6+4,4*8+9,(unsigned char *)waitW[CS2WCR_WW],CS2WCR_WW!=0?MINI_REV:MINI_OVER);

	switch	(mes_flag) {
		case 0:
			locate(1,7); PrintLine((unsigned char *) " ", 21);
			PrintMini(0*6,6*8+1,(unsigned char *)selects[select], MINI_REV);
			break;
		case 1:
			if ( modify ) {
				Bench(25,0);
				Bench(100,1);
				Benchmem(25);
				Benchmem(50);
			}
			break;
		case 2:
			if ( modify ) {
				Bench(25,0);
				Bench(100,1);
				BenchDD(1000);
			}
			break;
		default:
			break;
	}
	
	Bdisp_PutDisp_DD();
		
		
}


int AddIn_main(int isAppli, unsigned short OptionNum)
{
	int	FRQCR_STC,FRQCR_IFC,FRQCR_PFC;
	int	CS0WCR_WR,CS2WCR_WR,CS2WCR_WW;

    unsigned int key;
    int count,i;
	int select=0;
	int select_func=0;
	int freq;
	int BCRWCR=0;

	Emu_check();
	LoadFile(0);	// Startup auto load
	Bdisp_AllClr_VRAM();
	if ( CPU_check() == 0 ) { ErrorMSG("Not SH3 CPU !!!",CPU_check()); return 1; }
	freq_check();	// clock check
	update(select,select_func,1);

	while(1) {
		FRQCR_STC = CPG.FRQCR.BIT.STC ;
		FRQCR_IFC = CPG.FRQCR.BIT.IFC ;
		FRQCR_PFC = CPG.FRQCR.BIT.PFC ;
		CS0WCR_WR = BSC.CS0WCR.BIT.WR ;
		CS2WCR_WR = BSC.CS2WCR.BIT.WR ;
		CS2WCR_WW = BSC.CS2WCR.BIT.WW ;

		GetKey(&key);
		switch (key) {
			case KEY_CTRL_SHIFT:		// -------------------------shift key
					select_func=1;		// Current setting Save&load
					func_menu2();
				GetKey(&key);
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
					case KEY_CTRL_SETUP:		//
						about();
						break;
#ifdef OPTION_REG
					case KEY_CTRL_PRGM:
						select_func=3;			// BCR WCR edit
						func_menu4();
						break;
#endif
						
					case KEY_CHAR_LBRCKT:	// 	shift + "+" 					//	RAM WW wait +
						CS2WCR_WW+=1;
						if( CS2WCR_WW > 7 ) CS2WCR_WW=0;
						if( CS2WCR_WW == 1 ) {
							MSG1("RAM WW Check ");
							i=RAMtestn(CS2WCR_WW+1);
							MSGpop();
							if ( i ) { 		// error
								ErrorADRS("RAM R/W Error",i);
								break;
							}
						}
						BSC.CS2WCR.BIT.WW = CS2WCR_WW ;
						update(select,select_func,1);
						break;
					case KEY_CHAR_RBRCKT:	// 	shift + "-" 					//	RAM WW wait -
						CS2WCR_WW-=1;
						if( CS2WCR_WW < 1 ) CS2WCR_WW=1;
						if( CS2WCR_WW == 1 ) {
							MSG1("RAM WW Check ");
							i=RAMtestn(CS2WCR_WW+1);
							MSGpop();
							if ( i ) { 		// error
								ErrorADRS("RAM R/W Error",i);
								break;
							}
						}
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
				Bdisp_AllClr_VRAM();
				update(select,select_func,0);
				break;
            case KEY_CTRL_EXIT:
				mes_flag=0;
				select_func=0;
				Bdisp_AllClr_VRAM();
				update(select,select_func,0);
                break;
			case KEY_CTRL_EXE:			// update
				update(select,select_func,1);
				break;

			case KEY_CTRL_VARS:			//
				select_func=2;			// Register check&edit
				func_menu3();
				break;


			case KEY_CTRL_F1:
				switch (select_func) {
					case 0:
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
				        Bench(25,0);
				        Bench(100,1);			// bench
						Bdisp_PutDisp_DD();
						mes_flag+=1; if ( mes_flag == 3 ) mes_flag=1;
				        if ( mes_flag == 1 ) { Benchmem(25); Benchmem(50); }
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
				if (select == 3) select=1;
				if (select <  0) select=0;
				update(select,select_func,0);
				Bdisp_PutDisp_DD();
				break;
			case KEY_CTRL_DOWN:
				select+=1;
				if (select == 2) select=4;
				if (select >  4) select=4;
				update(select,select_func,0);
				Bdisp_PutDisp_DD();
				break;
				
			case KEY_CTRL_LEFT:
				switch (select) {
					case 0:	// PLL
						switch (FRQCR_STC) {
							case 0: // x1
								break;
							case 1: // x2
								FRQCR_STC=0;  // x2 -> x1
								FRQCR_IFC=0;  //    -> 1/1
								change_FRQCR(FRQCR_STC,FRQCR_IFC,FRQCR_PFC);
								update(select,select_func,1);
								break;
							case 2: // x3
								FRQCR_STC=1;  // x3 -> x2
								FRQCR_IFC=0;  //    -> 1/1
								FRQCR_PFC=1;  //    -> 1/2
								change_FRQCR(FRQCR_STC,FRQCR_IFC,FRQCR_PFC);
								update(select,select_func,1);
								break;
							case 3: // x4
								FRQCR_STC=2;  // x4 -> x3
								FRQCR_IFC=0;  //    -> 1/1
								FRQCR_PFC=2;  //    -> 1/3
								change_FRQCR(FRQCR_STC,FRQCR_IFC,FRQCR_PFC);
								update(select,select_func,1);
								break;
						}
						break;
					case 1:	// IFC
						if( FRQCR_IFC >= 2 ) break;
						switch (FRQCR_STC) {
							case 0: // x1
								break;
							case 1: // x2
								if( FRQCR_IFC == 0 ) FRQCR_IFC=1;  // 1/1 -> 1/2
								change_FRQCR(FRQCR_STC,FRQCR_IFC,FRQCR_PFC);
								update(select,select_func,1);
								break;
							case 2: // x3
								break;
							case 3: // x4
								if( FRQCR_IFC == 1 ) FRQCR_IFC=3;  // 1/2 -> 1/4
								if( FRQCR_IFC == 0 ) FRQCR_IFC=1;  // 1/1 -> 1/2
								change_FRQCR(FRQCR_STC,FRQCR_IFC,FRQCR_PFC);
								update(select,select_func,1);
								break;
						}
						break;
					case 2:	// SFC
						break;
					case 3:	// BFC
						break;
					case 4:	// PFC
						if( FRQCR_PFC >= 2 ) break;
						switch (FRQCR_STC) {
							case 0: // x1
								if( FRQCR_PFC == 1 ) FRQCR_PFC=3;  // 1/2 -> 1/4
								if( FRQCR_PFC == 0 ) FRQCR_PFC=1;  // 1/1 -> 1/2
								change_FRQCR(FRQCR_STC,FRQCR_IFC,FRQCR_PFC);
								update(select,select_func,1);
								break;
							case 1: // x2
								if( FRQCR_PFC == 1 ) FRQCR_PFC=3;  // 1/2 -> 1/4
								change_FRQCR(FRQCR_STC,FRQCR_IFC,FRQCR_PFC);
								update(select,select_func,1);
								break;
							case 2: // x3
								break;
							case 3: // x4
								break;
						}
						break;
				}
				break;


			case KEY_CTRL_RIGHT:
				switch (select) {
					case 0:	// PLL
						switch (FRQCR_STC) {
							case 0: // x1
								FRQCR_STC=1;  // x1 -> x2
								if( FRQCR_PFC==0 ) FRQCR_PFC=1;  //  1/1  -> 1/2
								change_FRQCR(FRQCR_STC,FRQCR_IFC,FRQCR_PFC);
								update(select,select_func,1);
								break;
							case 1: // x2
								FRQCR_STC=2;  // x2 -> x3
								FRQCR_IFC=0;  //    -> 1/1
								FRQCR_PFC=2;  //    -> 1/3
								change_FRQCR(FRQCR_STC,FRQCR_IFC,FRQCR_PFC);
								update(select,select_func,1);
								break;
							case 2: // x3
								FRQCR_STC=3;  // x3 -> x4
								FRQCR_IFC=0;  //    -> 1/1
								FRQCR_PFC=3;  //    -> 1/4
								change_FRQCR(FRQCR_STC,FRQCR_IFC,FRQCR_PFC);
								update(select,select_func,1);
								break;
							case 3: // x4
								break;
						}
						break;
					case 1:	// IFC
						if( FRQCR_IFC == 0 ) break;
						switch (FRQCR_STC) {
							case 0: // x1
								break;
							case 1: // x2
								if( FRQCR_IFC == 1 ) FRQCR_IFC=0;  // 1/2 -> 1/1
								change_FRQCR(FRQCR_STC,FRQCR_IFC,FRQCR_PFC);
								update(select,select_func,1);
								break;
							case 2: // x3
								break;
							case 3: // x4
								if( FRQCR_IFC == 1 ) FRQCR_IFC=0;  // 1/2 -> 1/1
								if( FRQCR_IFC == 3 ) FRQCR_IFC=1;  // 1/4 -> 1/2
								change_FRQCR(FRQCR_STC,FRQCR_IFC,FRQCR_PFC);
								update(select,select_func,1);
								break;
						}
						break;
					case 2:	// SFC
						break;
					case 3:	// BFC
						break;
					case 4:	// PFC
						if( FRQCR_PFC == 0 ) break;
						switch (FRQCR_STC) {
							case 0: // x1
								if( FRQCR_PFC == 1 ) FRQCR_PFC=0;  // 1/2 -> 1/1
								if( FRQCR_PFC == 3 ) FRQCR_PFC=1;  // 1/4 -> 1/2
								change_FRQCR(FRQCR_STC,FRQCR_IFC,FRQCR_PFC);
								update(select,select_func,1);
								break;
							case 1: // x2
								if( FRQCR_PFC == 3 ) FRQCR_PFC=1;  // 1/4 -> 1/2
								change_FRQCR(FRQCR_STC,FRQCR_IFC,FRQCR_PFC);
								update(select,select_func,1);
								break;
							case 2: // x3
								break;
							case 3: // x4
								break;
						}
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
					if ( CS0WCR_WR < WAIT_3 ) {
						MSG1("ROM Check ");
						i=ROMtestn(CS0WCR_WR);
						MSGpop();
						if ( i ) { 		// error
							ErrorADRS("ROM READ Error",i);
							break;
						}
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
					if ( CS2WCR_WR < WAIT_3 ) {
						MSG1("RAM Check ");
						i=RAMtestn(CS2WCR_WR);
						MSGpop();
						if ( i ) { 		// error
							ErrorADRS("RAM READ Error",i);
							break;
						}
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

    return 1;
}

//****************************************************************************
//**************  Please do not change the following source.  ****************
//****************************************************************************

#pragma section _BR_Size
unsigned long BR_Size;
#pragma section

#pragma section _TOP

int InitializeSystem(int isAppli, unsigned short OptionNum)
{
    return INIT_ADDIN_APPLICATION(isAppli, OptionNum);
}

#pragma section
