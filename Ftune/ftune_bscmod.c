/*
===============================================================================

 Ftune is SH7337/SH7355 CPG&BSC tuning utility for fx-9860G/GII series  v1.01

 copyright(c)2014,2015 by sentaro21
 e-mail sentaro21@pm.matrix.jp

===============================================================================
*/

#include "fxlib.h"
#include "timer.h"
#include "stdio.h"
#include "string.h"

#include "common.h"
#include "ftune.h"
#include "ftune_sub.h"

//---------------------------------------------------------------------------------------------
char *waitw[]={"0 ","1 ","2 ","4 "};
char *waits[]={"1 ","2 ","3 ","5 "};
char *busw[]= {"--"," 8","16","32"};

void dsp_BCR_mod(int freq){
	unsigned int key;
	int	IWW0,IWRWD0,IWRWS0,IWRRD0,IWRRS0;
	int	IWW2,IWRWD2,IWRWS2,IWRRD2,IWRRS2;
	int	flag=1;
	int selectb=10;

    while(flag) {

		IWW0  =BSC.CS0BCR.BIT.IWW ;
		IWRWD0=BSC.CS0BCR.BIT.IWRWD ;
		IWRWS0=BSC.CS0BCR.BIT.IWRWS ;
		IWRRD0=BSC.CS0BCR.BIT.IWRRD ;
		IWRRS0=BSC.CS0BCR.BIT.IWRRS ;
	
		IWW2  =BSC.CS2BCR.BIT.IWW ;
		IWRWD2=BSC.CS2BCR.BIT.IWRWD ;
		IWRWS2=BSC.CS2BCR.BIT.IWRWS ;
		IWRRD2=BSC.CS2BCR.BIT.IWRRD ;
		IWRRS2=BSC.CS2BCR.BIT.IWRRS ;

	    Bdisp_AllClr_VRAM();
	    locate( 2,1);	Print((unsigned char *)"ROM:CS0");
		HexPrintXY(2,2,"",BSC.CS0BCR.LONG);
		PrintXY( 0*6,2*8,(unsigned char *)" IWW  :",selectb==0?1:0);
		PrintXY( 0*6,3*8,(unsigned char *)" IWRWD:",selectb==1?1:0);
		PrintXY( 0*6,4*8,(unsigned char *)" IWRWS:",selectb==2?1:0);
		PrintXY( 0*6,5*8,(unsigned char *)" IWRRD:",selectb==3?1:0);
		PrintXY( 0*6,6*8,(unsigned char *)" IWRRS:",selectb==4?1:0);
		PrintXY( 7*6,2*8,(unsigned char *)waitw[IWW0],  IWW0  ==0?0:1);
		PrintXY( 7*6,3*8,(unsigned char *)waits[IWRWD0],IWRWD0==1?0:1);
		PrintXY( 7*6,4*8,(unsigned char *)waits[IWRWS0],IWRWS0==1?0:1);
		PrintXY( 7*6,5*8,(unsigned char *)waits[IWRRD0],IWRRD0==1?0:1);
		PrintXY( 7*6,6*8,(unsigned char *)waitw[IWRRS0],IWRRS0==0?0:1);
		
	    locate(12,1);	Print((unsigned char *)"RAM:CS2");
		HexPrintXY(12,2,"",BSC.CS2BCR.LONG);
		PrintXY(10*6,2*8,(unsigned char *)" IWW  :",selectb==10?1:0);
		PrintXY(10*6,3*8,(unsigned char *)" IWRWD:",selectb==11?1:0);
		PrintXY(10*6,4*8,(unsigned char *)" IWRWS:",selectb==12?1:0);
		PrintXY(10*6,5*8,(unsigned char *)" IWRRD:",selectb==13?1:0);
		PrintXY(10*6,6*8,(unsigned char *)" IWRRS:",selectb==14?1:0);
		PrintXY(17*6,2*8,(unsigned char *)waitw[IWW2],  IWW2  ==0?0:1);
		PrintXY(17*6,3*8,(unsigned char *)waits[IWRWD2],IWRWD2==1?0:1);
		PrintXY(17*6,4*8,(unsigned char *)waits[IWRWS2],IWRWS2==1?0:1);
		PrintXY(17*6,5*8,(unsigned char *)waits[IWRRD2],IWRRD2==1?0:1);
		PrintXY(17*6,6*8,(unsigned char *)waitw[IWRRS2],IWRRS2==0?0:1);

		if ( selectb < 10 ) { locate( 1,selectb+3   );Print((unsigned char *)points); }
					else    { locate(11,selectb+3-10);Print((unsigned char *)points); }

		Fkey_dispN(0," +  ");
		Fkey_dispN(1," -  ");
		Fkey_dispN(3,"Init");

		Bdisp_PutDisp_DD();
		
		GetKey( &key );
        switch (key) {
            case KEY_CTRL_AC:
            	flag=0;
                break;
            case KEY_CTRL_EXIT:
            	flag=0;
                break;

			case KEY_CTRL_UP:
				selectb-=1;
				if (selectb == -1) selectb=0;
				if (selectb ==  9) selectb=10;
				break;
			case KEY_CTRL_DOWN:
				selectb+=1;
				if (selectb == 5) selectb=4;
				if (selectb ==15) selectb=14;
				break;
			case KEY_CTRL_RIGHT:
				if (selectb < 10 ) selectb+=10;
				break;
			case KEY_CTRL_LEFT:
				if (selectb >= 10 ) selectb-=10;
				break;
				
			case KEY_CTRL_F4:
				BSC.CS0BCR.LONG = CS0BCR_default ;
				BSC.CS2BCR.LONG = CS2BCR_default ;
				break;
        
			case KEY_CHAR_PLUS:
			case KEY_CTRL_F1:		//	+
				switch (selectb) {
					case 0:	// IWW0
						if( IWW0 >= 3 ) break;
						IWW0+=1;
						BSC.CS0BCR.BIT.IWW = IWW0 ;
						break;
					case 1:	// IWRWD0
						if( IWRWD0 >= 3 ) break;
						IWRWD0+=1;
						BSC.CS0BCR.BIT.IWRWD = IWRWD0 ;
						break;
					case 2:	// IWRWS0
						if( IWRWS0 >= 3 ) break;
						IWRWS0+=1;
						BSC.CS0BCR.BIT.IWRWS = IWRWS0 ;
						break;
					case 3:	// IWRRD0
						if( IWRRD0 >= 3 ) break;
						IWRRD0+=1;
						BSC.CS0BCR.BIT.IWRRD = IWRRD0 ;
						break;
					case 4:	// IWRRS0
						if( IWRRS0 >= 3 ) break;
						IWRRS0+=1;
						BSC.CS0BCR.BIT.IWRRS = IWRRS0 ;
						break;
						
					case 10:	// IWW2
						if( IWW2 >= 3 ) break;
						IWW2+=1;
						BSC.CS2BCR.BIT.IWW = IWW2 ;
						break;
					case 11:	// IWRWD2
						if( IWRWD2 >= 3 ) break;
						IWRWD2+=1;
						BSC.CS2BCR.BIT.IWRWD = IWRWD2 ;
						break;
					case 12:	// IWRWS2
						if( IWRWS2 >= 3 ) break;
						IWRWS2+=1;
						BSC.CS2BCR.BIT.IWRWS = IWRWS2 ;
						break;
					case 13:	// IWRRD2
						if( IWRRD2 >= 3 ) break;
						IWRRD2+=1;
						BSC.CS2BCR.BIT.IWRRD = IWRRD2 ;
						break;
					case 14:	// IWRRS2
						if( IWRRS2 >= 3 ) break;
						IWRRS2+=1;
						BSC.CS2BCR.BIT.IWRRS = IWRRS2 ;
						break;
				}
				break;
				
			case KEY_CHAR_MINUS:
			case KEY_CTRL_F2:		//		-
				switch (selectb) {
					case 0:	// IWW0
						if( IWW0 <= 0 ) break;
						IWW0-=1;
						BSC.CS0BCR.BIT.IWW = IWW0 ;
						break;
					case 1:	// IWRWD0
						if( IWRWD0 <= 0 ) break;
						IWRWD0-=1;
						BSC.CS0BCR.BIT.IWRWD = IWRWD0 ;
						break;
					case 2:	// IWRWS0
						if( IWRWS0 <= 0 ) break;
						IWRWS0-=1;
						BSC.CS0BCR.BIT.IWRWS = IWRWS0 ;
						break;
					case 3:	// IWRRD0
						if( IWRRD0 <= 0 ) break;
						IWRRD0-=1;
						BSC.CS0BCR.BIT.IWRRD = IWRRD0 ;
						break;
					case 4:	// IWRRS0
						if( IWRRS0 <= 0 ) break;
						IWRRS0-=1;
						BSC.CS0BCR.BIT.IWRRS = IWRRS0 ;
						break;
						
					case 10:	// IWW2
						if( IWW2 <= 0 ) break;
						IWW2-=1;
						BSC.CS2BCR.BIT.IWW = IWW2 ;
						break;
					case 11:	// IWRWD2
						if( IWRWD2 <= 0 ) break;
						IWRWD2-=1;
						BSC.CS2BCR.BIT.IWRWD = IWRWD2 ;
						break;
					case 12:	// IWRWS2
						if( IWRWS2 <= 0 ) break;
						IWRWS2-=1;
						BSC.CS2BCR.BIT.IWRWS = IWRWS2 ;
						break;
					case 13:	// IWRRD2
						if( IWRRD2 <= 0 ) break;
						IWRRD2-=1;
						BSC.CS2BCR.BIT.IWRRD = IWRRD2 ;
						break;
					case 14:	// IWRRS2
						if( IWRRS2 <= 0 ) break;
						IWRRS2-=1;
						BSC.CS2BCR.BIT.IWRRS = IWRRS2 ;
						break;
				}
				break;

        }
    }
}


//---------------------------------------------------------------------------------------------
#ifdef OPTION_REG
void dsp_BCR34_mod(int freq){
	unsigned int key;
	int	IWW3,IWRWD3,IWRWS3,IWRRD3,IWRRS3;
	int	IWW4,IWRWD4,IWRWS4,IWRRD4,IWRRS4;
	int	flag=1;
	int selectb=0;

    while(flag) {

		IWW3  =BSC.CS3BCR.BIT.IWW ;
		IWRWD3=BSC.CS3BCR.BIT.IWRWD ;
		IWRWS3=BSC.CS3BCR.BIT.IWRWS ;
		IWRRD3=BSC.CS3BCR.BIT.IWRRD ;
		IWRRS3=BSC.CS3BCR.BIT.IWRRS ;
	
		IWW4  =BSC.CS4BCR.BIT.IWW ;
		IWRWD4=BSC.CS4BCR.BIT.IWRWD ;
		IWRWS4=BSC.CS4BCR.BIT.IWRWS ;
		IWRRD4=BSC.CS4BCR.BIT.IWRRD ;
		IWRRS4=BSC.CS4BCR.BIT.IWRRS ;

	    Bdisp_AllClr_VRAM();
	    locate( 2,1);	Print((unsigned char *)"CS3BCR");
		HexPrintXY(2,2,"",BSC.CS3BCR.LONG);
		PrintXY( 0*6,2*8,(unsigned char *)" IWW  :",selectb==0?1:0);
		PrintXY( 0*6,3*8,(unsigned char *)" IWRWD:",selectb==1?1:0);
		PrintXY( 0*6,4*8,(unsigned char *)" IWRWS:",selectb==2?1:0);
		PrintXY( 0*6,5*8,(unsigned char *)" IWRRD:",selectb==3?1:0);
		PrintXY( 0*6,6*8,(unsigned char *)" IWRRS:",selectb==4?1:0);
		PrintXY( 7*6,2*8,(unsigned char *)waitw[IWW3],  IWW3  ==3?0:1);
		PrintXY( 7*6,3*8,(unsigned char *)waits[IWRWD3],IWRWD3==3?0:1);
		PrintXY( 7*6,4*8,(unsigned char *)waits[IWRWS3],IWRWS3==3?0:1);
		PrintXY( 7*6,5*8,(unsigned char *)waits[IWRRD3],IWRRD3==3?0:1);
		PrintXY( 7*6,6*8,(unsigned char *)waitw[IWRRS3],IWRRS3==3?0:1);
		
	    locate(12,1);	Print((unsigned char *)"CS4BCR");
		HexPrintXY(12,2,"",BSC.CS4BCR.LONG);
		PrintXY(10*6,2*8,(unsigned char *)" IWW  :",selectb==10?1:0);
		PrintXY(10*6,3*8,(unsigned char *)" IWRWD:",selectb==11?1:0);
		PrintXY(10*6,4*8,(unsigned char *)" IWRWS:",selectb==12?1:0);
		PrintXY(10*6,5*8,(unsigned char *)" IWRRD:",selectb==13?1:0);
		PrintXY(10*6,6*8,(unsigned char *)" IWRRS:",selectb==14?1:0);
		PrintXY(17*6,2*8,(unsigned char *)waitw[IWW4],  IWW4  ==0?0:1);
		PrintXY(17*6,3*8,(unsigned char *)waits[IWRWD4],IWRWD4==1?0:1);
		PrintXY(17*6,4*8,(unsigned char *)waits[IWRWS4],IWRWS4==1?0:1);
		PrintXY(17*6,5*8,(unsigned char *)waits[IWRRD4],IWRRD4==1?0:1);
		PrintXY(17*6,6*8,(unsigned char *)waitw[IWRRS4],IWRRS4==0?0:1);

		if ( selectb < 10 ) { locate( 1,selectb+3   );Print((unsigned char *)points); }
					else    { locate(11,selectb+3-10);Print((unsigned char *)points); }

		Fkey_dispN(0," +  ");
		Fkey_dispN(1," -  ");
		Fkey_dispN(3,"Init");

		Bdisp_PutDisp_DD();
		
		GetKey( &key );
        switch (key) {
            case KEY_CTRL_AC:
            	flag=0;
                break;
            case KEY_CTRL_EXIT:
            	flag=0;
                break;

			case KEY_CTRL_UP:
				selectb-=1;
				if (selectb == -1) selectb=0;
				if (selectb ==  9) selectb=10;
				break;
			case KEY_CTRL_DOWN:
				selectb+=1;
				if (selectb == 5) selectb=4;
				if (selectb ==15) selectb=14;
				break;
			case KEY_CTRL_RIGHT:
				if (selectb < 10 ) selectb+=10;
				break;
			case KEY_CTRL_LEFT:
				if (selectb >= 10 ) selectb-=10;
				break;
				
			case KEY_CTRL_F4:
				BSC.CS3BCR.LONG = CS3BCR_default ;
				BSC.CS4BCR.LONG = CS4BCR_default ;
				break;
        
			case KEY_CHAR_PLUS:
			case KEY_CTRL_F1:		//	+
				switch (selectb) {
					case 0:	// IWW3
						if( IWW3 >= 3 ) break;
						IWW3+=1;
						BSC.CS3BCR.BIT.IWW = IWW3 ;
						break;
					case 1:	// IWRWD3
						if( IWRWD3 >= 3 ) break;
						IWRWD3+=1;
						BSC.CS3BCR.BIT.IWRWD = IWRWD3 ;
						break;
					case 2:	// IWRWS3
						if( IWRWS3 >= 3 ) break;
						IWRWS3+=1;
						BSC.CS3BCR.BIT.IWRRD = IWRRD3 ;
						break;
					case 3:	// IWRRD3
						if( IWRRD3 >= 3 ) break;
						IWRRD3+=1;
						BSC.CS3BCR.BIT.IWRRD = IWRRD3 ;
						break;
					case 4:	// IWRRS3
						if( IWRRS3 >= 3 ) break;
						IWRRS3+=1;
						BSC.CS3BCR.BIT.IWRRS = IWRRS3 ;
						break;
						
					case 10:	// IWW4
						if( IWW4 >= 3 ) break;
						IWW4+=1;
						BSC.CS4BCR.BIT.IWW = IWW4 ;
						break;
					case 11:	// IWRWD4
						if( IWRWD4 >= 3 ) break;
						IWRWD4+=1;
						BSC.CS4BCR.BIT.IWRWD = IWRWD4 ;
						break;
					case 12:	// IWRWS4
						if( IWRWS4 >= 3 ) break;
						IWRWS4+=1;
						BSC.CS4BCR.BIT.IWRWS = IWRWS4 ;
						break;
					case 13:	// IWRRD4
						if( IWRRD4 >= 3 ) break;
						IWRRD4+=1;
						BSC.CS4BCR.BIT.IWRRD = IWRRD4 ;
						break;
					case 14:	// IWRRS4
						if( IWRRS4 >= 3 ) break;
						IWRRS4+=1;
						BSC.CS4BCR.BIT.IWRRS = IWRRS4 ;
						break;
				}
				break;
				
			case KEY_CHAR_MINUS:
			case KEY_CTRL_F2:		//		-
				switch (selectb) {
					case 0:	// IWW3
						if( IWW3 <= 0 ) break;
						IWW3-=1;
						BSC.CS3BCR.BIT.IWW = IWW3 ;
						break;
					case 1:	// IWRWD3
						if( IWRWD3 <= 0 ) break;
						IWRWD3-=1;
						BSC.CS3BCR.BIT.IWRWD = IWRWD3 ;
						break;
					case 2:	// IWRWS3
						if( IWRWS3 <= 0 ) break;
						IWRWS3-=1;
						BSC.CS3BCR.BIT.IWRWS = IWRWS3 ;
						break;
					case 3:	// IWRRD3
						if( IWRRD3 <= 0 ) break;
						IWRRD3-=1;
						BSC.CS3BCR.BIT.IWRRD = IWRRD3 ;
						break;
					case 4:	// IWRRS3
						if( IWRRS3 <= 0 ) break;
						IWRRS3-=1;
						BSC.CS3BCR.BIT.IWRRS = IWRRS3 ;
						break;
						
					case 10:	// IWW4
						if( IWW4 <= 0 ) break;
						IWW4-=1;
						BSC.CS4BCR.BIT.IWW = IWW4 ;
						break;
					case 11:	// IWRWD4
						if( IWRWD4 <= 0 ) break;
						IWRWD4-=1;
						BSC.CS4BCR.BIT.IWRWD = IWRWD4 ;
						break;
					case 12:	// IWRWS4
						if( IWRWS4 <= 0 ) break;
						IWRWS4-=1;
						BSC.CS4BCR.BIT.IWRWS = IWRWS4 ;
						break;
					case 13:	// IWRRD4
						if( IWRRD4 <= 0 ) break;
						IWRRD4-=1;
						BSC.CS4BCR.BIT.IWRRD = IWRRD4 ;
						break;
					case 14:	// IWRRS4
						if( IWRRS4 <= 0 ) break;
						IWRRS4-=1;
						BSC.CS4BCR.BIT.IWRRS = IWRRS4 ;
						break;
				}
				break;

        }
    }
}
#endif

//---------------------------------------------------------------------------------------------
void dsp_BCR5_mod(int freq){
	unsigned int key;
	int	IWW5a,IWRWD5a,IWRWS5a,IWRRD5a,IWRRS5a;
	int	IWW5b,IWRWD5b,IWRWS5b,IWRRD5b,IWRRS5b;
	int	flag=1;
	int selectb=0;

    while(flag) {

		IWW5a  =BSC.CS5ABCR.BIT.IWW ;
		IWRWD5a=BSC.CS5ABCR.BIT.IWRWD ;
		IWRWS5a=BSC.CS5ABCR.BIT.IWRWS ;
		IWRRD5a=BSC.CS5ABCR.BIT.IWRRD ;
		IWRRS5a=BSC.CS5ABCR.BIT.IWRRS ;
	
		IWW5b  =BSC.CS5BBCR.BIT.IWW ;
		IWRWD5b=BSC.CS5BBCR.BIT.IWRWD ;
		IWRWS5b=BSC.CS5BBCR.BIT.IWRWS ;
		IWRRD5b=BSC.CS5BBCR.BIT.IWRRD ;
		IWRRS5b=BSC.CS5BBCR.BIT.IWRRS ;

	    Bdisp_AllClr_VRAM();
	    locate( 2,1);	Print((unsigned char *)"CS5ABCR");
		HexPrintXY(2,2,"",BSC.CS5ABCR.LONG);
		PrintXY( 0*6,2*8,(unsigned char *)" IWW  :",selectb==0?1:0);
		PrintXY( 0*6,3*8,(unsigned char *)" IWRWD:",selectb==1?1:0);
		PrintXY( 0*6,4*8,(unsigned char *)" IWRWS:",selectb==2?1:0);
		PrintXY( 0*6,5*8,(unsigned char *)" IWRRD:",selectb==3?1:0);
		PrintXY( 0*6,6*8,(unsigned char *)" IWRRS:",selectb==4?1:0);
		PrintXY( 7*6,2*8,(unsigned char *)waitw[IWW5a],  IWW5a  ==2?0:1);
		PrintXY( 7*6,3*8,(unsigned char *)waits[IWRWD5a],IWRWD5a==1?0:1);
		PrintXY( 7*6,4*8,(unsigned char *)waits[IWRWS5a],IWRWS5a==1?0:1);
		PrintXY( 7*6,5*8,(unsigned char *)waits[IWRRD5a],IWRRD5a==1?0:1);
		PrintXY( 7*6,6*8,(unsigned char *)waitw[IWRRS5a],IWRRS5a==2?0:1);
		
	    locate(12,1);	Print((unsigned char *)"CS5BBCR");
		HexPrintXY(12,2,"",BSC.CS5BBCR.LONG);
		PrintXY(10*6,2*8,(unsigned char *)" IWW  :",selectb==10?1:0);
		PrintXY(10*6,3*8,(unsigned char *)" IWRWD:",selectb==11?1:0);
		PrintXY(10*6,4*8,(unsigned char *)" IWRWS:",selectb==12?1:0);
		PrintXY(10*6,5*8,(unsigned char *)" IWRRD:",selectb==13?1:0);
		PrintXY(10*6,6*8,(unsigned char *)" IWRRS:",selectb==14?1:0);
		PrintXY(17*6,2*8,(unsigned char *)waitw[IWW5b],  IWW5b  ==2?0:1);
		PrintXY(17*6,3*8,(unsigned char *)waits[IWRWD5b],IWRWD5b==1?0:1);
		PrintXY(17*6,4*8,(unsigned char *)waits[IWRWS5b],IWRWS5b==1?0:1);
		PrintXY(17*6,5*8,(unsigned char *)waits[IWRRD5b],IWRRD5b==1?0:1);
		PrintXY(17*6,6*8,(unsigned char *)waitw[IWRRS5b],IWRRS5b==2?0:1);

		if ( selectb < 10 ) { locate( 1,selectb+3   );Print((unsigned char *)points); }
					else    { locate(11,selectb+3-10);Print((unsigned char *)points); }

		Fkey_dispN(0," +  ");
		Fkey_dispN(1," -  ");
		Fkey_dispN(3,"Init");

		Bdisp_PutDisp_DD();
		
		GetKey( &key );
        switch (key) {
            case KEY_CTRL_AC:
            	flag=0;
                break;
            case KEY_CTRL_EXIT:
            	flag=0;
                break;

			case KEY_CTRL_UP:
				selectb-=1;
				if (selectb == -1) selectb=0;
				if (selectb ==  9) selectb=10;
				break;
			case KEY_CTRL_DOWN:
				selectb+=1;
				if (selectb == 5) selectb=4;
				if (selectb ==15) selectb=14;
				break;
			case KEY_CTRL_RIGHT:
				if (selectb < 10 ) selectb+=10;
				break;
			case KEY_CTRL_LEFT:
				if (selectb >= 10 ) selectb-=10;
				break;
				
			case KEY_CTRL_F4:
				BSC.CS5ABCR.LONG = CS5aBCR_default ;
				BSC.CS5BBCR.LONG = CS5bBCR_default ;
				break;
        
			case KEY_CHAR_PLUS:
			case KEY_CTRL_F1:		//	+
				switch (selectb) {
					case 0:	// IWW5a
						if( IWW5a >= 3 ) break;
						IWW5a+=1;
						BSC.CS5ABCR.BIT.IWW = IWW5a ;
						break;
					case 1:	// IWRWD5a
						if( IWRWD5a >= 3 ) break;
						IWRWD5a+=1;
						BSC.CS5ABCR.BIT.IWRWD = IWRWD5a ;
						break;
					case 2:	// IWRWS5a
						if( IWRWS5a >= 3 ) break;
						IWRWS5a+=1;
						BSC.CS5ABCR.BIT.IWRWS = IWRWS5a ;
						break;
					case 3:	// IWRRD5a
						if( IWRRD5a >= 3 ) break;
						IWRRD5a+=1;
						BSC.CS5ABCR.BIT.IWRRD = IWRRD5a ;
						break;
					case 4:	// IWRRS5a
						if( IWRRS5a >= 3 ) break;
						IWRRS5a+=1;
						BSC.CS5ABCR.BIT.IWRRS = IWRRS5a ;
						break;
						
					case 10:	// IWW5b
						if( IWW5b >= 3 ) break;
						IWW5b+=1;
						BSC.CS5BBCR.BIT.IWW = IWW5b ;
						break;
					case 11:	// IWRWD5b
						if( IWRWD5b >= 3 ) break;
						IWRWD5b+=1;
						BSC.CS5BBCR.BIT.IWRWD = IWRWD5b ;
						break;
					case 12:	// IWRWS5b
						if( IWRWS5b >= 3 ) break;
						IWRWS5b+=1;
						BSC.CS5BBCR.BIT.IWRWS = IWRWS5b ;
						break;
					case 13:	// IWRRD5b
						if( IWRRD5b >= 3 ) break;
						IWRRD5b+=1;
						BSC.CS5BBCR.BIT.IWRRD = IWRRD5b ;
						break;
					case 14:	// IWRRS5b
						if( IWRRS5b >= 3 ) break;
						IWRRS5b+=1;
						BSC.CS5BBCR.BIT.IWRRS = IWRRS5b ;
						break;
				}
				break;
				
			case KEY_CHAR_MINUS:
			case KEY_CTRL_F2:		//		-
				switch (selectb) {
					case 0:	// IWW5a
						if( IWW5a <= 0 ) break;
						IWW5a-=1;
						BSC.CS5ABCR.BIT.IWW = IWW5a ;
						break;
					case 1:	// IWRWD5a
						if( IWRWD5a <= 0 ) break;
						IWRWD5a-=1;
						BSC.CS5ABCR.BIT.IWRWD = IWRWD5a ;
						break;
					case 2:	// IWRWS5a
						if( IWRWS5a <= 0 ) break;
						IWRWS5a-=1;
						BSC.CS5ABCR.BIT.IWRWS = IWRWS5a ;
						break;
					case 3:	// IWRRD5a
						if( IWRRD5a <= 0 ) break;
						IWRRD5a-=1;
						BSC.CS5ABCR.BIT.IWRRD = IWRRD5a ;
						break;
					case 4:	// IWRRS5a
						if( IWRRS5a <= 0 ) break;
						IWRRS5a-=1;
						BSC.CS5ABCR.BIT.IWRRS = IWRRS5a ;
						break;
						
					case 10:	// IWW5b
						if( IWW5b <= 0 ) break;
						IWW5b-=1;
						BSC.CS5BBCR.BIT.IWW = IWW5b ;
						break;
					case 11:	// IWRWD5b
						if( IWRWD5b <= 0 ) break;
						IWRWD5b-=1;
						BSC.CS5BBCR.BIT.IWRWD = IWRWD5b ;
						break;
					case 12:	// IWRWS5b
						if( IWRWS5b <= 0 ) break;
						IWRWS5b-=1;
						BSC.CS5BBCR.BIT.IWRWS = IWRWS5b ;
						break;
					case 13:	// IWRRD5b
						if( IWRRD5b <= 0 ) break;
						IWRRD5b-=1;
						BSC.CS5BBCR.BIT.IWRRD = IWRRD5b ;
						break;
					case 14:	// IWRRS5b
						if( IWRRS5b <= 0 ) break;
						IWRRS5b-=1;
						BSC.CS5BBCR.BIT.IWRRS = IWRRS5b ;
						break;
				}
				break;

        }
    }
}

//---------------------------------------------------------------------------------------------
#ifdef OPTION_REG
void dsp_BCR6_mod(int freq){
	unsigned int key;
	int	IWW6a,IWRWD6a,IWRWS6a,IWRRD6a,IWRRS6a;
	int	IWW6b,IWRWD6b,IWRWS6b,IWRRD6b,IWRRS6b;
	int	flag=1;
	int selectb=0;

    while(flag) {

		IWW6a  =BSC.CS6ABCR.BIT.IWW ;
		IWRWD6a=BSC.CS6ABCR.BIT.IWRWD ;
		IWRWS6a=BSC.CS6ABCR.BIT.IWRWS ;
		IWRRD6a=BSC.CS6ABCR.BIT.IWRRD ;
		IWRRS6a=BSC.CS6ABCR.BIT.IWRRS ;
	
		IWW6b  =BSC.CS6BBCR.BIT.IWW ;
		IWRWD6b=BSC.CS6BBCR.BIT.IWRWD ;
		IWRWS6b=BSC.CS6BBCR.BIT.IWRWS ;
		IWRRD6b=BSC.CS6BBCR.BIT.IWRRD ;
		IWRRS6b=BSC.CS6BBCR.BIT.IWRRS ;

	    Bdisp_AllClr_VRAM();
	    locate( 2,1);	Print((unsigned char *)"CS6ABCR");
		HexPrintXY(2,2,"",BSC.CS6ABCR.LONG);
		PrintXY( 0*6,2*8,(unsigned char *)" IWW  :",selectb==0?1:0);
		PrintXY( 0*6,3*8,(unsigned char *)" IWRWD:",selectb==1?1:0);
		PrintXY( 0*6,4*8,(unsigned char *)" IWRWS:",selectb==2?1:0);
		PrintXY( 0*6,5*8,(unsigned char *)" IWRRD:",selectb==3?1:0);
		PrintXY( 0*6,6*8,(unsigned char *)" IWRRS:",selectb==4?1:0);
		PrintXY( 7*6,2*8,(unsigned char *)waitw[IWW6a],  IWW6a  ==3?0:1);
		PrintXY( 7*6,3*8,(unsigned char *)waits[IWRWD6a],IWRWD6a==3?0:1);
		PrintXY( 7*6,4*8,(unsigned char *)waits[IWRWS6a],IWRWS6a==3?0:1);
		PrintXY( 7*6,5*8,(unsigned char *)waits[IWRRD6a],IWRRD6a==3?0:1);
		PrintXY( 7*6,6*8,(unsigned char *)waitw[IWRRS6a],IWRRS6a==3?0:1);
		
	    locate(12,1);	Print((unsigned char *)"CS6BBCR");
		HexPrintXY(12,2,"",BSC.CS6BBCR.LONG);
		PrintXY(10*6,2*8,(unsigned char *)" IWW  :",selectb==10?1:0);
		PrintXY(10*6,3*8,(unsigned char *)" IWRWD:",selectb==11?1:0);
		PrintXY(10*6,4*8,(unsigned char *)" IWRWS:",selectb==12?1:0);
		PrintXY(10*6,5*8,(unsigned char *)" IWRRD:",selectb==13?1:0);
		PrintXY(10*6,6*8,(unsigned char *)" IWRRS:",selectb==14?1:0);
		PrintXY(17*6,2*8,(unsigned char *)waitw[IWW6b],  IWW6b  ==3?0:1);
		PrintXY(17*6,3*8,(unsigned char *)waits[IWRWD6b],IWRWD6b==3?0:1);
		PrintXY(17*6,4*8,(unsigned char *)waits[IWRWS6b],IWRWS6b==3?0:1);
		PrintXY(17*6,5*8,(unsigned char *)waits[IWRRD6b],IWRRD6b==3?0:1);
		PrintXY(17*6,6*8,(unsigned char *)waitw[IWRRS6b],IWRRS6b==3?0:1);

		if ( selectb < 10 ) { locate( 1,selectb+3   );Print((unsigned char *)points); }
					else    { locate(11,selectb+3-10);Print((unsigned char *)points); }

		Fkey_dispN(0," +  ");
		Fkey_dispN(1," -  ");
		Fkey_dispN(3,"Init");

		Bdisp_PutDisp_DD();
		
		GetKey( &key );
        switch (key) {
            case KEY_CTRL_AC:
            	flag=0;
                break;
            case KEY_CTRL_EXIT:
            	flag=0;
                break;

			case KEY_CTRL_UP:
				selectb-=1;
				if (selectb == -1) selectb=0;
				if (selectb ==  9) selectb=10;
				break;
			case KEY_CTRL_DOWN:
				selectb+=1;
				if (selectb == 5) selectb=4;
				if (selectb ==15) selectb=14;
				break;
			case KEY_CTRL_RIGHT:
				if (selectb < 10 ) selectb+=10;
				break;
			case KEY_CTRL_LEFT:
				if (selectb >= 10 ) selectb-=10;
				break;
				
			case KEY_CTRL_F4:
				BSC.CS6ABCR.LONG = CS6aBCR_default ;
				BSC.CS6BBCR.LONG = CS6bBCR_default ;
				break;
        
			case KEY_CHAR_PLUS:
			case KEY_CTRL_F1:		//	+
				switch (selectb) {
					case 0:	// IWW6a
						if( IWW6a >= 3 ) break;
						IWW6a+=1;
						BSC.CS6ABCR.BIT.IWW = IWW6a ;
						break;
					case 1:	// IWRWD6a
						if( IWRWD6a >= 3 ) break;
						IWRWD6a+=1;
						BSC.CS6ABCR.BIT.IWRWD = IWRWD6a ;
						break;
					case 2:	// IWRWS6a
						if( IWRWS6a >= 3 ) break;
						IWRWS6a+=1;
						BSC.CS6ABCR.BIT.IWRWS = IWRWS6a ;
						break;
					case 3:	// IWRRD6a
						if( IWRRD6a >= 3 ) break;
						IWRRD6a+=1;
						BSC.CS6ABCR.BIT.IWRRD = IWRRD6a ;
						break;
					case 4:	// IWRRS6a
						if( IWRRS6a >= 3 ) break;
						IWRRS6a+=1;
						BSC.CS6ABCR.BIT.IWRRS = IWRRS6a ;
						break;
						
					case 10:	// IWW6b
						if( IWW6b >= 3 ) break;
						IWW6b+=1;
						BSC.CS6BBCR.BIT.IWW = IWW6b ;
						break;
					case 11:	// IWRWD6b
						if( IWRWD6b >= 3 ) break;
						IWRWD6b+=1;
						BSC.CS6BBCR.BIT.IWRWD = IWRWD6b ;
						break;
					case 12:	// IWRWS6b
						if( IWRWS6b >= 3 ) break;
						IWRWS6b+=1;
						BSC.CS6BBCR.BIT.IWRWS = IWRWS6b ;
						break;
					case 13:	// IWRRD6b
						if( IWRRD6b >= 3 ) break;
						IWRRD6b+=1;
						BSC.CS6BBCR.BIT.IWRRD = IWRRD6b ;
						break;
					case 14:	// IWRRS6b
						if( IWRRS6b >= 3 ) break;
						IWRRS6b+=1;
						BSC.CS6BBCR.BIT.IWRRS = IWRRS6b ;
						break;
				}
				break;
				
			case KEY_CHAR_MINUS:
			case KEY_CTRL_F2:		//		-
				switch (selectb) {
					case 0:	// IWW6a
						if( IWW6a <= 0 ) break;
						IWW6a-=1;
						BSC.CS6ABCR.BIT.IWW = IWW6a ;
						break;
					case 1:	// IWRWD6a
						if( IWRWD6a <= 0 ) break;
						IWRWD6a-=1;
						BSC.CS6ABCR.BIT.IWRWD = IWRWD6a ;
						break;
					case 2:	// IWRWS6a
						if( IWRWS6a <= 0 ) break;
						IWRWS6a-=1;
						BSC.CS6ABCR.BIT.IWRWS = IWRWS6a ;
						break;
					case 3:	// IWRRD6a
						if( IWRRD6a <= 0 ) break;
						IWRRD6a-=1;
						BSC.CS6ABCR.BIT.IWRRD = IWRRD6a ;
						break;
					case 4:	// IWRRS6a
						if( IWRRS6a <= 0 ) break;
						IWRRS6a-=1;
						BSC.CS6ABCR.BIT.IWRRS = IWRRS6a ;
						break;
						
					case 10:	// IWW6b
						if( IWW6b <= 0 ) break;
						IWW6b-=1;
						BSC.CS6BBCR.BIT.IWW = IWW6b ;
						break;
					case 11:	// IWRWD6b
						if( IWRWD6b <= 0 ) break;
						IWRWD6b-=1;
						BSC.CS6BBCR.BIT.IWRWD = IWRWD6b ;
						break;
					case 12:	// IWRWS6b
						if( IWRWS6b <= 0 ) break;
						IWRWS6b-=1;
						BSC.CS6BBCR.BIT.IWRWS = IWRWS6b ;
						break;
					case 13:	// IWRRD6b
						if( IWRRD6b <= 0 ) break;
						IWRRD6b-=1;
						BSC.CS6BBCR.BIT.IWRRD = IWRRD6b ;
						break;
					case 14:	// IWRRS6b
						if( IWRRS6b <= 0 ) break;
						IWRRS6b-=1;
						BSC.CS6BBCR.BIT.IWRRS = IWRRS6b ;
						break;
				}
				break;

        }
    }
}
#endif

//---------------------------------------------------------------------------------------------
char *wwhws[]  ={"0.5 ","1.5 ","2.5 ","3.5 "};
char *waitW[]  ={"=WR"," 0 "," 1 "," 2 "," 3 "," 4 "," 5 "," 6 "};
char *waitR[]  ={" 0 "," 1 "," 2 "," 3 "," 4 "," 5 "," 6 "," 8 ","10 ","12 ","14 ","18 ","24 "};

void dsp_WCR_mod(int freq){
	int	WW0,SW0,WR0,HW0;
	int	WW2,SW2,WR2,HW2;
	unsigned int key;
	int	flag=1;
	int selectb=10;

    while(flag) {

		WW0  = BSC.CS0WCR.BIT.WW ;
		SW0  = BSC.CS0WCR.BIT.SW ;
		HW0  = BSC.CS0WCR.BIT.HW ;
		WR0  = BSC.CS0WCR.BIT.WR ;
		
		WW2  = BSC.CS2WCR.BIT.WW ;
		SW2  = BSC.CS2WCR.BIT.SW ;
		HW2  = BSC.CS2WCR.BIT.HW ;
		WR2  = BSC.CS2WCR.BIT.WR ;

	    Bdisp_AllClr_VRAM();
	    locate( 2,1);	Print((unsigned char *)"ROM:CS0");
		HexPrintXY(2,2,"",BSC.CS0WCR.LONG);
		PrintXY( 0*6,2*8,(unsigned char *)" WW :",selectb==0?1:0);
		PrintXY( 0*6,3*8,(unsigned char *)" SW :",selectb==1?1:0);
		PrintXY( 0*6,4*8,(unsigned char *)" HW :",selectb==2?1:0);
		PrintXY( 0*6,5*8,(unsigned char *)" WR :",selectb==3?1:0);
		PrintXY( 5*6,2*8,(unsigned char *)waitW[WW0],  WW0==0     ?0:1);
		PrintXY( 5*6,3*8,(unsigned char *)wwhws[SW0],  SW0==0     ?0:1);
		PrintXY( 5*6,4*8,(unsigned char *)wwhws[HW0],  HW0==0     ?0:1);
		PrintXY( 5*6,5*8,(unsigned char *)waitR[WR0],  WR0==WAIT_2?0:1);
		
	    locate(12,1);	Print((unsigned char *)"RAM:CS2");
		HexPrintXY(12,2,"",BSC.CS2WCR.LONG);
		PrintXY(10*6,2*8,(unsigned char *)" WW :",selectb==10?1:0);
		PrintXY(10*6,3*8,(unsigned char *)" SW :",selectb==11?1:0);
		PrintXY(10*6,4*8,(unsigned char *)" HW :",selectb==12?1:0);
		PrintXY(10*6,5*8,(unsigned char *)" WR :",selectb==13?1:0);
		PrintXY(15*6,2*8,(unsigned char *)waitW[WW2],  WW2==0     ?0:1);
		PrintXY(15*6,3*8,(unsigned char *)wwhws[SW2],  SW2==0     ?0:1);
		PrintXY(15*6,4*8,(unsigned char *)wwhws[HW2],  HW2==0     ?0:1);
		PrintXY(15*6,5*8,(unsigned char *)waitR[WR2],  WR2==WAIT_2?0:1);

		if ( selectb < 10 ) { locate( 1,selectb+3   );Print((unsigned char *)points); }
					else    { locate(11,selectb+3-10);Print((unsigned char *)points); }

		Fkey_dispN(0," + ");
		Fkey_dispN(1," - ");
		
		Bdisp_PutDisp_DD();
		
		GetKey( &key );
//       Sleep(50);
        switch (key) {
            case KEY_CTRL_AC:
            	flag=0;
                break;
            case KEY_CTRL_EXIT:
            	flag=0;
                break;
        
			case KEY_CTRL_UP:
				selectb-=1;
				if (selectb ==  0) selectb= 1;
				if (selectb ==  9) selectb=10;
				break;
			case KEY_CTRL_DOWN:
				selectb+=1;
				if (selectb == 4) selectb=3;
				if (selectb ==14) selectb=13;
				break;
			case KEY_CTRL_RIGHT:
				if (selectb < 10 ) selectb+=10;
				break;
			case KEY_CTRL_LEFT:
				if (selectb == 10 ) break;
				if (selectb >= 10 ) selectb-=10;
				break;
				
			case KEY_CHAR_PLUS:
			case KEY_CTRL_F1:		//	+
				switch (selectb) {
					case 1:	// SW0
						if( SW0 >= 3 ) break;
						SW0+=1;
						BSC.CS0WCR.BIT.SW = SW0 ;
						break;
					case 2:	// HW0
						if( HW0 >= 3 ) break;
						HW0+=1;
						BSC.CS0WCR.BIT.HW = HW0 ;
						break;
					case 3:	// WR0
						if( WR0 >= WAIT24 ) break;
						WR0+=1;
						BSC.CS0WCR.BIT.WR = WR0 ;
						break;
						
					case 10:	// WW2
						WW2+=1;
						if( WW2 > 7 ) WW2=0;
						BSC.CS2WCR.BIT.WW = WW2 ;
						break;
					case 11:	// SW2
						if( SW2 >= 3 ) break;
						SW2+=1;
						BSC.CS2WCR.BIT.SW = SW2 ;
						break;
					case 12:	// HW2
						if( HW2 >= 3 ) break;
						HW2+=1;
						BSC.CS2WCR.BIT.HW = HW2 ;
						break;
					case 13:	// WR2
						if( WR2 >= WAIT24 ) break;
						WR2+=1;
						BSC.CS2WCR.BIT.WR = WR2 ;
						break;
				}
				break;
				
			case KEY_CHAR_MINUS:
			case KEY_CTRL_F2:		//		-
				switch (selectb) {
					case 1:	// SW0
						if( SW0 <= 0 ) break;
						SW0-=1;
						BSC.CS0WCR.BIT.SW = SW0 ;
						break;
					case 2:	// HW0
						if( HW0 <= 0 ) break;
						HW0-=1;
						BSC.CS0WCR.BIT.HW = HW0 ;
						break;
					case 3:	// WR0
						if( WR0 <= 0 ) break;
						WR0-=1;
						BSC.CS0WCR.BIT.WR = WR0 ;
						break;
						
					case 10:	// WW2
						WW2-=1;
						if( WW2 < 1 ) WW2=1;
						BSC.CS2WCR.BIT.WW = WW2 ;
						break;
					case 11:	// SW2
						if( SW2 <= 0 ) break;
						SW2-=1;
						BSC.CS2WCR.BIT.SW = SW2 ;
						break;
					case 12:	// HW2
						if( HW2 <= 0 ) break;
						HW2-=1;
						BSC.CS2WCR.BIT.HW = HW2 ;
						break;
					case 13:	// WR2
						if( WR2 <= 0 ) break;
						WR2-=1;
						BSC.CS2WCR.BIT.WR = WR2 ;
						break;
				}
				break;

        }
    }
}

//---------------------------------------------------------------------------------------------
#ifdef OPTION_REG
void dsp_WCR34_mod(int freq){
	int	WW3,SW3,WR3,HW3;
	int	WW4,SW4,WR4,HW4;
	unsigned int key;
	int	flag=1;
	int selectb=1;

    while(flag) {

		WW3  = BSC.CS3WCR.BIT.WW ;
		SW3  = BSC.CS3WCR.BIT.SW ;
		HW3  = BSC.CS3WCR.BIT.HW ;
		WR3  = BSC.CS3WCR.BIT.WR ;
		
		WW4  = BSC.CS4WCR.BIT.WW ;
		SW4  = BSC.CS4WCR.BIT.SW ;
		HW4  = BSC.CS4WCR.BIT.HW ;
		WR4  = BSC.CS4WCR.BIT.WR ;

	    Bdisp_AllClr_VRAM();
	    locate( 2,1);	Print((unsigned char *)"CS3WCR");
		HexPrintXY(2,2,"",BSC.CS3WCR.LONG);
		PrintXY( 0*6,2*8,(unsigned char *)" WW :",selectb==0?1:0);
		PrintXY( 0*6,3*8,(unsigned char *)" SW :",selectb==1?1:0);
		PrintXY( 0*6,4*8,(unsigned char *)" HW :",selectb==2?1:0);
		PrintXY( 0*6,5*8,(unsigned char *)" WR :",selectb==3?1:0);
		PrintXY( 5*6,2*8,(unsigned char *)waitW[WW3],  WW3==0     ?0:1);
		PrintXY( 5*6,3*8,(unsigned char *)wwhws[SW3],  SW3==0     ?0:1);
		PrintXY( 5*6,4*8,(unsigned char *)wwhws[HW3],  HW3==0     ?0:1);
		PrintXY( 5*6,5*8,(unsigned char *)waitR[WR3],  WR3==WAIT14?0:1);
		
	    locate(12,1);	Print((unsigned char *)"CS4WCR");
		HexPrintXY(12,2,"",BSC.CS4WCR.LONG);
		PrintXY(10*6,2*8,(unsigned char *)" WW :",selectb==10?1:0);
		PrintXY(10*6,3*8,(unsigned char *)" SW :",selectb==11?1:0);
		PrintXY(10*6,4*8,(unsigned char *)" HW :",selectb==12?1:0);
		PrintXY(10*6,5*8,(unsigned char *)" WR :",selectb==13?1:0);
		PrintXY(15*6,2*8,(unsigned char *)waitW[WW4],  WW4==0     ?0:1);
		PrintXY(15*6,3*8,(unsigned char *)wwhws[SW4],  SW4==0     ?0:1);
		PrintXY(15*6,4*8,(unsigned char *)wwhws[HW4],  HW4==0     ?0:1);
		PrintXY(15*6,5*8,(unsigned char *)waitR[WR4],  WR4==WAIT_2?0:1);

		if ( selectb < 10 ) { locate( 1,selectb+3   );Print((unsigned char *)points); }
					else    { locate(11,selectb+3-10);Print((unsigned char *)points); }

		Fkey_dispN(0," + ");
		Fkey_dispN(1," - ");
		Fkey_dispN(3,"Init");
		
		Bdisp_PutDisp_DD();
		
		GetKey( &key );
//       Sleep(50);
        switch (key) {
            case KEY_CTRL_AC:
            	flag=0;
                break;
            case KEY_CTRL_EXIT:
            	flag=0;
                break;
        
			case KEY_CTRL_UP:
				selectb-=1;
				if (selectb == -1) selectb= 0;
				if (selectb ==  9) selectb=10;
				break;
			case KEY_CTRL_DOWN:
				selectb+=1;
				if (selectb == 4) selectb=3;
				if (selectb ==14) selectb=13;
				break;
			case KEY_CTRL_RIGHT:
				if (selectb < 10 ) selectb+=10;
				break;
			case KEY_CTRL_LEFT:
				if (selectb >= 10 ) selectb-=10;
				break;
				
			case KEY_CTRL_F4:
				BSC.CS3WCR.LONG = CS3WCR_default ;
				BSC.CS4WCR.LONG = CS4WCR_default ;
				break;

			case KEY_CHAR_PLUS:
			case KEY_CTRL_F1:		//	+
				switch (selectb) {
					case 0:	// WW3
						if( WW3 >= 7 ) break;
						WW3+=1;
						BSC.CS3WCR.BIT.WW = WW3 ;
						break;
					case 1:	// SW3
						if( SW3 >= 3 ) break;
						SW3+=1;
						BSC.CS3WCR.BIT.SW = SW3 ;
						break;
					case 2:	// HW3
						if( HW3 >= 3 ) break;
						HW3+=1;
						BSC.CS3WCR.BIT.HW = HW3 ;
						break;
					case 3:	// WR3
						if( WR3 >= WAIT24 ) break;
						WR3+=1;
						BSC.CS3WCR.BIT.WR = WR3 ;
						break;
						
					case 10:	// WW4
						WW4+=1;
						if( WW4 > 7 ) WW4=0;
						BSC.CS4WCR.BIT.WW = WW4 ;
						break;
					case 11:	// SW4
						if( SW4 >= 3 ) break;
						SW4+=1;
						BSC.CS4WCR.BIT.SW = SW4 ;
						break;
					case 12:	// HW4
						if( HW4 >= 3 ) break;
						HW4+=1;
						BSC.CS4WCR.BIT.HW = HW4 ;
						break;
					case 13:	// WR4
						if( WR4 >= WAIT24 ) break;
						WR4+=1;
						BSC.CS4WCR.BIT.WR = WR4 ;
						break;
				}
				break;
				
			case KEY_CHAR_MINUS:
			case KEY_CTRL_F2:		//		-
				switch (selectb) {
					case 0:	// WW3
						if( WW3 <= 0 ) break;
						WW3-=1;
						BSC.CS3WCR.BIT.WW = WW3 ;
						break;
					case 1:	// SW3
						if( SW3 <= 0 ) break;
						SW3-=1;
						BSC.CS3WCR.BIT.SW = SW3 ;
						break;
					case 2:	// HW3
						if( HW3 <= 0 ) break;
						HW3-=1;
						BSC.CS3WCR.BIT.HW = HW3 ;
						break;
					case 3:	// WR3
						if( WR3 <= 0 ) break;
						WR3-=1;
						BSC.CS3WCR.BIT.WR = WR3 ;
						break;
						
					case 10:	// WW4
						if( WW4 <= 0 ) break;
						WW4-=1;
						BSC.CS4WCR.BIT.WW = WW4 ;
						break;
					case 11:	// SW4
						if( SW4 <= 0 ) break;
						SW4-=1;
						BSC.CS4WCR.BIT.SW = SW4 ;
						break;
					case 12:	// HW4
						if( HW4 <= 0 ) break;
						HW4-=1;
						BSC.CS4WCR.BIT.HW = HW4 ;
						break;
					case 13:	// WR4
						if( WR4 <= 0 ) break;
						WR4-=1;
						BSC.CS4WCR.BIT.WR = WR4 ;
						break;
				}
				break;

        }
    }
}
#endif

//---------------------------------------------------------------------------------------------
void dsp_WCR5_mod(int freq){
	int	WW5a,SW5a,WR5a,HW5a;
	int	WW5b,SW5b,WR5b,HW5b;
	unsigned int key;
	int	flag=1;
	int selectb=0;

    while(flag) {

		WW5a  = BSC.CS5AWCR.BIT.WW ;
		SW5a  = BSC.CS5AWCR.BIT.SW ;
		HW5a  = BSC.CS5AWCR.BIT.HW ;
		WR5a  = BSC.CS5AWCR.BIT.WR ;
		
		WW5b  = BSC.CS5BWCR.BIT.WW ;
		SW5b  = BSC.CS5BWCR.BIT.SW ;
		HW5b  = BSC.CS5BWCR.BIT.HW ;
		WR5b  = BSC.CS5BWCR.BIT.WR ;

	    Bdisp_AllClr_VRAM();
	    locate( 2,1);	Print((unsigned char *)"CS5AWCR");
		HexPrintXY(2,2,"",BSC.CS5AWCR.LONG);
		PrintXY( 0*6,2*8,(unsigned char *)" WW :",selectb==0?1:0);
		PrintXY( 0*6,3*8,(unsigned char *)" SW :",selectb==1?1:0);
		PrintXY( 0*6,4*8,(unsigned char *)" HW :",selectb==2?1:0);
		PrintXY( 0*6,5*8,(unsigned char *)" WR :",selectb==3?1:0);
		PrintXY( 5*6,2*8,(unsigned char *)waitW[WW5a],  WW5a==0     ?0:1);
		PrintXY( 5*6,3*8,(unsigned char *)wwhws[SW5a],  SW5a==1     ?0:1);
		PrintXY( 5*6,4*8,(unsigned char *)wwhws[HW5a],  HW5a==1     ?0:1);
		PrintXY( 5*6,5*8,(unsigned char *)waitR[WR5a],  WR5a==WAIT14?0:1);
		
	    locate(12,1);	Print((unsigned char *)"CS5BWCR");
		HexPrintXY(12,2,"",BSC.CS5BWCR.LONG);
		PrintXY(10*6,2*8,(unsigned char *)" WW :",selectb==10?1:0);
		PrintXY(10*6,3*8,(unsigned char *)" SW :",selectb==11?1:0);
		PrintXY(10*6,4*8,(unsigned char *)" HW :",selectb==12?1:0);
		PrintXY(10*6,5*8,(unsigned char *)" WR :",selectb==13?1:0);
		PrintXY(15*6,2*8,(unsigned char *)waitW[WW5b],  WW5b==0     ?0:1);
		PrintXY(15*6,3*8,(unsigned char *)wwhws[SW5b],  SW5b==1     ?0:1);
		PrintXY(15*6,4*8,(unsigned char *)wwhws[HW5b],  HW5b==1     ?0:1);
		PrintXY(15*6,5*8,(unsigned char *)waitR[WR5b],  WR5b==WAIT14?0:1);

		if ( selectb < 10 ) { locate( 1,selectb+3   );Print((unsigned char *)points); }
					else    { locate(11,selectb+3-10);Print((unsigned char *)points); }

		Fkey_dispN(0," + ");
		Fkey_dispN(1," - ");
		Fkey_dispN(3,"Init");
		
		Bdisp_PutDisp_DD();
		
		GetKey( &key );
        switch (key) {
            case KEY_CTRL_AC:
            	flag=0;
                break;
            case KEY_CTRL_EXIT:
            	flag=0;
                break;
        
			case KEY_CTRL_UP:
				selectb-=1;
				if (selectb == -1) selectb= 0;
				if (selectb ==  9) selectb=10;
				break;
			case KEY_CTRL_DOWN:
				selectb+=1;
				if (selectb == 4) selectb=3;
				if (selectb ==14) selectb=13;
				break;
			case KEY_CTRL_RIGHT:
				if (selectb < 10 ) selectb+=10;
				break;
			case KEY_CTRL_LEFT:
				if (selectb >= 10 ) selectb-=10;
				break;
				
			case KEY_CTRL_F4:
				BSC.CS5AWCR.LONG = CS5aWCR_default ;
				BSC.CS5BWCR.LONG = CS5bWCR_default ;
				break;

			case KEY_CHAR_PLUS:
			case KEY_CTRL_F1:		//	+
				switch (selectb) {
					case 0:	// WW5a
						WW5a+=1;
						if( WW5a > 7 ) WW5a=0;
						BSC.CS5AWCR.BIT.WW = WW5a ;
						break;
					case 1:	// SW5a
						if( SW5a >= 3 ) break;
						SW5a+=1;
						BSC.CS5AWCR.BIT.SW = SW5a ;
						break;
					case 2:	// HW5a
						if( HW5a >= 3 ) break;
						HW5a+=1;
						BSC.CS5AWCR.BIT.HW = HW5a ;
						break;
					case 3:	// WR5a
						if( WR5a >= WAIT24 ) break;
						WR5a+=1;
						BSC.CS5AWCR.BIT.WR = WR5a ;
						break;
						
					case 10:	// WW5b
						WW5b+=1;
						if( WW5b > 7 ) WW5b=0;
						BSC.CS5BWCR.BIT.WW = WW5b ;
						break;
					case 11:	// SW5b
						if( SW5b >= 3 ) break;
						SW5b+=1;
						BSC.CS5BWCR.BIT.SW = SW5b ;
						break;
					case 12:	// HW5b
						if( HW5b >= 3 ) break;
						HW5b+=1;
						BSC.CS5BWCR.BIT.HW = HW5b ;
						break;
					case 13:	// WR5b
						if( WR5b >= WAIT24 ) break;
						WR5b+=1;
						BSC.CS5BWCR.BIT.WR = WR5b ;
						break;
				}
				break;
				
			case KEY_CHAR_MINUS:
			case KEY_CTRL_F2:		//		-
				switch (selectb) {
					case 0:	// WW5a
						if( WW5a <= 0 ) break;
						WW5a-=1;
						BSC.CS5AWCR.BIT.WW = WW5a ;
						break;
					case 1:	// SW5a
						if( SW5a <= 0 ) break;
						SW5a-=1;
						BSC.CS5AWCR.BIT.SW = SW5a ;
						break;
					case 2:	// HW5a
						if( HW5a <= 0 ) break;
						HW5a-=1;
						BSC.CS5AWCR.BIT.HW = HW5a ;
						break;
					case 3:	// WR5a
						if( WR5a <= 0 ) break;
						WR5a-=1;
						BSC.CS5AWCR.BIT.WR = WR5a ;
						break;
						
					case 10:	// WW5b
						if( WW5b <= 0 ) break;
						WW5b-=1;
						BSC.CS5BWCR.BIT.WW = WW5b ;
						break;
					case 11:	// SW5b
						if( SW5b <= 0 ) break;
						SW5b-=1;
						BSC.CS5BWCR.BIT.SW = SW5b ;
						break;
					case 12:	// HW5b
						if( HW5b <= 0 ) break;
						HW5b-=1;
						BSC.CS5BWCR.BIT.HW = HW5b ;
						break;
					case 13:	// WR5b
						if( WR5b <= 0 ) break;
						WR5b-=1;
						BSC.CS5BWCR.BIT.WR = WR5b ;
						break;
				}
				break;

        }
    }
}

//---------------------------------------------------------------------------------------------
#ifdef OPTION_REG
void dsp_WCR6_mod(int freq){
	int	WW6a,SW6a,WR6a,HW6a;
	int	WW6b,SW6b,WR6b,HW6b;
	unsigned int key;
	int	flag=1;
	int selectb=0;

    while(flag) {

		WW6a  = BSC.CS6AWCR.BIT.WW ;
		SW6a  = BSC.CS6AWCR.BIT.SW ;
		HW6a  = BSC.CS6AWCR.BIT.HW ;
		WR6a  = BSC.CS6AWCR.BIT.WR ;
		
		WW6b  = BSC.CS6BWCR.BIT.WW ;
		SW6b  = BSC.CS6BWCR.BIT.SW ;
		HW6b  = BSC.CS6BWCR.BIT.HW ;
		WR6b  = BSC.CS6BWCR.BIT.WR ;

	    Bdisp_AllClr_VRAM();
	    locate( 2,1);	Print((unsigned char *)"CS6AWCR");
		HexPrintXY(2,2,"",BSC.CS6AWCR.LONG);
		PrintXY( 0*6,2*8,(unsigned char *)" WW :",selectb==0?1:0);
		PrintXY( 0*6,3*8,(unsigned char *)" SW :",selectb==1?1:0);
		PrintXY( 0*6,4*8,(unsigned char *)" HW :",selectb==2?1:0);
		PrintXY( 0*6,5*8,(unsigned char *)" WR :",selectb==3?1:0);
		PrintXY( 5*6,2*8,(unsigned char *)waitW[WW6a],  WW6a==0     ?0:1);
		PrintXY( 5*6,3*8,(unsigned char *)wwhws[SW6a],  SW6a==0     ?0:1);
		PrintXY( 5*6,4*8,(unsigned char *)wwhws[HW6a],  HW6a==0     ?0:1);
		PrintXY( 5*6,5*8,(unsigned char *)waitR[WR6a],  WR6a==WAIT14?0:1);
		
	    locate(12,1);	Print((unsigned char *)"CS6BWCR");
		HexPrintXY(12,2,"",BSC.CS6BWCR.LONG);
		PrintXY(10*6,2*8,(unsigned char *)" WW :",selectb==10?1:0);
		PrintXY(10*6,3*8,(unsigned char *)" SW :",selectb==11?1:0);
		PrintXY(10*6,4*8,(unsigned char *)" HW :",selectb==12?1:0);
		PrintXY(10*6,5*8,(unsigned char *)" WR :",selectb==13?1:0);
		PrintXY(15*6,2*8,(unsigned char *)waitW[WW6b],  WW6b==0     ?0:1);
		PrintXY(15*6,3*8,(unsigned char *)wwhws[SW6b],  SW6b==0     ?0:1);
		PrintXY(15*6,4*8,(unsigned char *)wwhws[HW6b],  HW6b==0     ?0:1);
		PrintXY(15*6,5*8,(unsigned char *)waitR[WR6b],  WR6b==WAIT14?0:1);

		if ( selectb < 10 ) { locate( 1,selectb+3   );Print((unsigned char *)points); }
					else    { locate(11,selectb+3-10);Print((unsigned char *)points); }

		Fkey_dispN(0," + ");
		Fkey_dispN(1," - ");
		Fkey_dispN(3,"Init");
		
		Bdisp_PutDisp_DD();
		
		GetKey( &key );
        switch (key) {
            case KEY_CTRL_AC:
            	flag=0;
                break;
            case KEY_CTRL_EXIT:
            	flag=0;
                break;
        
			case KEY_CTRL_UP:
				selectb-=1;
				if (selectb == -1) selectb= 0;
				if (selectb ==  9) selectb=10;
				break;
			case KEY_CTRL_DOWN:
				selectb+=1;
				if (selectb == 4) selectb=3;
				if (selectb ==14) selectb=13;
				break;
			case KEY_CTRL_RIGHT:
				if (selectb < 10 ) selectb+=10;
				break;
			case KEY_CTRL_LEFT:
				if (selectb >= 10 ) selectb-=10;
				break;
				
			case KEY_CTRL_F4:
				BSC.CS6AWCR.LONG = CS6aWCR_default ;
				BSC.CS6BWCR.LONG = CS6bWCR_default ;
				break;

			case KEY_CHAR_PLUS:
			case KEY_CTRL_F1:		//	+
				switch (selectb) {
					case 0:	// WW6a
						WW6a+=1;
						if( WW6a > 7 ) WW6a=0;
						BSC.CS6AWCR.BIT.WW = WW6a ;
						break;
					case 1:	// SW6a
						if( SW6a >= 3 ) break;
						SW6a+=1;
						BSC.CS6AWCR.BIT.SW = SW6a ;
						break;
					case 2:	// HW6a
						if( HW6a >= 3 ) break;
						HW6a+=1;
						BSC.CS6AWCR.BIT.HW = HW6a ;
						break;
					case 3:	// WR6a
						if( WR6a >= WAIT24 ) break;
						WR6a+=1;
						BSC.CS6AWCR.BIT.WR = WR6a ;
						break;
						
					case 10:	// WW6b
						WW6b+=1;
						if( WW6b > 7 ) WW6b=0;
						BSC.CS6BWCR.BIT.WW = WW6b ;
						break;
					case 11:	// SW6b
						if( SW6b >= 3 ) break;
						SW6b+=1;
						BSC.CS6BWCR.BIT.SW = SW6b ;
						break;
					case 12:	// HW6b
						if( HW6b >= 3 ) break;
						HW6b+=1;
						BSC.CS6BWCR.BIT.HW = HW6b ;
						break;
					case 13:	// WR6b
						if( WR6b >= WAIT24 ) break;
						WR6b+=1;
						BSC.CS6BWCR.BIT.WR = WR6b ;
						break;
				}
				break;
				
			case KEY_CHAR_MINUS:
			case KEY_CTRL_F2:		//		-
				switch (selectb) {
					case 0:	// WW6a
						if( WW6a <= 0 ) break;
						WW6a-=1;
						BSC.CS6AWCR.BIT.WW = WW6a ;
						break;
					case 1:	// SW6a
						if( SW6a <= 0 ) break;
						SW6a-=1;
						BSC.CS6AWCR.BIT.SW = SW6a ;
						break;
					case 2:	// HW6a
						if( HW6a <= 0 ) break;
						HW6a-=1;
						BSC.CS6AWCR.BIT.HW = HW6a ;
						break;
					case 3:	// WR6a
						if( WR6a <= 0 ) break;
						WR6a-=1;
						BSC.CS6AWCR.BIT.WR = WR6a ;
						break;
						
					case 10:	// WW6b
						if( WW6b <= 0 ) break;
						WW6b-=1;
						BSC.CS6BWCR.BIT.WW = WW6b ;
						break;
					case 11:	// SW6b
						if( SW6b <= 0 ) break;
						SW6b-=1;
						BSC.CS6BWCR.BIT.SW = SW6b ;
						break;
					case 12:	// HW6b
						if( HW6b <= 0 ) break;
						HW6b-=1;
						BSC.CS6BWCR.BIT.HW = HW6b ;
						break;
					case 13:	// WR6b
						if( WR6b <= 0 ) break;
						WR6b-=1;
						BSC.CS6BWCR.BIT.WR = WR6b ;
						break;
				}
				break;

        }
    }
}
#endif

