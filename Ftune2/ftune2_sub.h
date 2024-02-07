/*
===============================================================================

 Ftune is SH7337/SH7355 CPG&BSC tuning utility for fx-9860G/GII series  v1.10

 copyright(c)2014,2015,2016 by sentaro21
 e-mail sentaro21@pm.matrix.jp

===============================================================================
 */
 
void change_FRQCRs( unsigned int );
void change_FRQCR(int , int , int , int ,int ); 
int get_FRQCR(void);
int get_FRQCR_STC(void);
int get_FRQCR_IFC(void);
int get_FRQCR_SFC(void);
int get_FRQCR_BFC(void);
int get_FRQCR_PFC(void);

int CheckROMwait(int );
void SetROMwait(int ,int );
int CheckRAMwait(int );
int CheckRAMwaitW(int );
void SetRAMwait(int ,int );
int freq_sub(int ,int ,int ); 
int CheckFC(int ,int ,int ,int );
int UpIFC(int ,int ,int );
int UpSFC(int ,int ,int );
int UpBFC(int ,int ,int ,int ,int );
int UpPFC(int ,int ,int );
void PFC_auto_up(int ,int ,int ,int );
void PFC_auto_down(int ,int ,int );
void SetWait(int ,int ,int ,int ,int );
void SetWaitm(int ,int ,int ,int ,int );

void SetMaxFreq0(void);
void SetMaxFreq(void);
void wait_init(void);

extern int	MaxFreq_PLL;
extern int	MaxFreq_IFC;
extern int	MaxFreq_SFC;
extern int	MaxFreq_BFC;
extern int	MaxFreq_PFC;

extern int MaxFreq_ROMwaitDef[];
extern int MaxFreq_ROMwait0[];
extern int MaxFreq_ROMwait[];
extern int MaxFreq_RAMwaitDef[];
extern int MaxFreq_RAMwait0[];
extern int MaxFreq_RAMwait[];
extern int MaxFreq_RAMwaitWDef[];
extern int MaxFreq_RAMwaitW0[];
extern int MaxFreq_RAMwaitW[];

extern volatile unsigned int*ROMselect ;		// ROM wait check base address
extern volatile unsigned int*RAMselect ;		// RAM wait check base address

