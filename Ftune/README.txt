===============================================================================

 Ftune is SH7337/SH7355 CPG&BSC tuning utility for fx-9860G/Slim/GII series  v1.40

 copyright(c)2014/2015-2020 by sentaro21
 e-mail sentaro21@pm.matrix.jp

===============================================================================

fx-9860G/GII series  has been used is SH3 CPU called SH7337/SH7355.
It is similar to SH7705 (by RENESASS).

This tool modify CPG(Clock Pulse Generator) and BSC(Bus State Controller).

features:
 This tool can modify wait states,
 if decrement, improved more calculator performance.
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
CPG(Clock Pulse Generator)                               quote: SH7705 datasheet
-------------------------------------------------------------------------------
same as SH7705
It is thought that Clock Opereting Modes is mode 5.

PLL(STC)        PLL circuit Multiplication Ratio.
                base frequency 14.7456MHz at X'tal output.
                PLL circuit generates 29.4912MHz that doubled X'tal output.
                It is memory bus clock.
                default multiplication is x1.

IFC             Internal Clock Division Ratio.
                00:1/1  default
                01:1/2
                10:1/3
                11:1/4
                This is CPU Clock.

PFC             I/O Clock Division Ratio.
                00:1/1
                01:1/2  default
                10:1/3
                11:1/4

-------------------------------------------------------------------------------
BSC(Bus State Controller)                                quote: SH7705 datasheet
-------------------------------------------------------------------------------
        register structure for the BCR/WCR similar to SH7724.

        CS0BCR, CS0WCR  : FLASH ROM area
        CS2BCR, CS2WCR  : main  RAM area
        CS5ABCR,CS5AWCR : LCDC
        others unknown.


        CSn Space Bus Contorl Register (CSnBCR) (n=0,2,3,4,5A,5B,6A,6B)

        IWW: Specification for Idol Cycles between Write-Read/Write-Write Cycles.
                00: no idol cycle
                01: 1 idol cycle inserted
                10: 2 idol cycles inserted
                11: 4 idol cycles inserted

                (CS0BCR,CS2BCR)@SH7355 default value 0 idol cycles.
                lower frequency can modify more lower setteing.
                it work well effectively.

        IWRWD: Specification for Idol Cycles between Read-Write Cycles in Different Spaces.
                00: 1 idol cycle inserted
                01: 2 idol cycles inserted
                10: 3 idol cycles inserted
                11: 4 idol cycles inserted

                (CS0BCR,CS2BCR)@SH7355 default value 2 idol cycles.
                lower frequency can modify more lower setteing.
                it work well. but an effect is not felt.

        IWRWS: Specification for Idol Cycles between Read-Write Cycles in the Same Spaces.
                00: 1 idol cycle inserted
                01: 2 idol cycles inserted
                10: 3 idol cycles inserted
                11: 4 idol cycles inserted

                (CS0BCR,CS2BCR)@SH7355 default value 2 idol cycles.
                lower frequency can modify more lower setteing.
                it work well. but an effect is not felt.

        IWRRD: Specification for Idol Cycles between Read-Read Cycles in Different Spaces.
                00: 1 idol cycle inserted
                01: 2 idol cycles inserted
                10: 3 idol cycles inserted
                11: 4 idol cycles inserted

                (CS0BCR,CS2BCR)@SH7355 default value 2 idol cycles.
                lower frequency can modify more lower setteing.
                it work well. but an effect is not felt.

        IWRRS:  Specification for Idol Cycles between Read-Read Cycles in the Same Spaces.
                00: no idol cycle
                01: 1 idol cycle inserted
                10: 2 idol cycles inserted
                11: 4 idol cycles inserted

                (CS0BCR,CS2BCR)@SH7355 default value 0 idol cycles.
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

                (CS0WCR,CS2WCR)@SH7355 default value is the same cycles as WR settings.
                I think that it is not necessary to change it.
                but,set by independence becomes fast in the memory access.

        WR: Number of Wait Cycles in Read Access
                0000: 0 cycle           1000: 10 cycles
                0001: 1 cycle           1001: 12 cycles
                0010: 2 cycle           1010: 14 cycles
                0011: 3 cycles          1011: 18 cycles
                0100: 4 cycles          1100: 24 cycles
                0101: 5 cycles
                0110: 6 cycles
                0111: 8 cycles
                (CS0WCR,CS2WCR)@SH7355 default value is 2 cycles


        SW: Numer of Delay Cycles from Address,CSn Assertion to RD,WE Assertion.
                00: 0.5cycle
                01: 1.5cycles
                10: 2.5cycles
                11: 3.5cycles
                (CS0WCR,CS2WCR)@SH7355 default value is 0.5cycle.


        HW: Delay Cycles from RD WEn Negation to Address,CSn Negation.
                00: 0.5cycle
                01: 1.5cycles
                10: 2.5cycles
                11: 3.5cycles
                (CS0WCR,CS2WCR)@SH7355 default value is 0.5cycle.

-------------------------------------------------------------------------------
Memory bus frequency limits CPU core frequency by default setting.
 fx-9860G/GII series ROM   2wait   RAM 2wait  (default)

 This tool can modify wait states,
 if decrement, improved more calculator performance.

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
Ftune for SH3           5.05v (Battery voltage display)
 PLL:x1            * 29.49MHz
 IFC:1/1  CPU      * 29.49MHz
          RoR 2
 BFC:1/1  RaR 2    * 29.49MHz
 PFC:1/2  RaW =R   * 14.75MHz
messeage area / benchmark score
[function key]
-----------------------------------

RoR: Number of ROM Wait Cycles in Read  Access.   color reverse: chenged default.
RaR: Number of RAM Wait Cycles in Read  Access.   color reverse: chenged default.
RaW: Number of RAM Wait Cycles in Write Access.   color reverse: chenged default.

-------------------------------------------------------------------------------

-[UP]           select up   (PLL,IFC,PFC)
-[DOWN]         select down (PLL,IFC,PFC)

-[LEFT]         decrement 1 step multiplication or divider
-[RIGHT]        increment 1 step multiplication or divider

-[F1]   Load normal default setting      CPU  29MHz, PLLx1, default  wait
-[F2]   Recall [F2] setting       preset CPU  59MHz, PLLx2, default  wait
-[F3]   Recall [F3] setting       preset CPU  88MHz, PLLx3, default  wait
-[F4]   Recall [F4] setting       preset CPU 118MHz, PLLx4, default  wait
-[F5]   Recall [F5] setting       preset CPU 118MHz, PLLx4, modified wait
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

-[SETUP]        version display

-------------------------------------------------------------------------------
Caution
-------------------------------------------------------------------------------
RS232C depends on the I/O clock.
When you communicate in two, please put I/O clocks together.

A screen may collapse by screen capture via the USB when it becomes the high clock.
Please increase wait value of IWW of the RAM.

-------------------------------------------------------------------------------
Acknowledgements
-------------------------------------------------------------------------------
This program was based on UTIL( by Mario Roy) and imitated Ftune2( by sentaro21).
developed by CASIO SDK.

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
v1.40   2020. 2.20      Fixed the bug that can not work in the emulator. (1.20 enbug)
			Changed "X" display to multiplication symbol.

v1.20   2019. 8.31      Added battery voltage display at right top corner.
                        (Thanks to SimonLothar's sample program.)

v1.02   2015. 6.19      Check the CPU type at first.
                        It does not start when it is SH4A.

v1.01   2015. 2.28      renewed some source codes. as a result, file size decreased.
                        There is no functional change.

v1.00b  2014.12.21      fixed emulator not working.
                        benchmark address of I/O changed (0xA44C0014).

v1.00a  2014.11.27      fixed bug (freeze in X3 from X2 at IFC X2).
                        fixed careless miss...

v1.00   2014.11.14      first release

