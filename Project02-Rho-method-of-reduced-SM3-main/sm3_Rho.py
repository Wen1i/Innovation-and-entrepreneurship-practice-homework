from sm3 import *
import random
import time


def Rho(n):
    x = random.random()
    h1 = SM3(str(x))
    h2 = SM3(str(h1))
    while 1:
        h1 = SM3(str(h1))
        h2 = SM3(str(SM3(h2)))
        if h1[:n] == h2[:n]:
            break
    return h1[:n], h2[:n]


if __name__ == "__main__":
    while 1:
        n = int(input("规模（单位：Byte）："))
        start = time.time()
        for i in range(10):
            Rho(n)
        end = time.time()
        runtime = (end - start) / 10
        print("用时：", runtime, "s")
        if n == 0:
            break
