/*
===============================================================================

 Ftune3 is SH7305 CPG&BSC tuning utility for fx-9860GII-2      v2.00

 copyright(c)2014,2015,2016,2017,2018,2019 by sentaro21
 e-mail sentaro21@pm.matrix.jp

===============================================================================
*/

#define OPTION_REG	// CS3BCR,CS4BCR,CS6BCR,CS3WCR,CS4WCR,CS6WCR mod enable

extern char points[];

extern char IsEmu;

int CPU_check(void);
int IsG3() ;
int Is35E2() ;
int Emu_check() ;

void PrintXYR(int ,int ,char *,int );
void Fkey_dispN(int ,char *);
void Fkey_DISPN(int ,char *);
void Fkey_dispR(int ,char *) ;
void Fkey_Clear(int );
void Fkey_dispNP(int );
void Fkey_dispNM(int );
void Fkey_dispNInit(int );
void func_menu1(void);
void func_menu2(void);
void func_menu3(void);
void func_menu4(void);
void HexPrintXY(int , int , char *, unsigned int );
int YesNo( char*);
void ErrorMSG(char*, int );
void ErrorADRS(char*, int );
void MSG1(char*);
void MSG2(char*, char*);
void MSGpop(void);

int SysCalljmp( int r4, int r5, int r6, int r7, int No);
