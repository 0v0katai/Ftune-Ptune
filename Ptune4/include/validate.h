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

unsigned int best_rom_wait(i32 Bphi_f);
unsigned int best_ram_read(i32 Bphi_f);
unsigned int best_ram_write(i32 Bphi_f);
unsigned int best_TRC(i32 Bphi_f);
int TRC_equivalent(u8 reg_value);
bool auto_up_PFC();
bool auto_down_PFC();
bool exceed_limit();
