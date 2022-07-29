from utils import *
# 生成Satoshi的公私钥
sk, pk = key_gen()
print("public key of Satoshi:", pk)
# 伪造签名
u = random.randrange(1, N - 1)
v = random.randrange(1, N - 1)
R = EC_add(EC_multi(u, G), EC_multi(v, pk))
r = R[0] % N
s = (r * inv(v, N)) % N
sign = (r, s)
print("signature:", sign)
# 验证签名
print("verify the signature with pk...")
e = (r * u * inv(v, N)) % N
if (EC_multi(inv(s, N), EC_add(EC_multi(e, G), EC_multi(r, pk))))[0] % N == r:
    print("signature is legal!")
else:
    print("signature is illegal")