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
#include "Ptune3_setup.h"

//---------------------------------------------------------------------------------------------

unsigned int	SaveDataF0_FLLFRQ=FLLFRQ_default;		// FLL:900		default
unsigned int	SaveDataF0_FRQCR =FRQCR_default;		// PLL:x16 IFC:1/2 SFC:1/4 BFC:1/4 PFC:1/8
unsigned int	SaveDataF0_CS0BCR=CS0BCR_default;		// IWW:4 IWRRS:4
unsigned int	SaveDataF0_CS2BCR=CS2BCR_default;		// IWW:4 IWRRS:4
unsigned int	SaveDataF0_CS0WCR=CS0WCR_default;		// wait:8
unsigned int	SaveDataF0_CS2WCR=CS2WCR_default;		//
unsigned int	SaveDataF0_CS3BCR=CS3BCR_default;		// IWW:4 IWRRS:4
unsigned int	SaveDataF0_CS3WCR=CS3WCR_default;		// CL:2
unsigned int	SaveDataF0_CS5aBCR=CS5aBCR_default;		//
unsigned int	SaveDataF0_CS5aWCR=CS5aWCR_default;		//

unsigned int	SaveDataF2_FLLFRQ=0x00004000+900;		// FLL:900		 59MHz like CG10/20
unsigned int	SaveDataF2_FRQCR =(PLL_16x<<24)+(DIV_4<<20)+(DIV_8<<12)+(DIV_8<<8)+DIV_8;
unsigned int	SaveDataF2_CS0BCR=0x24920400;			// IWW:2 IWRRS:2
unsigned int	SaveDataF2_CS2BCR=0x24923400;			// IWW:2 IWRRS:2
unsigned int	SaveDataF2_CS0WCR=0x00000340;			// wait:6
unsigned int	SaveDataF2_CS2WCR=CS2WCR_default;		//
unsigned int	SaveDataF2_CS3BCR=0x24924400;			// IWW:2 IWRRS:2
unsigned int	SaveDataF2_CS3WCR=CS3WCR_default;		// CL:2
unsigned int	SaveDataF2_CS5aBCR=CS5aBCR_default;		//
unsigned int	SaveDataF2_CS5aWCR=CS5aWCR_default;		//

unsigned int	SaveDataF3_FLLFRQ=0x00004000+900;		// FLL:900		95.8MHz
unsigned int	SaveDataF3_FRQCR =(PLL_26x<<24)+(DIV_4<<20)+(DIV_8<<12)+(DIV_8<<8)+DIV_8;
unsigned int	SaveDataF3_CS0BCR=0x24920400;			// IWW:2 IWRRS:2
unsigned int	SaveDataF3_CS2BCR=0x24923400;			// IWW:2 IWRRS:2
unsigned int	SaveDataF3_CS0WCR=0x00000240;			// wait:4
unsigned int	SaveDataF3_CS2WCR=CS2WCR_default;		//
unsigned int	SaveDataF3_CS3BCR=0x24924400;			// IWW:2 IWRRS:2
unsigned int	SaveDataF3_CS3WCR=CS3WCR_default;		// CL:2
unsigned int	SaveDataF3_CS5aBCR=CS5aBCR_default;		//
unsigned int	SaveDataF3_CS5aWCR=CS5aWCR_default;		//

unsigned int	SaveDataF4_FLLFRQ=0x00004000+900;		// FLL:900		236MHz
unsigned int	SaveDataF4_FRQCR =(PLL_32x<<24)+(DIV_2<<20)+(DIV_4<<12)+(DIV_8<<8)+DIV16;
unsigned int	SaveDataF4_CS0BCR=0x24920400;			// IWW:2 IWRRS:2
unsigned int	SaveDataF4_CS2BCR=0x24923400;			// IWW:2 IWRRS:2
unsigned int	SaveDataF4_CS0WCR=0x000002C0;			// wait:5
unsigned int	SaveDataF4_CS2WCR=CS2WCR_default;		//
unsigned int	SaveDataF4_CS3BCR=0x24924400;			// IWW:2 IWRRS:2
unsigned int	SaveDataF4_CS3WCR=CS3WCR_default;		// CL:2
unsigned int	SaveDataF4_CS5aBCR=CS5aBCR_default;		//
unsigned int	SaveDataF4_CS5aWCR=CS5aWCR_default;		//

unsigned int	SaveDataF5_FLLFRQ=0x00004000+900;		// FLL:900		191MHz
unsigned int	SaveDataF5_FRQCR =(PLL_26x<<24)+(DIV_2<<20)+(DIV_4<<12)+(DIV_4<<8)+DIV_8;
unsigned int	SaveDataF5_CS0BCR=0x24920400;			// IWW:2 IWRRS:2
unsigned int	SaveDataF5_CS2BCR=0x24923400;			// IWW:2 IWRRS:2
unsigned int	SaveDataF5_CS0WCR=0x00000440;			// wait:10
unsigned int	SaveDataF5_CS2WCR=CS2WCR_default;		//
unsigned int	SaveDataF5_CS3BCR=0x24924400;			// IWW:2 IWRRS:2
unsigned int	SaveDataF5_CS3WCR=CS3WCR_default;		// CL:2
unsigned int	SaveDataF5_CS5aBCR=CS5aBCR_default;		//
unsigned int	SaveDataF5_CS5aWCR=CS5aWCR_default;		//

#define dataN	74

void SaveFile(){
	int state;
	int buf[dataN];
	int key;
	int i,sum;
	
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
	buf[12]=SaveDataF2_CS3BCR;
	buf[13]=SaveDataF2_CS0WCR;
	buf[14]=SaveDataF2_CS2WCR;
	buf[15]=SaveDataF2_CS3WCR;
	buf[16]=SaveDataF2_CS5aBCR;
	buf[17]=SaveDataF2_CS5aWCR;

	buf[18]=SaveDataF3_FLLFRQ;
	buf[19]=SaveDataF3_FRQCR;
	buf[20]=SaveDataF3_CS0BCR;
	buf[21]=SaveDataF3_CS2BCR;
	buf[22]=SaveDataF3_CS3BCR;
	buf[23]=SaveDataF3_CS0WCR;
	buf[24]=SaveDataF3_CS2WCR;
	buf[25]=SaveDataF3_CS3WCR;
	buf[26]=SaveDataF3_CS5aBCR;
	buf[27]=SaveDataF3_CS5aWCR;

	buf[28]=SaveDataF4_FLLFRQ;
	buf[29]=SaveDataF4_FRQCR;
	buf[30]=SaveDataF4_CS0BCR;
	buf[31]=SaveDataF4_CS2BCR;
	buf[32]=SaveDataF4_CS3BCR;
	buf[33]=SaveDataF4_CS0WCR;
	buf[34]=SaveDataF4_CS2WCR;
	buf[35]=SaveDataF4_CS3WCR;
	buf[36]=SaveDataF4_CS5aBCR;
	buf[37]=SaveDataF4_CS5aWCR;

	buf[38]=SaveDataF5_FLLFRQ;
	buf[39]=SaveDataF5_FRQCR;
	buf[40]=SaveDataF5_CS0BCR;
	buf[41]=SaveDataF5_CS2BCR;
	buf[42]=SaveDataF5_CS3BCR;
	buf[43]=SaveDataF5_CS0WCR;
	buf[44]=SaveDataF5_CS2WCR;
	buf[45]=SaveDataF5_CS3WCR;
	buf[46]=SaveDataF5_CS5aBCR;
	buf[47]=SaveDataF5_CS5aWCR;

	for(i=0;i<10;i++) buf[i+48]=MaxFreq_ROMwait[i];	// 48-57
	for(i=0;i< 8;i++) buf[i+58]=MaxFreq_RAMwait[i];	// 58-65
	for(i=1;i< 8;i++) buf[i+65]=MaxFreq_RAMwaitW[i];// 66-72

	sum=0;
	for(i=0;i<dataN-1;i++) sum+=buf[i];
	buf[dataN-1]=sum;

	state=MCSPutVar2((unsigned char*)"@PTUNE", (unsigned char*)"Ptune3", dataN*4, buf);
	switch ( state ) {
		case 0:
			break;
		case 37:									//  file already exists
			MsgBoxPush(5);
		    PrintXY(3,2,"  SAVE DATA      ",TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
		    PrintXY(3,3,"  Already Exists",TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
		    PrintXY(3,4,"  Overwrite OK?",TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
		    PrintXY(3,5,"     Yes:[F1]",TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
		    PrintXY(3,6,"     No :[F6]",TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
			Bdisp_PutDisp_DD();
			GetKey(&key);
			MsgBoxPop();
			if (key !=KEY_CTRL_F1) return;
		
			MCSDelVar2((unsigned char*)"@PTUNE",(unsigned char*)"Ptune3");
			state=MCSPutVar2((unsigned char*)"@PTUNE", (unsigned char*)"Ptune3", dataN*4, buf);
			break;	
		case 64:									//  dir not exists
 			state=MCS_CreateDirectory( (unsigned char*)"@PTUNE" );
			if (state != 0  ) { ErrorMSG("MKDIR error",state); return; }
			state=MCSPutVar2((unsigned char*)"@PTUNE", (unsigned char*)"Ptune3", dataN*4, buf);
			break;
	}
	if (state != 0  ) { ErrorMSG("Save error",state); return; }

	MsgBoxPush(5);
    PrintXY(3,3,"      Complete!   ",TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
    PrintXY(3,5,"     Press:[EXIT] ",TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
	GetKey(&key);
	MsgBoxPop();
 

}
void LoadFile(int check){
	int state,x;
	int key;
	int buf[dataN];
	int i,sum;

	state=MCSGetDlen2((unsigned char*)"@PTUNE", (unsigned char*)"Ptune3",&x);
	if (state!=0) {
		if (check) ErrorMSG("DATA not exists",state);
		return;
	
	}
	
	MCSGetData1(0, x, buf);
	
	sum=0;
	for(i=0;i<dataN-1;i++) sum+=buf[i];

	if ( buf[dataN-1] != sum ) {
		MCSDelVar2((unsigned char*)"@PTUNE",(unsigned char*)"Ptune3");
		StatusArea_sub();
	    PrintXY(2,2,"  SAVE DATA Broken",TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
	    PrintXY(2,4,"  Deleted file",TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
	    PrintXY(2,7,"  Push any Key",TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
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
	SaveDataF2_CS3BCR=buf[12];
	SaveDataF2_CS0WCR=buf[13];
	SaveDataF2_CS2WCR=buf[14];
	SaveDataF2_CS3WCR=buf[15];
	SaveDataF2_CS5aBCR=buf[16];
	SaveDataF2_CS5aWCR=buf[17];

	SaveDataF3_FLLFRQ=buf[18];
	SaveDataF3_FRQCR =buf[19];
	SaveDataF3_CS0BCR=buf[20];
	SaveDataF3_CS2BCR=buf[21];
	SaveDataF3_CS3BCR=buf[22];
	SaveDataF3_CS0WCR=buf[23];
	SaveDataF3_CS2WCR=buf[24];
	SaveDataF3_CS3WCR=buf[25];
	SaveDataF3_CS5aBCR=buf[26];
	SaveDataF3_CS5aWCR=buf[27];

	SaveDataF4_FLLFRQ=buf[28];
	SaveDataF4_FRQCR =buf[29];
	SaveDataF4_CS0BCR=buf[30];
	SaveDataF4_CS2BCR=buf[31];
	SaveDataF4_CS3BCR=buf[32];
	SaveDataF4_CS0WCR=buf[33];
	SaveDataF4_CS2WCR=buf[34];
	SaveDataF4_CS3WCR=buf[35];
	SaveDataF4_CS5aBCR=buf[36];
	SaveDataF4_CS5aWCR=buf[37];
	
	SaveDataF5_FLLFRQ=buf[38];
	SaveDataF5_FRQCR =buf[39];
	SaveDataF5_CS0BCR=buf[40];
	SaveDataF5_CS2BCR=buf[41];
	SaveDataF5_CS3BCR=buf[42];
	SaveDataF5_CS0WCR=buf[43];
	SaveDataF5_CS2WCR=buf[44];
	SaveDataF5_CS3WCR=buf[45];
	SaveDataF5_CS5aBCR=buf[46];
	SaveDataF5_CS5aWCR=buf[47];

	for(i=0;i<10;i++) MaxFreq_ROMwait[i]=buf[i+48];	// 48-57
	for(i=0;i< 8;i++) MaxFreq_RAMwait[i]=buf[i+58];	// 58-65
	for(i=1;i< 8;i++) MaxFreq_RAMwaitW[i]=buf[i+65];// 66-72

}


void SaveDataF0(){		// temp save
	SaveDataF0_FLLFRQ =CPG.FLLFRQ.LONG ;
	SaveDataF0_FRQCR  =CPG.FRQCRA.LONG ;
	SaveDataF0_CS0BCR =BSC.CS0BCR.LONG ;
	SaveDataF0_CS0WCR =BSC.CS0WCR.LONG ;
	SaveDataF0_CS2BCR =BSC.CS2BCR.LONG ;
	SaveDataF0_CS2WCR =BSC.CS2WCR.LONG ;
	SaveDataF0_CS3BCR =BSC.CS3BCR.LONG ;
	SaveDataF0_CS3WCR =BSC.CS3WCR.LONG ;
	SaveDataF0_CS5aBCR=BSC.CS5ABCR.LONG ;
	SaveDataF0_CS5aWCR=BSC.CS5AWCR.LONG ;
}
void SaveDataF2(){
	if (YesNo("Store to F2")==0) return;
	SaveDataF2_FLLFRQ =CPG.FLLFRQ.LONG ;
	SaveDataF2_FRQCR  =CPG.FRQCRA.LONG ;
	SaveDataF2_CS0BCR =BSC.CS0BCR.LONG ;
	SaveDataF2_CS0WCR =BSC.CS0WCR.LONG ;
	SaveDataF2_CS2BCR =BSC.CS2BCR.LONG ;
	SaveDataF2_CS2WCR =BSC.CS2WCR.LONG ;
	SaveDataF2_CS3BCR =BSC.CS3BCR.LONG ;
	SaveDataF2_CS3WCR =BSC.CS3WCR.LONG ;
	SaveDataF2_CS5aBCR=BSC.CS5ABCR.LONG  ;
	SaveDataF2_CS5aWCR=BSC.CS5AWCR.LONG;
}
void SaveDataF3(){
	if (YesNo("Store to F3")==0) return;
	SaveDataF3_FLLFRQ =CPG.FLLFRQ.LONG ;
	SaveDataF3_FRQCR  =CPG.FRQCRA.LONG ;
	SaveDataF3_CS0BCR =BSC.CS0BCR.LONG ;
	SaveDataF3_CS0WCR =BSC.CS0WCR.LONG ;
	SaveDataF3_CS2BCR =BSC.CS2BCR.LONG ;
	SaveDataF3_CS2WCR =BSC.CS2WCR.LONG ;
	SaveDataF3_CS3BCR =BSC.CS3BCR.LONG ;
	SaveDataF3_CS3WCR =BSC.CS3WCR.LONG ;
	SaveDataF3_CS5aBCR=BSC.CS5ABCR.LONG ;
	SaveDataF3_CS5aWCR=BSC.CS5AWCR.LONG ;
}
void SaveDataF4(){
	if (YesNo("Store to F4")==0) return;
	SaveDataF4_FLLFRQ =CPG.FLLFRQ.LONG ;
	SaveDataF4_FRQCR  =CPG.FRQCRA.LONG ;
	SaveDataF4_CS0BCR =BSC.CS0BCR.LONG ;
	SaveDataF4_CS0WCR =BSC.CS0WCR.LONG ;
	SaveDataF4_CS2BCR =BSC.CS2BCR.LONG ;
	SaveDataF4_CS2WCR =BSC.CS2WCR.LONG ;
	SaveDataF4_CS3BCR =BSC.CS3BCR.LONG ;
	SaveDataF4_CS3WCR =BSC.CS3WCR.LONG ;
	SaveDataF4_CS5aBCR=BSC.CS5ABCR.LONG ;
	SaveDataF4_CS5aWCR=BSC.CS5AWCR.LONG ;
}
void SaveDataF5(){
	if (YesNo("Store to F5")==0) return;
	SaveDataF5_FLLFRQ =CPG.FLLFRQ.LONG ;
	SaveDataF5_FRQCR  =CPG.FRQCRA.LONG ;
	SaveDataF5_CS0BCR =BSC.CS0BCR.LONG ;
	SaveDataF5_CS0WCR =BSC.CS0WCR.LONG ;
	SaveDataF5_CS2BCR =BSC.CS2BCR.LONG ;
	SaveDataF5_CS2WCR =BSC.CS2WCR.LONG ;
	SaveDataF5_CS3BCR =BSC.CS3BCR.LONG ;
	SaveDataF5_CS3WCR =BSC.CS3WCR.LONG ;
	SaveDataF5_CS5aBCR=BSC.CS5ABCR.LONG ;
	SaveDataF5_CS5aWCR=BSC.CS5AWCR.LONG ;
}


void LoadDataF0(){		// temp load
	BSC.CS0WCR.BIT.WR = WAIT18 ;
//	BSC.CS2WCR.BIT.WR = WAIT18 ;
	CPG.FLLFRQ.LONG   = SaveDataF0_FLLFRQ ;
	change_FRQCRs(      SaveDataF0_FRQCR ) ;
	BSC.CS0BCR.LONG   = SaveDataF0_CS0BCR ;
	BSC.CS0WCR.LONG   = SaveDataF0_CS0WCR ;
	BSC.CS2BCR.LONG   = SaveDataF0_CS2BCR ;
	BSC.CS2WCR.LONG   = SaveDataF0_CS2WCR ;
	BSC.CS3BCR.LONG   = SaveDataF0_CS3BCR ;
	BSC.CS3WCR.LONG   = SaveDataF0_CS3WCR ;
	if ( BSC.CS3WCR.BIT.A3CL == 1 ) SDMR3_CL2 = 0; else SDMR3_CL3 = 0;
	BSC.CS5ABCR.LONG  = SaveDataF0_CS5aBCR ;
	BSC.CS5AWCR.LONG  = SaveDataF0_CS5aWCR ;
}
void LoadDataF1(){				// default 
	BSC.CS0WCR.BIT.WR = WAIT18 ;
//	BSC.CS2WCR.BIT.WR = WAIT18 ;
	CPG.FLLFRQ.LONG   = FLLFRQ_default ;
	change_FRQCRs(       FRQCR_default ) ;
	BSC.CS0BCR.LONG   = CS0BCR_default ;
	BSC.CS0WCR.LONG   = CS0WCR_default ;
	BSC.CS2BCR.LONG   = CS2BCR_default ;
	BSC.CS2WCR.LONG   = CS2WCR_default ;
	BSC.CS3BCR.LONG   = CS3BCR_default ;
	BSC.CS3WCR.LONG   = CS3WCR_default ;
	if ( BSC.CS3WCR.BIT.A3CL == 1 ) SDMR3_CL2 = 0; else SDMR3_CL3 = 0;
	BSC.CS5ABCR.LONG  = CS5aBCR_default ;
	BSC.CS5AWCR.LONG  = CS5aWCR_default ;
}
void LoadDataF2(){
	BSC.CS0WCR.BIT.WR = WAIT18 ;
//	BSC.CS2WCR.BIT.WR = WAIT18 ;
	CPG.FLLFRQ.LONG   = SaveDataF2_FLLFRQ ;
	change_FRQCRs(      SaveDataF2_FRQCR ) ;
	BSC.CS0BCR.LONG   = SaveDataF2_CS0BCR ;
	BSC.CS0WCR.LONG   = SaveDataF2_CS0WCR ;
	BSC.CS2BCR.LONG   = SaveDataF2_CS2BCR ;
	BSC.CS2WCR.LONG   = SaveDataF2_CS2WCR ;
	BSC.CS3BCR.LONG   = SaveDataF2_CS3BCR ;
	BSC.CS3WCR.LONG   = SaveDataF2_CS3WCR ;
	if ( BSC.CS3WCR.BIT.A3CL == 1 ) SDMR3_CL2 = 0; else SDMR3_CL3 = 0;
	BSC.CS5ABCR.LONG  = SaveDataF2_CS5aBCR ;
	BSC.CS5AWCR.LONG  = SaveDataF2_CS5aWCR ;
}
void LoadDataF3(){
	BSC.CS0WCR.BIT.WR = WAIT18 ;
//	BSC.CS2WCR.BIT.WR = WAIT18 ;
	CPG.FLLFRQ.LONG   = SaveDataF3_FLLFRQ ;
	change_FRQCRs(      SaveDataF3_FRQCR ) ;
	BSC.CS0BCR.LONG   = SaveDataF3_CS0BCR ;
	BSC.CS0WCR.LONG   = SaveDataF3_CS0WCR ;
	BSC.CS2BCR.LONG   = SaveDataF3_CS2BCR ;
	BSC.CS2WCR.LONG   = SaveDataF3_CS2WCR ;
	BSC.CS3BCR.LONG   = SaveDataF3_CS3BCR ;
	BSC.CS3WCR.LONG   = SaveDataF3_CS3WCR ;
	if ( BSC.CS3WCR.BIT.A3CL == 1 ) SDMR3_CL2 = 0; else SDMR3_CL3 = 0;
	BSC.CS5ABCR.LONG  = SaveDataF3_CS5aBCR ;
	BSC.CS5AWCR.LONG  = SaveDataF3_CS5aWCR ;
}
void LoadDataF4(){
	BSC.CS0WCR.BIT.WR = WAIT18 ;
//	BSC.CS2WCR.BIT.WR = WAIT18 ;
	CPG.FLLFRQ.LONG   = SaveDataF4_FLLFRQ ;
	change_FRQCRs(      SaveDataF4_FRQCR ) ;
	BSC.CS0BCR.LONG   = SaveDataF4_CS0BCR ;
	BSC.CS2BCR.LONG   = SaveDataF4_CS2BCR ;
	BSC.CS0WCR.LONG   = SaveDataF4_CS0WCR ;
	BSC.CS2WCR.LONG   = SaveDataF4_CS2WCR ;
	BSC.CS3BCR.LONG   = SaveDataF4_CS3BCR ;
	BSC.CS3WCR.LONG   = SaveDataF4_CS3WCR ;
	if ( BSC.CS3WCR.BIT.A3CL == 1 ) SDMR3_CL2 = 0; else SDMR3_CL3 = 0;
	BSC.CS5ABCR.LONG  = SaveDataF4_CS5aBCR ;
	BSC.CS5AWCR.LONG  = SaveDataF4_CS5aWCR ;
}
void LoadDataF5(){
	BSC.CS0WCR.BIT.WR = WAIT18 ;
//	BSC.CS2WCR.BIT.WR = WAIT18 ;
	CPG.FLLFRQ.LONG   = SaveDataF5_FLLFRQ ;
	change_FRQCRs(      SaveDataF5_FRQCR ) ;
	BSC.CS0BCR.LONG   = SaveDataF5_CS0BCR ;
	BSC.CS0WCR.LONG   = SaveDataF5_CS0WCR ;
	BSC.CS2BCR.LONG   = SaveDataF5_CS2BCR ;
	BSC.CS2WCR.LONG   = SaveDataF5_CS2WCR ;
	BSC.CS3BCR.LONG   = SaveDataF5_CS3BCR ;
	BSC.CS3WCR.LONG   = SaveDataF5_CS3WCR ;
	if ( BSC.CS3WCR.BIT.A3CL == 1 ) SDMR3_CL2 = 0; else SDMR3_CL3 = 0;	
	BSC.CS5ABCR.LONG  = SaveDataF5_CS5aBCR ;
	BSC.CS5AWCR.LONG  = SaveDataF5_CS5aWCR ;
}

