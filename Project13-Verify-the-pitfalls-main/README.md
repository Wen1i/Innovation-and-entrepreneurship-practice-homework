# Project13 Verify the pitfalls
## 实验内容
1. k泄露导致d泄露<br>
2. 对不同的消息使用相同的k签名导致d泄露<br>
3. 两个不同的user使用相同的k,可以相互推测对方的私钥<br>
4. 验证(r,s) and (r,-s)均为合法签名<br>
5. ECDSA与Schnorr使用相同的d和k而泄露d<br>
6. ECDSA与SM2使用相同的d和k而泄露d
![image](https://user-images.githubusercontent.com/104118101/181774911-be767e31-a991-4d0b-b705-1ff2a7db903a.png)
![image](https://user-images.githubusercontent.com/104118101/181774960-88afa7f5-ca09-4420-a739-996afe864eaa.png)
![image](https://user-images.githubusercontent.com/104118101/181775051-c0c54622-6078-418d-b92d-2d6ef87f083c.png)
![image](https://user-images.githubusercontent.com/104118101/181775098-7a213bd2-70bc-4e54-a736-37cfdfeede73.png)
## 运行指导
分别运行ECDSA_pitfalls.py,SM2_pitfalls.py,Schnorr_pitfalls.py
## 运行结果
* ECDSA
![image](https://user-images.githubusercontent.com/104118101/180967553-777e7933-5032-4c82-932b-ed9c4435451b.png)

* Schnorr
![image](https://user-images.githubusercontent.com/104118101/180965423-af8040f8-7efd-4de2-a72e-7b58e00e3dbf.png)

* SM2
![image](https://user-images.githubusercontent.com/104118101/180967768-0cfc4c1f-8d86-4398-b989-221acb0640a8.png)
