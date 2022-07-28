from hashlib import sha256
import string
import random

map = string.punctuation + string.ascii_letters + string.digits


def Hash(data):
    return sha256(data.encode('utf-8')).hexdigest()


def con0(d0):
    return hex(0) + d0


def con1(d1, d2):
    return hex(1) + d1 + d2


class Merkel:
    def __init__(self, data):
        self.l = data
        self.root, self.LR = self.merkel()
        print('root hash:', self.root)
        print(self.LR)

    def merkel(self):
        lst = []
        LR = dict()  # 左叶子节点为0，右叶子节点为1
        h = 0
        if len(self.l) == 0:
            lst = sha256().hexdigest()
            print('depth:0')
            return lst, {}
        elif len(self.l) == 1:
            lst.append(Hash(con0(self.l[0])))
            print('depth:1')
            return lst, {}
        else:
            for i in self.l:
                lst.append(Hash(i))

            while len(lst) > 1:
                h += 1
                temp = []
                if len(lst) % 2 == 0:
                    while len(lst) > 1:
                        a = lst.pop(0)
                        LR[a] = 0
                        b = lst.pop(0)
                        LR[b] = 1
                        temp.append(Hash(con1(a, b)))
                    lst = temp
                else:
                    last = lst.pop(-1)
                    while len(lst) > 1:
                        a = lst.pop(0)
                        LR[a] = 0
                        b = lst.pop(0)
                        LR[b] = 1
                        temp.append(Hash(con1(a, b)))
                    temp.append(last)
                    LR[last] = 1
                    lst = temp
            print('depth:', h + 1)
            return lst[0], LR

    def aupath(self, m, l):
        k = 0
        P = []
        if len(l) == 2:
            P.append(l[(m + 1) % 2])
            return P
        elif len(l) > 2:
            for i in range(1, len(l)):
                if pow(2, i) >= len(l):
                    k = pow(2, i - 1)
                    break
            if m < k:
                P.extend(self.aupath(m, l[0:k]))
                P.append(self.mth(l[k:len(l)]))
                return P
            elif m >= k:
                P.extend(self.aupath(m - k, l[k:len(l)]))
                P.append(self.mth(l[0:k]))
                return P
        else:
            return P

    def AuditPaths(self, m):
        lst = []
        if len(self.l) > 1:
            for i in range(0, len(self.l)):
                lst.append(Hash(self.l[i]))
            return self.aupath(m, lst)
        elif len(self.l) == 1:
            return {}
        else:
            return -1

    def mt(self, l):
        # print('len:', len(l))
        k = 0
        if len(l) == 1:
            return l[0]
        elif len(l) == 2:
            return Hash(con1(l[0], l[1]))
        else:
            for i in range(0, len(l)):
                if pow(2, i) >= len(l):
                    k = pow(2, i - 1)
                    break
            return Hash(con1(self.mt(l[0:k]), self.mt(l[k:len(l)])))

    def mth(self, l):
        L = []
        h = 0
        if len(l) == 1:
            L = l[0]
            return L

        elif len(l) > 1:
            return self.mt(l)

    def proof(self, m, leaf):
        audit_path = self.AuditPaths(m)
        print('路径:', audit_path)

        leafhash = Hash(leaf)
        for i in audit_path:
            if self.LR[i] == 0:
                leafhash = Hash(con1(i, leafhash))
            else:
                leafhash = Hash(con1(leafhash, i))

        if leafhash == self.root:
            return True
        else:
            return False


def test(length):
    # 生成随机节点
    longrandom = ''
    for i in range(length):
        longrandom += random.choice(map)
    allstr = [longrandom[i:i + 2] for i in range(0, len(longrandom))]
    print("leaves:", allstr)
    print("nums", len(allstr))
    print("root:", Merkel(allstr).root)


if __name__ == '__main__':

    # 生成100000个节点的Merkel Tree
    test(100000)
    # 存在性证明
    l = ['1', '2', '3', '4', '5']
    ex = Merkel(l)
    print(ex.proof(1, '2'))



