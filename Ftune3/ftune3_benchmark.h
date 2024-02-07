/*
===============================================================================

 Ftune3 is SH7305 CPG&BSC tuning utility for fx-9860GII-2      v2.00

 copyright(c)2014,2015,2016,2017,2018,2019 by sentaro21
 e-mail sentaro21@pm.matrix.jp

===============================================================================
 */

#define P7305_EXTRA_TMU5_COUNT 0xA44D00D8

int gettimes(void);
void start_timer(void);
int get_timer(int);
void Timer_Routine(void);
void Bench(int , int );
int BenchDD(int);
void Benchmem(int);

