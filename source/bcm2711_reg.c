#include<bcm2711_reg.h>
#include <linux/io.h>

volatile int* alt_reg;
volatile int* set0_reg;
volatile int* set1_reg;
volatile int* clr0_reg;
volatile int* clr1_reg;
volatile int* read0_reg;
volatile int* read1_reg;

int gpio_reg_init_bcm2711(void)
{
    int flag = 0;
    alt_reg = ioremap(0xfe200000+GPIO_FSEL0, 4 * 6);
    if(alt_reg == NULL){flag = 0;goto FAIL_PROCESS;} 
    set0_reg = ioremap(0xfe200000+GPIO_SET0, 4);
    if(set0_reg == NULL){flag = 1;goto FAIL_PROCESS;} 
    set1_reg = ioremap(0xfe200000+GPIO_SET1, 4);
    if(set1_reg == NULL){flag = 2;goto FAIL_PROCESS;} 
    clr0_reg = ioremap(0xfe200000+GPIO_CLR0, 4);
    if(clr0_reg == NULL){flag = 3;goto FAIL_PROCESS;} 
    clr1_reg = ioremap(0xfe200000+GPIO_CLR1, 4);
    if(clr1_reg == NULL){flag = 4;goto FAIL_PROCESS;} 
    read0_reg = ioremap(0xfe200000+GPIO_LEV0, 4);
    if(read0_reg == NULL){flag = 5;goto FAIL_PROCESS;} 
    read1_reg = ioremap(0xfe200000+GPIO_LEV1, 4);
    if(read1_reg == NULL){flag = 6;goto FAIL_PROCESS;} 
    return NO_ERROR;

FAIL_PROCESS:
    if(alt_reg != NULL) iounmap(alt_reg);    
    if(set0_reg != NULL) iounmap(set0_reg);    
    if(set1_reg != NULL) iounmap(set1_reg);    
    if(clr0_reg != NULL) iounmap(clr0_reg);    
    if(clr1_reg != NULL) iounmap(clr1_reg);    
    if(read0_reg != NULL) iounmap(read0_reg);    
    if(read1_reg != NULL) iounmap(read1_reg);    

    printk(KERN_ERR"Error occured. Gpio ioremap init failed. Flag is %d. \n",flag);
    return UNKNOWN_ERROR;
}
void gpio_reg_clear(void)
{
    if(alt_reg != NULL) iounmap(alt_reg);    
    if(set0_reg != NULL) iounmap(set0_reg);    
    if(set1_reg != NULL) iounmap(set1_reg);    
    if(clr0_reg != NULL) iounmap(clr0_reg);    
    if(clr1_reg != NULL) iounmap(clr1_reg);    
    if(read0_reg != NULL) iounmap(read0_reg);    
    if(read1_reg != NULL) iounmap(read1_reg); 
}

// gpio_no     - 0 ~ 57; before used,should call GPIO_IO_INIT()
void gpio_set_output_mode(int gpio_no)
{
    int reg_index = gpio_no / 10;
    int bit_index = gpio_no % 10 * 3;
    
    SET(alt_reg + reg_index ,bit_index);
    RESET(alt_reg + reg_index ,bit_index + 1);
    RESET(alt_reg + reg_index ,bit_index + 2);
    
}
// gpio_no     - 0 ~ 57; before used,should call GPIO_IO_INIT()
void gpio_set_input_mode(int gpio_no)
{
    int reg_index = gpio_no / 10;
    int bit_index = gpio_no % 10 * 3;
    RESET(alt_reg + reg_index ,bit_index);
    RESET(alt_reg + reg_index ,bit_index + 1);
    RESET(alt_reg + reg_index ,bit_index + 2);
}

// set gpio output to 1,if want to set gpio output to 0, should use gpio_reset.
void gpio_set(int gpio_no)
{
    if(gpio_no < 32)
        SET(set0_reg,gpio_no);
    else
        SET(set1_reg,gpio_no % 32);
}

// set gpio output to 0,if want to set gpio output to 1, should use gpio_set.
void gpio_reset(int gpio_no)
{
    if(gpio_no < 32)
        SET(clr0_reg,gpio_no);
    else
        SET(clr1_reg,gpio_no % 32);
}

// read gpio level.return 0 or 1;
int gpio_read(int gpio_no)
{
    if(gpio_no < 32)
        return GET(read0_reg,gpio_no);
    else
        return GET(read1_reg,gpio_no % 32);
}