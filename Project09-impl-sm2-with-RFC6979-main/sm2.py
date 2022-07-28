from utils import *


if __name__ == '__main__':
    prikey, pubkey = generate_key()
    print('pk：', pubkey)
    message = input("message = ")
    ID = input("ID = ")
    Z_A = precompute(ID, A, B, G_X, G_Y, pubkey[0], pubkey[1])
    signature = sign(prikey, message, str(Z_A))
    print("sign: ", signature)
    if verify(pubkey, ID, message, signature) == 1:
        print('through verify')
