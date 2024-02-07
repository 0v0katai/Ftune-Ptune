/*
===============================================================================

 Ftune is SH7337/SH7355 CPG&BSC tuning utility for fx-9860G/GII series  v1.01

 copyright(c)2014,2015 by sentaro21
 e-mail sentaro21@pm.matrix.jp

===============================================================================
 */

#define P7305_EXTRA_TMU5_COUNT 0xA44D00D8

void Timer_Routine(void);
unsigned int gettimes(void);
void start_timer(void);
unsigned int get_timer(void);

void Bench(int , int );
void BenchDD(int);
void Benchmem(int);

