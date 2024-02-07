/*
===============================================================================

 Ftune2 is SH7305 CPG&BSC tuning utility for fx-9860GII-2      v1.01

 copyright(c)2014,2015 by sentaro21
 e-mail sentaro21@pm.matrix.jp

===============================================================================
*/

extern  unsigned  int   SaveDataF0_FLLFRQ ;
extern  unsigned  int   SaveDataF0_FRQCR  ;
extern  unsigned  int   SaveDataF0_CS0BCR ;
extern  unsigned  int   SaveDataF0_CS2BCR ;
extern  unsigned  int   SaveDataF0_CS0WCR ;
extern  unsigned  int   SaveDataF0_CS2WCR ;
extern  unsigned  int   SaveDataF0_CS5aBCR;
extern  unsigned  int   SaveDataF0_CS5aWCR;

extern  unsigned  int   SaveDataF2_FLLFRQ ;
extern  unsigned  int   SaveDataF2_FRQCR  ;
extern  unsigned  int   SaveDataF2_CS0BCR ;
extern  unsigned  int   SaveDataF2_CS2BCR ;
extern  unsigned  int   SaveDataF2_CS0WCR ;
extern  unsigned  int   SaveDataF2_CS2WCR ;
extern  unsigned  int   SaveDataF2_CS5aBCR;
extern  unsigned  int   SaveDataF2_CS5aWCR;

extern  unsigned  int   SaveDataF3_FLLFRQ ;
extern  unsigned  int   SaveDataF3_FRQCR  ;
extern  unsigned  int   SaveDataF3_CS0BCR ;
extern  unsigned  int   SaveDataF3_CS2BCR ;
extern  unsigned  int   SaveDataF3_CS0WCR ;
extern  unsigned  int   SaveDataF3_CS2WCR ;
extern  unsigned  int   SaveDataF3_CS5aBCR;
extern  unsigned  int   SaveDataF3_CS5aWCR;

extern  unsigned  int   SaveDataF4_FLLFRQ ;
extern  unsigned  int   SaveDataF4_FRQCR  ;
extern  unsigned  int   SaveDataF4_CS0BCR ;
extern  unsigned  int   SaveDataF4_CS2BCR ;
extern  unsigned  int   SaveDataF4_CS0WCR ;
extern  unsigned  int   SaveDataF4_CS2WCR ;
extern  unsigned  int   SaveDataF4_CS5aBCR;
extern  unsigned  int   SaveDataF4_CS5aWCR;

extern  unsigned  int   SaveDataF5_FLLFRQ ;
extern  unsigned  int   SaveDataF5_FRQCR  ;
extern  unsigned  int   SaveDataF5_CS0BCR ;
extern  unsigned  int   SaveDataF5_CS2BCR ;
extern  unsigned  int   SaveDataF5_CS0WCR ;
extern  unsigned  int   SaveDataF5_CS2WCR ;
extern  unsigned  int   SaveDataF5_CS5aBCR;
extern  unsigned  int   SaveDataF5_CS5aWCR;

void SaveFile();
void LoadFile(int );

void SaveDataF0();
void SaveDataF2();
void SaveDataF3();
void SaveDataF4();
void SaveDataF5();

void LoadDataF0();
void LoadDataF2();
void LoadDataF3();
void LoadDataF4();
void LoadDataF5();

