/*
===============================================================================

 Ftune is SH7337/SH7355 CPG&BSC tuning utility for fx-9860G/GII series  v1.01

 copyright(c)2014,2015,2019 by sentaro21
 e-mail sentaro21@pm.matrix.jp

===============================================================================
 */
#include "fxlib.h"
#include "timer.h"
#include "stdio.h"
#include "string.h"
#include "ftune_benchmark.h"
#include "common.h"

char IsSH3;	//	3:SH3   4:SH4
char IsHiddenRAM;

//---------------------------------------------------------------------------------------------
void * HiddenRAM(void){	// Check HiddenRAM 
	volatile unsigned int *NorRAM=(volatile unsigned int*)0xA8000000;	// Nomarl RAM TOP (no cache area)
	volatile unsigned int *HidRAM=(volatile unsigned int*)0x88040000;	// Hidden RAM TOP (cache area)
	int a,b;
	int K55=0x55555555;
	int KAA=0xAAAAAAAA;
	char * HidAddress=NULL;

	IsHiddenRAM=0;
	a= *NorRAM;
	b= *HidRAM;
	*NorRAM=K55;
	*HidRAM=KAA;
	if ( *NorRAM != *HidRAM ) {
			HidAddress=(char*)HidRAM;	// Hidden RAM Exist
			IsHiddenRAM=1;
	}
	*NorRAM=a;
	*HidRAM=b;
	return HidAddress;
}

int OS_Version(){
	int ver;
	unsigned char version[16];
	System_GetOSVersion( &version[0] );
	return (version[1]-'0')*100 + (version[3]-'0')*10 + (version[4]-'0');
}

//--------------------------------------------------------------------------------------------- battery
// refer to SuperH-based fx calculators (version 20) by SimonLothar

int GetMainBatteryVoltagePointer(){
	unsigned int ea;
	unsigned int j;
    ea = *(unsigned int*)0x8001007C;
    ea = ea + 0x049C*4;
    ea = *(unsigned int*)( ea );
    while ( *(unsigned short*)( ea ) != 0xE464 ){
      ea = ea + 2;
    };
    ea = ea + 2;
    j = *(unsigned char*)( ea + 1 );
    j = 4*j;
    j = ( ea + j + 4 ) & 0xFFFFFFFC;
    return *(unsigned int*)( j );
}
// battery == 1 : main (ADC channel C)
// battery != 1 : possibly the backup battery (ADC channel B)
// firstlevel: warning level
// secondlevel: shutdown level
 
int GetBatteryStatus( int battery, int*firstlevel, int*secondlevel ){
	int (*iGBS)( int ) = 0; // declare an int function pointer
	int*battconfig = (int*)0x80000334;
	*firstlevel = 0x17F;
	*secondlevel = 0x16D;

	iGBS = (int(*)(int))GetMainBatteryVoltagePointer();

	switch ( OS_Version() ){
	    case 102 : break;
	    case 103 : break;
	    case 104 :
	    case 105 :
	        if ( *battconfig==0x43444844 ){
	            *firstlevel = 0x1A5;
	            *secondlevel = 0x191;
	        };
	        break;
	    case 110 :
	    case 111 :
	        *firstlevel = 0x2AA;
	        *secondlevel = 0x288;
	        break;
	    case 200 :
	        if ( *battconfig==0x43444844 ){
	            *firstlevel = 0x1A5;
	            *secondlevel = 0x191;
	        };
	        break;
	};
	if (CPU_check()==2){	// IsSlim
	    *firstlevel = 0x2AA;
	    *secondlevel = 0x288;
	} 

	if (iGBS!=0) return (*iGBS)( battery );
	else return 0;
}


int GetMainBatteryVoltage( int battery ) {
	int firstlevel, secondlevel;
	if ( IsEmu ) return 500;
	return GetBatteryStatus( battery, &firstlevel, &secondlevel );
}

int BatteryStatus(){
	int r = GetMainBatteryVoltage( 1 ) ;
	switch  ( CPU_check() ) {
//		case 0:	// SH4A
//			break;
		case 1: // SH3
			HiddenRAM();
			if ( IsHiddenRAM == 0 ) r = r*111/100;	// fx-9860G
			else					r = r*101/100;	// fx-9860GII
			break;
		case 2: // Slim
			r = r* 30/100;
			break;
		default:
			break;
	}
	return r;
}

