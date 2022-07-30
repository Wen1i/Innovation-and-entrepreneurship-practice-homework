# 介绍
在无线电自供能设备Powercast-P2110-EVAL-01上成功部署了SM4算法，Powercast P2110-EVAL-01是由位于美国的 Powercast 公司所推出的面向无源感知领域的无源传感器网络开发套件，设备实物如图1所示（由学院老师支持使用）。
![image](https://user-images.githubusercontent.com/104118101/178275470-569891cf-7fb1-45e7-a1dc-2361e9725dac.png)
<br>在这里使用CoolTerm实现输出显示，输出结果如图2所示。<br>
![image](https://user-images.githubusercontent.com/104118101/178276064-9dbf8471-c89c-45bb-950b-9068760c96e4.png)

# 问题及解决办法
在部署时，遇到的困难如下：<br>
1.传输数据长度有限<br>
我们通过测试，传输22字节以上的时候会出现问题，具体地说，在End_Point(下简称EP)端利用MiApp_WriteData()把数据逐字节写入TxBuffer中，数据能够正常发出，但是在Access_Point(下简称AP)端无法正常接收该数据包。<br>
2. Access_Point端闪存有限<br>
AP端初始闪存已经占用了96%，只剩下4%的闪存了，加入加解密算法后会由于闪存不足而编译失败。<br>
3. 函数过长导致解密失败<br>
在测试SM4加密算法的时候，AP端无法正常接收数据包并解密。通过中途打印字符串的方法测试AP端main函数执行到了哪一步，我们发现，到如图的步骤之前，程序就无法继续执行下去而重启了，即MiApp_MessageAvailable() == 0，我们将其置为1则程序能正常进入if的第一个分支，如注释所言，只要数据包正常被接收的时候才会返回1，目前不清楚此处为0的原因。<br>
将MiApp_MessageAvailable() = 1后，发现由于解密函数时间较长，而在AP端执行解密函数的中途重置了程序，导致解密失败，无法正常输出数据，具体地说，通过在函数中间插入字符串的打印以验证其运行到哪一步。通过测试得知，前面部分的字符串可以打印，但是函数后半部分本该打印的字符串没有出现，于是可以确定AP端大概执行到解密函数的某一步停止而重启了，我认为，一可能是因为EP端发送数据过快，而AP端解密数据效率跟不上导致缓存中的数据被后来发送的数据覆盖了，而导致AP端的重启，二可能是因为AP端接收数据包有计时器，到一定时间后丢弃。<br>

问题1、2的解决方法：注释掉了一部分数据，考虑到一些数据可能不是足够必要的，分别去掉了TXID、rssiValH、rssiValL、extValH、extValL，这样做虽然牺牲了一部分数据，但是就可以解决传输数据长度有限的问题了，同时，由于传输的数据减少，在AP端相应需要打印的数据也变少了，我们注意到，AP端的PrintScreen()函数占用的闪存是最多的，以及GetRSSI()函数中的rssi_lookup一系列转化数据用的数组占用了大量闪存，注释掉这一部分不必要的代码后，闪存由原来的96%降到了77%，腾出了19%的闪存供加解密算法使用。<br>

问题3的解决办法：尝试在函数的后半段部分语句后面加上ConsolePutROMString ((ROM char*)"");，即可解密成功，我们认为是因为调用了ConsolePutROMString()函数让系统持续执行了下去而不重启。这样就可以正常输出解密后的数据同时不打印其他字符。


# 参考
Lifetime Power(R) Energy Harvesting Development Kit for Wireless Sensors
Powercast Corporation


Version	Changes
------- --------------------------
  1.1	Updated version for endpoint code revision
  1.0	Initial Release


FOLDERS
..\Powercast
  -  Code from the Microchip MiWi stack that was modified for the purposes of this Development Kit.


..\Microchip
  -  Support code from Microchpi MiWi stack that was not modified.


..\License
  -  Microchip MiWi DE 3.1 Software License Agreement


REQUIRE DOWNLOAD
  -  This software package requires Microchip's MPLABX IDE and X16 Compiler.  Download the latest versions by visiting Microchip's website: www.microchip.com


TO GET STARTED
  -  Open MPLABX
  -  Click File -> Open Project...
  -  Navigate to the package folder and open \Powercast\Access Point\Access Point.X
  -  or open \Powercast\End Point\End Point.X
  -  Click Run -> Build Project

POWERCAST SUPPORT
The source code and other documentation from Powercast are available at:
http://www.powercastco.com/documentation/


