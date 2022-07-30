"""
client
"""
import string
import random
from socket import *
from gmssl import sm3, func
import sympy


def msg_to_int(msg):
    msg_int = []
    for j in msg:

        my_int = 0
        count = 1
        for i in j:
            temp = ord(i) * count
            count = count + 1
            my_int = my_int + temp
        msg_int.append(my_int)
    return msg_int


# 生成随机的name和password
def user_u_p(n, u_low, u_high, p_low, p_high):
    """
    :param n: 用户个数
    :param u_low: user name长度的最小值
    :param u_high: user name长度的最大值
    :param p_low: password长度的最小值
    :param p_high: password长度的最大值
    :return: u_p_list
    """
    u_list = []
    for x in range(n):
        len_u = random.randint(u_low, u_high)
        u_list.append(''.join(random.SystemRandom().choice(string.ascii_letters + string.digits) for _ in range(len_u)))

    p_list = []
    for x in range(n):
        len_p = random.randint(p_low, p_high)
        p_list.append(''.join(random.SystemRandom().choice(string.ascii_letters + string.digits) for _ in range(len_p)))

    u_p_list = []
    for i in range(n):
        u_p_list.append(u_list[i] + p_list[i])
    return u_p_list


if __name__ == '__main__':
    print("Client...")
    while 1:
        # 建立连接
        client = socket(AF_INET, SOCK_STREAM)
        client.connect(('127.0.0.1', 12301))

        # user input name and password (u,p)
        # * client generate ephemeral secret key：a
        a = sympy.randprime(10 ** 2, 10 ** 3)
        # * client compute key-value (k,p)
        u_p = user_u_p(1, 4, 6, 6, 9)
        # 未泄露情况下的代码
        '''
        h = sm3.sm3_hash(func.bytes_to_list(bytes(str(u_p[0]), encoding='utf-8')))[31:]
        k = h[:2]
        h = msg_to_int([h])
        v = pow(h[0], a)
        '''
        # 泄露情况下的代码
        u_p[0] = 'wwl202000460010'
        h = sm3.sm3_hash(func.bytes_to_list(bytes(str(u_p[0]), encoding='utf-8')))[54:]
        k = h[:2]
        h = msg_to_int([h])
        v = pow(h[0], a)

        # Username and password detection
        sdata = k + str(v)
        client.send(sdata.encode('utf-8'))
        data = client.recv(65536 * 16).decode('UTF-8', 'ignore')
        if data[0] == ' ':
            print("未泄露")
        else:
            data = eval(data)
            if type(data[0] == str):
                print("1条信息泄露")
            else:
                num = len(data[0])
                print("{}条信息已经泄露".format(num))
        break
    client.close()
