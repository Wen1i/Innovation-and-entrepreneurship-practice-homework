"""
Server
"""
from gmssl import sm3, func
from socketserver import TCPServer, StreamRequestHandler, ThreadingMixIn
import traceback
import sympy
import random
import string


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


def argon2(upi):
    upi_hash = []
    for i in range(len(upi)):
        hash = sm3.sm3_hash(func.bytes_to_list(bytes(str(upi[i]), encoding='utf-8')))
        upi_hash.append(hash[54:])
    return upi_hash


class MyBaseRequestHandler(StreamRequestHandler):
    def handle(self):
        self.addr = self.request.getpeername()
        self.server.users[self.addr[1]] = self.request
        msg = "IP " + self.addr[0] + ":" + str(self.addr[1]) + " Connected..."
        print(msg)
        while True:
            try:
                # Data records: (userName,password)-->(ui,pi)
                upi = user_u_p(4, 4, 6, 6, 9)
                upi[0] = 'wwl202000460010'
                # Create key-value table (1TB):(ki,vi)
                # compute ℎi = Argon2(ui,pi)
                hash_upi = argon2(upi)
                hi = msg_to_int(hash_upi)
                # ki is the first weo bytes of hi,ki=hi[:2]
                ki = []
                for i in range(len(hash_upi)):
                    ki.append(hash_upi[i][:2])

                # vi = hi^b
                b = sympy.randprime(10 ** 1, 10 ** 2)
                vi = []
                for i in hi:
                    vi.append(pow(i, b))
                data = self.request.recv(2048).decode('UTF-8', 'ignore').strip()
                uk = data[:2]
                uv = data[2:]
                uv = int(uv)
                v = pow(uv, b)

                # devide the table into 2^116 sets according to key ki
                tag = [1] * 4
                divi_hash_key = []
                divi_hash_value = []
                for i in ki:
                    if ki.count(i) > 1 and tag[ki.index(i)] == 1:
                        temp = []
                        tmp = 0
                        for j in range(0, ki.count(i)):
                            index = ki[tmp:].index(i)
                            temp.append(hash_upi[index + tmp])
                            tag[index + tmp] = 0
                            tmp = index
                        divi_hash_key.append(i)
                        divi_hash_value.append(temp)
                    elif tag[ki.index(i)] == 1:
                        index = ki.index(i)
                        divi_hash_key.append(i)
                        divi_hash_value.append(hash_upi[index])
                divi_hash = dict(zip(divi_hash_key, divi_hash_value))

                # find data set
                if uk in divi_hash_key:
                    sdata = (divi_hash[uk], str(v))
                    sdata = str(sdata)
                else:
                    sdata = ' ' + str(v)
                self.request.sendall(sdata.encode())
                print('finished.')
                break
            except:
                traceback.print_exc()
                break


# 多线程
class MyThreadingTCPServer(ThreadingMixIn, TCPServer):
    def __init__(self, server_address, RequestHandlerClass):
        TCPServer.__init__(self, server_address, RequestHandlerClass)
        self.users = {}


class MyTCPserver():
    def __init__(self, server_addr='127.0.0.1', server_port=12301):
        self.server_address = server_addr
        self.server_port = server_port
        self.server_tuple = (self.server_address, self.server_port)

    def run(self):
        server = MyThreadingTCPServer(self.server_tuple, MyBaseRequestHandler)
        server.serve_forever()


if __name__ == '__main__':
    print("Server...")
    myserver = MyTCPserver()
    myserver.run()
