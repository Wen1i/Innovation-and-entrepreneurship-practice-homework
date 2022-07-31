import socket
import secrets
from sm2 import *

def Gen_Key(P1):
    d2 = secrets.randbelow(N)
    tmp = inv(d2, N)
    tmp = EC_multi(tmp, P1)
    P = EC_sub(tmp, G)
    return d2, P

def T1_to_T2(T1):
    T2 = EC_multi(inv(d2, N), T1)
    return T2

if __name__ == "__main__":
    # 建立连接
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.bind(('', 12300))
    print("server started...")
    # 【Gen_Key】
    data, addr=s.recvfrom(1024)
    data = data.decode()
    flag = data.index('||')
    P1 = (int(data[:flag]), int(data[flag + 2:]))
    d2, P = Gen_Key(P1)
    data = str(P[0]) + '||' + str(P[1])
    s.sendto(data.encode(), addr)

    #【P1_to_P2】
    data, addr=s.recvfrom(1024)
    data = data.decode()
    flag = data.index('||')
    T1 = (int(data[:flag]), int(data[flag + 2:]))
    T2 = T1_to_T2(T1)
    data = str(T2[0]) + '||' + str(T2[1])
    s.sendto(data.encode(), addr)

    s.close()
    print("server finished...")

