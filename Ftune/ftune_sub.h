/*
===============================================================================

 Ftune is SH7337/SH7355 CPG&BSC tuning utility for fx-9860G/GII series  v1.01

 copyright(c)2014,2015 by sentaro21
 e-mail sentaro21@pm.matrix.jp

===============================================================================
 */

void change_FRQCRs( unsigned int );
void change_FRQCR(int , int , int ); 
int get_FRQCR(void);
int get_FRQCR_STC(void);
int get_FRQCR_IFC(void);
int get_FRQCR_SFC(void);
int get_FRQCR_BFC(void);
int get_FRQCR_PFC(void);

extern fmult;

void freq_check(void);
int freq_sub(int ,int );

