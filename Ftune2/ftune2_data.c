/*
===============================================================================

 Ftune2 is SH7305 CPG&BSC tuning utility for fx-9860GII-2      v1.01

 copyright(c)2014,2015 by sentaro21
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
#include "ftune2_setup.h"

//---------------------------------------------------------------------------------------------

unsigned int	SaveDataF0_FLLFRQ =FLLFRQ_default;		// FLL:900
unsigned int	SaveDataF0_FRQCR  =FRQCRA_default;		// PLL:x16 IFC:1/8 SFC:1/8 BFC:1/8 PFC:1/16
unsigned int	SaveDataF0_CS0BCR =CS0BCR_default;		// IWW:2 IWRRS:2
unsigned int	SaveDataF0_CS2BCR =CS2BCR_default;		// IWW:2 IWRRS:2
unsigned int	SaveDataF0_CS0WCR =CS0WCR_default;		// wait:18
unsigned int	SaveDataF0_CS2WCR =CS2WCR_default;		// wati:2
unsigned int	SaveDataF0_CS5aBCR=CS5aBCR_default;		//
unsigned int	SaveDataF0_CS5aWCR=CS5aWCR_default;		//

unsigned int	SaveDataF2_FLLFRQ =0x00004000+900;		// FLL:900        same as PRIZM
unsigned int	SaveDataF2_FRQCR  =(PLL_16x<<24)+(DIV_4<<20)+(DIV_8<<12)+(DIV_8<<8)+DIV16;
unsigned int	SaveDataF2_CS0BCR =0x24920400;			// IWW:2 IWRRS:2
unsigned int	SaveDataF2_CS2BCR =0x24923400;			// IWW:2 IWRRS:2
unsigned int	SaveDataF2_CS0WCR =0x000001C0;			// wait:3
unsigned int	SaveDataF2_CS2WCR =0x00000140;			// wait:2
unsigned int	SaveDataF2_CS5aBCR=CS5aBCR_default;		//
unsigned int	SaveDataF2_CS5aWCR=CS5aWCR_default;		//

unsigned int	SaveDataF3_FLLFRQ =0x00004000+900;		// FLL:900        same as SH3
unsigned int	SaveDataF3_FRQCR  =(PLL_16x<<24)+(DIV_8<<20)+(DIV_8<<12)+(DIV_8<<8)+DIV16;
unsigned int	SaveDataF3_CS0BCR =0x04900400;			// IWW:0 IWRRS:0
unsigned int	SaveDataF3_CS2BCR =0x04903400;			// IWW:0 IWRRS:0
unsigned int	SaveDataF3_CS0WCR =0x00000140;			// wait:2
unsigned int	SaveDataF3_CS2WCR =0x00000140;			// wait:2
unsigned int	SaveDataF3_CS5aBCR=0x24920200;			//
unsigned int	SaveDataF3_CS5aWCR=CS5aWCR_default;		//

unsigned int	SaveDataF4_FLLFRQ =0x00004000+900;		// FLL:900
unsigned int	SaveDataF4_FRQCR  =(PLL_32x<<24)+(DIV_4<<20)+(DIV_8<<12)+(DIV_8<<8)+DIV16;
unsigned int	SaveDataF4_CS0BCR =0x04900400;			// IWW:0 IWRRS:0
unsigned int	SaveDataF4_CS2BCR =0x04903400;			// IWW:0 IWRRS:0
unsigned int	SaveDataF4_CS0WCR =0x000001C0;			// wait:3
unsigned int	SaveDataF4_CS2WCR =0x00020140;			// wait:2 WW:1
unsigned int	SaveDataF4_CS5aBCR=CS5aBCR_default;		//
unsigned int	SaveDataF4_CS5aWCR=CS5aWCR_default;		//

unsigned int	SaveDataF5_FLLFRQ =0x00004000+900;		// FLL:900
unsigned int	SaveDataF5_FRQCR  =(PLL_32x<<24)+(DIV_2<<20)+(DIV_4<<12)+(DIV_4<<8)+DIV16;
unsigned int	SaveDataF5_CS0BCR =0x14900400;			// IWW:1 IWRRS:0
unsigned int	SaveDataF5_CS2BCR =0x04903400;			// IWW:0 IWRRS:0
unsigned int	SaveDataF5_CS0WCR =0x000003C0;			// wait:8
unsigned int	SaveDataF5_CS2WCR =0x000302C0;			// wait:5 WW:2
unsigned int	SaveDataF5_CS5aBCR=CS5aBCR_default;		//
unsigned int	SaveDataF5_CS5aWCR=CS5aWCR_default;		//

#define dataN	66

void SaveFile(){
	int handle,state;
	int buf[dataN];
	unsigned int key;
	int i,sum;

	handle=Bfile_OpenMainMemory((const unsigned char*)"Ftune2");
	if (handle >= 0) {
		Bfile_CloseFile(handle);
		SaveDisp(SAVEDISP_PAGE1);
		PopUpWin(5);
	   	locate(3,2); Print((unsigned char *) "SAVE DATA     ");
	   	locate(3,3); Print((unsigned char *) "Already Exists");
	   	locate(3,4); Print((unsigned char *) "Overwrite OK?");
	   	locate(3,5); Print((unsigned char *) "   Yes:[F1]");
	   	locate(3,6); Print((unsigned char *) "   No :[F6]");
		Bdisp_PutDisp_DD();
		GetKey(&key);	
		RestoreDisp(SAVEDISP_PAGE1);
		if (key !=KEY_CTRL_F1) return;
	}
	
	if (handle==IML_FILEERR_ENTRYNOTFOUND) {
		handle=Bfile_CreateMainMemory((const unsigned char*)"Ftune2");
		if (handle<0) {ErrorMSG("Create Error",handle); return;}
		state=Bfile_CloseFile(handle);
		if (state<0)  {ErrorMSG("Close Error",state); return;}
	}

	if (handle<0) {ErrorMSG("Open Error",handle); return;}
	
	buf[ 0]=SetupData;
	buf[ 1]=MaxFreq_PLL;
	buf[ 2]=MaxFreq_IFC;
	buf[ 3]=MaxFreq_SFC;
	buf[ 4]=MaxFreq_BFC;
	buf[ 5]=MaxFreq_PFC;
	buf[ 6]=(int)ROMselect;
	buf[ 7]=(int)RAMselect;
		
	buf[ 8]=SaveDataF2_FLLFRQ;
	buf[ 9]=SaveDataF2_FRQCR;
	buf[10]=SaveDataF2_CS0BCR;
	buf[11]=SaveDataF2_CS2BCR;
	buf[12]=SaveDataF2_CS0WCR;
	buf[13]=SaveDataF2_CS2WCR;
	buf[14]=SaveDataF2_CS5aBCR;
	buf[15]=SaveDataF2_CS5aWCR;

	buf[16]=SaveDataF3_FLLFRQ;
	buf[17]=SaveDataF3_FRQCR;
	buf[18]=SaveDataF3_CS0BCR;
	buf[19]=SaveDataF3_CS2BCR;
	buf[20]=SaveDataF3_CS0WCR;
	buf[21]=SaveDataF3_CS2WCR;
	buf[22]=SaveDataF3_CS5aBCR;
	buf[23]=SaveDataF3_CS5aWCR;

	buf[24]=SaveDataF4_FLLFRQ;
	buf[25]=SaveDataF4_FRQCR;
	buf[26]=SaveDataF4_CS0BCR;
	buf[27]=SaveDataF4_CS2BCR;
	buf[28]=SaveDataF4_CS0WCR;
	buf[29]=SaveDataF4_CS2WCR;
	buf[30]=SaveDataF4_CS5aBCR;
	buf[31]=SaveDataF4_CS5aWCR;

	buf[32]=SaveDataF5_FLLFRQ;
	buf[33]=SaveDataF5_FRQCR;
	buf[34]=SaveDataF5_CS0BCR;
	buf[35]=SaveDataF5_CS2BCR;
	buf[36]=SaveDataF5_CS0WCR;
	buf[37]=SaveDataF5_CS2WCR;
	buf[38]=SaveDataF5_CS5aBCR;
	buf[39]=SaveDataF5_CS5aWCR;

	for(i=0;i<10;i++) buf[i+40]=MaxFreq_ROMwait[i];	// 40-49
	for(i=0;i< 8;i++) buf[i+50]=MaxFreq_RAMwait[i];	// 50-57
	for(i=1;i< 8;i++) buf[i+57]=MaxFreq_RAMwaitW[i];// 58-64

	sum=0;
	for(i=0;i<dataN-1;i++) sum+=buf[i];
	buf[dataN-1]=sum;

	handle=Bfile_OpenMainMemory((const unsigned char*)"Ftune2");
	if (handle<0) {ErrorMSG("Open Error",handle); return;}
	state=Bfile_WriteFile(handle,buf,dataN*4);
	if (state<0)  {ErrorMSG("Write Error",state); return;}
	state=Bfile_CloseFile(handle);
	if (state<0)  {ErrorMSG("Close Error",state); return;}
	
	SaveDisp(SAVEDISP_PAGE1);
	PopUpWin(5);
    locate(3,3); Print((unsigned char *) "   Complete!");
   	locate(3,5); Print((unsigned char *) "  Press:[EXIT]");
	Bdisp_PutDisp_DD();

	GetKey(&key);	
	RestoreDisp(SAVEDISP_PAGE1);
}

void test_wait_mem(void);

void LoadFile(int check){
	int handle,state;
	int buf[dataN];
	unsigned int key;
	int i,sum;

	handle=Bfile_OpenMainMemory((const unsigned char*)"Ftune2");
	if (handle<0)  {
		if (check) ErrorMSG("Open Error",handle);
		return;
	}
	state=Bfile_ReadFile(handle, buf, dataN*4, 0);
	if (state<0)  {ErrorMSG("Read Error",state); return;}
	state=Bfile_CloseFile(handle);
	if (state<0)  {ErrorMSG("Close Error",state); return;}
	
	sum=0;
	for(i=0;i<dataN-1;i++) sum+=buf[i];

	if ( buf[dataN-1] != sum ) {
		Bfile_DeleteMainMemory((const unsigned char*)"Ftune2");
		Bdisp_AllClr_VRAM();
		locate(2,2); Print((unsigned char *) "SAVE DATA Broken  ");
		locate(2,4); Print((unsigned char *) "Deleted file");
		locate(2,7); Print((unsigned char *) "Push any Key");
		GetKey(&key);
		return;
	}
	
	if (check) {
		if (YesNo("Load from file")==0) return;
	}

	SetupData=buf[ 0];
	MaxFreq_PLL=buf[ 1];
	MaxFreq_IFC=buf[ 2];
	MaxFreq_SFC=buf[ 3];
	MaxFreq_BFC=buf[ 4];
	MaxFreq_PFC=buf[ 5];
	ROMselect=(unsigned int*)buf[ 6];
	RAMselect=(unsigned int*)buf[ 7];
	
	SaveDataF2_FLLFRQ=buf[ 8];
	SaveDataF2_FRQCR =buf[ 9];
	SaveDataF2_CS0BCR=buf[10];
	SaveDataF2_CS2BCR=buf[11];
	SaveDataF2_CS0WCR=buf[12];
	SaveDataF2_CS2WCR=buf[13];
	SaveDataF2_CS5aBCR=buf[14];
	SaveDataF2_CS5aWCR=buf[15];

	SaveDataF3_FLLFRQ=buf[16];
	SaveDataF3_FRQCR =buf[17];
	SaveDataF3_CS0BCR=buf[18];
	SaveDataF3_CS2BCR=buf[19];
	SaveDataF3_CS0WCR=buf[20];
	SaveDataF3_CS2WCR=buf[21];
	SaveDataF3_CS5aBCR=buf[22];
	SaveDataF3_CS5aWCR=buf[23];

	SaveDataF4_FLLFRQ=buf[24];
	SaveDataF4_FRQCR =buf[25];
	SaveDataF4_CS0BCR=buf[26];
	SaveDataF4_CS2BCR=buf[27];
	SaveDataF4_CS0WCR=buf[28];
	SaveDataF4_CS2WCR=buf[29];
	SaveDataF4_CS5aBCR=buf[30];
	SaveDataF4_CS5aWCR=buf[31];
	
	SaveDataF5_FLLFRQ=buf[32];
	SaveDataF5_FRQCR =buf[33];
	SaveDataF5_CS0BCR=buf[34];
	SaveDataF5_CS2BCR=buf[35];
	SaveDataF5_CS0WCR=buf[36];
	SaveDataF5_CS2WCR=buf[37];
	SaveDataF5_CS5aBCR=buf[38];
	SaveDataF5_CS5aWCR=buf[39];

	for(i=0;i<10;i++) MaxFreq_ROMwait[i]=buf[i+40];	// 40-49
	for(i=0;i< 8;i++) MaxFreq_RAMwait[i]=buf[i+50];	// 50-57
	for(i=1;i< 8;i++) MaxFreq_RAMwaitW[i]=buf[i+57];// 58-64

}


void SaveDataF0(){		// temp save
	SaveDataF0_FLLFRQ =CPG.FLLFRQ.LONG ;
	SaveDataF0_FRQCR  =CPG.FRQCRA.LONG ;
	SaveDataF0_CS0BCR =BSC.CS0BCR.LONG ;
	SaveDataF0_CS2BCR =BSC.CS2BCR.LONG ;
	SaveDataF0_CS0WCR =BSC.CS0WCR.LONG ;
	SaveDataF0_CS2WCR =BSC.CS2WCR.LONG ;
	SaveDataF0_CS5aBCR=BSC.CS5ABCR.LONG ;
	SaveDataF0_CS5aWCR=BSC.CS5AWCR.LONG ;
}
void SaveDataF2(){
	if (YesNo("Store to F2")==0) return;
	SaveDataF2_FLLFRQ =CPG.FLLFRQ.LONG ;
	SaveDataF2_FRQCR  =CPG.FRQCRA.LONG ;
	SaveDataF2_CS0BCR =BSC.CS0BCR.LONG ;
	SaveDataF2_CS2BCR =BSC.CS2BCR.LONG ;
	SaveDataF2_CS0WCR =BSC.CS0WCR.LONG ;
	SaveDataF2_CS2WCR =BSC.CS2WCR.LONG ;
	SaveDataF2_CS5aBCR=BSC.CS5ABCR.LONG  ;
	SaveDataF2_CS5aWCR=BSC.CS5AWCR.LONG;
}
void SaveDataF3(){
	if (YesNo("Store to F3")==0) return;
	SaveDataF3_FLLFRQ =CPG.FLLFRQ.LONG ;
	SaveDataF3_FRQCR  =CPG.FRQCRA.LONG ;
	SaveDataF3_CS0BCR =BSC.CS0BCR.LONG ;
	SaveDataF3_CS2BCR =BSC.CS2BCR.LONG ;
	SaveDataF3_CS0WCR =BSC.CS0WCR.LONG ;
	SaveDataF3_CS2WCR =BSC.CS2WCR.LONG ;
	SaveDataF3_CS5aBCR=BSC.CS5ABCR.LONG ;
	SaveDataF3_CS5aWCR=BSC.CS5AWCR.LONG ;
}
void SaveDataF4(){
	if (YesNo("Store to F4")==0) return;
	SaveDataF4_FLLFRQ =CPG.FLLFRQ.LONG ;
	SaveDataF4_FRQCR  =CPG.FRQCRA.LONG ;
	SaveDataF4_CS0BCR =BSC.CS0BCR.LONG ;
	SaveDataF4_CS2BCR =BSC.CS2BCR.LONG ;
	SaveDataF4_CS0WCR =BSC.CS0WCR.LONG ;
	SaveDataF4_CS2WCR =BSC.CS2WCR.LONG ;
	SaveDataF4_CS5aBCR=BSC.CS5ABCR.LONG ;
	SaveDataF4_CS5aWCR=BSC.CS5AWCR.LONG ;
}
void SaveDataF5(){
	if (YesNo("Store to F5")==0) return;
	SaveDataF5_FLLFRQ =CPG.FLLFRQ.LONG ;
	SaveDataF5_FRQCR  =CPG.FRQCRA.LONG ;
	SaveDataF5_CS0BCR =BSC.CS0BCR.LONG ;
	SaveDataF5_CS2BCR =BSC.CS2BCR.LONG ;
	SaveDataF5_CS0WCR =BSC.CS0WCR.LONG ;
	SaveDataF5_CS2WCR =BSC.CS2WCR.LONG ;
	SaveDataF5_CS5aBCR=BSC.CS5ABCR.LONG ;
	SaveDataF5_CS5aWCR=BSC.CS5AWCR.LONG ;
}


void LoadDataF0(){		// temp load
	BSC.CS0WCR.BIT.WR = WAIT18 ;
	BSC.CS2WCR.BIT.WR = WAIT18 ;
	CPG.FLLFRQ.LONG   = SaveDataF0_FLLFRQ ;
	change_FRQCRs(      SaveDataF0_FRQCR ) ;
	BSC.CS0BCR.LONG   = SaveDataF0_CS0BCR ;
	BSC.CS2BCR.LONG   = SaveDataF0_CS2BCR ;
	BSC.CS0WCR.LONG   = SaveDataF0_CS0WCR ;
	BSC.CS2WCR.LONG   = SaveDataF0_CS2WCR ;
	BSC.CS5ABCR.LONG  = SaveDataF0_CS5aBCR ;
	BSC.CS5AWCR.LONG  = SaveDataF0_CS5aWCR ;
}
void LoadDataF1(){				// default 
	BSC.CS0WCR.BIT.WR = WAIT18 ;
	BSC.CS2WCR.BIT.WR = WAIT18 ;
	CPG.FLLFRQ.LONG   = FLLFRQ_default ;
	change_FRQCRs(      FRQCRA_default ) ;
	BSC.CS0BCR.LONG   = CS0BCR_default ;
	BSC.CS2BCR.LONG   = CS2BCR_default ;
	BSC.CS0WCR.LONG   = CS0WCR_default ;
	BSC.CS2WCR.LONG   = CS2WCR_default ;
	BSC.CS5ABCR.LONG  = CS5aBCR_default ;
	BSC.CS5AWCR.LONG  = CS5aWCR_default ;
}
void LoadDataF2(){
	BSC.CS0WCR.BIT.WR = WAIT18 ;
	BSC.CS2WCR.BIT.WR = WAIT18 ;
	CPG.FLLFRQ.LONG   = SaveDataF2_FLLFRQ ;
	change_FRQCRs(      SaveDataF2_FRQCR ) ;
	BSC.CS0BCR.LONG   = SaveDataF2_CS0BCR ;
	BSC.CS2BCR.LONG   = SaveDataF2_CS2BCR ;
	BSC.CS0WCR.LONG   = SaveDataF2_CS0WCR ;
	BSC.CS2WCR.LONG   = SaveDataF2_CS2WCR ;
	BSC.CS5ABCR.LONG  = SaveDataF2_CS5aBCR ;
	BSC.CS5AWCR.LONG  = SaveDataF2_CS5aWCR ;
}
void LoadDataF3(){
	BSC.CS0WCR.BIT.WR = WAIT18 ;
	BSC.CS2WCR.BIT.WR = WAIT18 ;
	CPG.FLLFRQ.LONG   = SaveDataF3_FLLFRQ ;
	change_FRQCRs(      SaveDataF3_FRQCR ) ;
	BSC.CS0BCR.LONG   = SaveDataF3_CS0BCR ;
	BSC.CS2BCR.LONG   = SaveDataF3_CS2BCR ;
	BSC.CS0WCR.LONG   = SaveDataF3_CS0WCR ;
	BSC.CS2WCR.LONG   = SaveDataF3_CS2WCR ;
	BSC.CS5ABCR.LONG  = SaveDataF3_CS5aBCR ;
	BSC.CS5AWCR.LONG  = SaveDataF3_CS5aWCR ;
}
void LoadDataF4(){
	BSC.CS0WCR.BIT.WR = WAIT18 ;
	BSC.CS2WCR.BIT.WR = WAIT18 ;
	CPG.FLLFRQ.LONG   = SaveDataF4_FLLFRQ ;
	change_FRQCRs(      SaveDataF4_FRQCR ) ;
	BSC.CS0BCR.LONG   = SaveDataF4_CS0BCR ;
	BSC.CS2BCR.LONG   = SaveDataF4_CS2BCR ;
	BSC.CS0WCR.LONG   = SaveDataF4_CS0WCR ;
	BSC.CS2WCR.LONG   = SaveDataF4_CS2WCR ;
	BSC.CS5ABCR.LONG  = SaveDataF4_CS5aBCR ;
	BSC.CS5AWCR.LONG  = SaveDataF4_CS5aWCR ;
}
void LoadDataF5(){
	BSC.CS0WCR.BIT.WR = WAIT18 ;
	BSC.CS2WCR.BIT.WR = WAIT18 ;
	CPG.FLLFRQ.LONG   = SaveDataF5_FLLFRQ ;
	change_FRQCRs(      SaveDataF5_FRQCR ) ;
	BSC.CS0BCR.LONG   = SaveDataF5_CS0BCR ;
	BSC.CS2BCR.LONG   = SaveDataF5_CS2BCR ;
	BSC.CS0WCR.LONG   = SaveDataF5_CS0WCR ;
	BSC.CS2WCR.LONG   = SaveDataF5_CS2WCR ;
	BSC.CS5ABCR.LONG  = SaveDataF5_CS5aBCR ;
	BSC.CS5AWCR.LONG  = SaveDataF5_CS5aWCR ;
}

