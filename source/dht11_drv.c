#include <linux/module.h> /* Needed by all modules */

#include <linux/kernel.h> /* Needed for KERN_ALERT */

#include <linux/init.h> /*Needed for __init */

#include <linux/io.h>
#include <linux/time.h>
#include <linux/timer.h>
#include <linux/version.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <bcm2711_reg.h>
#include <dht11.h>

MODULE_AUTHOR("HJY");
MODULE_DESCRIPTION("module of dht11 driver for raspi 4B");
MODULE_LICENSE("GPL");
MODULE_VERSION("V1.0.0");

static struct timer_list tml;
struct timeval oldtv;
u64 _start_ticks_;
u_char* dht_11_read(void);
int total = 0,suc = 0;

// 定义定时回调
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,14,0)
void callback(unsinged long arg)
[

]
#else
void callback(struct timer_list* tml)
{
    u_char* dht_11_buffer =  dht_11_read();
    if ( dht_11_buffer != NULL)
    {
        total++;
        suc++;
        printk(KERN_INFO"H : %u.%u \t\t T : %u.%u \n",dht_11_buffer[0],dht_11_buffer[1],dht_11_buffer[2],dht_11_buffer[3]);
    }
    else
    {
        total++;
    }
    printk(KERN_INFO"Status : total - %d,suc - %d \n", total , suc);
    mod_timer(tml,jiffies_64 + 1*HZ);
}
#endif

// 注册定时器
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,14,0)
void set_my_timer(struct timer_list *tml,unsigned long expire_ticks,void callback(unsigned long arg))
{
    init_timer(tml);
    tml->function = callback;
    tml->expire = expire_ticks;
    add_timer(tml);
}
#else
void set_my_timer(struct timer_list *tml,unsigned long expire_ticks,void callback(struct timer_list*))
{
    timer_setup(tml,callback,0);
    mod_timer(tml,expire_ticks);
    _start_ticks_=jiffies;
}
#endif


static int __init test_init(void){
    gpio_reg_init_bcm2711();
    set_my_timer(&tml,jiffies_64+1*HZ,callback);
    return 0;
}


static void __exit test_exit(void){
    gpio_reg_clear();
    del_timer(&tml);
}

module_init(test_init);

module_exit(test_exit);