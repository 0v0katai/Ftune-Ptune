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

//---------------------------------------------------------------------------------------------

int t_t0,t_t1,t_t2,t_tt;

int gettimes(){ 	
	return( RTC_GetTicks() );
}

void start_timer(){
	int cnt=0;

	t_t0=gettimes();
	while(gettimes()==t_t0);
	t_t1=t_t0+1;
	while(gettimes()==t_t1) cnt++;
	t_tt=cnt;
}

int get_timer(int prec){			// prec:1000  1/1000 count
	int cnt;
	cnt=0;
	t_t1=gettimes();
	while(gettimes()==t_t1) cnt++;
	t_t2=t_t1+1;
	return( ( (t_t2-t_t0-2)*prec-cnt*prec/t_tt )/128);
}


//---------------------------------------------------------------------------------------------

int cont_flag = 1;

void Timer_Routine(void){
    cont_flag = 0;
}

int Bench(int duration_ms, int display_count)
{
    int count = 0;
    char buffer[30];

    cont_flag = 1;
    SetTimer(ID_USER_TIMER1, duration_ms, &Timer_Routine);

//    while(cont_flag) {
//        count++;
//    }
	count=tcount(&cont_flag);

    KillTimer(ID_USER_TIMER1);

    if (display_count) {
		locate(1,7); PrintLine((unsigned char *) " ", 21);
        sprintf(buffer, " CPU.%d  ", count/100);
		PrintMini(0*6+0,6*8+1,(unsigned char *)buffer, MINI_REV);
    }
    return count/100;
}


int BenchDD(int duration_ms)
{
    char buffer[30];
    int i,k,prec=100000;
	int count;

	start_timer();
		Bdisp_PutDisp_DD();
	count = get_timer(prec) ;		// prec	
	
	start_timer();
	for(i=0;i<10;i++){
		Bdisp_PutDisp_DD();
		Bdisp_PutDisp_DD();
		Bdisp_PutDisp_DD();
		Bdisp_PutDisp_DD();
		Bdisp_PutDisp_DD();
	}
	count = get_timer(prec) ;		// prec	

	if(	(count > 10000 ) || ( count < 0 ) ) {
		prec=10000;
		start_timer();
		for(i=0;i<10;i++){
			Bdisp_PutDisp_DD();
			Bdisp_PutDisp_DD();
			Bdisp_PutDisp_DD();
			Bdisp_PutDisp_DD();
			Bdisp_PutDisp_DD();
		}
		count = get_timer(prec) ;		// prec	
		prec=10000;
	}

	sprintf(buffer," PutDispDD %5.1f fps  ",(float)prec*50/count);

	PrintMini( 7*6- 0,6*8+1,(unsigned char *)buffer, MINI_REV);
	return k;
}

void Benchmem(int duration_ms)
{
    int count;
    int	k;
    char buffer[30];

    cont_flag = 1;
    SetTimer(ID_USER_TIMER1, duration_ms, &Timer_Routine);
	count=memread((unsigned int*)0xA0010000,&cont_flag);		// Flash ROM non cacheable
    KillTimer(ID_USER_TIMER1);
   	sprintf(buffer, " o.%d    ", count);
	PrintMini( 7*6- 0,6*8+1,(unsigned char *)buffer, MINI_REV);

    cont_flag = 1;
    SetTimer(ID_USER_TIMER1, duration_ms, &Timer_Routine);
	count=memread((unsigned int*)0xA8010000,&cont_flag);		// SRAM non cacheable
    KillTimer(ID_USER_TIMER1);
   	sprintf(buffer, "a.%d    ", count);
	PrintMini( 8*6+26,6*8+1,(unsigned char *)buffer, MINI_REV);
    
    cont_flag = 1;
    SetTimer(ID_USER_TIMER1, duration_ms, &Timer_Routine);
//	count=memread((unsigned int*)0xA413FEC0,&cont_flag);		// RTC  I/O non cacheable
	count=memread((unsigned int*)0xA44C0014,&cont_flag);		// BCD  I/O non cacheable
    KillTimer(ID_USER_TIMER1);
   	sprintf(buffer, "p.%d   ", count);
	PrintMini( 8*6+54,6*8+1,(unsigned char *)buffer, MINI_REV);
}

