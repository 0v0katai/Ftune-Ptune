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

enum roR_default_table
{
    roR_0   = 14 * 1000000,
    roR_1   = 24 * 1000000,
    roR_2   = 35 * 1000000,
    roR_3   = 45 * 1000000,
    roR_4   = 56 * 1000000,
    roR_5   = 66 * 1000000,
    roR_6   = 77 * 1000000,
    roR_8   = 97 * 1000000,
    roR_10  = 115 * 1000000,
    roR_12  = 132 * 1000000,
    roR_14  = 158 * 1000000,
    roR_18  = 191 * 1000000
};

enum raR_default_table
{
    raR_0   = 14 * 1000000,
    raR_1   = 28 * 1000000,
    raR_2   = 49 * 1000000,
    raR_3   = 66 * 1000000,
    raR_4   = 83 * 1000000,
    raR_5   = 100 * 1000000,
    raR_6   = 117 * 1000000,
    raR_8   = 142 * 1000000
};

enum raW_default_table
{
    raW_0   = 28 * 1000000,
    raW_1   = 57 * 1000000,
    raW_2   = 85 * 1000000,
    raW_3   = 109 * 1000000,
    raW_4   = 137 * 1000000,
    raW_5   = 142 * 1000000,
    raW_6   = 147 * 1000000
};

enum raW_TRC_default_table
{
    raW_TRC_3   = 50 * 1000000,
    raW_TRC_4   = 100 * 1000000,
    raW_TRC_6   = 120 * 1000000,
    raW_TRC_9   = 130 * 1000000,
};

unsigned int best_rom_wait(i32 Bphi_f);
unsigned int best_ram_read(i32 Bphi_f);
unsigned int best_ram_write(i32 Bphi_f);
unsigned int best_TRC(i32 Bphi_f);
bool auto_up_PFC();
bool auto_down_PFC();
void modify_A3CL(u8 value);
bool exceed_limit();
