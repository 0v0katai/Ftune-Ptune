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
#include "Ptune3_common.h"
#include "Ptune3_benchmark.h"
#include "Ptune3_setup.h"

//---------------------------------------------------------------------------------------------

unsigned int TimerID,TMU5_COUNT;

int cont_flag = 1;

void Timer_Routine(void) {
	cont_flag = 0;
}
unsigned int gettimes(void){ 	
	return(  *(unsigned int*)P7305_EXTRA_TMU5_COUNT );
}
void start_timer(void){
	TMU5_COUNT = gettimes();
}
unsigned int get_timer(void){
	unsigned int cnt;
	cnt = TMU5_COUNT - gettimes();
	return( cnt*1000/32768 );
}

int tcount(int *cont_flag) {
//    while(cont_flag) {
//        count++;
//    }
   __asm__( 
      ".align 4\n\t"
      " nop\n\t"
      " nop\n\t"
      " nop\n\t"
      " mov #-1,R0\n\t"
      "TL00: add #1,R0\n"
      " mov.l @R4,R3\n\t"
      " tst R3,R3\n\t"
      " bf TL00\n\t"
      " rts\n\t"		// return value R0
      " nop\n\t"
   ); 
}

int memread(unsigned int *address,int cont_flag){	
   __asm__( 
      ".align 4\n\t"
      " mov #-1,R0\n\t"
      "ML00: add #1,R0\n"
      " mov #8,R7\n\t"
      " mov.l @R4,R1\n\t"
      " mov.l @R4,R1\n\t"
      " mov.l @R4,R1\n\t"
      "ML02: mov.l @R4,R1\n\t"
      " mov.l @R4,R1\n\t"
      " mov.l @R4,R1\n\t"
      " mov.l @R4,R1\n\t"
      " mov.l @R4,R1\n\t"
      " mov.l @R4,R1\n\t"
      " mov.l @R4,R1\n\t"
      " mov.l @R4,R1\n\t"
      " mov.l @R4,R1\n\t"
      " mov.l @R4,R1\n\t"
      
      " mov.l @R4,R1\n\t"
      " mov.l @R4,R1\n\t"
      " mov.l @R4,R1\n\t"
      " mov.l @R4,R1\n\t"
      " mov.l @R4,R1\n\t"
      " mov.l @R4,R1\n\t"
      " mov.l @R4,R1\n\t"
      " mov.l @R4,R1\n\t"
      " mov.l @R4,R1\n\t"
      " mov.l @R4,R1\n\t"
      
      " mov.l @R4,R1\n\t"
      " mov.l @R4,R1\n\t"
            
      " dt R7\n\t"
      " bf ML02\n\t"
      
      " mov.l @R5,R3\n\t"
      " tst R3,R3\n\t"
      " bf ML00\n\t"
      " rts\n\t"
      " nop\n\t"
   ); 
}

void Bench(int duration_ms, int disp) {
    char buffer[100];
	int count;

    cont_flag = 1;
    TimerID=Timer_Install(0, &Timer_Routine, duration_ms);
	if ( TimerID >= 5 ) Timer_Start( TimerID );
//	TMU5_COUNT = *(unsigned int*)P7305_EXTRA_TMU5_COUNT;

	count=tcount(&cont_flag);

//	TMU5_COUNT = TMU5_COUNT - *(unsigned int*)P7305_EXTRA_TMU5_COUNT ;
	if ( TimerID >= 5 ){
		Timer_Stop( TimerID );
		Timer_Deinstall( TimerID );
	}	

    if (disp) {
		PrintXY(  1,7, cls, TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
		itoa(count/100,(unsigned char*)buffer);
		strcat(buffer,"                                           ");
	    PrintMiniXY(0*18   , 6*24+2, " Cpu  ",COLOR_WHITE,COLOR_BLACK);
	    PrintMiniXY(0*18+48, 6*24+2, buffer  ,COLOR_WHITE,COLOR_BLACK);
    }
}

void BenchDD(int duration_ms) {
    char buffer[21]="  PutDisp ";
    char buffer2[21];
	int TMU5_COUNT;

//	cont_flag = 1;
//	TimerID=Timer_Install(0, &Timer_Routine, duration_ms);
//	if ( TimerID >= 5 ) Timer_Start( TimerID );
	TMU5_COUNT = *(unsigned int*)P7305_EXTRA_TMU5_COUNT;

	Bdisp_PutDisp_DD();

	TMU5_COUNT = TMU5_COUNT - *(unsigned int*)P7305_EXTRA_TMU5_COUNT ;
//	if ( TimerID >= 5 ){
//		Timer_Stop( TimerID );
//		Timer_Deinstall( TimerID );
//	}	

	sprintf62s(buffer2,3276800/TMU5_COUNT," fps     ");
	strcat(buffer,buffer2);
    PrintMiniXY(8*18+8, 6*24+2, buffer,COLOR_WHITE,COLOR_BLACK);
}

void Benchmem(int duration_ms) {
    char buffer[21];
	int count;

    cont_flag = 1;
    TimerID=Timer_Install(0, &Timer_Routine, duration_ms);
	if ( TimerID >= 5 ) Timer_Start( TimerID );
	count=memread((unsigned int*)0xA0010000,&cont_flag);		// Flash ROM non cacheable
	if ( TimerID >= 5 ){
		Timer_Stop( TimerID );
		Timer_Deinstall( TimerID );
	}	
	itoa(count,(unsigned char *)buffer);
	strcat(buffer,"    ");
    PrintMiniXY(7*18-  8   , 6*24+2, " Rom  ",COLOR_WHITE,COLOR_BLACK);
    PrintMiniXY(7*18+ 38   , 6*24+2, buffer  ,COLOR_WHITE,COLOR_BLACK);


    cont_flag = 1;
    TimerID=Timer_Install(0, &Timer_Routine, duration_ms);
	if ( TimerID >= 5 ) Timer_Start( TimerID );
	count=memread((unsigned int*)0xA8010000,&cont_flag);		// SRAM non cacheable
	if ( TimerID >= 5 ){
		Timer_Stop( TimerID );
		Timer_Deinstall( TimerID );
	}
	itoa(count,(unsigned char *)buffer);
	strcat(buffer,"    ");
    PrintMiniXY(7*18+ 90   , 6*24+2, " Ram  ",COLOR_WHITE,COLOR_BLACK);
    PrintMiniXY(7*18+ 90+46, 6*24+2, buffer  ,COLOR_WHITE,COLOR_BLACK);

	
    cont_flag = 1;
    TimerID=Timer_Install(0, &Timer_Routine, duration_ms);
	if ( TimerID >= 5 ) Timer_Start( TimerID );
	count=memread((unsigned int*)0xA4150000,&cont_flag);		//   I/O non cacheable
	if ( TimerID >= 5 ){
		Timer_Stop( TimerID );
		Timer_Deinstall( TimerID );
	}
	itoa(count,(unsigned char *)buffer);
	strcat(buffer,"    ");
    PrintMiniXY(7*18+186   , 6*24+2, " p   ",COLOR_WHITE,COLOR_BLACK);
    PrintMiniXY(7*18+186+20, 6*24+2, buffer  ,COLOR_WHITE,COLOR_BLACK);
}

