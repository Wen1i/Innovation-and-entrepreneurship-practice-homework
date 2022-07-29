from hashlib import sha256
from random import randint
from math import sqrt

A = 0
B = 7
P = 115792089237316195423570985008687907853269984665640564039457584007908834671663
N = 115792089237316195423570985008687907852837564279074904382605163141518161494337


def SHA256(s):
    msg = s
    return sha256(bytes(bytearray(msg, encoding='utf-8'))).hexdigest()


def inv(a, n):
    def ext_gcd(a, b, arr):
        if b == 0:
            arr[0] = 1
            arr[1] = 0
            return a
        g = ext_gcd(b, a%b, arr)
        t = arr[0]
        arr[0] = arr[1]
        arr[1] = t-int(a/b)*arr[1]
        return g
    arr = [0,1,]
    gcd = ext_gcd(a, n, arr)
    if gcd == 1:
        return (arr[0]%n+n)%n
    else:
        return -1

def EC_add(p, q):    # 表示无穷远点
    if p == 0 and q == 0: return 0  # 0 + 0 = 0
    elif p == 0: return q  # 0 + q = q
    elif q == 0: return p  # p + 0 = p
    else:
        if p[0] == q[0]:  
            if (p[1] + q[1]) % P == 0: return 0  # mutually inverse
            elif p[1] == q[1]: return EC_double(p)
        elif p[0] > q[0]:  # swap if px > qx
            tmp = p
            p = q
            q = tmp
        r = []
        slope = (q[1] - p[1]) * inv(q[0] - p[0], P) % P  # 斜率
        r.append((slope ** 2 - p[0] - q[0]) % P)
        r.append((slope * (p[0] - r[0])- p[1]) % P)
        return (r[0], r[1])

def EC_inv(p):
    r = [p[0]]
    r.append(P - p[1])
    return r

def EC_sub(p, q):
    q_inv = EC_inv(q)
    return EC_add(p, q_inv)


def EC_double(p):
    r = []
    slope = (3 * p[0] ** 2 + A) * inv(2 * p[1], P) % P
    r.append((slope ** 2 - 2 * p[0]) % P)
    r.append((slope * (p[0] - r[0]) - p[1]) % P)
    return (r[0], r[1])


def msg_to_dot(msg): # 映射到椭圆曲线上的点
    def Legendre(y,p): 
        return pow(y,(p - 1) // 2,p)
    def msg_to_x(m):
        """消息转换成横坐标"""
        mdigest = SHA256(m)
        while 1:  # cycle until x belong to QR
            x = int(mdigest, 16)
            if Legendre(x, P):
                break
            mdigest = SHA256(mdigest)
        return x
    def get_y(x):  # y^2 = x^3 + 7 mod P
        """y ** 2 = x ** 3 + 7
        已知横坐标，计算纵坐标"""
        right = (x ** 3 + 7) % P
        while 1:
            a = randint(0, P)
            if Legendre(a, P) == -1:
                break
        base = int(a + sqrt(a ** 2 - right))
        expo = (P + 1) // 2
        y = pow(base, expo, P)
        return y
    x = msg_to_x(msg)
    y = get_y(x)
    return (x, y)


def ADD(ecmh, msg):
    dot = msg_to_dot(msg)
    tmp = EC_add(ecmh, dot)
    return tmp

def single(msg):
    return ADD(0, msg)

def remove(ecmh, msg):
    dot = msg_to_dot(msg)
    tmp = EC_sub(ecmh, dot)
    return tmp

def combine(msg_set):
    ans = single(msg_set[0])
    num = len(msg_set) - 1
    for i in range(num):
        ans = ADD(ans, msg_set[i+1])
    return ans

if __name__=="__main__":
    m1 = "wwl"
    m2 = "202000460010"

    print('single(m1)', single(m1))
    print('single:\n', single(m2))
    print('combine([m1, m2]\n', combine([m1, m2]))
    print('combine([m2, m1]\n', combine([m2, m1]))
    print('ADD(single(m1), m2))\n', ADD(single(m1), m2))
    print('remove(combine([m1, m2]), m2\n', remove(combine([m1, m2]), m2))
