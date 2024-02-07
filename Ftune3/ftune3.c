/*
===============================================================================

 Ftune3 is SH7305 CPG&BSC tuning utility for fx-9860GIII / Graph 35+E II  v2.40

 copyright(c)2014,2015,2016,2017,2018,2019 by sentaro21
 e-mail sentaro21@pm.matrix.jp

===============================================================================
*/

#include "fxlib.h"
#include "timer.h"
#include "stdio.h"
#include "string.h"

#include "common.h"
#include "ftune3.h"
#include "ftune3_sub.h"
#include "ftune3_benchmark.h"
#include "ftune3_memcheck.h"
#include "ftune3_bscmod.h"
#include "ftune3_data.h"
#include "ftune3_setup.h"

//---------------------------------------------------------------------------------------------
void about(){
	unsigned int key;
	SaveDisp(SAVEDISP_PAGE1);
	PopUpWin(5);
	locate(3,2);Print((unsigned char *)"Ftune3 v2.40 ");
	locate(3,3);Print((unsigned char *)"Copyright(C)2020");
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
int	mes_flag=0;		// message select

void update(int select,int select_func,int modify) {
	int C_ROM,C_RAM;
    int count;
	char buf[20];
    char *divs[] ={"1/2","1/4","1/8","/16","/32","/64"};
    char *waits[]={"0 ","1 ","2 ","3 ","4 ","5 ","6 ","8 ","10","12","14","18","24"};
	char *waitW[]={"=R","0 ","1 ","2 ","3 ","4 ","5 ","6 "};
    char *selects[]={
		" FLL mult Ratio    upto x1023  ",
		" PLL mult Ratio   upto 750 MHz   ",
		" CPU core        upto 250 MHz  ",
		" SuperHyway     upto 150 MHz     ",
		" Bus Clock       upto 120 MHz    ",
		" Peripheral      upto  30 MHz    "};

	int	_FRQCR,FRQCR_STC,FRQCR_IFC,FRQCR_SFC,FRQCR_BFC,FRQCR_PFC;
	int	CS0WCR_WR,CS2WCR_WR,CS2WCR_WW;
	int	FLF;
	int freq;
	
	FLF       = CPG.FLLFRQ.BIT.FLF ;
	FRQCR_STC = CPG.FRQCRA.BIT.STC ;
	FRQCR_IFC = CPG.FRQCRA.BIT.IFC ;
	FRQCR_SFC = CPG.FRQCRA.BIT.SFC ;
	FRQCR_BFC = CPG.FRQCRA.BIT.BFC ;
	FRQCR_PFC = CPG.FRQCRA.BIT.PFC ;
	CS0WCR_WR = BSC.CS0WCR.BIT.WR ;
	CS2WCR_WR = BSC.CS2WCR.BIT.WR ;
	CS2WCR_WW = BSC.CS2WCR.BIT.WW ;

	locate(1,2); PrintLine((unsigned char *) " ", 21);
	locate(1,3); PrintLine((unsigned char *) " ", 21);
	locate(1,4); PrintLine((unsigned char *) " ", 21);
	locate(1,5); PrintLine((unsigned char *) " ", 21);
	locate(1,6); PrintLine((unsigned char *) " ", 21);
	if (select == -1){
		locate(1,1); PrintLine((unsigned char *) " ", 21);
		PrintXY(0*6,0*8,(unsigned char *)" FLL:",select==-1?1:0);
	} else {
		locate(1,1); PrintRLine((unsigned char *)" Ftune3 for SH4A       ",21);
		locate(13,1); 
		if ( IsG3() ) {
			if ( Is35E2() ) PrintRev((unsigned char *)"35E2");
			else 			PrintRev((unsigned char *)"GIII");
		}
		if (SetupData & 128) {
					sprintf(buf,"%4.2fv",(float)MainBatteryPercentage()/100);
					PrintMini(17*6,2,(unsigned char *)buf, MINI_REV);
		}	else	PrintMini(17*6,2,(unsigned char *)"v2.40 ", MINI_REV);

	}
	PrintXY(0*6,1*8,(unsigned char *)" PLL:",select==0?1:0);
	PrintXY(0*6,2*8,(unsigned char *)" IFC:",select==1?1:0);
	PrintXY(0*6,3*8,(unsigned char *)" SFC:",select==2?1:0);
	PrintXY(0*6,4*8,(unsigned char *)" BFC:",select==3?1:0);
	PrintXY(0*6,5*8,(unsigned char *)" PFC:",select==4?1:0);
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
	locate(1,select+2);Print((unsigned char *)points);
	if (select == -1){
		sprintf(buf,"\xA9%d ",FLF); PrintXY(5*6-1,0*8,(unsigned char *)buf,0);
		freq=(FLF*32768/100/2+50)/100;; sprintf(buf,"%7.2f MHz",(float)freq/100);		// FLL freq
		PrintMini(14*6-8,0*8+1,(unsigned char *)buf, FLF>=1024?MINI_REV:MINI_OVER);
		if (FLF==FLF_default)
			PrintMini(12*6+1,0*8+1,(unsigned char *)"*", MINI_OVER);
	}
	sprintf(buf,"\xA9%d ",FRQCR_STC+1);PrintXY(5*6-1,1*8,(unsigned char *)buf,0);
	freq=freq_sub(FLF,FRQCR_STC,-1); sprintf(buf,"%7.2f MHz",(float)freq/100);			// PLL freq
	PrintMini(14*6-8,1*8+1,(unsigned char *)buf, freq>=75000?MINI_REV:MINI_OVER);
	if ( (FRQCR_STC==PLL_16x) && ( FLF == FLF_default ) )
		PrintMini(12*6+1,1*8+1,(unsigned char *)"*", MINI_OVER);
	
	PrintXY(5*6-1,2*8,(unsigned char *)divs[FRQCR_IFC],0);
	freq=freq_sub(FLF,FRQCR_STC,FRQCR_IFC); sprintf(buf,"%6.2fMHz",(float)freq/100);	// CPU freq
	PrintXYR(8*6,2*8,"CPU ",freq>25100?1:0);
	PrintXYR(12*6,2*8,buf,freq>25100?1:0);
	if ( IsG3() ) {
		if ( (freq==freq_sub(FLF,PLL_16x,DIV_4)) && ( FLF == FLF_default ) )
			PrintMini(12*6+1,2*8+1,(unsigned char *)"*", MINI_OVER);
	} else {
		if ( (freq==freq_sub(FLF,PLL_16x,DIV_8)) && ( FLF == FLF_default ) )
			PrintMini(12*6+1,2*8+1,(unsigned char *)"*", MINI_OVER);
	}

	PrintXY(5*6-1,3*8,(unsigned char *)divs[FRQCR_SFC],0);
	freq=freq_sub(FLF,FRQCR_STC,FRQCR_SFC); sprintf(buf,"%7.2f MHz",(float)freq/100);	// SHW freq
	PrintMini(14*6-8,3*8+2,(unsigned char *)buf,  freq>15000?MINI_REV:MINI_OVER);
	if ( (freq==freq_sub(FLF,PLL_16x,DIV_8)) && ( FLF == FLF_default ) )
		PrintMini(12*6+1,3*8+2,(unsigned char *)"*", MINI_OVER);
	

	freq=freq_sub(FLF,FRQCR_STC,FRQCR_BFC); sprintf(buf,"%7.2f MHz",(float)freq/100);	// bus freq
//	if ( IsG3() ) {
	if ( ( BSC.CS5AWCR.BIT.SW < 2 ) && ( freq > 3000 ) ) BSC.CS5AWCR.BIT.SW = 2 ;	//	SW:2.5
	if ( ( BSC.CS5AWCR.BIT.SW < 3 ) && ( freq > 5000 ) ) BSC.CS5AWCR.BIT.SW = 3 ;	//	SW:3.5
//	}

	PrintXY(5*6-1,4*8,(unsigned char *)divs[FRQCR_BFC],0);
	PrintMini(14*6-8,4*8+2,(unsigned char *)buf,  freq>12000?MINI_REV:MINI_OVER);
	if ( IsG3() ) {
		if ( (freq==freq_sub(FLF_default,PLL_16x,DIV_8)) && (BSC.CS0BCR.LONG==CS0BCR_default2) && (BSC.CS2BCR.LONG==CS2BCR_default2)
		   						&& (FLF==FLF_default)    && (BSC.CS0WCR.LONG==CS0WCR_default2) && (BSC.CS2WCR.LONG==CS2WCR_default2) )
			PrintMini(12*6+1,4*8+2,(unsigned char *)"*", MINI_OVER);
	} else {
		if ( (freq==freq_sub(FLF_default,PLL_16x,DIV_8)) && (BSC.CS0BCR.LONG==CS0BCR_default) && (BSC.CS2BCR.LONG==CS2BCR_default)
		   						&& (FLF==FLF_default)    && (BSC.CS0WCR.LONG==CS0WCR_default) && (BSC.CS2WCR.LONG==CS2WCR_default) )
			PrintMini(12*6+1,4*8+2,(unsigned char *)"*", MINI_OVER);
	}
	
	C_ROM=MINI_OVER;
	if ( ROMselect != 0 ) {
		if (( MaxFreq_ROMwait0[CS0WCR_WR]*(95+ROMmargin())/100 < freq ) && ( freq < MaxFreq_ROMwait[CS0WCR_WR+1] )) C_ROM=MINI_REV;
	}
	PrintMini( 8*6+0,4*8-6,(unsigned char *)"    ", C_ROM);
	PrintMini( 8*6+1,4*8-6,(unsigned char *)"RoR ", C_ROM);
	PrintMini(10*6+4,4*8-6,(unsigned char *)waits[CS0WCR_WR],C_ROM);

	C_RAM=MINI_OVER;
	if ( RAMselect != 0 ) {
		if (( MaxFreq_RAMwait0[CS2WCR_WR]*(95+RAMmargin())/100 < freq ) && ( freq < MaxFreq_RAMwait[CS2WCR_WR+1] )) C_RAM=MINI_REV;
	}
	PrintMini( 8*6+0,4*8+2,(unsigned char *)"    ", C_RAM);
	PrintMini( 8*6+1,4*8+2,(unsigned char *)"RaR ", C_RAM);
	PrintMini(10*6+4,4*8+2,(unsigned char *)waits[CS2WCR_WR],C_RAM);

	C_RAM=MINI_OVER;
	if ( ( RAMselect != 0 ) && ( CS2WCR_WW != 0 ) ) {
		if (( MaxFreq_RAMwaitW0[CS2WCR_WW]*(95+RAMmargin())/100 < freq ) && ( freq < MaxFreq_RAMwaitW[CS2WCR_WW+1] )) C_RAM=MINI_REV;
	}
	PrintMini( 8*6+0,4*8+9,(unsigned char *)"    ", C_RAM);
	PrintMini( 8*6+1,4*8+9,(unsigned char *)"RaW  ", C_RAM);
	PrintMini(10*6+4,4*8+9,(unsigned char *)waitW[CS2WCR_WW],C_RAM);


	freq=freq_sub(FLF,FRQCR_STC,FRQCR_PFC); sprintf(buf,"%7.2f MHz",(float)freq/100);	// I/O freq
	PrintXY(5*6-1,5*8,(unsigned char *)divs[FRQCR_PFC],0);
	PrintMini(14*6-8,5*8+2,(unsigned char *)buf, freq>3000?MINI_REV:MINI_OVER);
	if ( (freq==freq_sub(FLF,PLL_16x,DIV16)) && ( FLF == FLF_default ) )
		PrintMini(12*6+1,5*8+2,(unsigned char *)"*", MINI_OVER);

	
	switch	(mes_flag) {
		case 0:
			locate(1,7); PrintLine((unsigned char *) " ", 21);
			PrintMini(0*6,6*8+1,(unsigned char *)selects[select+1], MINI_REV);
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

int AddIn_main(int isAppli, unsigned short OptionNum)
{
	int	_FRQCR,FRQCR_STC,FRQCR_IFC,FRQCR_SFC,FRQCR_BFC,FRQCR_PFC;
	int	CS0WCR_WR,CS2WCR_WR,CS2WCR_WW;
	int	FLF;

    unsigned int key;
	int select=0;
	int select_func=0;
	int freq;
	int i;
	int BCRWCR=0;
	
	Emu_check();
	SetMaxFreq();
	LoadFile(0);	// Startup auto load
	SetMaxFreq0();
	Bdisp_AllClr_VRAM();
	if ( CPU_check() != 4 ) { ErrorMSG("Not SH4A CPU !!!",CPU_check()); return 1; }
	if ( IsEmu==0 ) {
		if ( (ROMselect==0) && (SetupData&1) ) if (YesNo("Check Memory")) test_wait_mem();	// startup memory check
	}
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
					case KEY_CTRL_SETUP:		// setup
						init_menu();
						Bdisp_AllClr_VRAM();
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

			case KEY_CTRL_OPTN:
				select_func=wait_menu(select_func);		// wait menu
				Bdisp_AllClr_VRAM();
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
						if ( IsG3() ) 
							LoadDataF1_2();	// nomal  default 35+E2
						else
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
						BSC.CS5AWCR.BIT.SW = 3 ;	//	SW:3.5
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
							if ( FRQCR_STC < 2+FRQCR_BFC ) { change_STC(FRQCR_STC+1); update(select,select_func,1); break; }
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
