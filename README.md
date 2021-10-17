# 介绍

> DHT11-driver是一个基于树莓派4B实现DHT11温湿度传感器数据读取的小型Linux模块项目。
>
> 编译工具：gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu、make、linux-raspi-5.4.0 source等。
>
> 编译环境：Ubuntu20.04 desktop x86_64
>
> 目标环境：Ubuntu20.04 server arm64 （Raspi 4B）

## 编译

> 使用如下命令编译：
>
> ```bash
> make
> ```
>
> 清空编译输出：
>
> ```bash
> make clean
> ```

## 调试运行

> 假设目标主机为raspi，可以使用scp工具（ssh包自带）：
>
> ```bash
> scp dht11_drv.ko raspi:~/
> ```
>
> 加载/卸载模块
>
> ```bash
> ssh raspi "sudo insmod ~/dht11_drv.ko"
> ssh raspi "sudo rmmod dht11_drv"
> ```
>
> 查看调试信息（源码的部分调试代码已注释，如需要请自行解除注释）：
>
> ```bash
> tail -f /var/log/syslog
> ```

