import socket
import secrets
from sm2 import *


def Gen_Key(P1):
    d2 = secrets.randbelow(N)
    tmp = EC_multi(inv(d2, N), P1)
    P = EC_sub(tmp, G)
    return d2, P


def Gen_r_s2_s3(d2, Q1, e):
    e = int(e, 16)
    k2 = secrets.randbelow(N)
    k3 = secrets.randbelow(N)
    Q2 = EC_multi(k2, G)
    x1, y1 = EC_add(EC_multi(k3, Q1), Q2)
    r = (x1 + e) % N
    if r == 0: return 'error: r == 0'
    s2 = d2 * k3 % N
    s3 = d2 * (r + k2) % N
    return r, s2, s3

if __name__ == "__main__":
    # 建立连接
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.bind(('', 12300))
    print("server start...")

    # 【Gen_Key】<——P1——
    data, addr = s.recvfrom(1024)
    data = data.decode()
    flag1 = data.index('|')
    P1 = (int(data[:flag1]), int(data[flag1 + 1:]))
    d2, P = Gen_Key(P1)

    # 【Gen_r_s2_s3】 <——(Q1, e)—— r, s2, s3
    data, addr = s.recvfrom(1024)
    data = data.decode()
    flag1 = data.index('|')
    flag2 = data.index('||')
    Q1 = (int(data[:flag1]), int(data[flag1 + 1:flag2]))
    e = data[flag2 + 2:]
    r, s2, s3 = Gen_r_s2_s3(d2, Q1, e)
    data = str(r) + '|' + str(s2) + '||' + str(s3)
    s.sendto(data.encode(), addr)
    s.close()

    print("server finished...")
