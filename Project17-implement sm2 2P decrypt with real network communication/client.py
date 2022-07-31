# client

import secrets
from gmssl import sm3, func
import socket
from sm2 import *


def Gen_Key():
    d1 = secrets.randbelow(N)  # sk
    tmp = inv(d1, N)
    P1 = EC_multi(tmp, G)  # pk
    return d1, P1


def C_to_T1(d1, C1):
    if C1 == 0: return 'error'
    tmp = inv(d1, N)
    T1 = EC_multi(tmp, C1)
    return T1


def dec(T2, C1, C2, C3):
    tmp = EC_sub(T2, C1)  # kP = (x2, y2)
    x2 = hex(tmp[0])[2:]
    y2 = hex(tmp[1])[2:]
    klen = len(C2) * 4
    t = KDF(x2 + y2, klen)
    M_ = dec_XOR(C2, t)  # M_为M'
    u = sm3.sm3_hash(func.bytes_to_list(bytes((x2 + M_ + y2), encoding='utf-8')))
    if u != C3: return 'error:u != C3'
    return M_


if __name__ == "__main__":
    # 建立链接
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    # 【Gen_Key】
    d1, P1 = Gen_Key()
    data = str(P1[0]) + '||' + str(P1[1])
    s.sendto(data.encode(), ("127.0.0.1", 12300))
    data, addr = s.recvfrom(1024)
    data = data.decode()
    flag = data.index('||')
    pk = (int(data[:flag]), int(data[flag + 2:]))
    print("pk: ", pk)

    # 【C_to_T1】
    M = input("M: ")
    C1, C2, C3 = SM2_enc(M, pk)
    T1 = C_to_T1(d1, C1)
    data = str(T1[0]) + '||' + str(T1[1])
    s.sendto(data.encode(), addr)

    # 【dec】
    data, addr = s.recvfrom(1024)
    data = data.decode()
    flag = data.index('||')
    T2 = (int(data[:flag]), int(data[flag + 2:]))
    M_ = dec(T2, C1, C2, C3)
    print("解密结果: ", M_)
    s.close()
    print("client finished...")
