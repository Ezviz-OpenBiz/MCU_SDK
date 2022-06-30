1.运行python_tool.py脚本会将GEB4F721.json（换成自己的json文件时，修改下脚本里加载的文件名称）转化成./1.mcu_sdk/tsl_protocol.c

2.移植MCU SDK，参考链接【腾讯文档】萤石MCU对接流程
https://docs.qq.com/doc/DZndxTG9QSFhyS2pI

3.application中是stm32f103的示例工程，其中ota是用了两个应用分区，升级时可循环

