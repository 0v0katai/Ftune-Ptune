/*
===============================================================================

 Ptune3 is SH7305 CPG&BSC tuning utility for PRIZM fx-CG50/Graph 90+E  v0.21

 copyright(c)2014,2015,2016,2017 by sentaro21
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

