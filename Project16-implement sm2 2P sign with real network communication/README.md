## Project16 SM2 two-party sign
## 实验内容
![img](https://user-images.githubusercontent.com/104118101/182006647-9436f9c3-5e1f-485e-adc5-aedf2add2d9c.png)

设左边为client端，右边为server端<br>
client端进行补步骤为:
* 建立连接
* Gen_Key()
* Gen_Q1_e(Z, M)
* Sign(d1, k1, r, s2, s3)

server端进行的步骤为:
* 建立连接
* Gen_Key(P1)
* Gen_r_s2_s3(d2, Q1, e)

## 运行指导
先运行文件server.py, 再运行文件client.py,输入消息即可。

## 运行结果
![img_1](https://user-images.githubusercontent.com/104118101/182006654-3495ff4a-502f-4805-a49f-8526109d5399.png)
