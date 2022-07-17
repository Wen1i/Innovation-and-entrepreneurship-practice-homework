#coding:utf-8

#y^2=x^3+7
A = 0
B = 7

# 有限域的阶
P = 115792089237316195423570985008687907853269984665640564039457584007908834671663
# 椭圆曲线的阶
N = 115792089237316195423570985008687907852837564279074904382605163141518161494337


'''计算二次剩余 满足x^2=y mod p的x'''
def Legendre(y,p): # 判断二次（非）剩余
    return pow(y,(p - 1) // 2,p)
def Tonelli_Shanks(y,p):
    assert Legendre(y,p) == 1
    if p % 4 == 3:
        return pow(y,(p + 1) // 4,p)
    q = p - 1
    s = 0
    while q % 2 == 0:
        q = q // 2
        s += 1
    for z in range(2,p):
        if Legendre(z,p) == p - 1:
            c = pow(z,q,p)
            break
    r = pow(y,(q + 1) // 2,p)
    t = pow(y,q,p)
    m = s
    if t % p == 1:
        return r
    else:
        i = 0
        while t % p != 1: # 外层循环的判断条件
            temp = pow(t,2**(i+1),p)
            i += 1
            if temp % p == 1:
                b = pow(c,2**(m - i - 1),p)
                r = r * b % p
                c = b * b % p
                t = t * c % p
                m = i
                i = 0 # 每次内层循环结束后i值要更新为0
        return r


'''欧几里得求逆元'''
def extended_euclidean_algorithm(j, k):
    if j == k:
        return (j, 1, 0)
    else:
        i = 0
        j_array = [j]
        k_array = [k]
        q_array = []
        r_array = []

        prev_r_is_zero = False

        while not (prev_r_is_zero):
            q_array.append(k_array[i]//j_array[i])
            r_array.append(k_array[i]%j_array[i])
            k_array.append(j_array[i])
            j_array.append(r_array[i])
            i += 1
            if r_array[i-1] == 0:
                prev_r_is_zero = True
        i -= 1
        gcd = j_array[i]
        x_array = [1]
        y_array = [0]

        i -= 1
        total_steps = i

        while i >= 0:
            y_array.append(x_array[total_steps-i])
            x_array.append(y_array[total_steps-i] - q_array[i]*x_array[total_steps-i])
            i -= 1

        return (gcd, x_array[-1], y_array[-1])

def mod_inverse(j, n):
    (gcd, x, y) = extended_euclidean_algorithm(j, n)

    if gcd == 1:
        return x%n
    else:
        return -1


'''椭圆曲线加、乘'''
def elliptic_add(p, q):
    if p == 0 and q == 0: return 0
    elif p == 0: return q
    elif q == 0: return p
    else:
        # Swap p and q if px > qx.
        if p[0] > q[0]:
            temp = p
            p = q
            q = temp
        r = []
        slope = (q[1] - p[1])*mod_inverse(q[0] - p[0], P) % P

        r.append((slope**2 - p[0] - q[0]) % P)
        r.append((slope*(p[0] - r[0]) - p[1]) % P)

        return (r[0], r[1])


def elliptic_double(p):
    r = []

    slope = (3*p[0]**2 + A)*mod_inverse(2*p[1], P) % P

    r.append((slope**2 - 2*p[0])%P)
    r.append((slope*(p[0] - r[0]) - p[1])%P)

    return (r[0], r[1])

def elliptic_multiply(s, p):
    n = p
    r = 0 # 0 representing a point at infinity

    s_binary = bin(s)[2:] # convert s to binary and remove the "0b" in the beginning
    s_length = len(s_binary)

    for i in reversed(range(s_length)):
        if s_binary[i] == '1':
            r = elliptic_add(r, n)
        n = elliptic_double(n)

    return r
