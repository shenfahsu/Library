本文档使用 TAB = 4 对齐，使用keil5默认配置打开阅读比较方便。

【*】程序简介

-工程名称：GSM_Bluetooth_HFP
-实验平台: 秉火STM32 指南者 开发板 
-MDK版本：5.16
-ST固件库版本：3.5


【 ！】功能简介：
使用GSM模块的蓝牙耳机功能

【 ！】实验操作：

使用杜邦线连接好STM32开发板和GSM模块（连接串口和GND），给模块供电，

编译并下载本程序到开发板，复位运行，STM32的串口会有调试信息输出。

程序检测到模块后，串口提示蓝牙配对连接操作，
完成配对连接后，通过按键1来接听电话或者拨打电话（号码在bsp_gsm_bluetooth.c中const char usernum[]="10086";），按键2挂断电话。

【*】注意事项：
 -测试前确认手机端曾经跟模块配对但已经断开，请先解除配对，才能被模块搜到。
 -测试前应该在手机有网络信号，需要有两部手机才能完成测试。


/***************************************************************************************************************/

【*】 引脚分配

GSM模块：
GSM模块的TTL串口与STM32的usart2接口相连。

				GND<---->GND
				PA2<---->RXD
				PA3<---->TXD
		
 GSM模块需要使用DC电源独立供电


串口(TTL-USB TO USART)：
CH340的收发引脚与STM32的发收引脚相连。
	RX<--->PA9
	TX<--->PA10
												
/*********************************************************************************************/

【*】 版本

-程序版本：1.0
-发布日期：2013-10

/*********************************************************************************************/

【*】 联系我们

-秉火论坛    :http://www.firebbs.cn
-淘宝店铺    :https://fire-stm32.taobao.com

/*********************************************************************************************/