#include "sm4.h"
#include <string.h>
#include<iostream>
#include<immintrin.h>
using namespace std;

/* 用于初始化的0~ff~0 */
static const unsigned char initial[16] = 
{ 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 
 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10 };

/* S盒 */
static const unsigned char SboxTable[16][16] =
{
{0xd6,0x90,0xe9,0xfe,0xcc,0xe1,0x3d,0xb7,0x16,0xb6,0x14,0xc2,0x28,0xfb,0x2c,0x05},
{0x2b,0x67,0x9a,0x76,0x2a,0xbe,0x04,0xc3,0xaa,0x44,0x13,0x26,0x49,0x86,0x06,0x99},
{0x9c,0x42,0x50,0xf4,0x91,0xef,0x98,0x7a,0x33,0x54,0x0b,0x43,0xed,0xcf,0xac,0x62},
{0xe4,0xb3,0x1c,0xa9,0xc9,0x08,0xe8,0x95,0x80,0xdf,0x94,0xfa,0x75,0x8f,0x3f,0xa6},
{0x47,0x07,0xa7,0xfc,0xf3,0x73,0x17,0xba,0x83,0x59,0x3c,0x19,0xe6,0x85,0x4f,0xa8},
{0x68,0x6b,0x81,0xb2,0x71,0x64,0xda,0x8b,0xf8,0xeb,0x0f,0x4b,0x70,0x56,0x9d,0x35},
{0x1e,0x24,0x0e,0x5e,0x63,0x58,0xd1,0xa2,0x25,0x22,0x7c,0x3b,0x01,0x21,0x78,0x87},
{0xd4,0x00,0x46,0x57,0x9f,0xd3,0x27,0x52,0x4c,0x36,0x02,0xe7,0xa0,0xc4,0xc8,0x9e},
{0xea,0xbf,0x8a,0xd2,0x40,0xc7,0x38,0xb5,0xa3,0xf7,0xf2,0xce,0xf9,0x61,0x15,0xa1},
{0xe0,0xae,0x5d,0xa4,0x9b,0x34,0x1a,0x55,0xad,0x93,0x32,0x30,0xf5,0x8c,0xb1,0xe3},
{0x1d,0xf6,0xe2,0x2e,0x82,0x66,0xca,0x60,0xc0,0x29,0x23,0xab,0x0d,0x53,0x4e,0x6f},
{0xd5,0xdb,0x37,0x45,0xde,0xfd,0x8e,0x2f,0x03,0xff,0x6a,0x72,0x6d,0x6c,0x5b,0x51},
{0x8d,0x1b,0xaf,0x92,0xbb,0xdd,0xbc,0x7f,0x11,0xd9,0x5c,0x41,0x1f,0x10,0x5a,0xd8},
{0x0a,0xc1,0x31,0x88,0xa5,0xcd,0x7b,0xbd,0x2d,0x74,0xd0,0x12,0xb8,0xe5,0xb4,0xb0},
{0x89,0x69,0x97,0x4a,0x0c,0x96,0x77,0x7e,0x65,0xb9,0xf1,0x09,0xc5,0x6e,0xc6,0x84},
{0x18,0xf0,0x7d,0xec,0x3a,0xdc,0x4d,0x20,0x79,0xee,0x5f,0x3e,0xd7,0xcb,0x39,0x48}
};

/*  系统参数FK  */
static const unsigned long FK[4] = { 0xa3b1bac6,0x56aa3350,0x677d9197,0xb27022dc };

/*  CK_i,j=(4*i+j)*7%256 ,每次生成轮密钥都要一个ck */
static const unsigned long CK[32] =
{
0x00070e15,0x1c232a31,0x383f464d,0x545b6269,
0x70777e85,0x8c939aa1,0xa8afb6bd,0xc4cbd2d9,
0xe0e7eef5,0xfc030a11,0x181f262d,0x343b4249,
0x50575e65,0x6c737a81,0x888f969d,0xa4abb2b9,
0xc0c7ced5,0xdce3eaf1,0xf8ff060d,0x141b2229,
0x30373e45,0x4c535a61,0x686f767d,0x848b9299,
0xa0a7aeb5,0xbcc3cad1,0xd8dfe6ed,0xf4fb0209,
0x10171e25,0x2c333a41,0x484f565d,0x646b7279
};

/* 返回过S盒的结果
\param  index <unsigned char>  入S盒
\return ans <unsigned char>    出S盒
*/
unsigned char sm4Sbox(unsigned char index)
{
	unsigned char* Sbox = (unsigned char*)SboxTable;
	unsigned char ans = (unsigned char)Sbox[index];
	return ans;
}

/* 无符号char数组转无符号long型
* \param  in <unsigned char*>  无符号char数组
*         out <unsigned long*> 无符号long
*/
void uchar_ulong(unsigned char* in, unsigned long* out)
{
	*out = 0;
	for (int i = 0; i < 4; i++)
		*out = ((unsigned long)in[i] << (24 - i * 8)) ^ *out;
}

/* 无符号long转无符号char数组*/
void ulong_uchar(unsigned long in, unsigned char* out)
{
	int i = 0;
	for (i = 0; i < 4; i++)
		*(out + i) = (unsigned long)(in >> (24 - i * 8));
}

/* 循环左移*/
unsigned long rotleft(unsigned long x, int n)
{
	return ((x & 0xFFFFFFFF) << n) | (x >> (32 - n));
}

/* 交换：在设置解密密钥的时候用到*/
void swap(unsigned long* a, unsigned long* b)
{
	unsigned long tmp;
	tmp = *a;
	*a = *b;
	*b = tmp;
}

/* 合成置换T */
unsigned long sm4T(unsigned long in)
{
	unsigned char sin[4];
	unsigned char sout[4];//非线性变换的输入sin、输出sout
	ulong_uchar(in, sin);
	sout[0] = sm4Sbox(sin[0]);
	sout[1] = sm4Sbox(sin[1]);
	sout[2] = sm4Sbox(sin[2]);
	sout[3] = sm4Sbox(sin[3]);
    unsigned long B, C;//线性变换L的输入B、输出C
	uchar_ulong(sout, &B);
	C = B ^ (rotleft(B, 2)) ^ (rotleft(B, 10)) ^ (rotleft(B, 18)) ^ (rotleft(B, 24));
	return C;
}


/* 轮函数F */
unsigned long sm4F(unsigned long x0, unsigned long x1, unsigned long x2, unsigned long x3, unsigned long rk)
{
	return (x0 ^ sm4T(x1 ^ x2 ^ x3 ^ rk));
}

/* 轮密钥rk的计算*/
unsigned long sm4Cal_rk(unsigned long in)
{
	unsigned char sin[4];
	unsigned char sout[4];
	ulong_uchar(in, sin);
	sout[0] = sm4Sbox(sin[0]);
	sout[1] = sm4Sbox(sin[1]);
	sout[2] = sm4Sbox(sin[2]);
	sout[3] = sm4Sbox(sin[3]);
	unsigned long B, rk;//线性变换L的输入B、输出C
	uchar_ulong(sout, &B);
	rk = B ^ (rotleft(B, 13)) ^ (rotleft(B,23));
	return rk;
}

/* 密钥扩展算法*/
void sm4Setkey(unsigned char key[16], unsigned long SK[32])
{
	unsigned long mainKey[4];
	unsigned long k[36];
	uchar_ulong(key, &mainKey[0]);
	uchar_ulong(key+4, &mainKey[1]);
	uchar_ulong(key+8, &mainKey[2]);
	uchar_ulong(key+12, &mainKey[3]);
	k[0] = mainKey[0] ^ FK[0];
	k[1] = mainKey[1] ^ FK[1];
	k[2] = mainKey[2] ^ FK[2];
	k[3] = mainKey[3] ^ FK[3];
	for (int i = 0; i < 32; i++)
	{
		k[i + 4] = k[i] ^ (sm4Cal_rk(k[i + 1] ^ k[i + 2] ^ k[i + 3] ^ CK[i]));
		SK[i] = k[i + 4];
	}
}

/* 加密的context设置：mode确定是加密enc，轮密钥调用密钥扩展算法生成*/
void sm4Setkey_Enc(sm4Context* ctx,unsigned char key[16])
{
	ctx->mode = SM4_ENCRYPT;
	sm4Setkey(key, ctx->sk);
}

/* 解密的context设置：mode确定是解密dec，轮密钥调用密钥扩展算法生成*/
void sm4Setkey_Dec(sm4Context* ctx, unsigned char key[16])
{
	ctx->mode = SM4_DECRYPT;
	sm4Setkey(key, ctx->sk);
	for (int i = 0; i < 16; i++)
	{
		swap(&(ctx->sk[i]), &(ctx->sk[31 - i]));
	}
}


/* 一轮sm4*/
void sm4_1_Round(unsigned long SK[32],unsigned char plain[16],unsigned char cipher[16])
{
	unsigned long bigX[36];
	uchar_ulong(plain, &bigX[0]);
	uchar_ulong(plain+4, &bigX[1]);
	uchar_ulong(plain+8, &bigX[2]);
	uchar_ulong(plain+12, &bigX[3]);
	for (int i = 0; i < 32; i++)
	{
		bigX[i + 4] = sm4F(bigX[i], bigX[i + 1], bigX[i + 2], bigX[i + 3], SK[i]);
		//cout << "rk" << i << "=";
		//cout << hex << SK[i] << "    X" << i << "=";
		//cout << hex << bigX[i + 4] << endl;
	}
	ulong_uchar(bigX[35], cipher);
	ulong_uchar(bigX[34], cipher+4);
	ulong_uchar(bigX[33], cipher+8);
	ulong_uchar(bigX[32], cipher+12);
}

/* ecb模式 */
void sm4_ecb(unsigned long SK[32], unsigned char* input, unsigned char* output,unsigned long length)
{
	while (length > 0)
	{
		sm4_1_Round(SK, input, output);
		length -= 16;
		input += 16;
		output += 16;
	}
}

/* 展示的函数 */
void shower(unsigned char* show, unsigned long len)
{
	for (int i = 0; i < len; i++)
	{
		int tmp;
		tmp = int(show[i]);
		if (tmp < 16)
		{
			cout << "0";
			cout << hex << int(show[i]) << " ";
		}
		else
		{
			cout << hex << int(show[i]) << " ";
		}
	}
	cout << endl;
}

/* 初始化数组 */
void init(unsigned char* arr, int len)
{
	int i = 0;
	while (i < len)
	{
		for (int index = 0; index < 16; index++)
		{
			arr[i] = initial[index];
			i++;
		}
	}
}
