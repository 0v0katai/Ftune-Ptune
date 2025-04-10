#include <gint/mpu/bsc.h>

#define SH7305_SDMR3_CL2 ((volatile uint16_t *)0xfec15040)
#define SH7305_SDMR3_CL3 ((volatile uint16_t *)0xfec15060)

#define CL2 1
#define CL3 2

enum select_bwcr_option
{
    SELECT_CS0BCR = 0,
    SELECT_CS2BCR,
    SELECT_CS3BCR,
    SELECT_CS4BCR,
    SELECT_CS5ABCR,
    SELECT_CS5BBCR,
    SELECT_CS6ABCR,
    SELECT_CS6BBCR,
    SELECT_CS0WCR = 0,
    SELECT_CS2WCR,
    SELECT_CS3WCR,
    SELECT_CS4WCR,
    SELECT_CS5AWCR,
    SELECT_CS5BWCR,
    SELECT_CS6AWCR,
    SELECT_CS6BWCR
};

enum select_csnbcr_reg_option
{
    SELECT_IWW,
    SELECT_IWRWD,
    SELECT_IWRWS,
    SELECT_IWRRD,
    SELECT_IWRRS
};

enum select_csnwcr_reg_option
{
    SELECT_WW,
    SELECT_SW,
    SELECT_HW,
    SELECT_WR
};

enum select_cs3wcr_reg_option
{
    SELECT_TRP,
    SELECT_TRCD,
    SELECT_A3CL,
    SELECT_TRWL,
    SELECT_TRC
};

enum select_mode_option
{
    SELECT_BCR,
    SELECT_WCR
};

typedef byte_union(select_option,
                   u8 : 1;
                   u8 MODE : 1;
                   u8 CSn : 3;
                   u8 REG : 3;);
