/*
===============================================================================

 Ftune2 is SH7305 CPG&BSC tuning utility for fx-9860GII-2      v1.01

 copyright(c)2014,2015 by sentaro21
 e-mail sentaro21@pm.matrix.jp

===============================================================================
*/

#include "ftune2.h"
#include "ftune2_setup.h"

//---------------------------------------------------------------------------------------------
void FRQCR_kick(void) { 
    CPG.FRQCRA.BIT.KICK = 1 ;
    while((CPG.LSTATS & 1)!=0);
}
void change_FRQCRs( unsigned int value) { 
    CPG.FRQCRA.LONG = value ;
    FRQCR_kick();
}
void change_FRQCR(int pll, int ifc, int sfc, int bfc, int pfc) { 
    CPG.FRQCRA.LONG = ((pll<<24)+(ifc<<20)+(sfc<<12)+(bfc<<8)+pfc) ;
    FRQCR_kick();
}
void change_STC(int stc) {
    CPG.FRQCRA.BIT.STC = stc ;
    FRQCR_kick();
}
void change_IFC(int ifc) {
    CPG.FRQCRA.BIT.IFC = ifc ;
    FRQCR_kick();
}
void change_SFC(int sfc) {
    CPG.FRQCRA.BIT.SFC = sfc ;
    FRQCR_kick();
}
void change_BFC(int bfc) {
    CPG.FRQCRA.BIT.BFC = bfc ;
    FRQCR_kick();
}
void change_PFC(int pfc) {
    CPG.FRQCRA.BIT.PFC = pfc ;
    FRQCR_kick();
}
//---------------------------------------------------------------------------------------------
volatile unsigned int*ROMselect = 0 ;		// ROM wait check base address
volatile unsigned int*RAMselect = 0 ;		// RAM wait check base address

int	MaxFreq_PLL=Max_PLLdef;
int	MaxFreq_IFC=Max_IFCdef;
int	MaxFreq_SFC=Max_SFCdef;
int	MaxFreq_BFC=Max_BFCdef;
int	MaxFreq_PFC=Max_PFCdef;

int MaxFreq_ROMwaitDef[]={MaxROMwait_0,MaxROMwait_1,MaxROMwait_2,MaxROMwait_3,MaxROMwait_4,
						  MaxROMwait_5,MaxROMwait_6,MaxROMwait_8,MaxROMwait10,MaxROMwait12,
						  MaxROMwait14,MaxROMwait18};
int MaxFreq_ROMwait0[]=  {MaxROMwait_0,MaxROMwait_1,MaxROMwait_2,MaxROMwait_3,MaxROMwait_4,
						  MaxROMwait_5,MaxROMwait_6,MaxROMwait_8,MaxROMwait10,MaxROMwait12,
						  MaxROMwait14,MaxROMwait18};
int MaxFreq_ROMwait[]=   {MaxROMwait_0,MaxROMwait_1,MaxROMwait_2,MaxROMwait_3,MaxROMwait_4,
						  MaxROMwait_5,MaxROMwait_6,MaxROMwait_8,MaxROMwait10,MaxROMwait12,
						  MaxROMwait14,MaxROMwait18};

int MaxFreq_RAMwaitDef[]={MaxRAMwait_0,MaxRAMwait_1,MaxRAMwait_2,MaxRAMwait_3,MaxRAMwait_4,
						  MaxRAMwait_5,MaxRAMwait_6,MaxRAMwait_8,MaxRAMwait10};
int MaxFreq_RAMwait0[]=  {MaxRAMwait_0,MaxRAMwait_1,MaxRAMwait_2,MaxRAMwait_3,MaxRAMwait_4,
						  MaxRAMwait_5,MaxRAMwait_6,MaxRAMwait_8,MaxRAMwait10};
int MaxFreq_RAMwait[]=   {MaxRAMwait_0,MaxRAMwait_1,MaxRAMwait_2,MaxRAMwait_3,MaxRAMwait_4,
						  MaxRAMwait_5,MaxRAMwait_6,MaxRAMwait_8,MaxRAMwait10};
						  
int MaxFreq_RAMwaitWDef[]={-1,MaxRAMwaitW_0,MaxRAMwaitW_1,MaxRAMwaitW_2,MaxRAMwaitW_3,MaxRAMwaitW_4,
						  MaxRAMwaitW_5,MaxRAMwaitW_6};
int MaxFreq_RAMwaitW0[]=  {-1,MaxRAMwaitW_0,MaxRAMwaitW_1,MaxRAMwaitW_2,MaxRAMwaitW_3,MaxRAMwaitW_4,
						  MaxRAMwaitW_5,MaxRAMwaitW_6};
int MaxFreq_RAMwaitW[]=   {-1,MaxRAMwaitW_0,MaxRAMwaitW_1,MaxRAMwaitW_2,MaxRAMwaitW_3,MaxRAMwaitW_4,
						  MaxRAMwaitW_5,MaxRAMwaitW_6};

void SetMaxFreq0(void){
	int rom=100-ROMmargin();
	int ram=100-RAMmargin();
	int i;
	for(i=0;i<10;i++) MaxFreq_ROMwait0[i]=MaxFreq_ROMwait[i]*100/rom;
	for(i=0;i< 8;i++) MaxFreq_RAMwait0[i]=MaxFreq_RAMwait[i]*100/ram;
	for(i=1;i< 8;i++) MaxFreq_RAMwaitW0[i]=MaxFreq_RAMwaitW[i]*100/ram;
}
void SetMaxFreq(void){
	int rom=100-ROMmargin();
	int ram=100-RAMmargin();
	int i;
	for(i=0;i<10;i++) MaxFreq_ROMwait[i]=MaxFreq_ROMwait0[i]*rom/100;
	for(i=0;i< 8;i++) MaxFreq_RAMwait[i]=MaxFreq_RAMwait0[i]*ram/100;
	for(i=1;i< 8;i++) MaxFreq_RAMwaitW[i]=MaxFreq_RAMwaitW0[i]*ram/100;
}

void wait_init(void){
	ROMselect=0;
	RAMselect=0;
	
	MaxFreq_ROMwait0[WAIT_0]=MaxROMwait_0;
	MaxFreq_ROMwait0[WAIT_1]=MaxROMwait_1;
	MaxFreq_ROMwait0[WAIT_2]=MaxROMwait_2;
	MaxFreq_ROMwait0[WAIT_3]=MaxROMwait_3;
	MaxFreq_ROMwait0[WAIT_4]=MaxROMwait_4;
	MaxFreq_ROMwait0[WAIT_5]=MaxROMwait_5;
	MaxFreq_ROMwait0[WAIT_6]=MaxROMwait_6;
	MaxFreq_ROMwait0[WAIT_8]=MaxROMwait_8;
	MaxFreq_ROMwait0[WAIT10]=MaxROMwait10;
	MaxFreq_ROMwait0[WAIT12]=MaxROMwait12;

	MaxFreq_RAMwait0[WAIT_0]=MaxRAMwait_0;
	MaxFreq_RAMwait0[WAIT_1]=MaxRAMwait_1;
	MaxFreq_RAMwait0[WAIT_2]=MaxRAMwait_2;
	MaxFreq_RAMwait0[WAIT_3]=MaxRAMwait_3;
	MaxFreq_RAMwait0[WAIT_4]=MaxRAMwait_4;
	MaxFreq_RAMwait0[WAIT_5]=MaxRAMwait_5;
	MaxFreq_RAMwait0[WAIT_6]=MaxRAMwait_6;
	MaxFreq_RAMwait0[WAIT_8]=MaxRAMwait_8;
	MaxFreq_RAMwait0[WAIT_0]=MaxRAMwait_0;
	
	MaxFreq_RAMwaitW0[WAITW0]=MaxRAMwaitW_0;
	MaxFreq_RAMwaitW0[WAITW1]=MaxRAMwaitW_1;
	MaxFreq_RAMwaitW0[WAITW2]=MaxRAMwaitW_2;
	MaxFreq_RAMwaitW0[WAITW3]=MaxRAMwaitW_3;
	MaxFreq_RAMwaitW0[WAITW4]=MaxRAMwaitW_4;
	MaxFreq_RAMwaitW0[WAITW5]=MaxRAMwaitW_5;
	MaxFreq_RAMwaitW0[WAITW6]=MaxRAMwaitW_6;

	SetMaxFreq();
}


int CheckROMwait(int freq){						 // ROM
	int i;
	MaxFreq_ROMwait[WAIT14]=(MaxFreq_ROMwait[WAIT12]*2-MaxFreq_ROMwait[WAIT10])*995/1000;
	MaxFreq_ROMwait[WAIT18]=(MaxFreq_ROMwait[WAIT14]*2-MaxFreq_ROMwait[WAIT10])*995/1000;
	for (i=WAIT18; i>=WAIT_0; i--){
		if ( freq >= MaxFreq_ROMwait[i] ) break;
	}
	 return ( i+1 );
}

void SetROMwait(int freq,int CS0WCR_WR){
	int	WR;
	int	IWW0;
	IWW0  = BSC.CS0BCR.BIT.IWW ;
	if ( ( freq >= MaxFreq_ROMwait[WAIT_2] ) && ( IWW0 == 0 ) )	BSC.CS0BCR.BIT.IWW = ( 1 ) ;
	if ( ( freq >= MaxFreq_ROMwait[WAIT_6] ) && ( IWW0 == 1 ) )	BSC.CS0BCR.BIT.IWW = ( 2 ) ;
	if ( ( freq >= MaxFreq_ROMwait[WAIT12] ) && ( IWW0 == 2 ) )	BSC.CS0BCR.BIT.IWW = ( 3 ) ;

	WR=CheckROMwait(freq);
	if ( WR > CS0WCR_WR ) BSC.CS0WCR.BIT.WR = ( WR ) ;	//	Wait +
}

int CheckRAMwait(int freq){						 // RAM
	int i;
	MaxFreq_RAMwait[WAIT10]=(MaxFreq_RAMwait[WAIT_8]*2-MaxFreq_RAMwait[WAIT_6])*995/1000;
	for (i=WAIT10; i>=WAIT_0; i--){
		if ( freq >= MaxFreq_RAMwait[i] ) break;
	}
	 return ( i+1 );
}

int CheckRAMwaitW(int freq){						 // RAM write
	int i;
	for (i=WAITW6; i>=WAITW0; i--){
		if ( freq >= MaxFreq_RAMwaitW[i] ) break;
	}
	 return ( i+1 );
}

void SetRAMwait(int freq,int CS2WCR_WR){
	int	WR;

	WR=CheckRAMwait(freq);
	if ( WR > CS2WCR_WR ) BSC.CS2WCR.BIT.WR = ( WR ) ;	//	wait +
	
	if ( ( ((SetupData>>3)&1) == 0 ) && ( BSC.CS2WCR.BIT.WW == 0 ) ) return;		// WW wait auto  off
	BSC.CS2WCR.BIT.WW = CheckRAMwaitW(freq) ;
}

//---------------------------------------------------------------------------------------------

int freq_sub(int flf,int stc,int fc) {
	int freq_base=flf*32768*10/1000/2; 	// default 900*32.768KHz / 2 = 14.7456MHz
	int	freq_div=2;
	if (fc<0) freq_div=1;
	while(fc>0) {freq_div*=2; fc-=1;}
	return	( ((freq_base*(stc+1)/freq_div)+50)/100 );		// precision 10KHz   ex. 14.7456MHz => 1475
}
int CheckFC(int FLF,int FRQCR_STC,int FC,int maxfreq){
	int freq;
	freq=freq_sub(FLF,FRQCR_STC,FC);	// freq
	if (freq>=maxfreq) return 0;
	return freq;
}
int UpIFC(int FLF,int FRQCR_STC,int FRQCR_IFC){				// IFC freq UP
	int freq;
	if (freq=CheckFC(FLF,FRQCR_STC,FRQCR_IFC,MaxFreq_BFC) != 0) change_IFC(FRQCR_IFC);
	return freq;
}
int UpSFC(int FLF,int FRQCR_STC,int FRQCR_SFC){				// SFC freq UP
	int freq;
	if (freq=CheckFC(FLF,FRQCR_STC,FRQCR_SFC,MaxFreq_BFC) != 0) change_SFC(FRQCR_SFC);
	return freq;
}
int UpBFC(int FLF,int FRQCR_STC,int FRQCR_BFC,int CS0WCR_WR,int CS2WCR_WR){				// BFC freq UP
	int freq;
	freq=CheckFC(FLF,FRQCR_STC,FRQCR_BFC,MaxFreq_BFC);
	if (freq != 0) {
		SetROMwait(freq,CS0WCR_WR);
		SetRAMwait(freq,CS2WCR_WR);
		change_BFC(FRQCR_BFC);
	}
	return freq;
}
int UpPFC(int FLF,int FRQCR_STC,int FRQCR_PFC){				// PFC freq UP
	int freq;
	if (freq=CheckFC(FLF,FRQCR_STC,FRQCR_PFC,MaxFreq_BFC) != 0) change_PFC(FRQCR_PFC);
	return freq;
}

void PFC_auto_up(int FLF,int FRQCR_STC,int FRQCR_BFC,int FRQCR_PFC){
	if ( ( (SetupData>>5)&1 ) == 0 ) return;		// PFC auto up off
	if ( (CheckFC(FLF,FRQCR_STC,FRQCR_PFC-1,MaxFreq_PFC)) == 0 ) return;	// PFC/2  freq
	if ( FRQCR_PFC == FRQCR_BFC ) return;
	change_PFC(FRQCR_PFC-1);
}
void PFC_auto_down(int FLF,int FRQCR_STC,int FRQCR_PFC){
	if ( CheckFC(FLF,FRQCR_STC,FRQCR_PFC,MaxFreq_PFC) != 0) return;			// PFC freq
	if ( FRQCR_PFC >= DIV64 ) return;
	change_PFC(FRQCR_PFC+1);
}

void SetWait(int FLF,int FRQCR_STC,int FRQCR_BFC,int CS0WCR_WR,int CS2WCR_WR){
	int freq;
	freq=freq_sub(FLF,FRQCR_STC,FRQCR_BFC);	// Bus freq
	SetROMwait(freq,CS0WCR_WR);
	SetRAMwait(freq,CS2WCR_WR);
}

void SetWaitm(int FLF,int FRQCR_STC,int FRQCR_BFC,int CS0WCR_WR,int CS2WCR_WR){
	int freq;
	int	WR;
	int	IWW0;
	
	freq=freq_sub(FLF+1,FRQCR_STC,FRQCR_BFC);	// Bus freq  (FLF+1)
	if ( (SetupData>>3)&1 ) BSC.CS2WCR.BIT.WW = ( CheckRAMwaitW(freq) );		// WW wait auto decrement

	if ( ( (SetupData>>2)&1 ) == 0 ) return;		// wait auto decrement off
	WR=CheckROMwait(freq);
	if ( WR < CS0WCR_WR ) BSC.CS0WCR.BIT.WR = ( WR );	// ROM wait decrement
	WR=CheckRAMwait(freq);
	if ( WR < CS2WCR_WR ) BSC.CS2WCR.BIT.WR = ( WR );	// RAM wait decrement
	
	IWW0  = BSC.CS0BCR.BIT.IWW ;
	if ( ( (SetupData>>4)&1 ) == 0 ) return;		// IWW wait auto decrement off
	if ( ( freq <  MaxFreq_ROMwait[WAIT_2] ) && ( IWW0 > 0 ) )	BSC.CS0BCR.BIT.IWW = 0 ;
	if ( ( freq <  MaxFreq_ROMwait[WAIT_6] ) && ( IWW0 > 1 ) )	BSC.CS0BCR.BIT.IWW = 1 ;
	if ( ( freq <  MaxFreq_ROMwait[WAIT12] ) && ( IWW0 > 2 ) )	BSC.CS0BCR.BIT.IWW = 2 ;

}
