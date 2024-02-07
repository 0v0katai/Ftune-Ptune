/*
===============================================================================

 Ptune3 is SH7305 CPG&BSC tuning utility for PRIZM fx-CG50/Graph 90+E  v0.24

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
#include "Ptune3_common.h"
#include "Ptune3_setup.h"

//---------------------------------------------------------------------------------------------
int LocalizeMessage1( int msgno, char*result  ){	//
	__asm__(
	    "mov.l syscall_adress2, r2\n"
	    "mov.l localizeMessage1, r0\n"
	    "jmp @r2\n"
	    "nop\n"
	    "syscall_adress2: .long 0x80020070\n"
	    "localizeMessage1: .long 0x12FC");
}
//---------------------------------------------------------------------------------------------
unsigned char *cls="                      ";
char pointss[]={0x20,0x20,0x13,0x00};	// =>
char points[]={0x13,0x00};				// =>

void Print( char*msg ){
	Print_OS( (unsigned char*)msg, 0, 0 );
}

void PrintMiniXY(int X, int Y, char*buf, int COLOR,int BackCOLOR) {
	PrintMini(&X, &Y, (unsigned char*)buf, 0, 0xFFFFFFFF, 0, 0, COLOR, BackCOLOR, 1, 0);
}
	
int sprintf62s(  char*buf,int freq, char*mes){
	char tmp[10];
	int	k=0;
	if (freq/100 < 100)  k=1;
	if (freq/100 <  10)  k=2;
	itoa(freq/100,(unsigned char*)buf);
	strcat(buf,".");
	freq=freq-freq/100*100;
	if (freq< 10)  strcat(buf,"0");
	itoa(freq,(unsigned char*)tmp);
	strcat(buf,tmp);
	strcat(buf,mes);
	return k;
}
int sprintf62(  char*buf,int freq, char*mes){
	if ( ( (SetupData>>31)&1 ) && SSCGCR ) freq = freq *900/914;
	return sprintf62s(  buf,freq, mes);
}

void IntToHex( unsigned int value, unsigned char*buffer ){
	WordToHex( value >> 16, buffer );
	WordToHex( value & 0xFFFF, buffer+4 );
	buffer[ 8 ] = 0;
}
void HexPrintXY(int x, int y, unsigned char *str, unsigned int hex){
	char buffer[22];
	IntToHex(hex,buffer);
	locate_OS(x,y);Print(str);	Print( buffer );
}

//
void SetStatusMessage( char*msg, char short_color ){
	DefineStatusMessage( msg, 1, short_color, 7 );
	DisplayStatusArea();
}
//
void StatusArea( char*msg, char short_color ){
	Bdisp_AllClr_VRAM();
	SetStatusMessage( msg, short_color );
}

void StatusArea_sub(){
    EnableStatusArea(0);
    DefineStatusAreaFlags(DSA_SETDEFAULT, 0, 0, 0);
    DefineStatusAreaFlags(3, SAF_BATTERY | SAF_TEXT | SAF_GLYPH | SAF_ALPHA_SHIFT, 0, 0);
	StatusArea( "Ptune3 (PRIZM TuneUp Util) "VERSION"",TEXT_COLOR_RED);
}

//---------------------------------------------------------------------------------------------
int GetFkeyColor(int n){
//	unsigned short keycolor;
	return Bdisp_GetPoint_VRAM( n*64+4, 213 );
/*	switch (*keycolor) {
		case COLOR_BLACK: //TEXT_COLOR_BLACK
			return COLOR_BLACK;
			break;
		case COLOR_BLUE: //TEXT_COLOR_BLUE
			return COLOR_BLUE;
			break;
		case COLOR_GREEN: //TEXT_COLOR_GREEN
			return COLOR_GREEN;
			break;
		case COLOR_CYAN: //TEXT_COLOR_CYAN
			return COLOR_CYAN;
			break;
		case COLOR_RED: //TEXT_COLOR_RED
			return COLOR_RED;
			break;
		case COLOR_PURPLE: //TEXT_COLOR_PURPLE
			return COLOR_PURPLE;
			break;
		case COLOR_YELLOW: //TEXT_COLOR_YELLOW
			return COLOR_YELLOW;
			break;
		case COLOR_WHITE: // TEXT_COLOR_WHITE
			return COLOR_WHITE;
			break;
	}
*/
}

void Fkey_Clear(int n) {
		unsigned int* iresult;	
		GetFKeyPtr(0x0, &iresult);	// 
		FKey_Display(n, iresult);
}

void Fkey_dispN(int n,char *buf) {
		unsigned int* iresult;	
		int x;
		GetFKeyPtr(0x0476, &iresult);	//  .  		,white
		FKey_Display(n, iresult);
		PrintMiniXY(n*64+6, 7*24+5, (unsigned char *)buf, GetFkeyColor(n), COLOR_WHITE);
		for(x=n*64+4;x<n*64+60;x++){
			Bdisp_SetPoint_VRAM(x, 213, GetFkeyColor(n) );
			Bdisp_SetPoint_VRAM(x, 214, GetFkeyColor(n) );
		}
}

void Fkey_dispR(int n,char *buf) {
		unsigned char*keycolor = (unsigned char*) 0xFD8013E4;
		unsigned int* iresult;	
		GetFKeyPtr(0x0334, &iresult);	//  t		,black/
		FKey_Display(n, iresult);
		PrintMiniXY(n*64+2, 7*24+4, (unsigned char *)buf, COLOR_WHITE, GetFkeyColor(n) );
}

void Fkey_dispNP(int n) {
		Fkey_Clear(n);
		Fkey_dispI(n,fkey01);	//Fkey_dispN(n,"+ inc");
}
void Fkey_dispNM(int n) {
		Fkey_Clear(n);
		Fkey_dispI(n,fkey02);	//Fkey_dispN(n,"- dec");
}
void Fkey_dispNInit(int n) {
		unsigned int* iresult;
		GetFKeyPtr(0x005f, &iresult);	//INITIAL 	,white
		FKey_Display(3, iresult);
}		

void func_menu1(void){
		unsigned int* iresult;
		GetFKeyPtr(0x045c, &iresult);	//NORMAL	,white
		FKey_Display(0, iresult);
	
		Fkey_dispI(1,fkey03);	//Fkey_dispN(1,"  F2 ");
		Fkey_dispI(2,fkey04);	//Fkey_dispN(2,"  F3 ");
		Fkey_dispI(3,fkey05);	//Fkey_dispN(3,"  F4 ");
		Fkey_dispI(4,fkey06);	//Fkey_dispN(4,"  F5 ");
		if ( Is_Chinese() ) Fkey_dispI(5,fkey07c);	//Fkey_dispR(5,"Bench");
		else				Fkey_dispI(5,fkey07);	//Fkey_dispR(5,"Bench");
}
void func_menu2(void){
		unsigned int* iresult;
		GetFKeyPtr(0x0302, &iresult);	// SAVE 		,black/
		FKey_Display(0, iresult);
		
		GetFKeyPtr(0x03d0, &iresult);	// down arrow		,white
		FKey_Display(1, iresult);
		FKey_Display(2, iresult);
		FKey_Display(3, iresult);
		FKey_Display(4, iresult);

		GetFKeyPtr(0x03b7, &iresult);	// LOAD 		black/
		FKey_Display(5, iresult);
}
void func_menu3(void){
		Fkey_dispI(0,fkey09);	//Fkey_dispN(0," FRQ ");
		Fkey_dispI(1,fkey10);	//Fkey_dispN(1," BCR ");
		Fkey_dispI(2,fkey11);	//Fkey_dispR(2,"BC02");
		Fkey_dispI(3,fkey12);	//Fkey_dispR(3,"WC02");
		Fkey_dispI(4,fkey13);	//Fkey_dispR(4,"BCR5");
		Fkey_dispI(5,fkey14);	//Fkey_dispR(5,"WCR5");
}

#ifdef OPTION_REG
void func_menu4(void){
		Fkey_dispI(0,fkey15);	//Fkey_dispN(0,"BC34");
		Fkey_dispI(1,fkey16);	//Fkey_dispN(1,"WC34");
		Fkey_dispI(2,fkey13);	//Fkey_dispR(2,"BCR5");
		Fkey_dispI(3,fkey14);	//Fkey_dispR(3,"WCR5");
		Fkey_dispI(4,fkey17);	//Fkey_dispN(4,"BCR6");
		Fkey_dispI(5,fkey18);	//Fkey_dispN(5,"WCR6");
}
#endif

int YesNo( char*buffer){
	int key;
	char buf1[20]="  ";
		MsgBoxPush(5);
		strcat(buf1,buffer);
    	PrintXY(3,2,buf1,TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
    	PrintXY(3,4,"     Yes:[F1]",TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
    	PrintXY(3,5,"     No :[F6]",TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
		Bdisp_PutDisp_DD();
		GetKey(&key);	
		MsgBoxPop();
		Bdisp_PutDisp_DD();
		if ( key == KEY_CTRL_F1 ) return 1;
		return 0;
}

void ErrorMSG(char*buffer,int err){
	int key;
	char buf1[20]="  ";
	char buf2[20]="  code: ";

		MsgBoxPush(5);
		strcat(buf1,buffer);
    	PrintXY(3,2,buf1,TEXT_MODE_NORMAL, TEXT_COLOR_RED);
		itoa(err,buf2+7);
    	PrintXY(3,4,buf2,TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
    	PrintXY(3,6,"     Press:[EXIT]",TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
		Bdisp_PutDisp_DD();
		GetKey(&key);	
		MsgBoxPop();
		Bdisp_PutDisp_DD();
}
void ErrorADRS(char*buffer,int err){
	int key;
	char buf1[20]="  ";

		MsgBoxPush(5);
		strcat(buf1,buffer);
    	PrintXY(3,2,buf1,TEXT_MODE_NORMAL, TEXT_COLOR_RED);
		HexPrintXY(3,4,"addres:",err);
    	PrintXY(3,6,"     Press:[EXIT]",TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
		Bdisp_PutDisp_DD();
		GetKey(&key);	
		MsgBoxPop();
		Bdisp_PutDisp_DD();
}
void MSG1(char*buffer1){
		char buf1[20]="  ";
		strcat(buf1,buffer1);
		MsgBoxPush(1);
    	PrintXY(3,4,buf1,TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
		Bdisp_PutDisp_DD();
}
void MSG2(char*buffer1,char*buffer2){
		char buf1[20]="  ";
		char buf2[20]="  ";
		strcat(buf1,buffer1);
		strcat(buf2,buffer2);
		MsgBoxPush(2);
    	PrintXY(3,3,buf1,TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
    	PrintXY(3,4,buf2,TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
		Bdisp_PutDisp_DD();
}
void MSGpop(void){
		MsgBoxPop();
		Bdisp_PutDisp_DD();
}
