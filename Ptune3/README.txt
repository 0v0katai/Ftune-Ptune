===============================================================================

 Ptune3 is SH7305 CPG&BSC&DBSC tuning utility for PRIZM fx-CG50/Graph 90+E  v0.24

 copyright(c)2014/2015/2016/2017/2018/2019/2020 by sentaro21
 e-mail sentaro21@pm.matrix.jp

===============================================================================

It is about the same with Ptune2.
But cannot operate SDRAM timing well.
That still hasn't been solved.

###############################################################################
#As for the current version, only fx-CG50/Graph 90+E supports.                #
#This does not work in CG10/20.                                               #
###############################################################################

====== Warning!!! =============================================================
 This tool is made to work safely, but unknown malfunction may happen.
 This tool can cause damage on your calculator!
 Use it at your own risk.
 will not be responsible for any damage.
===============================================================================

-------------------------------------------------------------------------------
How to use
-------------------------------------------------------------------------------
Main screen
----------------------------------
 FLL:x900          * 14.75MHz
 PLL:x16           *235.93MHz
 IFC:1/2  CPU      *117.96MHz
 SFC:1/4  roR 8    * 58.98MHz
 BFC:1/4  CL  2    * 58.98MHz
 PFC:1/8           * 29.49MHz
messeage area / benchmark score
[function key]
-----------------------------------

roR: Number of ROM Wait Cycles in Read  Access.   color red: dangerous range.
CL : Number of SDRAM CL
-------------------------------------------------------------------------------

-[UP]           select up   (FLL,PLL,SFC,BFC,PFC)
-[DOWN]         select down (FLL,PLL,SFC,BFC,PFC)

-[LEFT]         decrement 1 step multiplication or divider
-[RIGHT]        increment 1 step multiplication or divider

-[SHIFT]+[UP]   select FLL multiplication ( =>setup select )

-[F1]   Load normal default setting      CPU 119MHz, PLLx16, bus  59MHz, default
-[F2]   Recall [F2] setting       preset CPU  59MHz, PLLx16, bus  29MHz, same as CG10/20
-[F3]   Recall [F3] setting       preset CPU  96MHz, PLLx26, bus  48MHz, same as CG10/20 overclock (Pover)
-[F4]   Recall [F4] setting       preset CPU 236MHz, PLLx32, bus  58MHz, only overclock CPU
-[F5]   Recall [F5] setting       preset CPU 192MHz, PLLx26, bus  95MHz, PLL x16 -> x26
-[F6]   simple benchmark
        CPUcore simple loop count per 100ms. ( 9860G add-in "UTIL" like )
        and MEMORY(ROM,RAM,I/O) access loop count per 50ms
        toggle more
        CPUcore and PutDsipDD speed (fps)

-[SHIFT]
    -[F1]       Save setting to file (main memory). load data automatically on the next run.
    -[F2]       Store current setting to function KEY [F2]
    -[F3]       Store current setting to function KEY [F3]
    -[F4]       Store current setting to function KEY [F4]
    -[F5]       Store current setting to function KEY [F5]
    -[F6]       Load setting from file (main memory)

-[OPTN]         List of bus frequency limit value table in each of wait.
    -[F4]       initialize default setting
    -[F5]       Only SDRAM Test
    -[F6]       Auto check for memory speed in each of wait.(ROM only)

-[VARS]
    -[F1]       register display  FRQCR,CCR
    -[F2]       register display  DBSC (SDRAM)

    -[F3]       modify CS0BCR CS2BCR
                select cursor key
        -[F1]   +
        -[F2]   -
        -[F4]   initialize default setting

    -[F4]       modify CS0WCR CS2WCR
                select cursor key
        -[F1]   +
        -[F2]   -
        -[F4]   initialize default setting

    -[F5]       modify CS3BCR CS4BCR
                select cursor key
        -[F1]   +
        -[F2]   -
        -[F4]   initialize default setting

    -[F6]       modify CS3WCR CS4WCR
                select cursor key
        -[F1]   +
        -[F2]   -
        -[F4]   initialize default setting

-[PRGM]
    -[F1]       modify CS3BCR CS4BCR
    -[F2]       modify CS3WCR CS4WCR
    -[F3]       modify CS5ABCR CS5BBCR
    -[F4]       modify CS5AWCR CS4BWCR
    -[F5]       modify CS6ABCR CS6BBCR
    -[F6]       modify CS6AWCR CS6BWCR

-[EXIT]         exit

-[AC]           default menu screen

-[EXE]          if benchmark selected it is carried out once again.

-[*]            increment ROM wait
-[/]            decrement ROM wait
                When it may be lowered, memory check begins
                if error,cannot decrement.

-[X^2]
-[^]            toggled On/Off of the spread spectrum function.


-[SETUP]                              default setting
        ROM Wait margin     0-15%       5%      ( less than 1% is dangerous. ) not reset wait table.
        RAM Wait margin     0-15%       5%      ( less than 1% is dangerous. ) not reset wait table.
        PLL frequency MAX              800MHz
        CPU frequency MAX              275MHz
        Shw frequency MAX              150MHz
        Bus frequency MAX              100MHz
        I/O frequency MAX               50MHz
        Startup mem check      on/off   off
        F1   yes/no check      on/off   off
        Wait Auto decrement    on/off   on
        RAM WW  Auto inc/dec   on/off   on
        ROM IWW Auto decrement on/off   off
        PFC  Auto increment    on/off   on
        FLL display noshift    on/off   off
        BATT volt display      on/off   on
	Actual Freq            on/off   on

-------------------------------------------------------------------------------
ROM memory check
-------------------------------------------------------------------------------
It is an important point of this tool to measure speed of the memory.
At first,look for the slowest 64K block.
and check the upper limit in each wait.
Check it by performing a reading of the same address twice whether it is not different.
It is a read error if different.

-------------------------------------------------------------------------------
SDRAM memory check
-------------------------------------------------------------------------------
This version is test by current memory wait timing.
An error may occur after SDRAM memory test.
Please reset after SDRAM memory check even if you do not appear an error.

-------------------------------------------------------------------------------
Gap correspondence of the frequency				later ver.0.20
-------------------------------------------------------------------------------
Real running frequency agrees with calculated frequency from internal PLL in CG10/20,
but there seem to be approximately 1.6% low frequency in CG50/Graph90+E.
Therefore added real running frequency display from ver.0.20.
Real running frequency is unknown exactly,
The temporary correction freq that multiplied 900/914 by internal PLL frequency.
The correction frequency <-> internal PLL frequency can change by setup.

refer to
http://www.casiopeia.net/forum/viewtopic.php?f=25&t=7327

(ver.0.21-)
In  CG50/Graph90+E, the spread spectrum function of the clock genelator is enabled by default,
It was confirmed that the actual operating frequency is lower than the frequency calculated from the internal PLL.
After 0.21, the notation on the screen display will change if the spread spectrum function is enabled.

-------------------------------------------------------------------------------
Acknowledgements
-------------------------------------------------------------------------------
This program was based on Pover(by Ashbad).
developed by PrizmSDK 0.3(Ptune3) .

fxReverse project documentation,
SuperH-based fx calculators,
Cemetech WikiPrizm,
served as a reference very much.

http://www.casiopeia.net/forum/viewtopic.php?f=11&t=1756&p=20678#p14588
http://casiopeia.net/forum/viewtopic.php?f=2&t=1783&start=50
https://tiplanet.org/forum/viewtopic.php?p=215399#p215399
Very thanks for information from SimonLothar and TeamFX and Critor.

I would be happy if this tool is useful for you.

-------------------------------------------------------------------------------
LICENCE
-------------------------------------------------------------------------------
This software is freeware.
The license follows GPLv2.

-------------------------------------------------------------------------------
v0.24   2020.2.20      Changed "X" display to multiplication symbol.

v0.23   2020.1.3       Improved icons of function key.(designed by Colon)

v0.22   2019.8.29      Changed the value of the battery voltage up 9% in CG50/Graph90+E.

v0.21   2019.2.20      Added On/Off of the spread spectrum function.(by Simon's document)
                       On/Off is toggled with [X^2] or [^] key.

v0.20   2018.8.19      Change ROM/SDRAM test program.
        2018.8.18      Added perform accurate frequency correction.(can selected by setup)
		       (The temporary correction freq = internal freq * 900 / 914)


v0.10   2017.10.1      Change SDRAM test program by current setting.
                       Change Icon. ( CG50 type )

v0.05   2017.7.20      Change SDRAM test program,
                       Change Default setting.
                       Fixed not save bugs (CS3BCR,CS3WCR)

v0.04   2017.4.19      Change ROM test program,
                       Fixed ROM test bug again.( to expect....)
                       Back CS2WCR register structure. ( SH7730 -> SH7724 )

v0.03   2017.4.18      Fixed ROM test bug ( Couldn't do it.. )

v0.02   2017.4.17      Change CS2WCR,CS3WCR register structure. ( SH7724 -> SH7730 )
                       To be able to modifing SDRAM timing. ( There is not the certainty )
                       To limit PLL x33.

v0.01   2017.4.15      first release

