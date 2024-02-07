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

//---------------------------------------------------------------------------------------------
unsigned short	SaveDataF0_FRQCR =FRQCR_default;		// PLL:x1 IFC:1/1 PFC:1/2
unsigned int	SaveDataF0_CS0BCR=CS0BCR_default;		// IWW:0 IWRRS:0
unsigned int	SaveDataF0_CS2BCR=CS2BCR_default;		// IWW:0 IWRRS:0
unsigned int	SaveDataF0_CS0WCR=CS0WCR_default;		// wait:2
unsigned int	SaveDataF0_CS2WCR=CS2WCR_default;		// wati:2
unsigned int	SaveDataF0_CS5aBCR=CS5aBCR_default;		//
unsigned int	SaveDataF0_CS5aWCR=CS5aWCR_default;		//

unsigned short	SaveDataF2_FRQCR =(PLL_2x<<8)+(DIV_1<<4)+DIV_2;
unsigned int	SaveDataF2_CS0BCR=CS0BCR_default;		// IWW:0 IWRRS:0
unsigned int	SaveDataF2_CS2BCR=CS2BCR_default;		// IWW:0 IWRRS:0
unsigned int	SaveDataF2_CS0WCR=CS0WCR_default;		// wait:2
unsigned int	SaveDataF2_CS2WCR=CS2WCR_default;		// wati:2
unsigned int	SaveDataF2_CS5aBCR=CS5aBCR_default;		//
unsigned int	SaveDataF2_CS5aWCR=CS5aWCR_default;		//

unsigned short	SaveDataF3_FRQCR =(PLL_3x<<8)+(DIV_1<<4)+DIV_3;
unsigned int	SaveDataF3_CS0BCR=CS0BCR_default;		// IWW:0 IWRRS:0
unsigned int	SaveDataF3_CS2BCR=CS2BCR_default;		// IWW:0 IWRRS:0
unsigned int	SaveDataF3_CS0WCR=CS0WCR_default;		// wait:2
unsigned int	SaveDataF3_CS2WCR=CS2WCR_default;		// wati:2
unsigned int	SaveDataF3_CS5aBCR=CS5aBCR_default;		//
unsigned int	SaveDataF3_CS5aWCR=CS5aWCR_default;		//

unsigned short	SaveDataF4_FRQCR =(PLL_4x<<8)+(DIV_1<<4)+DIV_4;
unsigned int	SaveDataF4_CS0BCR=CS0BCR_default;		// IWW:0 IWRRS:0
unsigned int	SaveDataF4_CS2BCR=CS2BCR_default;		// IWW:0 IWRRS:0
unsigned int	SaveDataF4_CS0WCR=CS0WCR_default;		// wait:2
unsigned int	SaveDataF4_CS2WCR=CS2WCR_default;		// wati:2
unsigned int	SaveDataF4_CS5aBCR=CS5aBCR_default;		//
unsigned int	SaveDataF4_CS5aWCR=CS5aWCR_default;		//

unsigned short	SaveDataF5_FRQCR =(PLL_4x<<8)+(DIV_1<<4)+DIV_4;
unsigned int	SaveDataF5_CS0BCR=CS0BCR_default;		// IWW:0 IWRRS:0
unsigned int	SaveDataF5_CS2BCR=CS2BCR_default;		// IWW:0 IWRRS:0
unsigned int	SaveDataF5_CS0WCR=0x000000C0;			// wait:1
unsigned int	SaveDataF5_CS2WCR=0x000100C0;			// wati:1 WW=0
unsigned int	SaveDataF5_CS5aBCR=CS5aBCR_default;		//
unsigned int	SaveDataF5_CS5aWCR=CS5aWCR_default;		//

#define dataN	29

void SaveFile(){
	int handle,state;
	int buf[dataN];
	unsigned int key;
	int i,sum;

	handle=Bfile_OpenMainMemory((const unsigned char*)"Ftune");
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
		handle=Bfile_CreateMainMemory((const unsigned char*)"Ftune");
		if (handle<0) {ErrorMSG("Create Error",handle); return;}
		state=Bfile_CloseFile(handle);
		if (state<0)  {ErrorMSG("Close Error",state); return;}
	}

	if (handle<0) {ErrorMSG("Open Error",handle); return;}
	
	buf[ 0]=0x5555AAAA;
		
	buf[ 1]=(int)SaveDataF2_FRQCR;
	buf[ 2]=SaveDataF2_CS0BCR;
	buf[ 3]=SaveDataF2_CS2BCR;
	buf[ 4]=SaveDataF2_CS0WCR;
	buf[ 5]=SaveDataF2_CS2WCR;
	buf[ 6]=SaveDataF2_CS5aBCR;
	buf[ 7]=SaveDataF2_CS5aWCR;
	
	buf[ 8]=(int)SaveDataF3_FRQCR;
	buf[ 9]=SaveDataF3_CS0BCR;
	buf[10]=SaveDataF3_CS2BCR;
	buf[11]=SaveDataF3_CS0WCR;
	buf[12]=SaveDataF3_CS2WCR;
	buf[13]=SaveDataF3_CS5aBCR;
	buf[14]=SaveDataF3_CS5aWCR;
	
	buf[15]=(int)SaveDataF4_FRQCR;
	buf[16]=SaveDataF4_CS0BCR;
	buf[17]=SaveDataF4_CS2BCR;
	buf[18]=SaveDataF4_CS0WCR;
	buf[19]=SaveDataF4_CS2WCR;
	buf[20]=SaveDataF4_CS5aBCR;
	buf[21]=SaveDataF4_CS5aWCR;
	
	buf[22]=(int)SaveDataF5_FRQCR;
	buf[23]=SaveDataF5_CS0BCR;
	buf[24]=SaveDataF5_CS2BCR;
	buf[25]=SaveDataF5_CS0WCR;
	buf[26]=SaveDataF5_CS2WCR;
	buf[27]=SaveDataF5_CS5aBCR;
	buf[28]=SaveDataF5_CS5aWCR;

	sum=0;
	for(i=0;i<dataN-1;i++) sum+=buf[i];
	buf[dataN-1]=sum;

	handle=Bfile_OpenMainMemory((const unsigned char*)"Ftune");
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

	handle=Bfile_OpenMainMemory((const unsigned char*)"Ftune");
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
		Bfile_DeleteMainMemory((const unsigned char*)"Ftune");
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

	
	SaveDataF2_FRQCR =buf[ 1];
	SaveDataF2_CS0BCR=buf[ 2];
	SaveDataF2_CS2BCR=buf[ 3];
	SaveDataF2_CS0WCR=buf[ 4];
	SaveDataF2_CS2WCR=buf[ 5];
	SaveDataF2_CS5aBCR=buf[ 6];
	SaveDataF2_CS5aWCR=buf[ 7];

	SaveDataF3_FRQCR =buf[ 8];
	SaveDataF3_CS0BCR=buf[ 9];
	SaveDataF3_CS2BCR=buf[10];
	SaveDataF3_CS0WCR=buf[11];
	SaveDataF3_CS2WCR=buf[12];
	SaveDataF3_CS5aBCR=buf[13];
	SaveDataF3_CS5aWCR=buf[14];

	SaveDataF4_FRQCR =buf[15];
	SaveDataF4_CS0BCR=buf[16];
	SaveDataF4_CS2BCR=buf[17];
	SaveDataF4_CS0WCR=buf[18];
	SaveDataF4_CS2WCR=buf[19];
	SaveDataF4_CS5aBCR=buf[20];
	SaveDataF4_CS5aWCR=buf[21];
	
	SaveDataF5_FRQCR =buf[22];
	SaveDataF5_CS0BCR=buf[23];
	SaveDataF5_CS2BCR=buf[24];
	SaveDataF5_CS0WCR=buf[25];
	SaveDataF5_CS2WCR=buf[26];
	SaveDataF5_CS5aBCR=buf[27];
	SaveDataF5_CS5aWCR=buf[28];

}


void SaveDataF0(){		// temp save
	SaveDataF0_FRQCR  =CPG.FRQCR.WORD ;
	SaveDataF0_CS0BCR =BSC.CS0BCR.LONG ;
	SaveDataF0_CS2BCR =BSC.CS2BCR.LONG ;
	SaveDataF0_CS0WCR =BSC.CS0WCR.LONG ;
	SaveDataF0_CS2WCR =BSC.CS2WCR.LONG ;
	SaveDataF0_CS5aBCR=BSC.CS5ABCR.LONG ;
	SaveDataF0_CS5aWCR=BSC.CS5AWCR.LONG ;
}
void SaveDataF2(){
	if (YesNo("Store to F2")==0) return;
	SaveDataF2_FRQCR  =CPG.FRQCR.WORD ;
	SaveDataF2_CS0BCR =BSC.CS0BCR.LONG ;
	SaveDataF2_CS2BCR =BSC.CS2BCR.LONG ;
	SaveDataF2_CS0WCR =BSC.CS0WCR.LONG ;
	SaveDataF2_CS2WCR =BSC.CS2WCR.LONG ;
	SaveDataF2_CS5aBCR=BSC.CS5ABCR.LONG ;
	SaveDataF2_CS5aWCR=BSC.CS5AWCR.LONG ;
}
void SaveDataF3(){
	if (YesNo("Store to F3")==0) return;
	SaveDataF3_FRQCR  =CPG.FRQCR.WORD ;
	SaveDataF3_CS0BCR =BSC.CS0BCR.LONG ;
	SaveDataF3_CS2BCR =BSC.CS2BCR.LONG ;
	SaveDataF3_CS0WCR =BSC.CS0WCR.LONG ;
	SaveDataF3_CS2WCR =BSC.CS2WCR.LONG ;
	SaveDataF3_CS5aBCR=BSC.CS5ABCR.LONG ;
	SaveDataF3_CS5aWCR=BSC.CS5AWCR.LONG ;
}
void SaveDataF4(){
	if (YesNo("Store to F4")==0) return;
	SaveDataF4_FRQCR  =CPG.FRQCR.WORD ;
	SaveDataF4_CS0BCR =BSC.CS0BCR.LONG ;
	SaveDataF4_CS2BCR =BSC.CS2BCR.LONG ;
	SaveDataF4_CS0WCR =BSC.CS0WCR.LONG ;
	SaveDataF4_CS2WCR =BSC.CS2WCR.LONG ;
	SaveDataF4_CS5aBCR=BSC.CS5ABCR.LONG ;
	SaveDataF4_CS5aWCR=BSC.CS5AWCR.LONG ;
}
void SaveDataF5(){
	if (YesNo("Store to F5")==0) return;
	SaveDataF5_FRQCR  =CPG.FRQCR.WORD ;
	SaveDataF5_CS0BCR =BSC.CS0BCR.LONG ;
	SaveDataF5_CS2BCR =BSC.CS2BCR.LONG ;
	SaveDataF5_CS0WCR =BSC.CS0WCR.LONG ;
	SaveDataF5_CS2WCR =BSC.CS2WCR.LONG ;
	SaveDataF5_CS5aBCR=BSC.CS5ABCR.LONG ;
	SaveDataF5_CS5aWCR=BSC.CS5AWCR.LONG ;
}


void LoadDataF0(){		// temp load
	change_FRQCRs(      SaveDataF0_FRQCR ) ;
	BSC.CS0BCR.LONG   = SaveDataF0_CS0BCR ;
	BSC.CS2BCR.LONG   = SaveDataF0_CS2BCR ;
	BSC.CS0WCR.LONG   = SaveDataF0_CS0WCR ;
	BSC.CS2WCR.LONG   = SaveDataF0_CS2WCR ;
	BSC.CS5ABCR.LONG  = SaveDataF0_CS5aBCR ;
	BSC.CS5AWCR.LONG  = SaveDataF0_CS5aWCR ;
}
void LoadDataF1(){				// default 
	change_FRQCRs(       FRQCR_default ) ;
	BSC.CS0BCR.LONG   = CS0BCR_default ;
	BSC.CS2BCR.LONG   = CS2BCR_default ;
	BSC.CS0WCR.LONG   = CS0WCR_default ;
	BSC.CS2WCR.LONG   = CS2WCR_default ;
	BSC.CS5ABCR.LONG  = CS5aBCR_default ;
	BSC.CS5AWCR.LONG  = CS5aWCR_default ;
}
void LoadDataF2(){
	change_FRQCRs(      SaveDataF2_FRQCR ) ;
	BSC.CS0BCR.LONG   = SaveDataF2_CS0BCR ;
	BSC.CS2BCR.LONG   = SaveDataF2_CS2BCR ;
	BSC.CS0WCR.LONG   = SaveDataF2_CS0WCR ;
	BSC.CS2WCR.LONG   = SaveDataF2_CS2WCR ;
	BSC.CS5ABCR.LONG  = SaveDataF2_CS5aBCR ;
	BSC.CS5AWCR.LONG  = SaveDataF2_CS5aWCR ;
}
void LoadDataF3(){
	change_FRQCRs(      SaveDataF3_FRQCR ) ;
	BSC.CS0BCR.LONG   = SaveDataF3_CS0BCR ;
	BSC.CS2BCR.LONG   = SaveDataF3_CS2BCR ;
	BSC.CS0WCR.LONG   = SaveDataF3_CS0WCR ;
	BSC.CS2WCR.LONG   = SaveDataF3_CS2WCR ;
	BSC.CS5ABCR.LONG  = SaveDataF3_CS5aBCR ;
	BSC.CS5AWCR.LONG  = SaveDataF3_CS5aWCR ;
}
void LoadDataF4(){
	change_FRQCRs(      SaveDataF4_FRQCR ) ;
	BSC.CS0BCR.LONG   = SaveDataF4_CS0BCR ;
	BSC.CS2BCR.LONG   = SaveDataF4_CS2BCR ;
	BSC.CS0WCR.LONG   = SaveDataF4_CS0WCR ;
	BSC.CS2WCR.LONG   = SaveDataF4_CS2WCR ;
	BSC.CS5ABCR.LONG  = SaveDataF4_CS5aBCR ;
	BSC.CS5AWCR.LONG  = SaveDataF4_CS5aWCR ;
}
void LoadDataF5(){
	change_FRQCRs(      SaveDataF5_FRQCR ) ;
	BSC.CS0BCR.LONG   = SaveDataF5_CS0BCR ;
	BSC.CS2BCR.LONG   = SaveDataF5_CS2BCR ;
	BSC.CS0WCR.LONG   = SaveDataF5_CS0WCR ;
	BSC.CS2WCR.LONG   = SaveDataF5_CS2WCR ;
	BSC.CS5ABCR.LONG  = SaveDataF5_CS5aBCR ;
	BSC.CS5AWCR.LONG  = SaveDataF5_CS5aWCR ;
}

