//refer to
//	http://community.casiocalc.org/topic/7000-power-graphic-2-development-thread/#entry56396
//
#include "common.h"

int GetMainBatteryVoltagePointer()
{
	unsigned int ea;
	unsigned int j;
	ea = *(unsigned int*)0x8001007C;
	ea = ea + 0x049C*4;
	ea = *(unsigned int*)(ea);
	while (*(unsigned short*)(ea) != 0xE464){
		ea = ea + 2;
	};
	ea = ea + 2;
	j = *(unsigned char*)(ea + 1);
	j = 4*j;
	j = (ea + j + 4) & 0xFFFFFFFC;
return *(unsigned int*)(j);
}

int GetBatteryStatus( int battery, int*firstlevel, int*secondlevel )
{
	int (*iGBS)( int ) = 0; // declare an int function pointer
	int*battconfig = (int*)0x80000334;
	iGBS = (int(*)(int))GetMainBatteryVoltagePointer();
	*firstlevel = 0x2AA;
	*secondlevel = 0x288;
	if (iGBS!=0) return (*iGBS)( battery );
	else return 0;
}

int MainBatteryPercentage( void )
{
	int firstlevel, secondlevel;
	int i;
	if ( IsEmu ) return 500;
	i = GetBatteryStatus( 1, &firstlevel, &secondlevel );
//	if (firstlevel > 0 ) return ( 2000*i )/(firstlevel*3);
//	else return 0;
	return i;
}
