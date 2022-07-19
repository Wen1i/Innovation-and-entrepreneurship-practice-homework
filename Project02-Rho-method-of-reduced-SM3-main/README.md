# Rho-method-of-reduced-SM3
## Pollard Rho原理
![image](https://user-images.githubusercontent.com/104118101/179362361-1d20b4f6-3fb8-4335-b9ba-b65ff1b45056.png)

## 关键代码
~~~
def Rho(n):
    x = random.random()
    h1 = SM3(str(x))
    h2 = SM3(str(h1))
    while 1:
        h1 = SM3(str(h1))
        h2 = SM3(str(SM3(h2)))
        if h1[:n] == h2[:n]:
            break
    return (h1[:n], h2[:n])
~~~
其中，参数为n，找到的碰撞为SM3杂凑值的前8nbit。
## 运行结果
|n|前几bit|time(单位:s)|
|:----:|:----:|:----:|
|1|8|0.6923840284347534|
|2|16|16.820635652542116|
|3|24|414.7569657087326|
|4|32|8057.379549980164|


