# SM3生日攻击
## 基本思想
1. 随机生成消息
2. 计算消息的sm3杂凑值
3. 寻找碰撞
## 运行指导
实现了两个版本的sm3生日攻击。<br>
* sm3_Birthday_Attack_gmssl.py：使用gmssl库中的sm3实现<br>
* sm3_Birthday_Attack_mysm3.py：使用自己写的sm3.py实现<br>

运行sm3_Birthday_Attack_gmssl.py（或sm3_Birthday_Attack_mysm3.py）文件，输入碰撞规模：<br> 
* 若想寻找杂凑值前 8n bit的碰撞，则输入n；<br>
* 若想结束程序，则输入0。
## 运行结果
均实现了32bit的生日攻击
* sm3_Birthday_Attack_gmssl.py

![img](https://user-images.githubusercontent.com/104118101/181449582-2257b895-1cc9-4261-aade-e5c20e474d0f.png)
* sm3_Birthday_Attack_mysm3.py

![image](https://user-images.githubusercontent.com/104118101/181449204-981c87a8-bbb3-42c8-bb80-a5fc22f48da4.png)

