# impl-sm2-with-RFC6979
## 实验内容
#### RFC6979
RFC6979是一种用“确定性”方式来产生k值的规范，k=SHA256(d+HASH(msg)),其中d是私钥，msg是消息，HASH选择使用的是国密算法sm3。
#### sm2
sm2是一种椭圆曲线公钥密码算法。
首先定义了椭圆曲线的参数<br>
各函数功能如下：<br>
~~~
extended_euclidean_algorithm()扩展欧几里得算法<br>
mod_inverse()模逆运算<br>
elliptic_add()椭圆曲线加<br>
elliptic_double()椭圆曲线自加<br>
elliptic_multiply()椭圆曲线倍点运算<br>
generate_key()生成公私钥对<br>
sign()实现sm2签名<br>
verify()实现sm2验证签名<br>
~~~
其中的k是由RFC6979生成的
## 运行指导
运行sm2.py文件，首先生成公私钥对，输入消息message和ID，即可对消息进行签名，验证签名是否合法
## 运行结果
![image](https://user-images.githubusercontent.com/104118101/181593224-3603849d-7946-4161-90b5-375711cc806d.png)

