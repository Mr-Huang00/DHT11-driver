#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/delay.h>
#include <bcm2711_reg.h>

#define DHT11 16
static u_char dht_11_buffer[5];

// 请求DHT11数据并获取到内存中。
// 下述过程，假定了芯片指令执行时间几乎为0和DHT11的响应时间几乎为0，同时也忽略传输延时。

// 题外话：
// DHT11有很多毛病，例如：
//  1. DHT11长时间接收低电平初始化或者通电太久等原因，会导致输出电平时间会紊乱，尤其是数据的0时间会变得特别长，甚至高达40us以上，导致数据判断出错。
//  2. DHT11即使刚通电没多久，数据'0'输出的时长也有较低概率超过40us。
// PS : 偶尔，DHT11能实现完美开机，进入超稳定状态模式，此时电平的输出时间异常准确。
// 为了避免上述问题，应该定期统计DHT11的数据正确率，当正确率低于某个阈值时，就要重启DHT11。
// 吐槽 ： ~其实经过实际操作才会知道，DHT11并没有对电平输出时间做出高精度控制，仅数据输出部分做出了底限度的精度控制。~
//
// (同时当Linux进行使用占用资源的前台操作时，定时时长会被延长，使得定时不准确，导致数据读取成功率会大幅度降低。该问题的解决办法一般是设置进程优先度，如果是定时操作，则可以设置中断处理高优先。)

// use gpio16
u_char* dht_11_read(void)
{
    memset( dht_11_buffer,0,5);
    //通知DHT11工作开始，即Linux系统向DHT11发出初始化信号
    //设置DHT11为输出
    //输出低电平，18ms以上
    //输出高电平，20-40us

    gpio_set_output_mode(DHT11);
    SET(clr0_reg,16);
    mdelay(20);
    SET(set0_reg,16);
    udelay(30);

    //DHT11工作开始，并发送确认信号到Linux系统，让Linux系统准备接收。
    //设置DHT11为输入状态
    //等待DHT11输入低电平，长80us
    //等待DHT11输入高电平，长80us
    gpio_set_input_mode(DHT11);

    // DHT11响应初始化信号所需时间很短，设为输入态后，应可立刻接收低电平信号。
    // 我猜测:只需维持足够时间（约20ms）的低电平输出，DHT11就会完成初始化，并向Linux系统输入低电平，此时，先将GPIO的引脚挂成高电平，然后将状态设为输入态的话，GPIO的值将几乎瞬间反转为DHT11输入的低电平态。
    // 相反，如果设为输入态后，依然没有被反转为低电平并维持足够长的时间（约80us）则表示初始化失败。

    // 维持时间长度的判定通过定时查询实现，电平维持时间不低于26us，故采取 26us/2 以下的查询间隔即可，此处选择5us。
    // （如果是裸机程序就直接晶振跳数计算）

    // 题外话：
    // 实际，考虑Linux系统的‘看不见’的延迟的话，在Linux上计算的时间间隔ΔT实际表示的时间含义Δtt肯定会大于实际时间间隔Δt。
    // 令Δtt = Δt，则ΔT < Δt。（其中Δtt = ΔT + System_Delay）
    // 
    // 查询统计时间间隔的判定点应该取较低的值，例如统计20us，则Linux中统计小于20us的值，如18us。
    // 具体考虑多小视实际情况：统计的间隔越长，累计的系统延时越大；系统越繁忙，系统延时累计速率越大。


    if(! gpio_read(DHT11))
    {
        int delay_counter = 0;
        // 在预定的80us左右输入低电平，表示初始化完成
        while(!gpio_read(DHT11))
        {
            udelay(5);
            delay_counter++;
            if(delay_counter > 20)
            {
                printk(KERN_INFO"DHT11 init fail .  low level interval is too long. %d\n",delay_counter);
                return NULL;
            }
        }

        // 如果DHT11能如同它说明书那样输出指定波形，那么应该加上最小值判定。
        // 然而事实上。DHT11有些时候会莫名其妙返回一个长度只有1单位delay_counter(5us左右)的响应，这既有可能是DHT11系统结构不稳定导致的也有可能是Linux的看不见的延时太大导致的。
        // 所以，经过实际测试后，我决定注释/删掉所有最小值延迟间隔判定的代码。

        // if(delay_counter < 4){
        //     printk(KERN_INFO"DHT11 init fail .  low level interval is too small. %d\n",delay_counter);
        //     return NULL;
        // }

        // 在预定的80us左右输入高电平，表示数据准备输入
        // DHT11进入不稳定状态后，此高电平会持续很长时间。
        delay_counter = 0;
        while(gpio_read(DHT11))
        {
            udelay(5);
            delay_counter++;
            if(delay_counter > 20)
            {
                printk(KERN_INFO"DHT11 init fail .  High level interval is too long. %d\n",delay_counter);
                return NULL;
            }
        }

    }
    else
    {
        printk(KERN_INFO"GPIO Reverse fail .When input , it is still high level.\n");
        return NULL;
    }

    //接收过程（循环过程）
    //数据输入开始
    //每组数据40位
    // 8bit 湿度无符号整型数据 + 8bit湿度小数数据 + 8 bit 温度无符号整型数据 + 8 bit 温度小数数据 + 8 bit 校验和
    // = 湿度：X0.X1 温度：X2.X3，校验和：(u_char)(X0+X1+X2+X3)

    int i = 0,j = 0;
    //读取40bit，即5字节
    for(i = 0; i<5;i++)
    {
        // 读取一个字节
        for(j = 0 ; j < 8;j++)
        {
            // 读取一位

            // 在预定的50us左右输入低电平，表示数据位输入标志（缓冲）
            int delay_counter = 0;
            while(!gpio_read(DHT11))
            {
                udelay(5);
                delay_counter++;
                if(delay_counter > 30)
                {
                    printk(KERN_INFO"Data translate fail .  Low level interval is too long. %d\n",delay_counter);
                    return NULL;
                }
            }
            
            // 判定高电平持续时间，26-28us表示数据'0'，70us表示数据'1'。
            delay_counter = 0;
            while(gpio_read(DHT11))
            {
                udelay(5);
                delay_counter++;
                if(delay_counter > 5000)
                {
                    printk(KERN_INFO"Data translate fail .  High level interval is too long. %d\n",delay_counter);
                    return NULL;
                }
            }
            
            int flag = 0;
            if(delay_counter > 5)
            {
                flag = 1;
            }

           // printk(KERN_INFO"%d count : %d \t\t flag %d",i*8 + j,delay_counter,flag);

            // 写入一位到buffer
            dht_11_buffer[i] |= flag;
            if(j < 7)
                dht_11_buffer[i] <<= 1;
        }
        // printk(KERN_INFO"====================\n");
    }

    // 校验验算
    u_char sum=0;
    for(i = 0;i<4;i++)
    {
        sum += dht_11_buffer[i];
    }

    if(sum != dht_11_buffer[4])
    {
        printk(KERN_INFO"DHT11 data reading fail............sum : %u \t\t sum_t : %u\n",sum,dht_11_buffer[4]);
        // printk(KERN_INFO"H : %u.%u \t\t T : %u.%u \n",dht_11_buffer[0],dht_11_buffer[1],dht_11_buffer[2],dht_11_buffer[3]);
        return NULL;
    }
    else
    {
        // printk(KERN_INFO"DHT11 data reading suc.\n");
        // printk(KERN_INFO"H : %u.%u \t\t T : %u.%u \n",dht_11_buffer[0],dht_11_buffer[1],dht_11_buffer[2],dht_11_buffer[3]);
        return dht_11_buffer;
    }
}
