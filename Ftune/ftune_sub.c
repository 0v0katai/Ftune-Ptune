/*
===============================================================================

 Ftune is SH7337/SH7355 CPG&BSC tuning utility for fx-9860G/GII series  v1.01

 copyright(c)2014,2015 by sentaro21
 e-mail sentaro21@pm.matrix.jp

===============================================================================
*/

#include	"ftune.h"

//---------------------------------------------------------------------------------------------
void change_FRQCRs( unsigned int value) { 
	WDT.WTCNT.WRITE = 0;
    WDT.WTCSR.WRITE = 0x65;    //  0b01100101
    CPG.FRQCR.WORD  = 0x1000 | value;
}
void change_FRQCR(int stc, int ifc, int pfc) { 
    change_FRQCRs( 0x1000 | (stc << 8) | (ifc << 4) | (pfc) );
}
int get_FRQCR(void){
    return CPG.FRQCR.WORD;
}
int get_FRQCR_STC(void) {
    return ( CPG.FRQCR.BIT.STC ) ;
}
int get_FRQCR_IFC(void) {
    return ( CPG.FRQCR.BIT.IFC ) ;
}
int get_FRQCR_SFC(void) {
    return 0;
}
int get_FRQCR_BFC(void) {
    return 0;
}
int get_FRQCR_PFC(void) {
    return ( CPG.FRQCR.BIT.PFC ) ;
}

//---------------------------------------------------------------------------------------------

int fmult;

void freq_check(void) {					// if hardware X'tal x2 modified check
    int fc,count;

	fc=get_FRQCR_STC()+1;
	count=Bench(25,0);
	count=Bench(25,0)/fc;
	if ( count < 1500 ) fmult=1;		// normal    x1
				else    fmult=2;		// overclock x2
}

int freq_sub(int stc,int fc) {
	int freq_base=147456*2; // 14.7456MHz*2
	int	freq_div=1;
	if (fc==0) freq_div=1;
	if (fc==1) freq_div=2;
	if (fc==2) freq_div=3;
	if (fc==3) freq_div=4;
	return	((freq_base*(stc+1)/freq_div)*fmult/100);
}
//---------------------------------------------------------------------------------------------

