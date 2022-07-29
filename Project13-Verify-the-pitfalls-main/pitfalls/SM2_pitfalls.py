from SM2 import *
from gmssl import sm3, func
import secrets


# 使用sm2签名算法签名
def sm2_sign_and_assign_k(k, sk, msg, ZA):
    M = ZA + msg
    M = bytes(M, encoding='utf-8')
    e = sm3.sm3_hash(func.bytes_to_list(M))
    e = int(e, 16)  # str -> 16进制整型
    a_dot = EC_multi(k, G)  # (x1, y1) = kG
    r = (e + a_dot[0]) % N  # r = (e + x1) % n
    s = 0
    if r != 0 and r + k != N:
        s = (inv(1 + sk, N) * (k - r * sk)) % N
    if s != 0:
        return (r, s)


# 使用ECDSA签名算法签名
def ECDSA_sign_and_assign_k(k, msg, sk):
    R = EC_multi(k, G)
    r = R[0] % N  # Rx mod n
    e = sm3.sm3_hash(func.bytes_to_list(bytes(msg, encoding='utf-8')))  # e = hash(m)
    e = int(e, 16)
    tmp1 = inv(k, N)
    tmp2 = (e + sk * r) % N
    s = tmp1 * tmp2 % N
    return (r, s)


# 【1】k泄露导致d泄露
def sm2_leaking_k():
    """
    A:KeyGen-->(sk_a,pk_a)
       Sign-->Sig_ska(msg)
    """
    ID = 'A'
    msg = "wwl"
    sk_a, pk_a = key_gen()
    k = secrets.randbelow(N)  # 泄露的k
    ZA = precompute(ID, A, B, G_X, G_Y, pk_a[0], pk_a[1])
    Sign = sm2_sign_and_assign_k(k, sk_a, msg, str(ZA))
    r, s = Sign
    print("sk_a \t\t\t(private key of A)\t\t", '0x' + hex(sk_a)[2:].rjust(64, '0'))
    print("msg \t\t\t(msg from A)\t\t\t", msg)
    print("Sign_ska_msg\t(A sign msg using sk_a)", Sign)

    """
    B:deduce sk_a from k,Sign
      deduce result: d = (k - s) / (s + r)
    """
    d = (k - s) * inv(s + r, N) % N
    print("d \t\t\t\t(B deduced sk_a)\t\t", '0x' + hex(d)[2:].rjust(64, '0'))
    if d == sk_a:
        print("d=sk_a, B get true sk_a!!!")
    else:
        print("B not get true sk_a")

    """
    B: forge Sign using deduced sk_a(d)
       forge result: Sign_f
       Verify Sign_f using pk_a
    """
    # msg_f是B签名的消息
    msg_f = "not wwl"
    print("msg_f\t\t\t(msg for forge from B)\t", msg_f)
    pk_f = EC_multi(d, G)  # d对应的公钥
    ZA_f = precompute(ID, A, B, G_X, G_Y, pk_f[0], pk_f[1])
    Sign_f = sm2_sign(d, msg_f, str(ZA_f))
    print("Sign_f\t\t\t(B Sign msg_f using d)\t", Sign_f)
    print("B Verify using pk_a...")
    if sm2_verify(pk_a, ID, msg_f, Sign_f) == 1:
        print("pass...forge successfully!")
    else:
        print("false...forge unsuccessfully")


# 【2】对不同的消息使用相同的k签名导致d泄露
def sm2_reusing_k():
    """
    A:KeyGen-->(sk_a,pk_a)
       Sign1-->Sig_ska(msg1)
       Sign2-->Sig_ska(msg2)
    """
    sk, pk = key_gen()
    print("sk_a \t\t\t(private key of A)\t\t", '0x' + hex(sk)[2:].rjust(64, '0'))
    msg1 = "sdu"
    msg2 = "wwl"
    print("msg1 \t\t\t(msg from A)\t\t\t", msg1)
    print("msg2 \t\t\t(msg from A)\t\t\t", msg2)
    k = secrets.randbelow(N)  # same k
    ID = 'A'
    ZA = precompute(ID, A, B, G_X, G_Y, pk[0], pk[1])
    Sign1 = sm2_sign_and_assign_k(k, sk, msg1, str(ZA))
    Sign2 = sm2_sign_and_assign_k(k, sk, msg2, str(ZA))

    '''
    B: deduce sk_a through msg1,msg2,Sign1,Sign2
       deduce result: d = (s2 - s1) / (s1 - s2 + r1 - r2) mod N
    '''
    r1, s1 = Sign1
    r2, s2 = Sign2
    d = (s2 - s1) * inv((s1 - s2 + r1 - r2), N) % N
    print("d \t\t\t\t(B deduced sk_a)\t\t", '0x' + hex(d)[2:].rjust(64, '0'))
    if d == sk:
        print("d=sk_a, B get true sk_a!!!")
    else:
        print("B not get true sk_a")

    """
        B: forge Sign using deduced sk_a(d)
           forge result: Sign_f
           Verify Sign_f using pk_a
    """
    msg_f = "20000460010"
    print("msg_f\t\t\t(msg for forge from B)\t", msg_f)
    pk_f = EC_multi(d, G)
    ZA_f = precompute(ID, A, B, G_X, G_Y, pk_f[0], pk_f[1])
    Sign_f = sm2_sign(d, msg_f, str(ZA_f))
    print("Sign_f\t\t\t(B Sign msg_f using d)\t", Sign_f)
    print("B Verify using pk_a...")
    if sm2_verify(pk, ID, msg_f, Sign_f) == 1:
        print("pass...forge successfully!")
    else:
        print("false...forge unsuccessfully")


# 【3】两个不同的user使用相同的k,可以相互推测对方的私钥
def same_k_of_different_users():
    # A1和A2使用相同的k签名
    k = secrets.randbelow(N)  # 相同的k值
    """
        A1:KeyGen-->(sk_a1,pk_a1)
           Sign1-->Sig_ska1(msg1)
    """
    sk_a1, pk_a1 = key_gen()
    msg_a1 = "I'm A1"
    ID_a1 = 'A1'
    ZA1 = precompute(ID_a1, A, B, G_X, G_Y, pk_a1[0], pk_a1[1])
    Sign1 = sm2_sign_and_assign_k(k, sk_a1, msg_a1, str(ZA1))
    print("sk_a1 \t\t\t(private key of A1)\t\t", '0x' + hex(sk_a1)[2:].rjust(64, '0'))

    '''
        A2: deduce sk_a1 through msg_a1,Sign1
            deduce result: d1 = (k - s) / (s + r)
    '''
    r1, s1 = Sign1
    d1 = (k - s1) * inv(s1 + r1, N) % N
    print("d1 \t\t\t\t(A2 deduced sk_a)\t\t", '0x' + hex(d1)[2:].rjust(64, '0'))
    if d1 == sk_a1:
        print("d1=sk_a1, A2 get true sk_a1!!!")
    else:
        print("A2 not get true sk_a1")

    """
        A2:KeyGen-->(sk_a2,pk_a2)
           Sign1-->Sig_ska2(msg2)
    """
    sk_a2, pk_a2 = key_gen()
    msg_a2 = "I'm A2"
    ID_a2 = 'A2'
    ZA2 = precompute(ID_a2, A, B, G_X, G_Y, pk_a2[0], pk_a2[1])
    Sign2 = sm2_sign_and_assign_k(k, sk_a2, msg_a2, str(ZA2))
    print("sk_a2 \t\t\t(private key of A2)\t\t", '0x' + hex(sk_a2)[2:].rjust(64, '0'))

    '''
        A1: deduce sk_a2 through msg_a2,Sign2
            deduce result: d2 = (k - s) / (s + r)
    '''
    r2, s2 = Sign2
    d2 = (k - s2) * inv(s2 + r2, N) % N
    print("d2 \t\t\t\t(A1 deduced sk_a)\t\t", '0x' + hex(d2)[2:].rjust(64, '0'))
    if d2 == sk_a2:
        print("d2=sk_a2, A1 get true sk_a2!!!")
    else:
        print("A1 not get true sk_a2")


#【4】 ECDSA与SM2使用相同的d和k导致d泄露
def same_dk_of_ECDSA_SM2():
    # same d and k
    sk, pk = key_gen()
    print("same sk\t\t\t", "0x" + hex(sk)[2:].rjust(64, '0'))
    k = secrets.randbelow(N)
    # ECDSA签名(1)
    message1 = "ECSDA"
    signature1 = ECDSA_sign_and_assign_k(k, message1, sk)
    # SM2签名(2)
    message2 = "sm2"
    ID = 'A'
    ZA = precompute(ID, A, B, G_X, G_Y, pk[0], pk[1])
    signature2 = sm2_sign_and_assign_k(k, sk, message2, str(ZA))

    """
    deduce sk from msg1，msg2，Sign1，Sign2
    deduce result: d = (s1s2 - e1) / (r1 - s1s1 - s1r2)
    """
    r1, s1 = signature1
    r2, s2 = signature2
    e1 = int(sm3.sm3_hash(func.bytes_to_list(bytes(message1, encoding='utf-8'))), 16)
    tmp1 = s1 * s2 - e1 % N
    tmp2 = r1 - s1 * s2 - s1 * r2 % N
    tmp2 = inv(tmp2, N)
    d = tmp1 * tmp2 % N

    print("d (deduced sk)  ", '0x' + hex(d)[2:].rjust(64, '0'))
    if d == sk:
        print("d=sk, get true sk_a!!!")
    else:
        print("not get true sk_a")


if __name__ == '__main__':
    print("===============================k泄露导致d泄露====================================")
    sm2_leaking_k()
    print("")
    print("=======================对不同的消息使用相同的k签名导致d泄露===========================")
    sm2_reusing_k()
    print("")
    print("==================两个不同的user使用相同的k,可以相互推测对方的私钥====================")
    same_k_of_different_users()
    print("")
    print("========================ECDSA与SM2使用相同的d和k导致d泄露=========================")
    same_dk_of_ECDSA_SM2()
