#ifndef __BCM2711_REG_H
#define __BCM2711_REG_H

#define GPIO_BASE_ADDRESS   0x7e200000
#define GPIO_TOTAL_BYTES_LENGTH   0xf4
#define GPIO_FSEL0          0x00
#define GPIO_FSEL1          0x04
#define GPIO_FSEL2          0x08
#define GPIO_FSEL3          0x0c
#define GPIO_FSEL4          0x10
#define GPIO_FSEL5          0x14
#define GPIO_SET0           0x1c
#define GPIO_SET1           0x20
#define GPIO_CLR0           0x28
#define GPIO_CLR1           0x2c
#define GPIO_LEV0           0x34
#define GPIO_LEV1           0x38

#define GPIO_F_INPUT        0x00
#define GPIO_F_OUTPUT       0x01
#define GPIO_F_ALT0         0x02
#define GPIO_F_ALT1         0x03
#define GPIO_F_ALT2         0x04
#define GPIO_F_ALT3         0x05
#define GPIO_F_ALT4         0x06
#define GPIO_F_ALT5         0x07

// BA       - Byte Adress
// POSITION - bit position. 0 ~ 7
#define SET(BA,POSITION)     *(BA) |= 0x1 << (POSITION)
#define RESET(BA,POSITION)   *(BA) &= ~(0x1 << (POSITION))
#define GET(BA,POSITION)     ((*(BA) >> POSITION) & 0x1)

#define NO_ERROR 0
#define UNKNOWN_ERROR -1

extern volatile int* alt_reg;
extern volatile int* set0_reg;
extern volatile int* set1_reg;
extern volatile int* clr0_reg;
extern volatile int* clr1_reg;
extern volatile int* read0_reg;
extern volatile int* read1_reg;

int gpio_reg_init_bcm2711(void);
void gpio_reg_clear(void);
void gpio_set_output_mode(int gpio_no);
void gpio_set_input_mode(int gpio_no);
void gpio_set(int gpio_no);
void gpio_reset(int gpio_no);
int gpio_read(int gpio_no);

#endif