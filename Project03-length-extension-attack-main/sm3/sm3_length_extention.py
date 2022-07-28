from mySM3 import *


def msg2bin(msg):
    msg_bin = ''
    for i in msg:
        ascii_i = ord(i)
        msg_bin = msg_bin + bin(ascii_i)[2:].rjust(8, '0')
    return msg_bin


def msg2hex(msg):
    msg_hex = ''
    for i in msg:
        ascii_i = ord(i)
        msg_hex = msg_hex + hex(ascii_i)[2:].rjust(2, '0')
    return msg_hex


def bin2hex(num):
    length = len(num) // 4
    return hex(int(num, 2))[2:].rjust(length, '0')


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
        result = result + hex(v[0][j])[2:].rjust(8, '0')
    return result


if __name__ == "__main__":

    msg = "password is 001"
    print("原始消息:", msg)
    hash_msg = sm3(fill(msg2bin(msg)))
    IV = []    # 将初始IV设置为原消息的hash值
    for i in range(0, len(hash_msg), 8):
        IV.append(int(hash_msg[i:i + 8], 16))
    msg_append = "100"
    print("附加消息:", msg_append)
    hash_attack = sm3_lenattack_hash(fill(msg2bin(msg)), msg2bin(msg_append), IV)
    print("构造的hash值:", hash_attack)
    msg_extend_fill = fill(msg2bin(msg)) + msg2bin(msg_append)
    msg_extend = fill(msg_extend_fill)
    hash_extend = sm3(msg_extend)
    print("新消息的hash值:", hash_extend)
    print("攻击成功!" if hash_attack == hash_extend else "攻击失败!")
