from myMD5 import *

if __name__ == '__main__':
    md5 = MD5()
    msg = "0123456789"
    hash_msg = md5.md5(msg)
    msg_append = "9876543210"
    msg_extend, hash_extend = md5.extend_length_attack(hash_msg, msg[-2:], len(msg), "hack")
    hash_attack = md5.md5(msg[:-2] + msg_extend)
    print("原始消息:",msg)
    print("附加消息:",msg_append)
    print("构造的hash值:", hash_attack)
    print("新消息的hash值:", hash_extend)
    if hash_attack == hash_extend:
        print("攻击成功！")
    else:
        print("攻击失败")
