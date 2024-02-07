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
#include "Ptune3_memcheck.h"
#include "Ptune3_data.h"
#include "Ptune3_setup.h"

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

int ReadSR(void){
   __asm__( 
	".align 4\n"
	"	stc		SR,r0\n"
	"	rts\n"
	"	nop\n"
	);
}
int SetSR(int sr ){
   __asm__( 
	".align 4\n"
	"	ldc		r4,SR\n"
	"	rts\n"
	"	nop\n"
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
	if ( *ROM == *ROM ) ad=MEMtestsR(ROM, &CPG.FLLFRQ.LONG, 0x432A,n);	// 432A:x810
	BSC.CS0WCR.BIT.WR  = CS0WCR_WR ;
	CPG.FLLFRQ.BIT.FLF = FLF ;					// test freq ret
	return ( (int)ad );
}

int RAMtestR(int FLF,int CS2WCR_WR,volatile unsigned int*RAM,int n){
	int a,b;
	int sr;
	unsigned int*ad;
	ad=RAM;
	sr=ReadSR();
	SetSR(sr & 0xFFFFFF0F );	// disable IRQ
	CPG.FLLFRQ.BIT.FLF = FLF*102/100 ;			// test freq up
//	BSC.CS2WCR.BIT.WR  = CS2WCR_WR-1 ;
	if ( *RAM == *RAM ) ad=MEMtestsR(RAM, &CPG.FLLFRQ.LONG, 0x432A,n);	// 432A:x810
	SetSR(sr);
//	BSC.CS2WCR.BIT.WR  = CS2WCR_WR ;
//	CPG.FLLFRQ.BIT.FLF = FLF ;					// test freq ret
	return ( (int)ad );
}

int RAMtestW(int FLF,int CS2WCR_WW,volatile unsigned int*RAM, int n){
	int a,b;
	int sr;
	unsigned int*ad;
	ad=RAM;
	sr=ReadSR();
	SetSR(sr & 0xFFFFFF0F );	// disable IRQ
	CPG.FLLFRQ.BIT.FLF = FLF*102/100 ;			// test freq up
//	BSC.CS2WCR.BIT.WW  = CS2WCR_WW-1 ;		// WW
	if ( *RAM == *RAM ) ad=MEMtestsW(RAM, &CPG.FLLFRQ.LONG, 0x432A,n);	// 432A:x810
	SetSR(sr);
//	BSC.CS2WCR.BIT.WW  = CS2WCR_WW ;		// WW
//	CPG.FLLFRQ.BIT.FLF = FLF ;					// test freq ret
	return ( (int)ad );
}

void* GetVRAMAdress()	//	from Gravity Duck source ( by PierrotLL )
{
	__asm__(
	    "mov.l syscall_adress, r2\n"
	    "mov.l getVRAM, r0\n"
	    "jmp @r2\n"
	    "nop\n"
	    "syscall_adress: .long 0x80020070\n"
	    "getVRAM: .long 0x01E6");
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
/*
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
//		BSC.CS2WCR.BIT.WR = ( wait + 2 );
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
			PrintMiniXY(4*18-  6+k*12,y*19+3,buffer, COLOR_BLUE,COLOR_WHITE);
			T_Bdisp_PutDisp_DD(2);
		}
	}
	
	LoadDataF0();
}
*/
void test_wait_mem(int RAMtest ){	// ---------------------------------------------------------------- ROM RAM test
	volatile unsigned int*ROM     = (unsigned int*) 0xA0000000;	// 
	unsigned int tmpRAM[CheckN+1];	// RAM write test area
	unsigned int*ROMsel,*RAMsel;
	unsigned int *RAM;
	char buffer[22];
	int y,k;
	int freq,FLF,PLL,IFC,SFC,BFC,PFC,wait,i=0,FLFP,step;
	unsigned int key;
	int	f,Min_sel,MinFreq;
	int WWcheck=0;
	int PLLstart,PLLend;

	SaveDataF0();
	if ( RAMtest ) { StatusArea_sub(); goto RAMTEST; }

	if ( ROMselect != 0 ) goto skip;

	WWcheck=1;
	StatusArea_sub();
	PrintXY(2,1,"  Check Memory ",TEXT_MODE_NORMAL,TEXT_COLOR_BLACK);
	
											// ---------------------------------ROM test area search
	T_Bdisp_PutDisp_DD_count=1;
	LoadDataF1();	// default	CPU:118MHz  bus:58MHz
	FLF=900;	// base 14.75MHz
	Min_sel=0,MinFreq=9999999;
	ROMsel=ROMbase;
	wait=WAIT_3;
	freq=MaxFreq_ROMwaitDef[wait];
	PLL=(freq*8/1475)-1;
	BSC.CS0WCR.BIT.WR = ( wait + 2 );
//	BSC.CS2WCR.BIT.WR = ( CheckRAMwait(freq) + 2 );
	if ( PLL>=31 ) goto ROMTEST;		//		CG50
	change_FRQCR(PLL,DIV_4,DIV_8,DIV_8,DIV16);
	FLFP=FLF;
	while ( freq > freq_sub(FLFP,PLL,DIV_8) ) FLFP++;
	
		HexPrintXY( 2,3,"ROM:",ROMsel);					//	1st area check
		Bdisp_PutDisp_DD();
		step=2;
		for(FLF=FLFP;FLF<1350;FLF+=step){
			freq=freq_sub(FLF,PLL,DIV_8);		// Bus freq
			k=sprintf62(buffer,freq,"MHz ");					// Max freq
			PrintMiniXY(14*18+k*12,2*24+4,buffer,COLOR_BLACK,COLOR_WHITE);
			T_Bdisp_PutDisp_DD(8);
//			BSC.CS2WCR.BIT.WR = ( CheckRAMwait(freq) + 1 );
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
		step=2;
		for(FLF=FLFP;FLF>FLFP-50;FLF-=step){
			freq=freq_sub(FLF,PLL,DIV_8);		// Bus freq
			k=sprintf62(buffer,freq,"MHz ");					// Max freq
			PrintMiniXY(14*18+k*12,2*24+4,buffer,COLOR_BLACK,COLOR_WHITE);
			T_Bdisp_PutDisp_DD(8);
//			BSC.CS2WCR.BIT.WR = ( CheckRAMwait(freq) + 1 );
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

  ROMTEST:
skip:
	StatusArea_sub();
											// ---------------------------------ROM test
	T_Bdisp_PutDisp_DD_count=1;
	LoadDataF1();	// default
//	BSC.CS5AWCR.LONG = 0x00020AC1 ;	// LCDC wait
	FLF=900;	// base 14.75MHz
	HexPrintXY(14,8,"",ROMselect);
	change_FRQCR(16-1,DIV_4,DIV_8,DIV_8,DIV_8);	// BFC down to 29MHz
	
	for(y=WAIT_0;y<=WAIT_6;y++){
		PrintMiniXY(0*18+  0     ,y*19+3,waitroRw[y], COLOR_BLACK,COLOR_WHITE);
		Bdisp_PutDisp_DD();
		
		wait=y;
		freq=MaxFreq_ROMwaitDef[wait];
		BSC.CS0WCR.BIT.WR = ( WAIT_8 );
//		BSC.CS2WCR.BIT.WR = ( CheckRAMwait(freq) + 2 );

		switch ( wait ) {
			case WAIT_0:
				PLLstart= 4-1; PLLend= 7-1;
				break;
			case WAIT_1:
				PLLstart= 7-1; PLLend=10-1;
				break;
			case WAIT_2:
				PLLstart=10-1; PLLend=13-1;
				break;
			case WAIT_3:			
				PLLstart=12-1; PLLend=17-1;
				break;
			case WAIT_4:
				PLLstart=15-1; PLLend=19-1;
				break;
			case WAIT_5:
				PLLstart=18-1; PLLend=22-1;
				break;
			case WAIT_6:
				PLLstart=21-1; PLLend=26-1;
				break;
			case WAIT_8:
				PLLstart=25-1; PLLend=30-1;
				break;
			case WAIT10:
				PLLstart=30-1; PLLend=31-1;
				break;
			case WAIT12:
				PLLstart=31-1; PLLend=32-1;
				break;
		}

		for ( PLL=PLLstart; PLL<PLLend; PLL+=1 ){;	// 58MHz start
			IFC=DIV_4; SFC=DIV_4; BFC=DIV_4; PFC=DIV_8;
			freq=freq_sub(FLFP,PLL,BFC);
			change_FRQCR(PLL,IFC,SFC,BFC,PFC);
			step=2;
			FLFP=900;
			for(FLF=850;FLF<1000;FLF+=step){
				freq=freq_sub(FLF,PLL,BFC)*995/1000;		// Bus freq
				if ( ROMtestR(FLF,wait+1,ROMselect,65536/4) ) break;		// error
				MaxFreq_ROMwait0[wait] = freq;
				freq = freq*(100-ROMmargin())/100;
				MaxFreq_ROMwait[wait]  = freq;
				
				k=sprintf62(buffer,freq,"MHz ");					// Max freq
				PrintMiniXY(4*18-  6     ,y*19+3,"  ",   COLOR_BLUE,COLOR_WHITE);
				PrintMiniXY(4*18-  6+k*12,y*19+3,buffer, COLOR_BLUE,COLOR_WHITE);
				T_Bdisp_PutDisp_DD(2);
			}
		}
	}

	
/*	
	if ( MaxFreq_ROMwait[WAIT_2] <= MaxFreq_ROMwaitDef[WAIT_2] ) {
		MaxFreq_ROMwait0[WAIT_2] = (MaxFreq_ROMwait0[WAIT_1]*2-MaxFreq_ROMwait0[WAIT_0]);
		MaxFreq_ROMwait[WAIT_2]  = MaxFreq_ROMwait0[WAIT_2]*(100-ROMmargin())/100;
	}
	if ( MaxFreq_ROMwait[WAIT_3] <= MaxFreq_ROMwaitDef[WAIT_3] ) {
		MaxFreq_ROMwait0[WAIT_3] = (MaxFreq_ROMwait0[WAIT_2]*2-MaxFreq_ROMwait0[WAIT_1]);
		MaxFreq_ROMwait[WAIT_3]  = MaxFreq_ROMwait0[WAIT_3]*(100-ROMmargin())/100;
	}
	if ( MaxFreq_ROMwait[WAIT_4] <= MaxFreq_ROMwaitDef[WAIT_4] ) {
		MaxFreq_ROMwait0[WAIT_4] = (MaxFreq_ROMwait0[WAIT_3]*2-MaxFreq_ROMwait0[WAIT_2]);
		MaxFreq_ROMwait[WAIT_4]  = MaxFreq_ROMwait0[WAIT_4]*(100-ROMmargin())/100;
	}
	if ( MaxFreq_ROMwait[WAIT_5] <= MaxFreq_ROMwaitDef[WAIT_5] ) {
		MaxFreq_ROMwait0[WAIT_5] = (MaxFreq_ROMwait0[WAIT_4]*2-MaxFreq_ROMwait0[WAIT_3]);
		MaxFreq_ROMwait[WAIT_5]  = MaxFreq_ROMwait0[WAIT_5]*(100-ROMmargin())/100;
	}
	if ( MaxFreq_ROMwait[WAIT_6] <= MaxFreq_ROMwaitDef[WAIT_6] ) {
		MaxFreq_ROMwait0[WAIT_6] = (MaxFreq_ROMwait0[WAIT_5]*2-MaxFreq_ROMwait0[WAIT_4]);
		MaxFreq_ROMwait[WAIT_6]  = MaxFreq_ROMwait0[WAIT_6]*(100-ROMmargin())/100;
	}
*/	if ( MaxFreq_ROMwait[WAIT_8] <= MaxFreq_ROMwaitDef[WAIT_8] ) {
		MaxFreq_ROMwait0[WAIT_8] = (MaxFreq_ROMwait0[WAIT_6]*3-MaxFreq_ROMwait0[WAIT_5]*2);
		MaxFreq_ROMwait[WAIT_8]  = MaxFreq_ROMwait0[WAIT_8]*(100-ROMmargin())/100;
	}

	if ( MaxFreq_ROMwait[WAIT10] <= MaxFreq_ROMwaitDef[WAIT10] ) {
		MaxFreq_ROMwait0[WAIT10] = (MaxFreq_ROMwait0[WAIT_8]*2-MaxFreq_ROMwait0[WAIT_6])*99/100;
		MaxFreq_ROMwait[WAIT10]  = MaxFreq_ROMwait0[WAIT10]*(100-ROMmargin())/100;
	}
	if ( MaxFreq_ROMwait[WAIT12] <= MaxFreq_ROMwaitDef[WAIT12] ) {
		MaxFreq_ROMwait0[WAIT12] = (MaxFreq_ROMwait0[WAIT10]*2-MaxFreq_ROMwait0[WAIT_8])*98/100;
		MaxFreq_ROMwait[WAIT12]  = MaxFreq_ROMwait0[WAIT12]*(100-ROMmargin())/100;
	}
	
	LoadDataF0();
	if ( RAMtest == 0 ) return;


RAMTEST:
											// ---------------------------------1st SDRAM test
//	LoadDataF1();	// default
//	BSC.CS2BCR.LONG  = 0x00003400 ; // idol wait all zero
//	BSC.CS5AWCR.LONG = 0x00020AC1 ;	// LCDC wait
	RAMselect=GetVRAMAdress()+0x600000;
	
  RAMTESTloop:
	HexPrintXY(14,8,"",RAMselect);
	PrintMiniXY(0*18+206    , 4*19+3,"Check Max Freq", COLOR_BLACK,COLOR_WHITE);

	wait=WAIT_8;
	y=0;
	PrintMiniXY(0*18+206    , y*19+3,"SDRAM:", COLOR_BLACK,COLOR_WHITE);
	Bdisp_PutDisp_DD();
	
	for ( PLL=20; PLL<32; PLL+=1 ){;	// 73MHz start
		IFC=DIV_2; SFC=DIV_4; BFC=DIV_4; PFC=DIV_8; if ( PLL>26 ) { IFC=DIV_4; SFC=DIV_4; BFC=DIV_4; PFC=DIV16; } 
		freq = freq_sub(FLFP,PLL,BFC);
		BSC.CS0WCR.BIT.WR = ( CheckROMwait(freq) + 2 );
		change_FRQCR(PLL,IFC,SFC,BFC,PFC);
		step=1;
		FLFP=900;
		for(FLF=FLFP;FLF<1100;FLF+=step){
			freq = freq_sub(FLF,PLL,BFC)*95/100;		// Bus freq   margin +5%
			BSC.CS0WCR.BIT.WR = ( CheckROMwait(freq) + 1 );
			if ( RAMtestR(FLF,wait+1,RAMselect,65536/4) ) goto RAMEXIT;		// error
			MaxFreq_RAMwait0[wait]=freq;
			freq = freq*(100-RAMmargin())/100;
			MaxFreq_RAMwait[wait] =freq;
			
			k=sprintf62(buffer,freq,"MHz ");					// Max freq
			PrintMiniXY(4*18+200     ,y*19+3,"  ",   COLOR_BLUE,COLOR_WHITE);
			PrintMiniXY(4*18+200+k*12,y*19+3,buffer, COLOR_BLUE,COLOR_WHITE);
			T_Bdisp_PutDisp_DD(2);
		}
	}
  RAMEXIT:
	FLFP=FLF;
											// ---------------------------------SDRAM area search
	RAMsel=RAMselect;
	Min_sel=0; MinFreq=9999999;
	for(f=0; f<32; f++){								// 2nd- area check
		HexPrintXY(10,7,"RAM:",RAMsel);
		Bdisp_PutDisp_DD();
		step=1;
		for(FLF=FLFP+1;FLF>FLFP-50;FLF-=step){
			freq = freq_sub(FLF,PLL,BFC)*95/100;		// Bus freq   margin +5%
			freq = freq*(100-RAMmargin())/100;
			k=sprintf62(buffer,freq,"MHz ");					// Max freq
			PrintMiniXY(14*18+k*12,5*24+4,buffer,COLOR_BLACK,COLOR_WHITE);
			T_Bdisp_PutDisp_DD(8);
			BSC.CS0WCR.BIT.WR = ( CheckROMwait(freq) + 1 );
			if ( RAMtestR(FLF,wait+1,RAMsel,65536/4) == 0 ) {		// ok
				freq = freq_sub(FLF+1,PLL,BFC)*95/100;		// error Bus freq
				freq = freq*(100-RAMmargin())/100;
				if ( freq < MinFreq ) {
					Min_sel=f; MinFreq=freq; RAMselect=RAMsel; FLFP=FLF+1;
					HexPrintXY(10,8,"RAM_",RAMsel);
					
					k=sprintf62(buffer,freq,"MHz ");					// Max freq
					PrintMiniXY(4*18+200     ,y*19+3,"  ",   COLOR_BLUE,COLOR_WHITE);
					PrintMiniXY(4*18+200+k*12,y*19+3,buffer, COLOR_BLUE,COLOR_WHITE);
					T_Bdisp_PutDisp_DD(2);
				}
				break;
			}
		}
		RAMsel+=0x10000/4;
	}
    PrintXY(10,6,"              ",TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
    PrintXY(10,7,"              ",TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
    PrintXY(10,8,"              ",TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);

											// ---------------------------------2nd SDRAM test
	HexPrintXY(14,8,"",RAMselect);
//	PrintMiniXY(0*18+206    , 4*19+3,"Check Max Freq", COLOR_BLACK,COLOR_WHITE);

	wait=WAIT_8;
	y=0;
//	PrintMiniXY(0*18+206    , y*19+3,"SDRAM:", COLOR_BLACK,COLOR_WHITE);
	Bdisp_PutDisp_DD();
	
	for ( PLL=22; PLL<32; PLL+=1 ){;	// 80MHz start
		IFC=DIV_2; SFC=DIV_4; BFC=DIV_4; PFC=DIV_8; if ( PLL>26 ) { IFC=DIV_4; SFC=DIV_4; BFC=DIV_4; PFC=DIV16; } 
		freq = freq_sub(FLFP,PLL,BFC);
		BSC.CS0WCR.BIT.WR = ( CheckROMwait(freq) + 2 );
		change_FRQCR(PLL,IFC,SFC,BFC,PFC);
		step=1;
		FLFP=900;
		for(FLF=FLFP;FLF<1100;FLF+=step){
			freq = freq_sub(FLF,PLL,BFC)*95/100;		// Bus freq   margin +5%
			BSC.CS0WCR.BIT.WR = ( CheckROMwait(freq) + 1 );
			if ( RAMtestR(FLF,wait+1,RAMselect,65536/4) ) goto RAMEXIT2;		// error
			MaxFreq_RAMwait0[wait]=freq;
			freq = freq*(100-RAMmargin())/100;
			MaxFreq_RAMwait[wait] =freq;
			
			k=sprintf62(buffer,freq,"MHz ");					// Max freq
			PrintMiniXY(4*18+200     ,y*19+3,"  ",   COLOR_BLUE,COLOR_WHITE);
			PrintMiniXY(4*18+200+k*12,y*19+3,buffer, COLOR_BLUE,COLOR_WHITE);
			T_Bdisp_PutDisp_DD(2);
		}
	}
  RAMEXIT2:
//	MaxFreq_BFC=MaxFreq_RAMwait[wait];	

//	LoadDataF0();
											// ---------------------------------SDRAM Write test
	
//	LoadDataF1();	// default
//	BSC.CS5AWCR.LONG = 0x00020AC1 ;	// LCDC wait
	wait=WAIT_6+1;
	y=2;
	PrintMiniXY(0*18+206    , y*19+3,"Write:", COLOR_BLACK,COLOR_WHITE);
	RAM= ( ((int)&tmpRAM & 0x0FFFFFFF) | 0xA0000000 );
	HexPrintXY(14,8,"",(int)RAM );
	Bdisp_PutDisp_DD();
	
	for ( PLL=22; PLL<32; PLL+=1 ){;	// 80MHz start
		IFC=DIV_2; SFC=DIV_4; BFC=DIV_4; PFC=DIV_8; if ( PLL>26 ) { IFC=DIV_4; SFC=DIV_4; BFC=DIV_4; PFC=DIV16; } 
		freq=freq_sub(FLFP,PLL,BFC);
		BSC.CS0WCR.BIT.WR = ( CheckROMwait(freq) + 2 );
		change_FRQCR(PLL,IFC,SFC,BFC,PFC);
		step=1;
		FLFP=900;
		for(FLF=FLFP;FLF<1100;FLF+=step){
			freq=freq_sub(FLF,PLL,BFC)*95/100;		// Bus freq   margin +5%
			BSC.CS0WCR.BIT.WR = ( CheckROMwait(freq) + 1 );
			if ( RAMtestW(FLF,wait+1,RAM,CheckN) ) goto RAMWWEXIT;		// error
			MaxFreq_RAMwaitW0[wait]=freq;
			freq = freq*(100-RAMmargin())/100;
			MaxFreq_RAMwaitW[wait] =freq;
			
			k=sprintf62(buffer,freq,"MHz ");					// Max freq
			PrintMiniXY(4*18+200     ,y*19+3,"  ",   COLOR_BLUE,COLOR_WHITE);
			PrintMiniXY(4*18+200+k*12,y*19+3,buffer, COLOR_BLUE,COLOR_WHITE);
			T_Bdisp_PutDisp_DD(2);
		}
	}
  RAMWWEXIT:
//	if ( MaxFreq_BFC < MaxFreq_RAMwaitW[wait] ) MaxFreq_BFC=MaxFreq_RAMwaitW[wait];
	MaxFreq_BFC=MaxFreq_RAMwaitW0[wait]/10*10;
	LoadDataF0();

/*
	for(y=0;y<=6;y++){
		PrintMiniXY(0*18+0      , y*19+3,waitraWw[y], COLOR_BLACK,COLOR_WHITE);
		Bdisp_PutDisp_DD();
		
		wait=y+1;
		freq=MaxFreq_RAMwaitW0[wait];		// WW freq
		PLL=(freq*8/1475)-1; IFC=DIV_4; SFC=DIV_8; BFC=DIV_8; PFC=DIV16;
		if ( wait >= WAITW2 ) { PLL=(freq*4/1475)-1; IFC=DIV_4; SFC=DIV_4; BFC=DIV_4; PFC=DIV16; }
		if ( wait >= WAITW4 ) { PLL=(freq*2/1475)-1; IFC=DIV_2; SFC=DIV_2; BFC=DIV_2; PFC=DIV16; }
		BSC.CS0WCR.BIT.WR = ( WAIT24 );			// set ROM WR
//		BSC.CS2WCR.BIT.WR = ( WAIT12 );			// set RAM WR
		change_FRQCR(PLL,IFC,SFC,BFC,PFC);
		FLFP=900; while ( freq > freq_sub(FLFP,PLL,BFC) ) FLFP++;
		step=1;
		for(FLF=FLFP*95/100;FLF<1500;FLF+=step){			// +++ v1.02
			freq=freq_sub(FLF,PLL,BFC);		// Bus freq
//			BSC.CS2WCR.BIT.WR = ( CheckRAMwait(freq) + 1 );	// set RAM WR
			if ( RAMtestW(FLF,wait+1,RAM,CheckN) ) break;		// error
			MaxFreq_RAMwaitW0[wait]=freq * MaxFreq_RAMwait0[WAIT_2] / freqadjust;
			freq = MaxFreq_RAMwaitW0[wait]* (100-RAMmargin())/100;
			MaxFreq_RAMwaitW[wait] = freq;
			
			k=sprintf62(buffer,freq,"MHz ");					// Max WW freq
			PrintMiniXY(4*18-  6+k*12,y*19+3,buffer, COLOR_BLUE,COLOR_WHITE);
			T_Bdisp_PutDisp_DD(2);
		}
	}
	
*/
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
				PrintMiniXY(4*18-  6+k*12,y*19+3,buffer,     MaxFreq_ROMwait0[y]>MaxFreq_ROMwaitDef[y] ? COLOR_BLUE:COLOR_BLACK,COLOR_WHITE);
			}
		} else {
			for( y=0; y<=6; y++){
				PrintMiniXY(0*18+  0    , y*19+3,waitraWw[y], COLOR_BLACK,COLOR_WHITE);
				freq=MaxFreq_RAMwaitW[y+1]; k=sprintf62(buffer,freq,"MHz  ");				// Max freq RAM WW
				PrintMiniXY(4*18-  6+k*12,y*19+3,buffer,     MaxFreq_RAMwaitW0[y+1]>MaxFreq_RAMwaitWDef[y+1] ? COLOR_BLUE:COLOR_BLACK,COLOR_WHITE);
			}
		}	

			for( y=0; y<=0; y++){
				PrintMiniXY(0*18+206    , y*19+3,"SDRAM:", COLOR_BLACK,COLOR_WHITE);
				freq=MaxFreq_RAMwait[WAIT_8]; k=sprintf62(buffer,freq,"MHz  ");					// Max freq RAM RW
				PrintMiniXY(4*18+200+k*12,y*19+3,buffer,     MaxFreq_RAMwait0[WAIT_8]>MaxFreq_RAMwaitDef[WAIT_8] ? COLOR_BLUE:COLOR_BLACK,COLOR_WHITE);
			}
			for( y=2; y<=2; y++){
				PrintMiniXY(0*18+206    , y*19+3,"Write:", COLOR_BLACK,COLOR_WHITE);
				freq=MaxFreq_RAMwaitW[WAIT_6+1]; k=sprintf62(buffer,freq,"MHz  ");					// Max freq RAM RW
				PrintMiniXY(4*18+200+k*12,y*19+3,buffer,     MaxFreq_RAMwaitW0[WAIT_6+1]>MaxFreq_RAMwaitWDef[WAIT_6+1] ? COLOR_BLUE:COLOR_BLACK,COLOR_WHITE);
			}
			
		GetFKeyPtr(0x005f, &iresult);	//INITIAL 	,white
		FKey_Display(3, iresult);

		Fkey_dispR(4," RAM ");
		Fkey_dispR(5," ROM ");

//		GetFKeyPtr(0x0003, &iresult);	//TEST		,black/
//		FKey_Display(5, iresult);
	
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
//			case KEY_CTRL_F5:		//
//				ROMRAM=1-ROMRAM;
//	            break;
			case KEY_CTRL_F5:		//
				if (YesNo("Check SDRAM")==0) break;
				test_wait_mem(1);	// only RAMTEST
	            break;
			case KEY_CTRL_F6:		//
				if (YesNo("Check ROM")==0) break;
				test_wait_mem(0);	// only ROMTEST
	            break;
		}
	}
	return select_func;
}

