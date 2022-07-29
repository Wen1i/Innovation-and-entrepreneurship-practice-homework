from ECDSA import *
import secrets
from gmssl import sm3, func

def ECDSA_sign_and_return_k(m, sk):
    while 1:
        k = secrets.randbelow(N)  # N is prime, then k <- Zn*
        R = EC_multi(k, G)
        r = R[0] % N  # Rx mod n
        if r != 0: break
    e = sm3.sm3_hash(func.bytes_to_list(bytes(m, encoding='utf-8')))  # e = hash(m)
    e = int(e, 16)
    s = (inv(k, N) * (e + sk * r) % N) % N
    return (r, s), k


def ECDSA_sign_and_assign_k(m, k, sk):
    R = EC_multi(k, G)
    r = R[0] % N  # Rx mod n
    e = sm3.sm3_hash(func.bytes_to_list(bytes(m, encoding='utf-8')))  # e = hash(m)
    e = int(e, 16)
    tmp1 = inv(k, N)
    tmp2 = (e + sk * r) % N
    s = tmp1 * tmp2 % N
    return (r, s), k


# 【1】k泄露导致d泄露
def ECDSA_leaking_k():
    """
    A:KeyGen-->(sk_a,pk_a)
       Sign-->Sig_ska(msg)
    """
    sk, pk = key_gen()
    msg_a="wwl"
    signature, k = ECDSA_sign_and_return_k(msg_a, sk)
    print("sk_a \t\t\t(private key of A)\t\t", '0x' + hex(sk)[2:].rjust(64, '0'))

    """
    B:deduce sk_a from msg,k,Sign
      deduce result: d =  (s * k - e) / r
    """
    r, s = signature
    e = sm3.sm3_hash(func.bytes_to_list(bytes(msg_a, encoding='utf-8')))  # e = hash(m)
    e = int(e, 16)
    d = (s * k - e) % N * inv(r, N) % N
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
    msg_f = "hhh"
    Sign_f = ECDSA_sign(msg_f, d)
    print("B Verify using pk_a...")
    if ECDSA_verify(Sign_f, msg_f, pk) == 1:
        print("pass...forge successfully!")
    else:
        print("false...forge unsuccessfully")


# 【2】对不同的消息使用相同的k签名导致d泄露
def ECDSA_reusing_k():
    """
    A:KeyGen-->(sk_a,pk_a)
       Sign1-->Sig_ska(msg1)
       Sign2-->Sig_ska(msg2)
    """
    sk, pk = key_gen()
    print("sk_a \t\t\t(private key of A)\t\t", '0x' + hex(sk)[2:].rjust(64, '0'))
    msg1 = "sdu"
    msg2 = "wwl"
    signature1, k1 = ECDSA_sign_and_return_k(msg1, sk)
    signature2, k2 = ECDSA_sign_and_assign_k(msg2, k1, sk)

    '''
           B: deduce sk_a through msg1,msg2,Sign1,Sign2
              deduce result: d = [(s1 - s2) * k - (e1 - e2)] / (r1 - r2)
    '''

    r1, s1 = signature1
    r2, s2 = signature2
    r = r1
    e1 = sm3.sm3_hash(func.bytes_to_list(bytes(msg1, encoding='utf-8')))  # e = hash(m)
    e1 = int(e1, 16)
    e2 = sm3.sm3_hash(func.bytes_to_list(bytes(msg2, encoding='utf-8')))  # e = hash(m)
    e2 = int(e2, 16)
    d = (((e1 - e2) * s2) % N * inv((s1 - s2) % N, N) - e2) * inv(r, N) % N
    print("d \t\t\t\t(B deduced sk_a)\t\t", '0x' + hex(d)[2:].rjust(64, '0'))
    if d == sk:
        print("d=sk_a, B get true sk_a!!!")
    else:
        print("B not get true sk_a")
    if k1 == k2:
        print("Sign1 Sign2 use same k")
    else:
        print("Sign1 Sign2 not use same k")

    """
        B: forge Sign using deduced sk_a(d)
           forge result: Sign_f
           Verify Sign_f using pk_a
    """
    msg_f = "20000460010"
    Sign_f = ECDSA_sign(msg_f, d)
    print("B Verify using pk_a...")
    if ECDSA_verify(Sign_f, msg_f, pk) == 1:
        print("pass...forge successfully!")
    else:
        print("false...forge unsuccessfully")


# 【3】两个不同的user使用相同的k,可以相互推测对方的私钥
def same_k_of_different_users():
    # A1和A2使用相同的k签名
    """
            A1:KeyGen-->(sk_a1,pk_a1)
               Sign1-->Sig_ska1(msg1)
    """
    sk_a1, pk_a1 = key_gen()
    msg_a1 = "I'm A1"
    Sign1, k = ECDSA_sign_and_return_k(msg_a1, sk_a1)
    print("sk_a1 \t\t\t(private key of A1)\t\t", '0x' + hex(sk_a1)[2:].rjust(64, '0'))

    '''
        A2: deduce sk_a1 through msg_a1,Sign1
            deduce result: d1 = (s * k - e) / r
    '''
    r1, s1 = Sign1
    e1 = sm3.sm3_hash(func.bytes_to_list(bytes(msg_a1, encoding='utf-8')))  # e = hash(m)
    e1 = int(e1, 16)
    d1 = (s1 * k - e1) % N * inv(r1, N) % N
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
    Sign2,k = ECDSA_sign_and_return_k(msg_a2, sk_a2)
    print("sk_a2 \t\t\t(private key of A2)\t\t", '0x' + hex(sk_a2)[2:].rjust(64, '0'))

    '''
        A1: deduce sk_a2 through msg_a2,Sign2
            deduce result: = (s * k - e) / r
    '''
    r2, s2 = Sign2
    e2 = sm3.sm3_hash(func.bytes_to_list(bytes(msg_a2, encoding='utf-8')))  # e = hash(m)
    e2 = int(e2, 16)
    d2 = (s2 * k - e2) % N * inv(r2, N) % N
    print("d2 \t\t\t\t(A1 deduced sk_a)\t\t", '0x' + hex(d2)[2:].rjust(64, '0'))
    if d2 == sk_a2:
        print("d2=sk_a2, A1 get true sk_a2!!!")
    else:
        print("A1 not get true sk_a2")

# 【4】验证(r,s) and (r,-s)均为合法签名
def verify_Malleability():
    # Alice生成消息签名
    sk, pk = key_gen()
    message = "wwl"
    signature = ECDSA_sign(message, sk)
    r, s = signature
    signature_test = (r, -s)
    print(" Verify (r,-s)...")
    if ECDSA_verify(signature_test, message, pk) == 1:
        print("pass!")
    else:
        print("false!")


if __name__ == '__main__':
    print("===============================k泄露导致d泄露====================================")
    ECDSA_leaking_k()
    print("")
    print("=======================对不同的消息使用相同的k签名导致d泄露===========================")
    ECDSA_reusing_k()
    print("")
    print("==================两个不同的user使用相同的k,可以相互推测对方的私钥====================")
    same_k_of_different_users()
    print("")
    print("=======================验证(r,s) and (r,-s)均为合法签名=========================")
    verify_Malleability()