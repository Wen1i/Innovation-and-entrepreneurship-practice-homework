# Project12-deduce-pk
## 实验原理
给定ECDSA的结果（r,s)，可以得到两个候选公钥:<br>
1. 根据r值计算得到两个椭圆曲线的两个候选点R1和R2
2. 求解杂凑值e
3. 得到候选公钥$K1 = r^{-1} (sR1 - eG)$，$K2 = r^{-1}(sR’ - eG)$
## 运行结果
![image](https://user-images.githubusercontent.com/104118101/179404197-863c4cd0-5e8c-4cda-a82d-87e5e2c2466d.png)
