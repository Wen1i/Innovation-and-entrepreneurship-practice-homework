from Schnorr import *
from gmssl import sm3, func
import secrets


def Schnorr_sign_and_assign_k(k, M, sk):
    R = EC_multi(k, G)
    tmp = str(R[0]) + str(R[1]) + M
    e = int(sm3.sm3_hash(func.bytes_to_list(bytes(tmp, encoding='utf-8'))), 16)
    s = k + e * sk % N
    return (R, s)


def ECDSA_sign_and_assign_k(k, m, sk):
    R = EC_multi(k, G)
    r = R[0] % N  # Rx mod n
    e = sm3.sm3_hash(func.bytes_to_list(bytes(m, encoding='utf-8')))  # e = hash(m)
    e = int(e, 16)
    tmp1 = inv(k, N)
    tmp2 = (e + sk * r) % N
    s = tmp1 * tmp2 % N
    return (r, s)


# 【1】k泄露导致d泄露
def Schnorr_leaking_k():
    """
    A:KeyGen-->(sk_a,pk_a)
       Sign-->Sig_ska(msg)
    """
    sk, pk = key_gen()
    msg_a = "wwl"
    k = secrets.randbelow(N)  # 该k为泄露的k
    signature = Schnorr_sign_and_assign_k(k, msg_a, sk)
    print("sk_a \t\t\t(private key of A)\t\t", '0x' + hex(sk)[2:].rjust(64, '0'))

    """
    B:deduce sk_a from msg,k,Sign
      deduce result: d = (s - k) / d mod N
    """
    R, s = signature
    tmp = str(R[0]) + str(R[1]) + msg_a
    e = int(sm3.sm3_hash(func.bytes_to_list(bytes(tmp, encoding='utf-8'))), 16)
    d = (s - k % N) * inv(e, N) % N
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
    msg_f = "message of B"
    Sign_f = Schnorr_sign(msg_f, d)
    print("B Verify using pk_a...")
    if Schnorr_verify(Sign_f, msg_f, pk) == 1:
        print("pass...forge successfully!")
    else:
        print("false...forge unsuccessfully")

# 【2】对不同的消息使用相同的k签名导致d泄露
def Schnorr_reusing_k():
    """
    A:KeyGen-->(sk_a,pk_a)
       Sign1-->Sig_ska(msg1)
       Sign2-->Sig_ska(msg2)
    """
    sk, pk = key_gen()
    print("sk_a \t\t\t(private key of A)\t\t", '0x' + hex(sk)[2:].rjust(64, '0'))
    msg1 = "sdu"
    msg2 = "wwl"
    k = secrets.randbelow(N)  # 相同的k值
    signature1 = Schnorr_sign_and_assign_k(k, msg1, sk)
    signature2 = Schnorr_sign_and_assign_k(k, msg2, sk)

    '''
       B: deduce sk_a through msg1,msg2,Sign1,Sign2
          deduce result: d = (s1 - s2) / (e1 - e2)
    '''
    R1, s1 = signature1
    R2, s2 = signature2
    if R1 != R2: return 'error'
    R = R1
    tmp = str(R[0]) + str(R[1]) + msg1
    e1 = int(sm3.sm3_hash(func.bytes_to_list(bytes(tmp, encoding='utf-8'))), 16)
    tmp = str(R[0]) + str(R[1]) + msg2
    e2 = int(sm3.sm3_hash(func.bytes_to_list(bytes(tmp, encoding='utf-8'))), 16)
    d = ((s1 - s2) % N) * inv((e1 - e2), N) % N
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
    Sign_f = Schnorr_sign(msg_f, d)
    print("B Verify using pk_a...")
    if Schnorr_verify(Sign_f, msg_f, pk) == 1:
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
    Sign1 = Schnorr_sign_and_assign_k(k, msg_a1, sk_a1)
    print("sk_a1 \t\t\t(private key of A1)\t\t", '0x' + hex(sk_a1)[2:].rjust(64, '0'))

    '''
        A2: deduce sk_a1 through msg_a1,Sign1
            deduce result: d1  = (s - k) / d mod N
    '''
    r1, s1 = Sign1
    tmp = str(r1[0]) + str(r1[1]) + msg_a1
    e1 = int(sm3.sm3_hash(func.bytes_to_list(bytes(tmp, encoding='utf-8'))), 16)
    d1 = (s1 - k % N) * inv(e1, N) % N
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
    Sign2 = Schnorr_sign_and_assign_k(k, msg_a2, sk_a2)
    print("sk_a2 \t\t\t(private key of A2)\t\t", '0x' + hex(sk_a2)[2:].rjust(64, '0'))

    '''
        A1: deduce sk_a2 through msg_a2,Sign2
            deduce result: d2 = (s - k) / d mod N
    '''
    r2, s2 = Sign2
    tmp = str(r2[0]) + str(r2[1]) + msg_a2
    e2 = int(sm3.sm3_hash(func.bytes_to_list(bytes(tmp, encoding='utf-8'))), 16)
    d2 = (s2 - k % N) * inv(e2, N) % N
    print("d2 \t\t\t\t(A1 deduced sk_a)\t\t", '0x' + hex(d2)[2:].rjust(64, '0'))
    if d2 == sk_a2:
        print("d2=sk_a2, A1 get true sk_a2!!!")
    else:
        print("A1 not get true sk_a2")


# 【4】验证(r,s) and (r,-s)均为合法签名
def verify_Malleability():
    # Alice生成消息签名
    sk, pk = key_gen()
    message = "message of A"
    signature = Schnorr_sign(message, sk)
    r, s = signature
    signature_test = (r, -s)
    print("B Verify (r,-s)...")
    if Schnorr_verify(signature_test, message, pk) == 1:
        print("pass!")
    else:
        print("false!")


#【5】 ECDSA与Schnorr使用相同的d和k而泄露d
def same_dk_of_ECDSA_Schnorr():
    # same d and k
    sk, pk = key_gen()
    print("same sk\t\t\t", "0x" + hex(sk)[2:].rjust(64, '0'))
    k = secrets.randbelow(N)
    # ECDSA签名(1)
    message1 = "ECSDA"
    signature1 = ECDSA_sign_and_assign_k(k, message1, sk)
    # Schnorr签名(2)
    message2 = "Schnorr"
    signature2 = Schnorr_sign_and_assign_k(k, message2, sk)

    """
    deduce sk from msg1，msg2，Sign1，Sign2
    deduce result: d = (s2 - e1 / s1) / (r / s1 + e2)
    """

    r, s1 = signature1
    R, s2 = signature2
    e1 = int(sm3.sm3_hash(func.bytes_to_list(bytes(message1, encoding='utf-8'))), 16)
    tmp = str(R[0]) + str(R[1]) + message2
    e2 = int(sm3.sm3_hash(func.bytes_to_list(bytes(tmp, encoding='utf-8'))), 16)
    tmp1 = (s2 - inv(s1, N) * e1) % N
    tmp2 = (inv(s1, N) * r + e2) % N
    d = tmp1 * inv(tmp2, N) % N
    print("d (deduced sk)  ", '0x' + hex(d)[2:].rjust(64, '0'))
    if d == sk:
        print("d=sk, get true sk_a!!!")
    else:
        print("not get true sk_a")


if __name__ == '__main__':
    print("===============================k泄露导致d泄露====================================")
    Schnorr_leaking_k()
    print("")
    print("=======================对不同的消息使用相同的k签名导致d泄露===========================")
    Schnorr_reusing_k()
    print("")
    print("==================两个不同的user使用相同的k,可以相互推测对方的私钥====================")
    same_k_of_different_users()
    print("")
    print("=======================验证(r,s) and (r,-s)均为合法签名=========================")
    verify_Malleability()
    print("")
    print("========================ECDSA与SM2使用相同的d和k导致d泄露=========================")
    same_dk_of_ECDSA_Schnorr()

