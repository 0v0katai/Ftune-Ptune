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

enum roR_10k_table
{
    roR_0     = 1600-200,
    roR_1     = 2700-300,
    roR_2     = 3800-200,
    roR_3     = 4900-250,
    roR_4     = 6000-300,
    roR_5     = 7100-350,
    roR_6     = 8200-400,
    roR_8     = 10600-500,
    roR_10    = 12500-600,
    roR_12    = 14600-700,
    roR_14    = 16600-800,
    roR_18    = 20000-900
};

enum raR_10k_table
{
    raR_0     = 1900-400,
    raR_1     = 3800-800,
    raR_2     = 5700-500,
    raR_3     = 7600-600,
    raR_4     = 9500-900,
    raR_5     = 11400-900,
    raR_6     = 13300-900,
    raR_8     = 10100-0000,
    raR_10    = 10000-0000
};

// void change_FLL(const i8 value);
// void change_PLL(const i8 value);
unsigned int best_rom_wait(u32 Bphi_f);
void init_roR(u32 *roR_data);
void up_roR_IWW();
void down_roR_IWW();
void up_BFC();
void up_PFC();
void auto_up_PFC();
void auto_down_PFC();
bool exceed_limit();