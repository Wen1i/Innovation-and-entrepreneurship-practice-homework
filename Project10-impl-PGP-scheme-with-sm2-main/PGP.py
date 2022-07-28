import base64
from gmssl import sm2, func
from gmssl.sm4 import CryptSM4, SM4_ENCRYPT, SM4_DECRYPT


print("------------------------Sender 加密--------------------------")
# 由sm2utils生成SM2的密钥
sk_s = "00B9AB0B828FF68872F21A837FC303668428DEA11DCD1B24429D0C99E24EED83D5"
pk_s = "B9C9A6E04E9C91F7BA880429273747D7EF5DDEB0BB2FF6317EB00BEF331A83081A6994B8993F3F5D6EADDDB81872266C87C018FB4162F5AF347B483E24620207"
sk_r = "228a9707053e1b333fb8cb839567a9db4ca1cf5381e9a6a539774e6c3563cdfa"
pk_r = "893cb9392dabd2fac095f657a7e0bc308e32f4b79380d478547f57123dccb3bc4a3a2d009f5826b6624c99dd41baf470a8baf46722f2d36b1d26f19af112c5cd"
print("Generate sm2Key......")
print("pk_s:",pk_s,"\npk_r:",pk_r)

# 创建sm4Key和sm4对象
print("\nGenerate Session Key......")
sm4Key_str = func.random_hex(16)  # 随机产生16位 'a2bc65bca8e41795'
sm4Key = bytes(sm4Key_str, encoding='utf-8')  # 字符串转bytes
sm4_crypt = CryptSM4()  # 初始化CryptSM4
sm4_crypt.set_key(sm4Key, mode=SM4_ENCRYPT)  # 初始化key到CryptSM4 这里传的是SM_DECRYPT
print("Session key：", sm4Key)

# 创建sm2对象,用pk_r对sm4Key进行加密
print("\nEnc Session key with pk_r......")
sm2_crypt = sm2.CryptSM2(private_key=None, public_key=pk_r)  # 附公钥
encryptKey = sm2_crypt.encrypt(sm4Key)  # 对sm4Key(bytes) 进行加密,返回bytes
encryptKey = base64.b64encode(encryptKey)  # bytes 转base64
print("encryptKey",encryptKey)

# sm4对象对数据进行加密
print("\nEnc data with sm4Key......")
data = input("data:")
data = data.encode("utf-8")  # 列表转字符串转bytes
encryptData = sm4_crypt.crypt_ecb(data)  # 对数据(bytes)加密
encryptData = base64.b64encode(encryptData)  # bytes 转base64
encryptData = encryptData.decode("utf-8")  # 由于转为base64,还是bytes,json不支持,故转为str
print("encryptData:",encryptData)

# 用sk_s对sm4Key签名
print("\nSign Session Key with sk_s......")
sm2_crypt_s = sm2.CryptSM2(private_key=sk_s, public_key=None)  # 私钥签名, 公钥验证
random_hex_str = func.random_hex(sm2_crypt.para_len)  # para_len为64, 生成64位随机字符串
sign = sm2_crypt_s.sign(sm4Key, random_hex_str)  # 签名需要用私钥 , sign为字符串格式
sign = base64.b64encode(bytes(sign, encoding='utf-8'))  # 字符串转base64
sign = sign.decode("utf-8")  # bytes转字符串
print("sign:",sign)

print("\nsender send msg(encryptData||encryptKey||sign) to receiver......")
result = {"encryptData":encryptData,"encryptKey":encryptKey,"sign":sign}
print(result)


print("------------------------Receiver 解密--------------------------")
sign = base64.b64decode(result['sign'])
encryptKey =base64.b64decode(result["encryptKey"])
encryptData = base64.b64decode(result["encryptData"])

# sm2(附私钥)解密sm4Key
print("\nDec encryptKey with sk_r to obtain Session Key......")
sm2_crypt_r =sm2.CryptSM2(private_key=sk_r,public_key=None)
sm4Key_r = sm2_crypt_r.decrypt(encryptKey)  # 解密, 返回bytes
print("Session Key that receiver obtain:",sm4Key_r)

# 公钥签名验证
print("\nVerify Sign with pk_s.....")
sm2_crypt_sr =sm2.CryptSM2(private_key=None,public_key=pk_s)
assert sm2_crypt_sr.verify(sign,sm4Key_r)

# sm4解密得到data
print("\nDec encryptData with Session Key to obtain data.....")
sm4_crypt_r=CryptSM4()
sm4_crypt_r.set_key(sm4Key_r,mode=SM4_DECRYPT)
data_r=sm4_crypt_r.crypt_ecb(encryptData)
data_r = data_r.decode('utf-8')
print("data that receiver obtain:",data_r)
