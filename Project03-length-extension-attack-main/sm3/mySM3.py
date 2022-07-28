"""
SM3长度扩展攻击
    1.首先计算原消息(secret)的hash值
    2.在secret+padding之后附加一段消息,用原消息的hash值作为IV计算附加消息之后的hash值,得到消息扩展后的hash1
    3.用sm3加密伪造后的整体消息，得到hash2
    4.验证hash1 与 hash2 是否相等
"""
from math import ceil

T1 = [0x79cc4519]
T2 = [0x7a879d8a]
T = T1 * 16 + T2 * 48
V = [0x7380166f, 0x4914b2b9, 0x172442d7, 0xda8a0600, 0xa96f30bc, 0x163138aa, 0xe38dee4d, 0xb0fb0e4e]


# 将消息(字符串)转换成二进制
def msg2bin(msg):
    msg_bin = ''
    for i in msg:
        ascii_i = ord(i)
        msg_bin = msg_bin + bin(ascii_i)[2:].rjust(8, '0')
    return msg_bin

# 将消息(字符串)转换成十六进制
def msg2hex(msg):
    msg_hex = ''
    for i in msg:
        ascii_i = ord(i)
        msg_hex = msg_hex + hex(ascii_i)[2:].rjust(2, '0')
    return msg_hex

def bin2hex(num):
    length = len(num) // 4
    return hex(int(num,2))[2:].rjust(length,'0')

########################## SM3的实现 ##########################

# ff函数
def ff(x, y, z, j):
    # x,y,z分别为3个32位向量（int），j为轮数（0<=j<64）
    if 0 <= j < 16:
        ret = x ^ y ^ z
    elif 16 <= j < 64:
        ret = (x & y) | (x & z) | (y & z)
    return ret


# gg函数
def gg(x, y, z, j):
    # x,y,z分别为3个32位向量（int），j为轮数（0<=j<64）
    if 0 <= j < 16:
        ret = x ^ y ^ z
    elif 16 <= j < 64:
        ret = (x & y) | ((~x) & z)
    return ret


def shift_left(x, n):
    # x是32位的向量
    strx = bin(x)[2:].zfill(32)
    add = '0' * n
    strret = strx[n:] + add
    ret = int(strret, 2)
    return ret


# p1置换(消息扩展)
def p1(x):
    ret = x ^ (shift_left(x, 15)) ^ (shift_left(x, 23))
    return ret


# p0置换(压缩函数)
def p0(x):
    ret = x ^ (shift_left(x, 9)) ^ (shift_left(x, 17))
    return ret


# 压缩函数cf
def sm3_cf(vi, bi):
    # 消息扩展
    w = []
    # w列表都是十进制整数
    # 生成w0-w15
    for i in range(16):
        wei = 0x1000000  # 权重 初始为2^24
        rel = 0
        for j in range(i * 4, (i + 1) * 4):  # 将4字节数整合成一个字
            rel = rel + bi[j] * wei  # bi[j]数据类型是二进制对应的整数，比如0b101--5
            wei = int(wei / 0x100)  # 权重每次减少2^8
        w.append(rel)

    # 生成w16-w67
    for k in range(16, 68):
        tmp = p1(w[k - 16] ^ w[k - 9] ^ shift_left(w[k - 3], 15)) ^ shift_left(w[k - 13], 7) ^ w[k - 6]
        w.append(tmp)
    w1 = []
    # 生成w’0-w‘64
    for k in range(0, 64):
        tmp = w[k] ^ w[k + 4]
        w1.append(tmp)
    A, B, C, D, E, F, G, H = vi
    # 64轮加密
    for j in range(0, 64):
        #
        ss1 = shift_left((shift_left(A, 12)) ^ E ^ (shift_left(T[j], j % 32)), 7)
        #
        ss2 = ss1 ^ shift_left(A, 12)
        tt1 = ff(A, B, C, j) ^ D ^ ss2 ^ w1[j]
        tt2 = gg(E, F, G, j) ^ H ^ ss1 ^ w[j]
        D = C
        C = shift_left(B, 9)
        B = A
        A = tt1
        H = G
        G = shift_left(F, 19)
        F = E
        E = p0(tt2)
    v_i = [A, B, C, D, E, F, G, H]
    return [v_i[i] ^ vi[i] for i in range(0, 8)]


# 对消息进行填充
def fill(msg):
    length = len(msg)

    # 填充1比特'1'
    msg_bin = msg + '1'

    # 填充0的个数
    k = (448 - length - 1) % 512
    if k < 0:
        k += 512
    # 填充后的消息
    msg_bin = msg_bin + '0' * k + bin(length)[2:].rjust(64, '0')

    return msg_bin

# 对消息进行分组
def msg_divide(msg):
    # 填充之后的长度
    length = len(msg)
    # 按照字节划分
    temp = []
    for i in range(0, length, 8):
        tmp = msg[i:i + 8]
        tmp = int(tmp, 2)
        temp.append(tmp)
    # 对消息进行分组
    b = []
    for i in range(len(temp) // 64):
        instate = temp[i * 64:(i + 1) * 64]
        b.append(instate)
    return b

# sm3算法
def sm3(msg):
    # 消息填充,msg是字符串类型
    b = msg_divide(msg)
    v = []

    for i in range(len(b)):
        if i == 0:
            v.append(sm3_cf(V, b[i]))
        else:
            v[0] = sm3_cf(v[i - 1], b[i])

    result = ''
    for j in range(8):
        result = result + hex(v[0][j])[2:].rjust(8,'0')
    return result

# 长度扩展攻击
def sm3_lenattack_hash(msg_orgin, msg_pad, new_v):
    msg = msg_orgin + msg_pad
    msg = fill(msg)
    msg = msg[len(msg_orgin):]
    b = msg_divide(msg)
    v = []
    for i in range(len(b)):
        if i == 0:
            v.append(sm3_cf(new_v, b[i]))
        else:
            v[0] = sm3_cf(v[i - 1], b[i])

    result = ''
    for j in range(8):
        result = result + hex(v[0][j])[2:].rjust(8,'0')
    return result



