/*
===============================================================================

 Ftune/Ftune2 common file  v1.02

 copyright(c)2014,2015 by sentaro21
 e-mail sentaro21@pm.matrix.jp

===============================================================================
*/
#include "fxlib.h"
#include "stdio.h"
#include "string.h"

#include "common.h"

char points[]={0x13,0x00};				// =>
char IsEmu=0;

//---------------------------------------------------------------------------------------------
int CPU_check(void) {					// SH3:1 SH4A:0   2:Slim
	int slim = 0;
	if ( *(unsigned int*)0x80000300 == 0x80005D7C ){
		if ( ( *(unsigned char*)0xA4000128 & 0x08 ) == 0 ) {
			slim = 1;;
		}
	}
	return ! ( ( *(unsigned short*)0xFFFFFF80 == 0 ) && ( *(unsigned short*)0xFFFFFF84 == 0 ) ) + slim;
}

int Emu_check() {
//	int i,t,s=RTC_GetTicks();
//	for(i=0;i<140;i++){
//		Bdisp_PutDisp_DD();
//	}
//	t=RTC_GetTicks()-s;
//	if ( t<6) IsEmu=1;	
	if ( *(int*)0x8000FFD0 == 0 ) IsEmu=1;	// is emulator
}

//---------------------------------------------------------------------------------------------
void PrintXYR(int x,int y,char *buf,int rev){
 	if ( rev ) PrintXY(x,y,(unsigned char *)buf,1);
		else   PrintXY(x,y,(unsigned char *)buf,0);
}

void Fkey_Clear(int n) {
	int y;
	for (y=0; y<8; y++){
		Bdisp_ClearLineVRAM(n*21+2,7*8+y,n*21+20,7*8+y);
	}
}
void Fkey_dispN(int n,char *buf) {
	Fkey_Clear(n);
	PrintMini(n*21+4,7*8+2,(unsigned char *)buf, MINI_OVER);
	Bdisp_DrawLineVRAM(n*21+2,7*8+0,n*21+20,7*8+0);
	Bdisp_DrawLineVRAM(n*21+2,7*8+0,n*21+2,7*8+7);
	Bdisp_ClearLineVRAM(n*21+3,7*8+1,n*21+3,7*8+7);
	Bdisp_ClearLineVRAM(n*21+20,7*8+1,n*21+20,7*8+7);
}
void Fkey_DISPN(int n,char *buf) {
	Fkey_Clear(n);
	PrintXY(n*21+2,7*8+1,(unsigned char *)buf,0);
	Bdisp_DrawLineVRAM(n*21+2,7*8+0,n*21+20,7*8+0);
	Bdisp_DrawLineVRAM(n*21+2,7*8+0,n*21+2,7*8+7);
	Bdisp_ClearLineVRAM(n*21+3,7*8+1,n*21+3,7*8+7);
	Bdisp_ClearLineVRAM(n*21+20,7*8+1,n*21+20,7*8+7);
}
void Fkey_dispR(int n,char *buf) {
	Fkey_Clear(n);
	Bdisp_DrawLineVRAM(n*21+2,7*8+0,n*21+20,7*8+0);
	Bdisp_DrawLineVRAM(n*21+2,7*8+1,n*21+20,7*8+1);
	Bdisp_DrawLineVRAM(n*21+2,7*8+2,n*21+20,7*8+2);
	Bdisp_DrawLineVRAM(n*21+2,7*8+3,n*21+20,7*8+3);
	Bdisp_DrawLineVRAM(n*21+2,7*8+4,n*21+20,7*8+4);
	Bdisp_DrawLineVRAM(n*21+2,7*8+5,n*21+19,7*8+5);
	Bdisp_DrawLineVRAM(n*21+2,7*8+6,n*21+18,7*8+6);
	Bdisp_DrawLineVRAM(n*21+2,7*8+7,n*21+17,7*8+7);
	Bdisp_DrawLineVRAM(n*21+2,7*8+0,n*21+ 2,7*8+7);
	PrintMini(n*21+3,7*8+1     ,(unsigned char *)buf, MINI_REV);
	Bdisp_ClearLineVRAM(n*21+21,7*8+0,n*21+21,7*8+7);
	Bdisp_ClearLineVRAM(n*21+22,7*8+0,n*21+22,7*8+7);
	Bdisp_ClearLineVRAM(n*21+20,7*8+5,n*21+20,7*8+5);
	Bdisp_ClearLineVRAM(n*21+19,7*8+6,n*21+20,7*8+6);
	Bdisp_ClearLineVRAM(n*21+18,7*8+7,n*21+20,7*8+7);
}

void Fkey_dispNP(int n) {
	Fkey_DISPN(n," +");
}
void Fkey_dispNM(int n) {
	Fkey_DISPN(n," -");
}
void Fkey_dispNInit(int n) {
	Fkey_dispN(n,"Init");
}

void func_menu1(void){
		Fkey_dispN(0,"Nrm ");
		Fkey_dispN(1," F2 ");
		Fkey_dispN(2," F3 ");
		Fkey_dispN(3," F4 ");
		Fkey_dispN(4," F5 ");
		Fkey_dispR(5,"benc");

}
void func_menu2(void){
	char E693[]={0x20,0xe6,0x93,0x20,0x00};
		Fkey_dispR(0,"Save");
		Fkey_dispN(1,E693);
		Fkey_dispN(2,E693);
		Fkey_dispN(3,E693);
		Fkey_dispN(4,E693);
		Fkey_dispR(5,"Load");
}
void func_menu3(void){
		Fkey_dispN(0,"FRQ ");
		Fkey_dispN(1,"BCR ");
		Fkey_dispR(2,"Bc02");
		Fkey_dispR(3,"Wc02");
		Fkey_dispR(4,"Bc5 ");
		Fkey_dispR(5,"Wc5 ");
}
#ifdef OPTION_REG
void func_menu4(void){
		Fkey_dispN(0,"B34 ");
		Fkey_dispN(1,"W34 ");
		Fkey_dispR(2,"Bc5 ");
		Fkey_dispR(3,"Wc5 ");
		Fkey_dispN(4,"Bc6 ");
		Fkey_dispN(5,"Wc6 ");
}
#endif


void HexPrintXY(int x, int y, char *str, unsigned int hex){
	char buffer[22];
	sprintf(buffer,"%08X",hex);
	locate(x,y); Print((unsigned char *)str); Print((unsigned char *)buffer);
}

int YesNo( char*buffer){
	unsigned int key;

	SaveDisp(SAVEDISP_PAGE1);
	PopUpWin(5);
	locate(3,2); Print((unsigned char *)buffer);
	locate(3,4); Print((unsigned char *) "   Yes:[F1]");
	locate(3,5); Print((unsigned char *) "   No :[F6]");
	Bdisp_PutDisp_DD();

	GetKey(&key);	
	RestoreDisp(SAVEDISP_PAGE1);
	Bdisp_PutDisp_DD();

	if ( key == KEY_CTRL_F1 ) return 1;
	return 0;
}
void ErrorMSG(char*buffer,int err){
	unsigned int key;
	char buf[20];

	SaveDisp(SAVEDISP_PAGE1);
	PopUpWin(5);
	sprintf(buf,"code:%d",err);
	locate(3,2); Print((unsigned char *)buffer);
	locate(3,4); Print((unsigned char *)buf);
	locate(3,6); Print((unsigned char *) "   Press:[EXIT]");
	Bdisp_PutDisp_DD();

	GetKey(&key);	
	RestoreDisp(SAVEDISP_PAGE1);
	Bdisp_PutDisp_DD();
}
void ErrorADRS(char*buffer,int err){
	unsigned int key;
	char buf[20];

	SaveDisp(SAVEDISP_PAGE1);
	PopUpWin(5);
	locate(3,2); Print((unsigned char *)buffer);
	HexPrintXY(3,4,"addres:",err);
	locate(3,6); Print((unsigned char *) "   Press:[EXIT]");
	Bdisp_PutDisp_DD();

	GetKey(&key);	
	RestoreDisp(SAVEDISP_PAGE1);
	Bdisp_PutDisp_DD();
}

void MSG1(char*buffer1){
	SaveDisp(SAVEDISP_PAGE1);
	PopUpWin(1);
	locate(3,4); Print((unsigned char *)buffer1);
	Bdisp_PutDisp_DD();
}
void MSG2(char*buffer1,char*buffer2){
	SaveDisp(SAVEDISP_PAGE1);
	PopUpWin(2);
	locate(3,3); Print((unsigned char *)buffer1);
	locate(3,4); Print((unsigned char *)buffer2);
	Bdisp_PutDisp_DD();
}
void MSGpop(void){
	RestoreDisp(SAVEDISP_PAGE1);
	Bdisp_PutDisp_DD();
}

