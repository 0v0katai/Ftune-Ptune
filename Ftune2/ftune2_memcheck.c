/*
===============================================================================

 Ftune2 is SH7305 CPG&BSC tuning utility for fx-9860GII-2      v1.02

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
#include "ftune2_benchmark.h"
#include "ftune2_memcheck.h"
#include "ftune2_data.h"
#include "ftune2_setup.h"

//---------------------------------------------------------------------------------------------
unsigned int* MEMtestsR(volatile unsigned int*MEM,volatile unsigned int *FLLREG,int FLL,int n);
//	int j,a,b;
//	j=0;
// LOOP:
//		a=*MEM;
//		b=*MEM++;
//		if ( a != b ) goto RET;
//	j++; if ( j < n ) goto LOOP;
//	MEM=0;
// RET:
//	*FLLREG = FLL;
//	return ( MEM );

unsigned int* MEMtestsW(volatile unsigned int*MEM,volatile unsigned int *FLLREG,int FLL,int n);
//	int j,a,b;
//	a=0x5555;
//	b=0xAAAA;
//	j=0;
// LOOP:
//		*MEM=b;
//		*MEM=a;
//		c=*MEM;
//		if ( a != c ) goto RET;
//		*MEM=a;
//		*MEM=b;
//		c=*MEM;
//		if ( b != c ) goto RET;
//	j++; if ( j < n ) goto LOOP;
//	MEM=0;
// RET:
//	*FLLREG = FLL;
//	return ( MEM );


int ROMmargin(void) { 
 	return (SetupData>>8) & 0x0f ; 
}
int RAMmargin(void) { 
 	return (SetupData>>12) & 0x0f ; 
}

int ROMtestR(int FLF,int CS0WCR_WR,volatile unsigned int*ROM,int n){
	int a,b;
	unsigned int*ad;
	ad=ROM;
	CPG.FLLFRQ.BIT.FLF = FLF*101/100 ;			// test freq up
	BSC.CS0WCR.BIT.WR  = CS0WCR_WR-1 ;
	if ( *ROM == *ROM ) ad=MEMtestsR(ROM, &CPG.FLLFRQ.LONG, 0x4374,n);
	BSC.CS0WCR.BIT.WR  = CS0WCR_WR ;
	CPG.FLLFRQ.BIT.FLF = FLF ;					// test freq ret
	return ( (int)ad );
}

int RAMtestR(int FLF,int CS2WCR_WR,volatile unsigned int*RAM,int n){
	int a,b;
	unsigned int*ad;
	ad=RAM;
	CPG.FLLFRQ.BIT.FLF = FLF*101/100 ;			// test freq up
	BSC.CS2WCR.BIT.WR  = CS2WCR_WR-1 ;
	if ( *RAM == *RAM ) ad=MEMtestsR(RAM, &CPG.FLLFRQ.LONG, 0x4374,n);
	BSC.CS2WCR.BIT.WR  = CS2WCR_WR ;
	CPG.FLLFRQ.BIT.FLF = FLF ;					// test freq ret
	return ( (int)ad );
}

int RAMtestW(int FLF,int CS2WCR_WW,volatile unsigned int*RAM, int n){
	int a,b;
	unsigned int*ad;
	ad=RAM;
	CPG.FLLFRQ.BIT.FLF = FLF*102/100 ;			// test freq up
	BSC.CS2WCR.BIT.WW  = CS2WCR_WW-1 ;		// WW
	if ( *RAM == *RAM ) ad=MEMtestsW(RAM, &CPG.FLLFRQ.LONG, 0x4374,n);
	BSC.CS2WCR.BIT.WW  = CS2WCR_WW ;		// WW
	CPG.FLLFRQ.BIT.FLF = FLF ;					// test freq ret
	return ( (int)ad );
}

//---------------------------------------------------------------------------------------------

int ROMtestn(int FLF,int CS0WCR_WR){
	volatile unsigned int*ROM     = (unsigned int*) 0xA0000000;	// 
	char buf[20];
	int	i,j;
	int FLFbk=FLF;
	FLF = FLF*(100+ROMmargin())/100 ;			// test freq set
	j=ROMtestR(FLF,CS0WCR_WR+1,ROM,1000);		//	into CACHE
	if ( ROMselect != 0 ) {
		for(i=0;i<100;i++){
			sprintf(buf,"%d  ",i);
			locate(13,4); Print((unsigned char *)buf);
			Bdisp_PutDisp_DD();
			j=ROMtestR(FLF,CS0WCR_WR,ROMselect,65536/4); if ( j ) break;
			j=ROMtestR(FLF,CS0WCR_WR,ROMselect,65536/4); if ( j ) break;
		}
	} else {
		for(i=0;i<126;i++){
			HexPrintXY(7,4,"",(int)ROM);
			Bdisp_PutDisp_DD();
			j=ROMtestR(FLF,CS0WCR_WR,ROM,65536/4); if ( j )  break;
			ROM+=0x8000/4;
		}
	} 
	CPG.FLLFRQ.BIT.FLF = FLFbk ;					// test freq ret
	return ( j );
}

int RAMtestn(int FLF,int CS2WCR_WR){
	volatile unsigned int*RAM     = (unsigned int*) 0xA8000000;		// ?
	char buf[20];
	int	i,j;
	int FLFbk=FLF;
	FLF = FLF*(100+RAMmargin())/100 ;			// test freq set
	j=RAMtestR(FLF,CS2WCR_WR+1,RAM,1000);			//	into CACHE
	if ( RAMselect != 0 ) {
		for(i=0;i<100;i++){
			sprintf(buf,"%d  ",i);
			locate(13,4); Print((unsigned char *)buf);
			Bdisp_PutDisp_DD();
			j=RAMtestR(FLF,CS2WCR_WR,RAMselect,65536/4); if ( j ) break;
			j=RAMtestR(FLF,CS2WCR_WR,RAMselect,65536/4); if ( j ) break;
		}
	} else {
		for(i=0;i<112;i++){
			HexPrintXY(7,4,"",(int)RAM);
			Bdisp_PutDisp_DD();
			j=RAMtestR(FLF,CS2WCR_WR,RAM,65536/4);  if ( j ) break;
			RAM+=0x1000/4;
		}
	}
	CPG.FLLFRQ.BIT.FLF = FLFbk ;					// test freq ret
	return ( j );
}

//---------------------------------------------------------------------------------------------
char *waitroRw[]={"O_0:","O_1:","O_2:","O_3:","O_4:","O_5:","O_6:","O_8:","O10:","O12:"};
char *waitraRw[]={"A_0:","A_1:","A_2:","A_3:","A_4:","A_5:","A_6:","A_8:"};
char *waitraWw[]={"W_0:","W_1:","W_2:","W_3:","W_4:","W_5:","W_6:"};

#define CheckN 2000

void test_wait_WW(void){	// ---------------------------------------------------------------- RAM WW test
	unsigned int tmpRAM[CheckN+1];	// RAM write test area
	unsigned int *RAM;
	char buffer[22];
	int y,k;
	int freq,FLF,PLL,IFC,SFC,BFC,PFC,wait,i=0,FLFP,step;
	unsigned int key;
	int	freqadjust;
											// ---------------------------------Work RAM Read/Write test
	SaveDataF0();	
	Bdisp_AllClr_VRAM();
	locate( 1,8);	Print((unsigned char *)"Write Check");

											// ---------------------------------Work RAM read 1st
	LoadDataF1();	// default
	BSC.CS2BCR.LONG = 0x00003400 ; // idol wait all zero
	FLF=900;	// base 14.75MHz
	RAM=(unsigned int*) ( ((int)&tmpRAM & 0x0FFFFFFF) | 0xA0000000 );
	HexPrintXY(14,8,"",(int)RAM );

		PrintMini( 0+68,1*6,(unsigned char *)waitraRw[WAIT_2],MINI_OVER);
		freq=MaxFreq_RAMwait0[WAIT_2];
		sprintf(buffer,"%6.2fMHz",(float)freq/100);	
		PrintMini(17+68,1*6,(unsigned char *)buffer,MINI_REV);

	y=WAIT_2;
		PrintMini( 0+68,y*6,(unsigned char *)waitraRw[y],MINI_OVER);
		Bdisp_PutDisp_DD();
		
		wait=y;
		freq=MaxFreq_RAMwait0[wait];
		BSC.CS2WCR.BIT.WR = ( wait + 2 );
		BSC.CS0WCR.BIT.WR = ( CheckROMwait(freq) + 2 );
		PLL=(freq*8/1475)-1; IFC=DIV_4; SFC=DIV_8; BFC=DIV_8; PFC=DIV16;
		change_FRQCR(PLL,IFC,SFC,BFC,PFC);
		FLFP=900; while ( freq > freq_sub(FLFP,PLL,BFC) ) FLFP++;
		step=1;
		for(FLF=FLFP*85/100;FLF<1500;FLF+=step){			// +++ v1.02
			freq=freq_sub(FLF,PLL,BFC)*995/1000;;		// Bus freq
			BSC.CS0WCR.BIT.WR = ( CheckROMwait(freq) + 1 );
			if ( RAMtestR(FLF,wait+1,RAM,CheckN) ) break;		// error
			
			sprintf(buffer,"%6.2fMHz",(float)freq/100);			// Max freq
			PrintMini(17+68,y*6,(unsigned char *)buffer,MINI_REV);
			Bdisp_PutDisp_DD();
		}
	freqadjust=freq;
			
	LoadDataF0();

											// ---------------------------------Work RAM Write test
	
	LoadDataF1();	// default
	BSC.CS2BCR.LONG = 0x00003400 ; // idol wait all zero
	FLF=900;	// base 14.75MHz

	for(y=0;y<=6;y++){
		PrintMini( 0   ,y*6,(unsigned char *)waitraWw[y],MINI_OVER);
		Bdisp_PutDisp_DD();
		
		wait=y+1;
		freq=MaxFreq_RAMwaitW0[wait];		// WW freq
		PLL=(freq*8/1475)-1; IFC=DIV_4; SFC=DIV_8; BFC=DIV_8; PFC=DIV16;
		if ( wait >= WAITW1 ) { PLL=(freq*4/1475)-1; IFC=DIV_4; SFC=DIV_4; BFC=DIV_4; PFC=DIV16; }
		if ( wait >= WAITW2 ) { PLL=(freq*2/1475)-1; IFC=DIV_2; SFC=DIV_2; BFC=DIV_2; PFC=DIV16; }
		BSC.CS0WCR.BIT.WR = ( WAIT24 );			// set ROM WR
		BSC.CS2WCR.BIT.WR = ( WAIT12 );			// set RAM WR
		change_FRQCR(PLL,IFC,SFC,BFC,PFC);
		FLFP=900; while ( freq > freq_sub(FLFP,PLL,BFC) ) FLFP++;
		step=1;
		for(FLF=FLFP*85/100;FLF<1500;FLF+=step){			// +++ v1.02
			freq=freq_sub(FLF,PLL,BFC)*995/1000;		// Bus freq
			BSC.CS2WCR.BIT.WR = ( CheckRAMwait(freq) + 1 );	// set RAM WR
			if ( RAMtestW(FLF,wait+1,RAM,CheckN) ) break;		// error
			MaxFreq_RAMwaitW0[wait]=freq * MaxFreq_RAMwait0[WAIT_2] / freqadjust;
			freq = MaxFreq_RAMwaitW0[wait]* (100-RAMmargin())/100;
			MaxFreq_RAMwaitW[wait] = freq;
			
			sprintf(buffer,"%6.2fMHz",(float)freq/100);			// Max WW freq
			PrintMini(17   ,y*6,(unsigned char *)buffer,MINI_REV);
			Bdisp_PutDisp_DD();
		}
	}
	
	LoadDataF0();
}


void test_wait_mem(void){	// ---------------------------------------------------------------- ROM RAM test
	volatile unsigned int*ROM     = (unsigned int*) 0xA0000000;	// 
	unsigned int*ROMsel,*RAMsel;
	char buffer[22];
	int y,k;
	int freq,FLF,PLL,IFC,SFC,BFC,PFC,wait,i=0,FLFP,step;
	unsigned int key;
	int	f,Min_sel,MinFreq;
	int WWcheck=0;

	SaveDataF0();
	if ( ROMselect != 0 ) goto skip;
	
	WWcheck=1;
	Bdisp_AllClr_VRAM();
	locate( 2,1);	Print((unsigned char *)"Check Memory");

											// ---------------------------------ROM test area search
	LoadDataF1();	// default
	FLF=900;	// base 14.75MHz
	Min_sel=0,MinFreq=9999999;
	ROMsel=ROMbase;
	wait=WAIT_3;
	freq=MaxFreq_ROMwait0[wait];
	PLL=(freq*8/1475)-1;
	BSC.CS0WCR.BIT.WR = ( wait + 2 );
	BSC.CS2WCR.BIT.WR = ( CheckRAMwait(freq) + 2 );
	change_FRQCR(PLL,DIV_4,DIV_8,DIV_8,DIV16);
	FLFP=FLF;
	while ( freq > freq_sub(FLFP,PLL,DIV_8) ) FLFP++;
	
		HexPrintXY( 2,3,"ROM:",(int)ROMsel);					//	1st area check
		Bdisp_PutDisp_DD();
		step=1;
		for(FLF=FLFP;FLF<1500;FLF+=step){
			freq=freq_sub(FLF,PLL,DIV_8);		// Bus freq
			sprintf(buffer,"%6.2fMHz",(float)freq/100);			// Max freq
			PrintMini(14*6,2*8+2,(unsigned char *)buffer,MINI_OVER);
			Bdisp_PutDisp_DD();
			BSC.CS2WCR.BIT.WR = ( CheckRAMwait(freq) + 1 );
			if ( ROMtestR(FLF,wait+1,ROMsel,65536/4) ) {		// error
				if ( freq < MinFreq ) {
					Min_sel=f; MinFreq=freq; ROMselect=ROMsel; FLFP=FLF;
					HexPrintXY( 2,4,"ROM_",(int)ROMsel);
					sprintf(buffer,"%6.2fMHz",(float)freq/100);			// 
					PrintMini(14*6,3*8+2,(unsigned char *)buffer,MINI_OVER);
					Bdisp_PutDisp_DD();
				}
				break;
			}
		}

	ROMsel=ROMbase;
	for(f=0; f<64; f++){								// 2nd- area check
		HexPrintXY( 2,3,"ROM:",(int)ROMsel);
		Bdisp_PutDisp_DD();
		step=1;
		for(FLF=FLFP;FLF>500;FLF-=step){
			freq=freq_sub(FLF,PLL,DIV_8);		// Bus freq
			sprintf(buffer,"%6.2fMHz",(float)freq/100);			// Max freq
			PrintMini(14*6,2*8+2,(unsigned char *)buffer,MINI_OVER);
			Bdisp_PutDisp_DD();
			BSC.CS2WCR.BIT.WR = ( CheckRAMwait(freq) + 1 );
			if ( ROMtestR(FLF,wait+1,ROMsel,65536/4) == 0 ) {		// ok
				freq=freq_sub(FLF+1,PLL,DIV_8);		// error Bus freq
				if ( freq < MinFreq ) {
					Min_sel=f; MinFreq=freq; ROMselect=ROMsel; FLFP=FLF+1;
					HexPrintXY( 2,4,"ROM_",(int)ROMsel);
					sprintf(buffer,"%6.2fMHz",(float)freq/100);			// 
					PrintMini(14*6,3*8+2,(unsigned char *)buffer,MINI_OVER);
					Bdisp_PutDisp_DD();
				}
				break;
			}
		}
		ROMsel+=0x04000;
	}
	MaxFreq_ROMwait0[WAIT_0]=MinFreq*32/100;
	MaxFreq_ROMwait0[WAIT_1]=MinFreq*54/100;
	
											// ---------------------------------RAM test area search
	LoadDataF1();	// default
	BSC.CS2BCR.LONG = 0x00003400 ; // idol wait all zero
	FLF=900;	// base 14.75MHz
	Min_sel=0,MinFreq=9999999;
	RAMsel=RAMbase;
	wait=WAIT_2;
	freq=MaxFreq_RAMwait0[wait];
	PLL=(freq*8/1475)-1;
	BSC.CS2WCR.BIT.WR = ( wait + 2 );
	BSC.CS0WCR.BIT.WR = ( CheckROMwait(freq) + 2 );
	change_FRQCR(PLL,DIV_4,DIV_8,DIV_8,DIV16);
	FLFP=FLF;
	while ( freq > freq_sub(FLFP,PLL,DIV_8) ) FLFP++;
	
		HexPrintXY( 2,6,"RAM:",(int)RAMsel);					//	1st area check
		Bdisp_PutDisp_DD();
		step=1;
		for(FLF=FLFP;FLF<1500;FLF+=step){
			freq=freq_sub(FLF,PLL,DIV_8);		// Bus freq
			sprintf(buffer,"%6.2fMHz",(float)freq/100);			//
			PrintMini(14*6,5*8+2,(unsigned char *)buffer,MINI_OVER);
			Bdisp_PutDisp_DD();
			BSC.CS0WCR.BIT.WR = ( CheckROMwait(freq) + 1 );
			if ( RAMtestR(FLF,wait+1,RAMsel,65536/4) ) {		// error
				if ( freq < MinFreq ) {
					Min_sel=f; MinFreq=freq; RAMselect=RAMsel; FLFP=FLF;
					HexPrintXY( 2,7,"RAM_",(int)RAMsel);
					sprintf(buffer,"%6.2fMHz",(float)freq/100);			// 
					PrintMini(14*6,6*8+2,(unsigned char *)buffer,MINI_OVER);
					Bdisp_PutDisp_DD();
				}
				break;
			}
		}

	RAMsel=RAMbase;
	for(f=0; f<64; f++){								// 2nd- area check	
		HexPrintXY( 2,6,"RAM:",(int)RAMsel);
		Bdisp_PutDisp_DD();
		step=1;
		for(FLF=FLFP;FLF>500;FLF-=step){
			freq=freq_sub(FLF,PLL,DIV_8);		// Bus freq
			sprintf(buffer,"%6.2fMHz",(float)freq/100);			//
			PrintMini(14*6,5*8+2,(unsigned char *)buffer,MINI_OVER);
			Bdisp_PutDisp_DD();
			BSC.CS0WCR.BIT.WR = ( CheckROMwait(freq) + 1 );
			if ( RAMtestR(FLF,wait+1,RAMsel,65536/4) == 0 ) {		// ok
				freq=freq_sub(FLF+1,PLL,DIV_8);		// error Bus freq
				if ( freq < MinFreq ) {
					Min_sel=f; MinFreq=freq; RAMselect=RAMsel; FLFP=FLF+1;
					HexPrintXY( 2,7,"RAM_",(int)RAMsel);
					sprintf(buffer,"%6.2fMHz",(float)freq/100);			// 
					PrintMini(14*6,6*8+2,(unsigned char *)buffer,MINI_OVER);
					Bdisp_PutDisp_DD();
				}
				break;
			}
		}
		RAMsel+=0x0800;
	}
	MaxFreq_RAMwait0[WAIT_0]=MinFreq*32/100;
	MaxFreq_RAMwait0[WAIT_1]=MinFreq*64/100;

skip:
	Bdisp_AllClr_VRAM();
											// ---------------------------------ROM test
	LoadDataF1();	// default
	FLF=900;	// base 14.75MHz
	HexPrintXY(14,8,"",(int)ROMselect);
	
	for(y=WAIT_0;y<=WAIT12;y++){											// ROM test
		PrintMini( 0   ,y*6,(unsigned char *)waitroRw[y],MINI_OVER);
		Bdisp_PutDisp_DD();
		
		wait=y;
		freq=MaxFreq_ROMwait0[wait];
		if (( wait >= WAIT_2 ) && ( wait <= WAIT_6 )) freq=MaxFreq_ROMwait0[wait-1]*2-MaxFreq_ROMwait0[wait-2];
		if (( wait == WAIT_8 ))                       freq=MaxFreq_ROMwait0[wait-1]*3-MaxFreq_ROMwait0[wait-2]*2;
		if (( wait >= WAIT10 ))                       freq=(MaxFreq_ROMwait0[wait-1]*2-MaxFreq_ROMwait0[wait-2])*95/100;
		BSC.CS0WCR.BIT.WR = ( wait + 2 );
		BSC.CS2WCR.BIT.WR = ( CheckRAMwait(freq) + 2 );
		PLL=(freq*8/1475)-1; IFC=DIV_4; SFC=DIV_8; BFC=DIV_8; PFC=DIV16;
		if ( wait >= WAIT_5 ) { PLL=(freq*4/1475)-1; IFC=DIV_4; SFC=DIV_4; BFC=DIV_4; PFC=DIV16; }
		if ( wait >= WAIT12 ) { PLL=(freq*2/1475)-1; IFC=DIV_2; SFC=DIV_2; BFC=DIV_2; PFC=DIV32; }
		change_FRQCR(PLL,IFC,SFC,BFC,PFC);
		FLFP=900; while ( freq > freq_sub(FLFP,PLL,BFC) ) FLFP++;
		step=1;
		for(FLF=FLFP*85/100;FLF<1500;FLF+=step){			// +++ v1.02
			freq=freq_sub(FLF,PLL,BFC)*995/1000;		// Bus freq
			BSC.CS2WCR.BIT.WR = ( CheckRAMwait(freq) + 1 );
			if ( ROMtestR(FLF,wait+1,ROMselect,65536/4) ) break;		// error
			MaxFreq_ROMwait0[wait]=freq;
			freq = freq*(100-ROMmargin())/100;
			MaxFreq_ROMwait[wait] =freq;
			
			sprintf(buffer,"%6.2fMHz",(float)freq/100);			// Max freq
			PrintMini(17   ,y*6,(unsigned char *)buffer,MINI_REV);
			Bdisp_PutDisp_DD();
		}
	}

											// ---------------------------------RAM test
	LoadDataF1();	// default
	BSC.CS2BCR.LONG = 0x00003400 ; // idol wait all zero
	FLF=900;	// base 14.75MHz
	HexPrintXY(14,8,"",(int)RAMselect);

	for(y=WAIT_0;y<=WAIT_8;y++){											// RAM test
		PrintMini( 0+68,y*6,(unsigned char *)waitraRw[y],MINI_OVER);
		Bdisp_PutDisp_DD();
		
		wait=y;
		freq=MaxFreq_RAMwait0[wait];
		if (( wait >= WAIT_2 ) && ( wait <= WAIT_6 )) freq=(MaxFreq_RAMwait0[wait-1]*2-MaxFreq_RAMwait0[wait-2]);
		if (( wait == WAIT_8 ))                       freq=(MaxFreq_RAMwait0[wait-1]*3-MaxFreq_RAMwait0[wait-2]*2)*95/100;
		BSC.CS2WCR.BIT.WR = ( wait + 2 );
		BSC.CS0WCR.BIT.WR = ( CheckROMwait(freq) + 2 );
		PLL=(freq*8/1475)-1; IFC=DIV_4; SFC=DIV_8; BFC=DIV_8; PFC=DIV16;
		if ( wait >= WAIT_3 ) { PLL=(freq*4/1475)-1; IFC=DIV_4; SFC=DIV_4; BFC=DIV_4; PFC=DIV16; }
		if ( wait >= WAIT_8 ) { PLL=(freq*2/1475)-1; IFC=DIV_2; SFC=DIV_2; BFC=DIV_2; PFC=DIV32; }
		change_FRQCR(PLL,IFC,SFC,BFC,PFC);
		FLFP=900; while ( freq > freq_sub(FLFP,PLL,BFC) ) FLFP++;
		step=1;
		for(FLF=FLFP*85/100;FLF<1500;FLF+=step){			// +++ v1.02
			freq=freq_sub(FLF,PLL,BFC)*995/1000;;		// Bus freq
			BSC.CS0WCR.BIT.WR = ( CheckROMwait(freq) + 1 );
			if ( RAMtestR(FLF,wait+1,RAMselect,65536/4) ) break;		// error
			 MaxFreq_RAMwait0[wait]=freq;
			freq = freq*(100-RAMmargin())/100;
			MaxFreq_RAMwait[wait] =freq;

			sprintf(buffer,"%6.2fMHz",(float)freq/100);			// Max freq
			PrintMini(17+68,y*6,(unsigned char *)buffer,MINI_REV);
			Bdisp_PutDisp_DD();
		}
	}
	
	LoadDataF0();
	
	if ( WWcheck ) test_wait_WW();
}


//---------------------------------------------------------------------------------------------

int wait_menu(int	select_func){
	char buffer[22];
	int y;
	int freq;
	int	flag=1;
	unsigned int key;
	int ROMRAM=0;
		
	while (flag) {
		Bdisp_AllClr_VRAM();
		if ( ROMRAM == 0 ) {
			for( y=0; y<=WAIT12; y++){
				PrintMini( 0   ,y*6,(unsigned char *)waitroRw[y],MINI_OVER);
				freq=MaxFreq_ROMwait[y]; sprintf(buffer,"%6.2fMHz",(float)freq/100);			// Max freq
				PrintMini(17   ,y*6,(unsigned char *)buffer, MaxFreq_ROMwait0[y]>MaxFreq_ROMwaitDef[y] ? MINI_REV:MINI_OVER);
			}
		} else {
			for( y=0; y<=6; y++){
				PrintMini( 0   ,y*6,(unsigned char *)waitraWw[y],MINI_OVER);
				freq=MaxFreq_RAMwaitW[y+1]; sprintf(buffer,"%6.2fMHz",(float)freq/100);			// Max freq
				PrintMini(17   ,y*6,(unsigned char *)buffer, MaxFreq_RAMwaitW0[y+1]>MaxFreq_RAMwaitWDef[y+1] ? MINI_REV:MINI_OVER);
			}
		}

		for( y=0; y<=WAIT_8; y++){
			PrintMini( 0+68,y*6,(unsigned char *)waitraRw[y],MINI_OVER);
			freq=MaxFreq_RAMwait[y]; sprintf(buffer,"%6.2fMHz",(float)freq/100);			// Max freq
			PrintMini(17+68,y*6,(unsigned char *)buffer, MaxFreq_RAMwait0[y]>MaxFreq_RAMwaitDef[y] ? MINI_REV:MINI_OVER);
		}
		
		Fkey_dispNInit(3);
		Fkey_dispR(4,"R/W");
		Fkey_dispR(5,"Test");
		
		Bdisp_PutDisp_DD();

		GetKey( &key );
        switch (key) {
            case KEY_CTRL_AC:
            	flag=0;
                break;
            case KEY_CTRL_EXIT:
            	flag=0;
                break;
			case KEY_CTRL_SETUP:		// setup
				init_menu();
				break;
			case KEY_CTRL_VARS:
            	flag=0;
				select_func=2;			// Register check&edit
				break;
			case KEY_CTRL_F4:		//
				if (YesNo("Wait initialize")==0) break;
				wait_init();
	            break;
			case KEY_CTRL_F5:		//
				ROMRAM=1-ROMRAM;
	            break;
			case KEY_CTRL_F6:		//
				if (YesNo("Check Memory")==0) break;
				if ( ROMselect == 0 ) ROMRAM=0;
				if ( ROMRAM == 0 ) test_wait_mem(); else test_wait_WW();
	            break;
			}
	}
	return select_func;
}

