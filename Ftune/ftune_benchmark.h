/*
===============================================================================

 Ftune is SH7337/SH7355 CPG&BSC tuning utility for fx-9860G/GII series  v1.01

 copyright(c)2014,2015 by sentaro21
 e-mail sentaro21@pm.matrix.jp

===============================================================================
 */

int RTC_GetTicks();
void System_GetOSVersion( unsigned char*version );

int gettimes(void);
void start_timer(void);
int get_timer(int);
void Timer_Routine(void);
int Bench(int , int );
int BenchDD(int);
void Benchmem(int);

int BatteryStatus();
