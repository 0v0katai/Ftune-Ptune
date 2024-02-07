/*
===============================================================================

 Ftune is SH7337/SH7355 CPG&BSC tuning utility for fx-9860G/GII series  v1.01

 copyright(c)2014,2015 by sentaro21
 e-mail sentaro21@pm.matrix.jp

===============================================================================
*/

unsigned int* MEMtestsR(volatile unsigned int* ,volatile unsigned int * ,int ,int );
unsigned int* MEMtestsW(volatile unsigned int* ,volatile unsigned int * ,int ,int );

int ROMtestR(int ,volatile unsigned int* ,int );
int RAMtestR(int ,volatile unsigned int* ,int );
int RAMtestW(int ,volatile unsigned int* ,int );
int ROMtestn(int );
int RAMtestn(int );
int RAMtestnW(int );

