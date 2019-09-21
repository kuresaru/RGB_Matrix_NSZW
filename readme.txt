2019年6月6日12:04:11

RGB点阵测试程序3
1.按照rgb.h中的头文件连接点阵屏
2.在SDIO上连接SD卡  SD卡根目录放入特制文件 文件名为01.bin
3.自动开始播放

----------------------------

STM32F407ZGT6   VS Code + gcc

绝对路径需要配置:
    c_cpp_properties.json       includePath(gcc/include)
	launch.json					程序编译输出elf文件路径
其它
    HSE: 8MHz
	
Video: https://www.bilibili.com/video/av54707666