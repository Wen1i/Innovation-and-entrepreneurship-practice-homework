from sm3 import *
import random
import time


def BirthdayAttack(n):
    while 1:
        x = random.random()
        y = random.random()
        h1 = SM3(str(x))
        h2 = SM3(str(y))
        if h1[:n] == h2[:n]:
            break
    return (h1[:n], h2[:n])


while 1:
    n = int(input("规模（单位：Byte）："))
    start = time.time()
    # for i in range(10):
    BirthdayAttack(n)
    end = time.time()
    runtime = (end - start)
    print("用时：", runtime, "s")
    if n == 0:
        break
