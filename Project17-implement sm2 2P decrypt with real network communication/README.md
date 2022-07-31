# Project16 SM2 two-party decrypt

## 实验内容

![image](https://user-images.githubusercontent.com/104118101/182007851-9145ad44-6b3d-491d-97df-dd913d85d6ff.png)

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
![image](https://user-images.githubusercontent.com/104118101/182007841-2b6101ec-7fde-4238-b006-99cc52810d66.png)
