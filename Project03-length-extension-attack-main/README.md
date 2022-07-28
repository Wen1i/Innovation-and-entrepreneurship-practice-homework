# length-extension-attack
实现了长度扩展攻击：md5，sha256，sm3
## 攻击原理
    1.首先计算原消息msg的hash值<br>
    2.在msg+padding之后附加一段消息,用原消息的hash值作为IV计算附加消息之后的hash值,得到消息扩展后的hash_attack<br>
    3.得到hash_extend<br>
    4.验证hash_attack 与 hash_extend 是否相等<br>
## 运行指导
分别运行sha256_length_extention.py,md5_length_extention.py,sm3_length_extention.py
## 运行结果
### sha256

![image](https://user-images.githubusercontent.com/104118101/181478701-007076d9-e557-46da-b882-f4bf752185d6.png)
### md5

![img_1](https://user-images.githubusercontent.com/104118101/181478625-4d265489-1c7e-4b28-a8fc-2f183e72d5a9.png)
### sm3

![img](https://user-images.githubusercontent.com/104118101/181478764-b8e23166-83d4-4f8f-9c4e-d0ba78a865b0.png)

