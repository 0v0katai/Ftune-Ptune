/*
===============================================================================

 Ptune2 is SH7305 CPG&BSC tuning utility for PRIZM fx-CG10/20  v1.24

 copyright(c)2014,2015,2016 by sentaro21
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
#include "Ptune2_data.h"
#include "Ptune2_setup.h"

//---------------------------------------------------------------------------------------------

unsigned int* MEMtestsR(volatile unsigned int*MEM,volatile unsigned int *FLLFRQ,int FLF,int n){
//	int j,a,b;
//	j=0;
// LOOP:
//		a=*MEM;
//		b=*MEM++;
//		if ( a != b ) goto RET;
//	j++; if ( j < n ) goto LOOP;
//	MEM=0;
// RET:
//	*FLLFRQ = FLF;
//	return ( MEM  );
   __asm__( 
	".align 4\n"
	"	mov	r4,r0\n"
	"LOOP:\n"
	"	mov.l	@r0,r3\n"
	"	mov.l	@r0+,r2\n"
	"	cmp/eq	r2,r3\n"
	"	bf	RET\n"
	"	dt	r7\n"
	"	bf/s	LOOP\n"
	"	nop\n"
	"	mov	#0,r0\n"
	"RET:\n"
	"	mov.l	r6,@r5\n"
	"	rts\n"
	"	nop\n"
	);
}

unsigned int* MEMtestsW(volatile unsigned int*MEM,volatile unsigned int *FLLFRQ,int FLF,int n){
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
//	*FLLFRQ = FLF;
//	return ( MEM );
   __asm__( 
	".align 4\n"
	"	mov.l	K5555,r2\n"
	"	mov.l	KAAAA,r3\n"
	"	mov		r4,r0\n"
	"LOOPW:\n"
	"	mov.l	r3,@r0\n"
	"	mov.l	r2,@r0\n"
	"	mov.l	@r0,r4\n"
	"	cmp/eq	r2,r4\n"
	"	bf	RETW\n"
	"	mov.l	r2,@r0\n"
	"	mov.l	r3,@r0\n"
	"	mov.l	@r0+,r4\n"
	"	cmp/eq	r3,r4\n"
	"	bf	RETW\n"
	"	dt	r7\n"
	"	bf/s	LOOPW\n"
	"	nop\n"
	"	mov	#0,r0\n"
	"RETW:\n"
	"	mov.l	r6,@r5\n"
	"	rts\n"
	"	nop\n"
	".align 4\n"
	"K5555:	.long	0x55555555\n"
	"KAAAA:	.long	0xAAAAAAAA\n"
	);
}

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
int T_Bdisp_PutDisp_DD_count=1;

void T_Bdisp_PutDisp_DD( int n ){
	T_Bdisp_PutDisp_DD_count--;
	if ( T_Bdisp_PutDisp_DD_count < 0 ) {
		Bdisp_PutDisp_DD();
		T_Bdisp_PutDisp_DD_count=n;
	}
}

int ROMtestn(int FLF,int CS0WCR_WR){
	volatile unsigned int*ROM     = ROMbase;	// 
	char buf[20];
	int	i,j;
	int FLFbk=FLF;
	T_Bdisp_PutDisp_DD_count=1;
	FLF = FLF*(100+ROMmargin())/100 ;			// test freq set
	j=ROMtestR(FLF,CS0WCR_WR+1,ROM,1000);		//	into CACHE
	if ( ROMselect != 0 ) {
		for(i=0;i<100;i++){
			itoa(i,buf+2); strcat(buf,"  ");
			PrintXY(13,4,buf,TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
			Bdisp_PutDisp_DD();
			j=ROMtestR(FLF,CS0WCR_WR,ROMselect,65536/4); if ( j ) break;
			j=ROMtestR(FLF,CS0WCR_WR,ROMselect,65536/4); if ( j ) break;
		}
	} else {
		for(i=0;i<128*4;i++){
			HexPrintXY(7,4,"",(int)ROM);
			T_Bdisp_PutDisp_DD(8);
			j=ROMtestR(FLF,CS0WCR_WR,ROM,65536/4); if ( j ) break;
			ROM+=0x010000/4;
		} 
	}
	CPG.FLLFRQ.BIT.FLF = FLFbk ;					// test freq ret
	return ( j );
}

int RAMtestn(int FLF,int CS2WCR_WR){
	volatile unsigned int*RAM     = RAMbase;		// ?
	char buf[20];
	int	i,j;
	int FLFbk=FLF;
	T_Bdisp_PutDisp_DD_count=1;
	FLF = FLF*(100+RAMmargin())/100 ;			// test freq set
	j=RAMtestR(FLF,CS2WCR_WR+1,RAM,1000);			//	into CACHE
	if ( RAMselect != 0 ) {
		for(i=0;i<100;i++){
			itoa(i,buf+2); strcat(buf,"  ");
			PrintXY(13,4,buf,TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
			Bdisp_PutDisp_DD();
			j=RAMtestR(FLF,CS2WCR_WR,RAMselect,65536/4); if ( j ) break;
			j=RAMtestR(FLF,CS2WCR_WR,RAMselect,65536/4); if ( j ) break;
		}
	} else {
		for(i=0;i<124;i++){
			HexPrintXY(7,4,"",(int)RAM);
			Bdisp_PutDisp_DD();
			j=RAMtestR(FLF,CS2WCR_WR,RAM,65536/4);  if ( j ) break;
			RAM+=0x001000/4;
		} 
	}
	CPG.FLLFRQ.BIT.FLF = FLFbk ;					// test freq ret
	return ( j );
}

//---------------------------------------------------------------------------------------------
char *waitroRw[]={"roRw0:","roRw1:","roRw2:","roRw3:","roRw4:","roRw5:","roRw6:","roRw8:","roR10:","roR12:"};
char *waitraRw[]={"raRw0:","raRw1:","raRw2:","raRw3:","raRw4:","raRw5:","raRw6:","raRw8:"};
char *waitraWw[]={"raWw0:","raWw1:","raWw2:","raWw3:","raWw4:","raWw5:","raWw6:"};

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
	StatusArea_sub();
	PrintXY(1,8,"  Write Check ",TEXT_MODE_NORMAL,TEXT_COLOR_BLACK);

											// ---------------------------------Work RAM read 1st
	LoadDataF1();	// default
	BSC.CS5AWCR.LONG = 0x00020AC1 ;	// LCDC wait
	FLF=900;	// base 14.75MHz
	RAM= ( ((int)&tmpRAM & 0x0FFFFFFF) | 0xA0000000 );
	HexPrintXY(14,8,"",(int)RAM );

		PrintMiniXY(0*18+206     ,1*19+3,waitraRw[WAIT_2], COLOR_BLACK,COLOR_WHITE);
		freq=MaxFreq_RAMwait0[WAIT_2];
		k=sprintf62(buffer,freq,"MHz ");
		PrintMiniXY(4*18+200+k*12,1*19+3,buffer, COLOR_BLUE,COLOR_WHITE);

	y=WAIT_2;
		PrintMiniXY(0*18+206     ,y*19+3,waitraRw[y], COLOR_BLACK,COLOR_WHITE);
		Bdisp_PutDisp_DD();
		
		wait=y;
		freq=MaxFreq_RAMwait0[wait];
		BSC.CS2WCR.BIT.WR = ( wait + 2 );
		BSC.CS0WCR.BIT.WR = ( CheckROMwait(freq) + 2 );
		PLL=(freq*8/1475)-1; IFC=DIV_4; SFC=DIV_8; BFC=DIV_8; PFC=DIV16;
		change_FRQCR(PLL,IFC,SFC,BFC,PFC);
		FLFP=900; while ( freq > freq_sub(FLFP,PLL,BFC) ) FLFP++;
		step=1;
		for(FLF=FLFP*95/100;FLF<1500;FLF+=step){
			freq=freq_sub(FLF,PLL,BFC)*995/1000;;		// Bus freq
			BSC.CS0WCR.BIT.WR = ( CheckROMwait(freq) + 1 );
			if ( RAMtestR(FLF,wait+1,RAM,CheckN) ) break;		// error
			
			k=sprintf62(buffer,freq,"MHz ");					// Max freq
			PrintMiniXY(4*18+200+k*12,y*19+3,buffer, COLOR_BLUE,COLOR_WHITE);
			Bdisp_PutDisp_DD();
		}
	freqadjust=freq;
			
	LoadDataF0();

											// ---------------------------------Work RAM Write test
	
	LoadDataF1();	// default
	BSC.CS5AWCR.LONG = 0x00020AC1 ;	// LCDC wait
	FLF=900;	// base 14.75MHz

	for(y=0;y<=6;y++){
		PrintMiniXY(0*18+0      , y*19+3,waitraWw[y], COLOR_BLACK,COLOR_WHITE);
		Bdisp_PutDisp_DD();
		
		wait=y+1;
		freq=MaxFreq_RAMwaitW0[wait];		// WW freq
		PLL=(freq*8/1475)-1; IFC=DIV_4; SFC=DIV_8; BFC=DIV_8; PFC=DIV16;
		if ( wait >= WAITW2 ) { PLL=(freq*4/1475)-1; IFC=DIV_4; SFC=DIV_4; BFC=DIV_4; PFC=DIV16; }
		if ( wait >= WAITW4 ) { PLL=(freq*2/1475)-1; IFC=DIV_2; SFC=DIV_2; BFC=DIV_2; PFC=DIV16; }
		BSC.CS0WCR.BIT.WR = ( WAIT24 );			// set ROM WR
		BSC.CS2WCR.BIT.WR = ( WAIT12 );			// set RAM WR
		change_FRQCR(PLL,IFC,SFC,BFC,PFC);
		FLFP=900; while ( freq > freq_sub(FLFP,PLL,BFC) ) FLFP++;
		step=1;
		for(FLF=FLFP*95/100;FLF<1500;FLF+=step){			// +++ v1.02
			freq=freq_sub(FLF,PLL,BFC)*99/100;		// Bus freq
			BSC.CS2WCR.BIT.WR = ( CheckRAMwait(freq) + 1 );	// set RAM WR
			if ( RAMtestW(FLF,wait+1,RAM,CheckN) ) break;		// error
			MaxFreq_RAMwaitW0[wait]=freq * MaxFreq_RAMwait0[WAIT_2] / freqadjust;
			freq = MaxFreq_RAMwaitW0[wait]* (100-RAMmargin())/100;
			MaxFreq_RAMwaitW[wait] = freq;
			
			k=sprintf62(buffer,freq,"MHz ");					// Max WW freq
			PrintMiniXY(4*18-  4+k*12,y*19+3,buffer, COLOR_BLUE,COLOR_WHITE);
			T_Bdisp_PutDisp_DD(2);
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
	StatusArea_sub();
	PrintXY(2,1,"  Check Memory ",TEXT_MODE_NORMAL,TEXT_COLOR_BLACK);
	
											// ---------------------------------ROM test area search
	T_Bdisp_PutDisp_DD_count=1;
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
	
		HexPrintXY( 2,3,"ROM:",ROMsel);					//	1st area check
		Bdisp_PutDisp_DD();
		step=2;
		for(FLF=FLFP;FLF<1500;FLF+=step){
			freq=freq_sub(FLF,PLL,DIV_8);		// Bus freq
			k=sprintf62(buffer,freq,"MHz ");					// Max freq
			PrintMiniXY(14*18+k*12,2*24+4,buffer,COLOR_BLACK,COLOR_WHITE);
			T_Bdisp_PutDisp_DD(8);
			BSC.CS2WCR.BIT.WR = ( CheckRAMwait(freq) + 1 );
			if ( ROMtestR(FLF,wait+1,ROMsel,65536/4) ) {		// error
				if ( freq < MinFreq ) {
					Min_sel=f; MinFreq=freq; ROMselect=ROMsel; FLFP=FLF;
					HexPrintXY( 2,4,"ROM_",ROMsel);
					k=sprintf62(buffer,freq,"MHz ");
					PrintMiniXY(14*18+k*12,3*24+4,buffer,COLOR_BLACK,COLOR_WHITE);
					Bdisp_PutDisp_DD();
				}
				break;
			}
		}
		
	ROMsel=ROMbase;	
	for(f=0; f<512; f++){								// 2nd- area check
		HexPrintXY( 2,3,"ROM:",ROMsel);
		T_Bdisp_PutDisp_DD(8);
		step=1;
		for(FLF=FLFP;FLF>FLFP-50;FLF-=step){
			freq=freq_sub(FLF,PLL,DIV_8);		// Bus freq
			k=sprintf62(buffer,freq,"MHz ");					// Max freq
			PrintMiniXY(14*18+k*12,2*24+4,buffer,COLOR_BLACK,COLOR_WHITE);
			T_Bdisp_PutDisp_DD(8);
			BSC.CS2WCR.BIT.WR = ( CheckRAMwait(freq) + 1 );
			if ( ROMtestR(FLF,wait+1,ROMsel,65536/4) == 0 ) {		// ok
				freq=freq_sub(FLF+1,PLL,DIV_8);		// error Bus freq
				if ( freq < MinFreq ) {
					Min_sel=f; MinFreq=freq; ROMselect=ROMsel; FLFP=FLF+1;
					HexPrintXY( 2,4,"ROM_",ROMsel);
					k=sprintf62(buffer,freq,"MHz ");
					PrintMiniXY(14*18+k*12,3*24+4,buffer,COLOR_BLACK,COLOR_WHITE);
					Bdisp_PutDisp_DD();
				}
				break;
			}
		}
		ROMsel+=0x10000/4;
	}
	MaxFreq_ROMwait0[WAIT_0]=MinFreq*32/100;
	MaxFreq_ROMwait0[WAIT_1]=MinFreq*54/100;
		
											// ---------------------------------RAM test area search
	T_Bdisp_PutDisp_DD_count=1;
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

		HexPrintXY( 2,6,"RAM:",RAMsel);					//	1st area check
		Bdisp_PutDisp_DD();		
		step=1;
		for(FLF=FLFP;FLF<1500;FLF+=step){
			freq=freq_sub(FLF,PLL,DIV_8);		// Bus freq
			k=sprintf62(buffer,freq,"MHz ");					// Max freq
			PrintMiniXY(14*18+k*12,5*24+4,buffer,COLOR_BLACK,COLOR_WHITE);
			T_Bdisp_PutDisp_DD(8);
			BSC.CS0WCR.BIT.WR = ( CheckROMwait(freq) + 1 );
			if ( RAMtestR(FLF,wait+1,RAMsel,65536/4) ) {		// error
				if ( freq < MinFreq ) {
					Min_sel=f; MinFreq=freq; RAMselect=RAMsel; FLFP=FLF;
					HexPrintXY( 2,7,"RAM_",RAMsel);
					k=sprintf62(buffer,freq,"MHz ");
					PrintMiniXY(14*18+k*12,6*24+4,buffer,COLOR_BLACK,COLOR_WHITE);
					Bdisp_PutDisp_DD();
				}
				break;
			}
		}
		
	RAMsel=RAMbase;
	for(f=0; f<32; f++){								// 2nd- area check
		HexPrintXY( 2,6,"RAM:",RAMsel);
		Bdisp_PutDisp_DD();
		step=1;
		for(FLF=FLFP;FLF>FLFP-50;FLF-=step){
			freq=freq_sub(FLF,PLL,DIV_8);		// Bus freq
			k=sprintf62(buffer,freq,"MHz ");					// Max freq
			PrintMiniXY(14*18+k*12,5*24+4,buffer,COLOR_BLACK,COLOR_WHITE);
			T_Bdisp_PutDisp_DD(8);
			BSC.CS0WCR.BIT.WR = ( CheckROMwait(freq) + 1 );
			if ( RAMtestR(FLF,wait+1,RAMsel,65536/4) == 0 ) {		// ok
				freq=freq_sub(FLF+1,PLL,DIV_8);		// error Bus freq
				if ( freq < MinFreq ) {
					Min_sel=f; MinFreq=freq; RAMselect=RAMsel; FLFP=FLF+1;
					HexPrintXY( 2,7,"RAM_",RAMsel);
					k=sprintf62(buffer,freq,"MHz ");
					PrintMiniXY(14*18+k*12,6*24+4,buffer,COLOR_BLACK,COLOR_WHITE);
					Bdisp_PutDisp_DD();
				}
				break;
			}
		}
		RAMsel+=0x10000/4;
	}
	MaxFreq_RAMwait0[WAIT_0]=MinFreq*32/100;
	MaxFreq_RAMwait0[WAIT_1]=MinFreq*64/100;

skip:
	StatusArea_sub();
											// ---------------------------------ROM test
	T_Bdisp_PutDisp_DD_count=1;
	LoadDataF1();	// default
	BSC.CS5AWCR.LONG = 0x00020AC1 ;	// LCDC wait
	FLF=900;	// base 14.75MHz
	HexPrintXY(14,8,"",ROMselect);
		
	for(y=WAIT_0;y<=WAIT12;y++){
		PrintMiniXY(0*18+  0     ,y*19+3,waitroRw[y], COLOR_BLACK,COLOR_WHITE);
		Bdisp_PutDisp_DD();
		
		wait=y;
		freq=MaxFreq_ROMwait0[wait];
		if (( wait >= WAIT_2 ) && ( wait <= WAIT_6 )) freq=MaxFreq_ROMwait0[wait-1]*2-MaxFreq_ROMwait0[wait-2];
		if (( wait == WAIT_8 ))                       freq=MaxFreq_ROMwait0[wait-1]*3-MaxFreq_ROMwait0[wait-2]*2;
		if (( wait >= WAIT10 ))                       freq=MaxFreq_ROMwait0[wait-1]*2-MaxFreq_ROMwait0[wait-2];
		if (wait<WAIT_4) PLL=(freq*8/1475)-1; else  PLL=(freq*4/1475)-1; 
		BSC.CS0WCR.BIT.WR = ( wait + 2 );
		BSC.CS2WCR.BIT.WR = ( CheckRAMwait(freq) + 2 );
		PLL=(freq*8/1475)-1; IFC=DIV_4; SFC=DIV_8; BFC=DIV_8; PFC=DIV16;
		if ( wait >= WAIT_6 ) { PLL=(freq*4/1475)-1; IFC=DIV_4; SFC=DIV_4; BFC=DIV_4; PFC=DIV16; }
		change_FRQCR(PLL,IFC,SFC,BFC,PFC);
		FLFP=900; while ( freq > freq_sub(FLFP,PLL,BFC) ) FLFP++;
		step=1;
		for(FLF=FLFP*95/100;FLF<1500;FLF+=step){
			freq=freq_sub(FLF,PLL,BFC)*995/1000;		// Bus freq
			BSC.CS2WCR.BIT.WR = ( CheckRAMwait(freq) + 1 );
			if ( ROMtestR(FLF,wait+1,ROMselect,65536/4) ) break;		// error
			MaxFreq_ROMwait0[wait]=freq;
			freq = freq*(100-ROMmargin())/100;
			MaxFreq_ROMwait[wait] =freq;
			
			k=sprintf62(buffer,freq,"MHz ");					// Max freq
			PrintMiniXY(4*18-  4+k*12,y*19+3,buffer, COLOR_BLUE,COLOR_WHITE);
			T_Bdisp_PutDisp_DD(2);
		}

	}
											// ---------------------------------RAM test
	LoadDataF1();	// default
	BSC.CS2BCR.LONG  = 0x00003400 ; // idol wait all zero
	BSC.CS5AWCR.LONG = 0x00020AC1 ;	// LCDC wait
	FLF=900;	// base 14.75MHz
	HexPrintXY(14,8,"",RAMselect);

	for(y=WAIT_0;y<=WAIT_8;y++){
		PrintMiniXY(0*18+206    , y*19+3,waitraRw[y], COLOR_BLACK,COLOR_WHITE);
		Bdisp_PutDisp_DD();
		
		wait=y;
		freq=MaxFreq_RAMwait0[wait];
		if (( wait >= WAIT_2 ) && ( wait <= WAIT_6 )) freq=MaxFreq_RAMwait0[wait-1]*2-MaxFreq_RAMwait0[wait-2];
		if (( wait == WAIT_8 ))                       freq=MaxFreq_RAMwait0[wait-1]*3-MaxFreq_RAMwait0[wait-2]*2;
		BSC.CS2WCR.BIT.WR = ( wait + 2 );
		BSC.CS0WCR.BIT.WR = ( CheckROMwait(freq) + 2 );
		PLL=(freq*8/1475)-1; IFC=DIV_4; SFC=DIV_8; BFC=DIV_8; PFC=DIV16;
		if ( wait >= WAIT_3 ) { PLL=(freq*4/1475)-1; IFC=DIV_4; SFC=DIV_4; BFC=DIV_4; PFC=DIV16; }
		if ( wait >= WAIT_8 ) { PLL=(freq*2/1475)-1; IFC=DIV_2; SFC=DIV_2; BFC=DIV_2; PFC=DIV16; }
		change_FRQCR(PLL,IFC,SFC,BFC,PFC);
		FLFP=900; while ( freq > freq_sub(FLFP,PLL,BFC) ) FLFP++;
		step=1;
		for(FLF=FLFP*95/100;FLF<1500;FLF+=step){
			freq=freq_sub(FLF,PLL,BFC)*995/1000;;		// Bus freq
			BSC.CS0WCR.BIT.WR = ( CheckROMwait(freq) + 1 );
			if ( RAMtestR(FLF,wait+1,RAMselect,65536/4) ) break;		// error
			MaxFreq_RAMwait0[wait]=freq;
			freq = freq*(100-RAMmargin())/100;
			MaxFreq_RAMwait[wait] =freq;
			
			k=sprintf62(buffer,freq,"MHz ");					// Max freq
			PrintMiniXY(4*18+200+k*12,y*19+3,buffer, COLOR_BLUE,COLOR_WHITE);
			T_Bdisp_PutDisp_DD(2);
		}
	}
	
	LoadDataF0();

	if ( WWcheck ) test_wait_WW();
}


//---------------------------------------------------------------------------------------------

int wait_menu(int	select_func){
	char buffer[22];
	int y,k;
	int freq;
	int	flag=1;
	unsigned int key;
	unsigned int* iresult;	
	int ROMRAM=0;

	
	while (flag) {
		StatusArea_sub();
		if ( ROMRAM == 0 ) {
			for( y=0; y<=WAIT12; y++){
				PrintMiniXY(0*18+  0     ,y*19+3,waitroRw[y], COLOR_BLACK,COLOR_WHITE);
				freq=MaxFreq_ROMwait[y]; k=sprintf62(buffer,freq,"MHz  ");					// Max freq ROM RW
				PrintMiniXY(4*18-  4+k*12,y*19+3,buffer,     MaxFreq_ROMwait0[y]>MaxFreq_ROMwaitDef[y] ? COLOR_BLUE:COLOR_BLACK,COLOR_WHITE);
			}
		} else {
			for( y=0; y<=6; y++){
				PrintMiniXY(0*18+  0    , y*19+3,waitraWw[y], COLOR_BLACK,COLOR_WHITE);
				freq=MaxFreq_RAMwaitW[y+1]; k=sprintf62(buffer,freq,"MHz  ");				// Max freq RAM WW
				PrintMiniXY(4*18-  4+k*12,y*19+3,buffer,     MaxFreq_RAMwaitW0[y+1]>MaxFreq_RAMwaitWDef[y+1] ? COLOR_BLUE:COLOR_BLACK,COLOR_WHITE);
			}
		}	

			for( y=0; y<=WAIT_8; y++){
				PrintMiniXY(0*18+206    , y*19+3,waitraRw[y], COLOR_BLACK,COLOR_WHITE);
				freq=MaxFreq_RAMwait[y]; k=sprintf62(buffer,freq,"MHz  ");					// Max freq RAM RW
				PrintMiniXY(4*18+200+k*12,y*19+3,buffer,     MaxFreq_RAMwait0[y]>MaxFreq_RAMwaitDef[y] ? COLOR_BLUE:COLOR_BLACK,COLOR_WHITE);
			}
			
		GetFKeyPtr(0x005f, &iresult);	//INITIAL 	,white
		FKey_Display(3, iresult);

		Fkey_dispI(4,fkey08);	//Fkey_dispN(4," R/W ");

		GetFKeyPtr(0x0003, &iresult);	//TEST		,black/
		FKey_Display(5, iresult);
	
		Bdisp_PutDisp_DD();

		GetKey_DisableCatalog( &key );
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

