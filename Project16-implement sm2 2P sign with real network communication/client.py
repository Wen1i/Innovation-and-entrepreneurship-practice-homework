# client
from sm2 import *
import secrets
from gmssl import sm3, func
import socket
import time


def Gen_Key():
    d1 = secrets.randbelow(N)
    tmp = inv(d1, N)
    P1 = EC_multi(tmp, G)
    return d1, P1


def Gen_Q1_e(Z, M):
    M_ = bytes(Z + M, encoding='utf-8')
    e = sm3.sm3_hash(func.bytes_to_list(M_))
    k1 = secrets.randbelow(N)
    Q1 = EC_multi(k1, G)
    return k1, Q1, e


def Sign(d1, k1, r, s2, s3):
    s = ((d1 * k1) * s2 + d1 * s3 - r) % N
    if s != 0 or s != N - r:
        return (r, s)
    else:
        return 'error'


if __name__ == "__main__":
    # 建立连接
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    print("connected...")

    # 【Gen_Key】 —— P1 ——>
    d1, P1 = Gen_Key()
    data = str(P1[0]) + '|' + str(P1[1])
    s.sendto(data.encode(), ("127.0.0.1", 12300))

    # 【Gen_r_s2_s3】 —— Q1, e ——>
    time.sleep(1)
    ID_client = 'client'
    ID_server = 'server'
    Z = ID_client + ID_server
    M = input("massage:")
    k1, Q1, e = Gen_Q1_e(Z, M)
    data = str(Q1[0]) + '|' + str(Q1[1]) + '||' + e
    s.sendto(data.encode(), ("127.0.0.1", 12300))

    # 【Sign】 <—— r, r2, r3 ——
    data, addr = s.recvfrom(1024)
    data = data.decode()
    flag1 = data.index('|')
    flag2 = data.index('||')
    r = int(data[:flag1])
    s2 = int(data[flag1 + 1:flag2])
    s3 = int(data[flag2 + 2:])
    s_ = Sign(d1, k1, r, s2, s3)
    print("Sign:",s_)

    s.close()
    print("client finished...")
