# SM3的长度扩展攻击
## 实验步骤

SM3长度扩展攻击<br>
    1.首先计算原消息msg的hash值<br>
    2.在msg+padding之后附加一段消息,用原消息的hash值作为IV计算附加消息之后的hash值,得到消息扩展后的hash_attack<br>
    3.用sm3加密伪造后的整体消息，得到hash_extend<br>
    4.验证hash_attack 与 hash_extend 是否相等<br>
## 运行指导

运行文件sm3_length_extension.py

## 运行结果
攻击成功
![image](https://user-images.githubusercontent.com/104118101/181464505-33bd00ca-4650-4669-8dd3-122d546da7a1.png)
