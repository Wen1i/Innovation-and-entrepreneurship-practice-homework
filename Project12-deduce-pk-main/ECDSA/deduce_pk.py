import secrets
from gmssl import sm3, func
from ECDSA_utils import *

def deduce_pk_from_sig(signature, msg):
    r, s = signature
    # kG = R = (x, y)
    x = r % P
    y2 = pow(x, 3) + A * x + B
    y = Tonelli_Shanks(y2, P)
    R1 = (x, y)
    R2 = (x, P - y)
    e = sm3.sm3_hash(func.bytes_to_list(bytes(msg, encoding='utf-8')))
    pk1 = EC_multi(inv(r, N), EC_sub(EC_multi(s, R1), EC_multi(int(e, 16), G)))
    pk2 = EC_multi(inv(r, N), EC_sub(EC_multi(s, R2), EC_multi(int(e, 16), G)))
    return pk1,pk2


if __name__ == '__main__':
    sk, pk = key_gen()
    print('true pk:',pk)
    msg = 'wwl202000460010'
    sign = ECDSA_sign(msg,sk)
    print("Sign:",sign)
    pk1, pk2 = deduce_pubkey_from_sign(sign, msg)
    print('deduce pk according to signature......')
    print('Candidate pk1:', pk1)
    print('Candidate pk2:', pk2)

