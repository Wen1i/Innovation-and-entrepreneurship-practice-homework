from gmssl import sm3,func
import random
import time


def BirthdayAttack(n):
    while 1:
        x = random.random()
        y = random.random()
        h1 = sm3.sm3_hash(func.bytes_to_list(bytes(str(x), encoding='utf-8')))
        h2 = sm3.sm3_hash(func.bytes_to_list(bytes(str(y), encoding='utf-8')))
        if h1[:n] == h2[:n]:
            break
    return (h1[:n], h2[:n])


count = 0

while 1:
    n = int(input("规模（单位：Byte）："))
    start = time.time()
    for i in range(10):
        BirthdayAttack(n)
    end = time.time()
    runtime = (end - start) / 10
    print("用时：", runtime, "s")
    if n == 0:
        break
