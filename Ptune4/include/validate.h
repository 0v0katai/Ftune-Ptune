#define CPG SH7305_CPG
#define BSC SH7305_BSC

extern i32 settings[];

enum wait_bit
{
    WAIT_0, WAIT_1, WAIT_2, WAIT_3, WAIT_4,
    WAIT_5, WAIT_6, WAIT_8, WAIT_10,
	WAIT_12, WAIT_14, WAIT_18, WAIT_24
};

enum div_bit
{
    DIV_2, DIV_4, DIV_8, DIV_16, DIV_32, DIV_64
};

enum roR_table
{
    roR_0     = (16 - 2) * 1000000,
    roR_1     = (27 - 3) * 1000000,
    roR_2     = (38 - 2) * 1000000,
    roR_3     = (49 - 3) * 1000000,
    roR_4     = (60 - 3) * 1000000,
    roR_5     = (71 - 4) * 1000000,
    roR_6     = (82 - 4) * 1000000,
    roR_8     = (106 - 5) * 1000000,
    roR_10    = (125 - 6) * 1000000,
    roR_12    = (146 - 7) * 1000000,
    roR_14    = (166 - 8) * 1000000,
    roR_18    = (200 - 9) * 1000000
};

unsigned int best_rom_wait(i32 Bphi_f);
unsigned int best_TRC(i32 Bphi_f);
void init_roR();
bool auto_up_PFC();
bool auto_down_PFC();
void modify_A3CL(u8 value);
bool exceed_limit();