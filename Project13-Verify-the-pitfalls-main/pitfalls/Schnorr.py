"""
Schnorr签名:KeyGen,Sign,Verify
"""
import secrets
from gmssl import sm3, func

# 定义椭圆曲线参数、基点和阶
A = 0
B = 7
G_X = 55066263022277343669578718895168534326250603453777594175500187360389116729240
G_Y = 32670510020758816978083085130507043184471273380659243275938904335757337482424
G = (G_X, G_Y)
P = 115792089237316195423570985008687907853269984665640564039457584007908834671663
N = 115792089237316195423570985008687907852837564279074904382605163141518161494337
h = 1

# 利用扩展欧几里得求逆
def inv(a, n):
    def ext_gcd(a, b, arr):
        if b == 0:
            arr[0] = 1
            arr[1] = 0
            return a
        g = ext_gcd(b, a % b, arr)
        t = arr[0]
        arr[0] = arr[1]
        arr[1] = t - int(a / b) * arr[1]
        return g

    arr = [0, 1, ]
    gcd = ext_gcd(a, n, arr)
    if gcd == 1:
        return (arr[0] % n + n) % n
    else:
        return -1

# 加法
def EC_add(p, q):
    # 0：无穷远点
    if p == 0 and q == 0:
        return 0  # 0 + 0 = 0
    elif p == 0:
        return q  # 0 + q = q
    elif q == 0:
        return p  # p + 0 = p
    else:
        if p[0] == q[0]:
            if (p[1] + q[1]) % P == 0:
                return 0
            elif p[1] == q[1]:
                return EC_double(p)
        elif p[0] > q[0]:
            tmp = p
            p = q
            q = tmp
        r = []
        slope = (q[1] - p[1]) * inv(q[0] - p[0], P) % P  # 斜率
        r.append((slope ** 2 - p[0] - q[0]) % P)
        r.append((slope * (p[0] - r[0]) - p[1]) % P)
        return r[0], r[1]

# 逆元
def EC_inv(p):
    r = [p[0], P - p[1]]
    return r


# 减法:p - q
def EC_sub(p, q):
    q_inv = EC_inv(q)
    return EC_add(p, q_inv)

# 自加:p+p
def EC_double(p):
    r = []
    slope = (3 * p[0] ** 2 + A) * inv(2 * p[1], P) % P
    r.append((slope ** 2 - 2 * p[0]) % P)
    r.append((slope * (p[0] - r[0]) - p[1]) % P)
    return r[0], r[1]

# 多倍点:ap
def EC_multi(a, p):
    n = p
    r = 0
    _bin = bin(a)[2:]
    _len = len(_bin)
    for i in reversed(range(_len)):
        if _bin[i] == '1':
            r = EC_add(r, n)
        n = EC_double(n)
    return r

# bit长度
def get_bit_num(x):
    if isinstance(x, int):  # when int
        num = 0
        tmp = x >> 64
        while tmp:
            num += 64
            tmp >>= 64
        tmp = x >> num >> 8
        while tmp:
            num += 8
            tmp >>= 8
        x >>= num
        while x:
            num += 1
            x >>= 1
        return num
    elif isinstance(x, str):  # when string
        return len(x.encode()) << 3
    elif isinstance(x, bytes):  # when bytes
        return len(x) << 3
    return 0


# 密钥生成
def key_gen():
    sk = int(secrets.token_hex(32), 16)
    pk = EC_multi(sk, G)
    return sk, pk

# 生成签名
def Schnorr_sign(M, sk):
    k = secrets.randbelow(N)
    R = EC_multi(k, G)
    tmp = str(R[0]) + str(R[1]) + M
    e = int(sm3.sm3_hash(func.bytes_to_list(bytes(tmp, encoding='utf-8'))), 16)
    s = k + e * sk % N
    return R, s

# 验证签名
def Schnorr_verify(signature, M, pk):
    R, s = signature
    tmp = str(R[0]) + str(R[1]) + M
    e = int(sm3.sm3_hash(func.bytes_to_list(bytes(tmp, encoding='utf-8'))), 16)
    tmp1 = EC_multi(s, G)
    tmp2 = EC_multi(e, pk)
    tmp2 = EC_add(R, tmp2)
    return tmp1 == tmp2
