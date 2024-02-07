/*
===============================================================================

 Ptune3 is SH7305 CPG&BSC tuning utility for PRIZM fx-CG50/Graph 90+E  v0.21

 copyright(c)2014,2015,2016,2017 by sentaro21
 e-mail sentaro21@pm.matrix.jp

===============================================================================
*/

int ROMmargin(void);
int RAMmargin(void);
int ROMtestR(int ,int ,volatile unsigned int*,int );
int RAMtestR(int ,int ,volatile unsigned int*,int );
int RAMtestW(int ,int ,volatile unsigned int*,int );
int ROMtestn(int ,int );
int RAMtestn(int ,int );
