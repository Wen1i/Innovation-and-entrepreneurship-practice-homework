
import math


class Word:
    def __init__(self, x):
        self.data = 0
        if isinstance(x, int) or isinstance(x, float):
            self.data = int(x) & 0xffffffff
        elif isinstance(x, str):
            assert len(x) == 4
            self.data = (ord(x[3]) << 24) | \
                        (ord(x[2]) << 16) | \
                        (ord(x[1]) << 8) | \
                        ord(x[0])
        else:
            raise Exception("word input must be int or str")

    def __repr__(self):
        return "Word(%s)" % self.data

    def __str__(self):
        return "%08x" % self.data

    def __add__(self, other):
        if isinstance(other, int):
            other = Word(other)
        if not isinstance(other, Word):
            raise Exception("word must add word or int")
        return Word(self.data + other.data)

    def __lshift__(self, other):
        assert isinstance(other, int)
        other %= 32
        return Word((self.data << other) | (self.data >> (32 - other)))

    def __xor__(self, other):
        if isinstance(other, int):
            other = Word(other)
        if not isinstance(other, Word):
            raise Exception("word must xor word or int")
        return Word(self.data ^ other.data)

    def __or__(self, other):
        if isinstance(other, int):
            other = Word(other)
        if not isinstance(other, Word):
            raise Exception("word must or word or int")
        return Word(self.data | other.data)

    def __and__(self, other):
        if isinstance(other, int):
            other = Word(other)
        if not isinstance(other, Word):
            raise Exception("word must and word or int")
        return Word(self.data & other.data)

    def __invert__(self):
        return Word(~self.data)

    def to_bin_str(self):
        return "".join([chr(self.data & 0xff),
                        chr((self.data >> 8) & 0xff),
                        chr((self.data >> 16) & 0xff),
                        chr((self.data >> 24) & 0xff)
                        ])

    def to_hex_byte_str(self):
        return "".join(["%02x" % ord(c) for c in self.to_bin_str()])


class MD5:
    T = [Word(0)] + [Word(int(4294967296 * abs(math.sin(i)))) for i in range(1, 65)]
    ROUNDS = [item.strip()[1:-1].split() for item in """
    [ABCD  0  7  1]
    [DABC  1 12  2]
    [CDAB  2 17  3]
    [BCDA  3 22  4]
    [ABCD  4  7  5]
    [DABC  5 12  6]
    [CDAB  6 17  7]
    [BCDA  7 22  8]
    [ABCD  8  7  9]
    [DABC  9 12 10]
    [CDAB 10 17 11]
    [BCDA 11 22 12]
    [ABCD 12  7 13]
    [DABC 13 12 14]
    [CDAB 14 17 15]
    [BCDA 15 22 16]
    [ABCD  1  5 17]
    [DABC  6  9 18]
    [CDAB 11 14 19]
    [BCDA  0 20 20]
    [ABCD  5  5 21]
    [DABC 10  9 22]
    [CDAB 15 14 23]
    [BCDA  4 20 24]
    [ABCD  9  5 25]
    [DABC 14  9 26]
    [CDAB  3 14 27]
    [BCDA  8 20 28]
    [ABCD 13  5 29]
    [DABC  2  9 30]
    [CDAB  7 14 31]
    [BCDA 12 20 32]
    [ABCD  5  4 33]
    [DABC  8 11 34]
    [CDAB 11 16 35]
    [BCDA 14 23 36]
    [ABCD  1  4 37]
    [DABC  4 11 38]
    [CDAB  7 16 39]
    [BCDA 10 23 40]
    [ABCD 13  4 41]
    [DABC  0 11 42]
    [CDAB  3 16 43]
    [BCDA  6 23 44]
    [ABCD  9  4 45]
    [DABC 12 11 46]
    [CDAB 15 16 47]
    [BCDA  2 23 48]
    [ABCD  0  6 49]
    [DABC  7 10 50]
    [CDAB 14 15 51]
    [BCDA  5 21 52]
    [ABCD 12  6 53]
    [DABC  3 10 54]
    [CDAB 10 15 55]
    [BCDA  1 21 56]
    [ABCD  8  6 57]
    [DABC 15 10 58]
    [CDAB  6 15 59]
    [BCDA 13 21 60]
    [ABCD  4  6 61]
    [DABC 11 10 62]
    [CDAB  2 15 63]
    [BCDA  9 21 64]
    """.strip().split("\n")]
    for round in ROUNDS:
        for i in range(1, 4):
            round[i] = int(round[i])

    def __init__(self):
        pass

    def md5(self, s,
            a=Word("\x01\x23\x45\x67"),
            b=Word("\x89\xab\xcd\xef"),
            c=Word("\xfe\xdc\xba\x98"),
            d=Word("\x76\x54\x32\x10"),
            debug=False):
        self.debug = debug
        extend_data = self.get_padding_bits(s) + self.get_append_length(s)
        if self.debug:
            print("extend data: %s" % repr(extend_data))
        s += extend_data
        self.init_md_buffer(a, b, c, d)
        for i in range(0, len(s), 64):
            self.deal_one_group(s[i:i + 64])
        return str(self)

    def extend_length_attack(self, last_md5, last_group_data, length, extend_data, debug=False):
        """
        :param last_md5: 上一个MD5字符串
        :param last_group_data: 上一组数据的最后一些字写
        :param length: 被用来计算MD5字节数
        :param extend_data: 拓展数据 < 56字节
        :param debug: 是否开启debug
        :return: (拓展数据, 新的md5值)
        """
        self.debug = debug
        assert len(last_md5) == 32

        last_md5 = [Word(int("%s%s%s%s" % (last_md5[8*i+6:8*i+8],
                                              last_md5[8*i+4:8*i+6],
                                              last_md5[8*i+2:8*i+4],
                                              last_md5[8*i:8*i+2]), 16)) for i in range(4)]
        self.init_md_buffer(last_md5[0], last_md5[1], last_md5[2], last_md5[3])
        before_extend_data = last_group_data + "\x80"
        length_word = self.get_append_length(length=length)
        length -= len(last_group_data)
        if length % 64 <= 56:
            before_extend_data += "\x00" * (56 - len(before_extend_data) - length % 64) + length_word
            length = (length // 64 + 1) * 64 + len(extend_data)
        elif length % 64 == 0:
            before_extend_data += "\x00" * (56 - len(before_extend_data)) + length_word
            length = (length // 64 + 1) * 64 + len(extend_data)
        else:
            before_extend_data += "\x00" * (56 - len(before_extend_data) + (64 - length % 64)) + length_word
            length = (length // 64 + 2) * 64 + len(extend_data)
        s = extend_data + self.get_padding_bits(extend_data) + self.get_append_length(length=length)
        self.deal_one_group(s)
        return before_extend_data + extend_data, str(self)

    def init_md_buffer(self,
                       a,
                       b,
                       c,
                       d):
        self.A = a
        self.B = b
        self.C = c
        self.D = d

    def get_padding_bits(self, s):

        app_size = len(s) % 64 - 56
        if app_size < 0:
            return "\x80" + "\x00" * (-app_size - 1)
        elif app_size > 0:
            return "\x80" + "\x00" * (63 - app_size)
        else:
            return ""

    def get_append_length(self, s=None, length=-1):
        assert s is not None or length >= 0
        if length < 0:
            length = len(s)
        sz = length * 8
        high_word = Word((sz & 0xffffffff00000000) >> 32)
        low_word = Word(sz & 0xffffffff)
        return low_word.to_bin_str() + high_word.to_bin_str()


    @staticmethod
    def F(x: Word, y: Word, z: Word):
        return (x & y) | (~x & z)

    @staticmethod
    def G(x: Word, y: Word, z: Word):
        return (x & z) | (y & ~z)

    @staticmethod
    def H(x: Word, y: Word, z: Word):
        return x ^ y ^ z

    @staticmethod
    def I(x: Word, y: Word, z: Word):
        return y ^ (x | ~z)

    def __(self, i, a: Word, b: Word, c: Word, d: Word, m: Word, s: int, t: int, f):
        if self.debug:
            print("step %s: a=%s, b=%s, c=%s, d=%s, m=%s, s=%s, t=%s, f=%s" % (
            i, str(a), str(b), str(c), str(d), str(m), s, t, f.__name__))
        return b + ((f(b, c, d) + a + m + t) << s)

    def __run_round(self, idx, ms):
        FUNC_MAP = [MD5.F, MD5.G, MD5.H, MD5.I]
        cur_round = MD5.ROUNDS[idx]
        m = ms[cur_round[1]]
        self.__dict__[cur_round[0][0]] = self.__(idx,
                                                 self.__dict__[cur_round[0][0]],
                                                 self.__dict__[cur_round[0][1]],
                                                 self.__dict__[cur_round[0][2]],
                                                 self.__dict__[cur_round[0][3]],
                                                 m,
                                                 cur_round[2],
                                                 MD5.T[cur_round[3]],
                                                 FUNC_MAP[(cur_round[3] - 1) // 16])

    def deal_one_group(self, s: str):
        """
        :param s: 一组数据 64字节
        :param i: 当前是第i组数据
        :return:
        """
        AA, BB, CC, DD = self.A, self.B, self.C, self.D
        ms = [Word(s[i:i + 4]) for i in range(0, len(s), 4)]
        for i in range(64):
            self.__run_round(i, ms)
        self.A += AA
        self.B += BB
        self.C += CC
        self.D += DD

    def __str__(self):
        return "".join([self.A.to_hex_byte_str(),
                        self.B.to_hex_byte_str(),
                        self.C.to_hex_byte_str(),
                        self.D.to_hex_byte_str()])


if __name__ == '__main__':
    md5 = MD5()
    test_strs = ["",  # test len(s) = 0
                 "123",  # test len(s) < 56
                 "1234567890"*5 + "123456",  # test len(s) = 56
                 "1234567890"*6,  # test 56 < len(s) < 64
                 "1234567890"*6 + "1234",  # test len(s) = 64
                 "1234567890"*7,  # test len(s) > 64
                 ]
    for test_str in test_strs:
        test_length = len(test_str)
        last_md5 = md5.md5(test_str)
        extend_data, new_hash = md5.extend_length_attack(last_md5, test_str[-2:] if test_str else "", test_length, "hack")
        print(md5.md5(test_str[:-2] + extend_data), new_hash)