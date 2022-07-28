from gmssl import sm3, func
import random
import time


def Rho(n):
    x = random.random()
    h1 = sm3.sm3_hash(func.bytes_to_list(bytes(str(x), encoding='utf-8')))
    h2 = sm3.sm3_hash(func.bytes_to_list(bytes(str(h1), encoding='utf-8')))
    while 1:
        h1 = sm3.sm3_hash(func.bytes_to_list(bytes(str(h1), encoding='utf-8')))
        tmp = sm3.sm3_hash(func.bytes_to_list(bytes(str(h2), encoding='utf-8')))
        h2 = sm3.sm3_hash(func.bytes_to_list(bytes(str(tmp), encoding='utf-8')))
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
