/*
===============================================================================

 Ftune is SH7337/SH7355 CPG&BSC tuning utility for fx-9860G/GII series  v1.01

 copyright(c)2014,2015 by sentaro21
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
#include "ftune_memcheck.h"

//---------------------------------------------------------------------------------------------
#define CheckN 2000

//unsigned int* MEMtestsR(volatile unsigned int*MEM,volatile unsigned int *CSnWCR,int wait,int n)
//	int j,a,b;
//	j=0;
// LOOP:
//		a=*MEM;
//		b=*MEM++;
//		if ( a != b ) goto RET;
//	j++; if ( j < n ) goto LOOP;
//	MEM=0;
// RET:
//	*CSnWCR = wait;
//	return ( MEM );

//unsigned int* MEMtestsW(volatile unsigned int*MEM,volatile unsigned int *CSnWCR,int wait,int n)
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
//	*CSnWCR = wait;
//	return ( MEM );



int ROMtestR(int CS0WCR_WR,volatile unsigned int*ROM,int n){
	int a,b;
	unsigned int*ad;
	int tmpCS0WCR;
	tmpCS0WCR = BSC.CS0WCR.LONG;
	ad=ROM;
	BSC.CS0WCR.BIT.WR  = CS0WCR_WR-1 ;
	if ( *ROM == *ROM ) ad=MEMtestsR(ROM, &BSC.CS0WCR.LONG, CS0WCR_default,n);
	BSC.CS0WCR.LONG = tmpCS0WCR;
	return ( (int)ad );
}

int RAMtestR(int CS2WCR_WR,volatile unsigned int*RAM,int n){
	int a,b;
	unsigned int*ad;
	int tmpCS2WCR;
	tmpCS2WCR = BSC.CS2WCR.LONG;
	ad=RAM;
	BSC.CS2WCR.BIT.WR  = CS2WCR_WR-1 ;
	if ( *RAM == *RAM ) ad=MEMtestsR(RAM, &BSC.CS2WCR.LONG, CS2WCR_default,n);
	BSC.CS2WCR.LONG = tmpCS2WCR;
	return ( (int)ad );
}

int RAMtestW(int CS2WCR_WW,volatile unsigned int*RAM, int n){
	int a,b;
	unsigned int*ad;
	int tmpCS2WCR;
	tmpCS2WCR = BSC.CS2WCR.LONG;
	ad=RAM;
	BSC.CS2WCR.BIT.WW  = CS2WCR_WW-1 ;		// WW
	if ( *RAM == *RAM ) ad=MEMtestsW(RAM, &BSC.CS2WCR.LONG, CS2WCR_default,n);
	BSC.CS2WCR.LONG = tmpCS2WCR;
	return ( (int)ad );
}


int ROMtestn(int CS0WCR_WR){
	volatile unsigned int*ROM     = (unsigned int*) 0xA0000000;	// 
	char buf[20];
	int	i,j;
	j=ROMtestR(CS0WCR_WR+1,ROM,1000);		//	into CACHE
	for(i=0;i<126;i++){
		HexPrintXY(7,4,"",(int)ROM);
		Bdisp_PutDisp_DD();
		j=ROMtestR(CS0WCR_WR,ROM,65536/4); if ( j ) return ( j );
		ROM+=0x8000/4;
	}
	return ( j );
}

int RAMtestn(int CS2WCR_WR){
	volatile unsigned int*RAM     = (unsigned int*) 0xA8000000;		// ?
	char buf[20];
	int	i,j;
	j=RAMtestR(CS2WCR_WR+1,RAM,1000);			//	into CACHE
	for(i=0;i<112;i++){
		HexPrintXY(7,4,"",(int)RAM);
		Bdisp_PutDisp_DD();
		j=RAMtestR(CS2WCR_WR,RAM,65536/4); if ( j ) return ( j );
		RAM+=0x1000/4;
	}
	return ( j );
}

int RAMtestnW(int CS2WCR_WW){
	unsigned int tmpRAM[CheckN+1];	// RAM write test area
	unsigned int *RAM;
	char buf[20];
	int	i,j;
	RAM=(unsigned int*) ( ((int)&tmpRAM & 0x0FFFFFFF) | 0xA0000000 );
	j=RAMtestW(CS2WCR_WW+1,RAM,CheckN);			//	into CACHE
	for(i=0;i<100;i++){
		HexPrintXY(7,4,"",(int)RAM);
		Bdisp_PutDisp_DD();
		j=RAMtestW(CS2WCR_WW,RAM,CheckN); if ( j ) return ( j );
	}
	return ( j );
}
