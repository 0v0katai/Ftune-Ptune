===============================================================================

 Ftune3 is SH7305 CPG&BSC tuning utility for fx-9860GIII / Graph 35+E II     v2.40

 copyright(c)2014/2015-2020 by sentaro21
 e-mail sentaro21@pm.matrix.jp

===============================================================================

PRIZM and fx-9860GIII  has been used is SH4A CPU called SH7305.
It is similar to SH7724 (by RENESASS).

This tool modify CPG(Clock Pulse Generator) and BSC(Bus State Controller).

features:
 Your Calculator to the world of over 200MHz!!
 memory speed test automatically.
 freq/wait settings store and recall with the function key.
 Save setting to file (main memory). load data automatically on the next run.

weak points:
 As for this tool, test is insufficient.
 Unexpected malfunction may happen.
 Battery consumptions considerably increase at the over clock.

====== Warning!!! =============================================================
 This tool is made to work safely, but unknown malfunction may happen.
 This tool can cause damage on your calculator!
 Use it at your own risk.
 will not be responsible for any damage.
===============================================================================

-------------------------------------------------------------------------------
CPG(Clock Pulse Generator)                               quote: SH7724 datasheet
-------------------------------------------------------------------------------
        FLL: FLL circuit Multiplication Ratio.
             The FLL circuit multiples the clock frequency(32.768KHz) input from the RTC_CLK.
             default  multiplication rate is 900.
             FLL circuit output  32768Hz*900/2=14.7456MHz
             this is same as old SH3 model oscillatory frequency.

        PLL: PLL circuit Multiplication Ratio.
             base frequency 14.7456MHz at FLL output
             Multiplication Ratio is 1-64
             It becomes half speed when about over 800MHz.
             (PLL Clock is up to about 750MHz)

        IFC: CPU Clock Division Ratio.
             default Division Ratio is 1/8 at 29.49MHz.

        SFC: SHW Clock Division Ratio.
             default Division Ratio is 1/8 at 29.49MHz.

        BFC: bus Clock Division Ratio.
             default Division Ratio is 1/8 at 29.49MHz.
             USB cannot recognize that bus clocks over about 130MHz.

        PFC: I/O Clock Division Ratio.
             default Division Ratio is 1/16 at 14.75MHz.
             Even 40MHz or more can work, but influence a note of the flash memory.
             It is safe to set to frequency near as possible to a default.(about less than 30MHz)

                Division Ratio  (not same to  SH7724)
                0000: 1/2
                0001: 1/4
                0010: 1/8       IFC,SFC,BFC default
                0011: 1/16      PFC default
                0100: 1/32
                0101: 1/64
        Note.
        IFC >= SFC >= BFC >= PFC
        IFC:SFC  only  1:1 1:2
        this tool adjust automatically.


-------------------------------------------------------------------------------
BSC(Bus State Controller)                                quote: SH7724 datasheet
-------------------------------------------------------------------------------
        register structure for the BCR/WCR similar to SH7724.

        CS0BCR, CS0WCR  : FLASH ROM area
        CS2BCR, CS2WCR  : main  RAM area
        CS5ABCR,CS5AWCR : LCDC
        others unknown.


        CSn Space Bus Contorl Register (CSnBCR) (n=0,2,3,4,5A,5B,6A,6B)

        IWW: Specification for Idol Cycles between Write-Read/Write-Write Cycles.
                000: no idol cycle
                001: 1 idol cycle inserted
                010: 2 idol cycles inserted
                011: 4 idol cycles inserted
                100: 6 idol cycles inserted
                101: 8 idol cycles inserted
                110: 10 idol cycles inserted
                111: 12idol cycles inserted

                (CS0BCR,CS2BCR)@SH7305 default value 2 idol cycles.
                lower frequency can modify more lower setteing.
                it work well effectively.

        IWRWD: Specification for Idol Cycles between Read-Write Cycles in Different Spaces.
                000: no idol cycle
                001: 1 idol cycle inserted
                010: 2 idol cycles inserted
                011: 4 idol cycles inserted
                100: 6 idol cycles inserted
                101: 8 idol cycles inserted
                110: 10 idol cycles inserted
                111: 12idol cycles inserted

                (CS0BCR,CS2BCR)@SH7305 default value 2 idol cycles.
                lower frequency can modify more lower setteing.
                it work well. but an effect is not felt.

        IWRWS: Specification for Idol Cycles between Read-Write Cycles in the Same Spaces.
                000: no idol cycle
                001: 1 idol cycle inserted
                010: 2 idol cycles inserted
                011: 4 idol cycles inserted
                100: 6 idol cycles inserted
                101: 8 idol cycles inserted
                110: 10 idol cycles inserted
                111: 12idol cycles inserted

                (CS0BCR,CS2BCR)@SH7305 default value 2 idol cycles.
                lower frequency can modify more lower setteing.
                it work well. but an effect is not felt.

        IWRRD: Specification for Idol Cycles between Read-Read Cycles in Different Spaces.
                000: no idol cycle
                001: 1 idol cycle inserted
                010: 2 idol cycles inserted
                011: 4 idol cycles inserted
                100: 6 idol cycles inserted
                101: 8 idol cycles inserted
                110: 10 idol cycles inserted
                111: 12idol cycles inserted

                (CS0BCR,CS2BCR)@SH7305 default value 2 idol cycles.
                lower frequency can modify more lower setteing.
                it work well. but an effect is not felt.

        IWRRS:  Specification for Idol Cycles between Read-Read Cycles in the Same Spaces.
                000: no idol cycle
                001: 1 idol cycle inserted
                010: 2 idol cycles inserted
                011: 4 idol cycles inserted
                100: 6 idol cycles inserted
                101: 8 idol cycles inserted
                110: 10 idol cycles inserted
                111: 12idol cycles inserted

                (CS0BCR,CS2BCR)@SH7305 default value 2 idol cycles.
                lower frequency can modify more lower setteing.
                it work well effectively.



        CSn Space Wait Contorl Register (CSnWCR) (n=0,2,3,4,5A,5B,6A,6B)

        WW: Number of Wait Cycles in Write Access
                000: The same cycles as WR settings
                001: 0 cycle
                010: 1 cycle
                011: 2 cycles
                100: 3 cycles
                101: 4 cycles
                110: 5 cycles
                111: 6 cycles

                (CS0WCR,CS2WCR)@SH7305 default value is the same cycles as WR settings.
                I think that it is not necessary to change it.
                but,set by independence becomes fast in the memory access.
                CS5AWCR work well effectively. (When abnormality happens to LCD)

        WR: Number of Wait Cycles in Read Access
                0000: 0 cycle           1000: 10 cycles
                0001: 1 cycle           1001: 12 cycles
                0010: 2 cycle           1010: 14 cycles
                0011: 3 cycles          1011: 18 cycles
                0100: 4 cycles          1100: 24 cycles
                0101: 5 cycles
                0110: 6 cycles
                0111: 8 cycles

                CS0WCR@PRIZM     default value is  3 cycles
                CS0WCR@9860GII-2 default value is 18 cycles
                if increment it, and more bus frequency.

                CS2WCR@SH7305 default value is 2 cycles
                if increment it, and more bus frequency.


        SW: Numer of Delay Cycles from Address,CSn Assertion to RD,WE Assertion.
                00: 0.5cycle
                01: 1.5cycles
                10: 2.5cycles
                11: 3.5cycles
                (CS0WCR,CS2WCR)@SH7305 default value is 0.5cycle.


        HW: Delay Cycles from RD WEn Negation to Address,CSn Negation.
                00: 0.5cycle
                01: 1.5cycles
                10: 2.5cycles
                11: 3.5cycles
                (CS0WCR,CS2WCR)@SH7305 default value is 0.5cycle.


-------------------------------------------------------------------------------
Memory bus frequency limits CPU core frequency by default setting.

 fx-9860GII-2 model ROM 18wait   RAM 2wait  (default)
   ROM frequency up to 180MHz by default (18 wait)
   RAM frequency up to about 70MHz-75MHz (at unknown SRAM chip)

 This tool can modify wait states,
 if increment, improved more memory bus frequency.
 and more calculator performance.

 CPU Clock up to 265-300MHz (safty about 260MHz)
 SHW Clock up to 175-200MHz (safty about 160MHz)
 bus Clock up to 175-200MHz (safty about 120MHz)
 I/O Clock up to    - 40MHz (safty about  30MHz)
(The limits are different by individual difference.)

 Core clock is effective in a small program.
 Overall performance is influenced by bus clock.

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
 IFC:1/4  CPU      * 58.98MHz
 SFC:1/8  RoR 3    * 29.49MHz
 BFC:1/8  RaR 2    * 29.49MHz
 PFC:/16  RaW =R   * 14.75MHz
messeage area / benchmark score
[function key]
-----------------------------------

RoR: Number of ROM Wait Cycles in Read  Access.   color reverse: dangerous range.
RaR: Number of RAM Wait Cycles in Read  Access.   color reverse: dangerous range.
RaW: Number of RAM Wait Cycles in Write Access.   color reverse: dangerous range.

-------------------------------------------------------------------------------

-[UP]           select up   (FLL,PLL,SFC,BFC,PFC)
-[DOWN]         select down (FLL,PLL,SFC,BFC,PFC)

-[LEFT]         decrement 1 step multiplication or divider
-[RIGHT]        increment 1 step multiplication or divider

-[SHIFT]+[UP]   select FLL multiplication ( =>setup select )

-[F1]   Load normal default setting      CPU  29MHz, PLLx16, bus  29MHz, default  wait
-[F2]   Recall [F2] setting       preset CPU  59MHz, PLLx16, bus  29MHz, same as Prizm default
-[F3]   Recall [F3] setting       preset CPU  29MHz, PLLx16, bus  29MHz, same as SH3 model
-[F4]   Recall [F4] setting       preset CPU 118MHz, PLLx32, bus  59MHz, modified wait
-[F5]   Recall [F5] setting       preset CPU 236MHz, PLLx32, bus 118MHz, modified wait
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
    -[F5]       ROM & RAM / RAM WRITE & READ table chenge.
                The limit frequency of the RAM does not increase from a certain spot,
                but there becomes the limit of the SHW clock & bus clock.
    -[F6]       Auto check for memory speed in each of wait.

-[VARS]
    -[F1]       register display  FRQCR,CCR
    -[F2]       register display  BCR/WCR toggle

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

    -[F5]       modify CS5ABCR CS5BBCR
                select cursor key
        -[F1]   +
        -[F2]   -
        -[F4]   initialize default setting

    -[F6]       modify CS5AWCR CS5BWCR
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

-[+]            increment RAM wait
-[-]            decrement RAM wait
                When it may be lowered, memory check begins
                if error,cannot decrement.

-[SHIFT]+[+]    increment RAM WW wait  "=R" same cycles as WR settings
-[SHIFT]+[-]    decrement RAM WW wait  "=R" same cycles as WR settings

-[SETUP]                              default setting
        ROM Wait margin     0-15%       3%      ( less than 1% is dangerous. ) not reset wait table.
        RAM Wait margin     0-15%       3%      ( less than 1% is dangerous. ) not reset wait table.
        PLL frequency MAX              800MHz
        CPU frequency MAX              285MHz
        Shw frequency MAX              180MHz
        Bus frequency MAX              134MHz
        I/O frequency MAX               30MHz
        Startup mem check      on/off   on
        F1   yes/no check      on/off   off
        Wait Auto decrement    on/off   on
        RAM WW  Auto inc/dec   on/off   on
        ROM IWW Auto decrement on/off   on
        PFC  Auto increment    on/off   on
        FLL display noshift    on/off   off
        BATT volt display      on/off   off

-------------------------------------------------------------------------------
Auto memory check
-------------------------------------------------------------------------------
It is an important point of this tool to measure speed of the memory.
At first,look for the slowest 64K block.
and check the upper limit in each wait.
Check it by performing a reading of the same address twice whether it is not different.
It is a read error if different.
the measurement of the write speed tests,
measure and acquire a speed difference revision level first.
read and write in a memory area of 4KB.

When with a USB power supply, 1%-3% become quick in the memory speed.
Please measure it in battery.

-------------------------------------------------------------------------------
Caution
-------------------------------------------------------------------------------
USB cannot recognize that bus clocks over about 130MHz.
I do not know the measures of this.

RS232C depends on the I/O clock.
When you communicate in two, please put I/O clocks together.

In the prizm,
when bus clocks over about 130MHz-150MHz, a screen collapses.
Please modify CS5ABCR or CS5AWCR.
  increment IWW (CS5ABCR) or increment WW,SW,HW (CS5AWCR)
So it can be settled.
but turn off and turn on it do not return to a previous state. ( return default )

and bult-in overclocking games,
will freeze surely when operate it in PLL 26x or more.

-------------------------------------------------------------------------------
Acknowledgements
-------------------------------------------------------------------------------
This program was based on Pover(by Ashbad).
developed by PrizmSDK 0.3(Ptune2) & CASIO SDK(Ftune2).

fxReverse project documentation,
SuperH-based fx calculators,
Cemetech WikiPrizm,
served as a reference very much.

Very thanks for result of the analysis of all senior users.

I would be happy if this tool is useful for you.

-------------------------------------------------------------------------------
LICENCE
-------------------------------------------------------------------------------
This software is freeware.
The license follows GPLv2.

-------------------------------------------------------------------------------
v2.40   2020.2.20       Fixed the bug that the battery voltage display occured system error in the emulator.
			Changed "X" display to multiplication symbol.

v2.30   2019.2.11       Newly supported fx-9860GIII/9750GIII.(only display)

v2.20   2019.8.31       Corrected the value of battery voltage display.

v2.02   2019.7.13       In the case of Graph 35+E II, fixed a bug that causes the screen receiver transfer error if the bus clock exceeds about 58 MHz.

v2.01   2019.7.12       In the case of Graph 35+E II, if the bus clock exceeds about 80 MHz, the LCD transfer will fail, so the default settings for [F4] and [F5] have been optimized.
                        And, the setting of CS5AWCR:SW is automatically changed by the rise of the bus clock.

v2.00   2019.7.7        Added support Graph 35+E II.
                        Supported models are judged automatically.
                        And it has been renamed to Ftune3.

v1.11a  2017.10.20      changed memory test value. ( When memory test error is given in previous version, please try this version.)

v1.11   2016. 7.31      corrected version display errors. (1.10->1.11)
        2016. 7.21      Changed memory test including a hidden RAM area.(upper 256KB)

v1.10   2016. 4.24      It is not required re-measurement when changed memory wait margin setting by setup.
                        Change default memory speed frequency table.

v1.02   2015. 6.20      Check the CPU type at first.
                        It does not start when it is SH3.
                        changed some icon designs to be easy to distinguish it from Ftune(SH3)

v1.01   2015. 2.28      To solve problems in memtest failure.(When PLL limit is low, and memory limit is high.)
                        renewed some source codes. as a result, file size decreased.

v1.00b  2014.12.21      fixed benchmark result of CPU.(adjustment alignment)
                        RAM write safety zone (reverse dsp) 90%->95%.
                        default preset change  F3:same as SH3.

v1.00a  2014.11.27      There is not any change operatively.
                        only fixed careless miss...

v1.00   2014.11.14      first release


