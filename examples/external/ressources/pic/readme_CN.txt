/*****************************************************************************
* | File      	:   Readme_CN.txt
* | Author      :   Waveshare team
* | Function    :   Help with use
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2020-06-17
* | Info        :   在这里提供一个中文版本的使用文档，以便你的快速使用
******************************************************************************/
这个文件是帮助您使用本例程。

1.基本信息：
本例程使用单独LCD模块进行了验证，你可以在工程的Examples\中查看对应的测试例程;
本例程均在Raspberry Pi 4B上进行了验证;

2.管脚连接：
管脚连接你可以在 \lib\lcdconfig.py中查看，这里也再重述一次：
EPD  	=>	RPI(BCM)
VCC    	->    	5V
GND    	->    	GND
DIN    	->    	10(SPI0_MOSI)
CLK    	->    	11(SPI0_SCK)
CS     	->    	8(CE0)
DC     	->    	25
RST    	->    	27
BL  	->    	18

3.安装库：

    sudo apt-get update
    sudo apt-get install python-pip
    sudo apt-get install python-pil
    sudo apt-get install python-numpy
    sudo pip install RPi.GPIO

或

    sudo apt-get update
    sudo apt-get install python3-pip
    sudo apt-get install python3-pil
    sudo apt-get install python3-numpy
    sudo pip3 install RPi.GPIO

4.基本使用：
由于本工程是一个综合工程，对于使用而言，你可能需要阅读以下内容：
你可以在examples\目录中查看测试程序
请注意你购买的是哪一款的LCD模块。
栗子：
    如果你购买的1.54inch LCD，那么你应该执行命令：
    sudo python ./1inch54_LCD_test.py

 
