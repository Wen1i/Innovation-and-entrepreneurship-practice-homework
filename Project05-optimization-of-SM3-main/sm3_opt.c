#include <string.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <iomanip>
#include <memory>
#include <stdint.h>
#include <ctime>
#include <ratio>
#include <chrono>
#include <time.h>
#include <stdlib.h>
#include "sm3_opt.h"

using namespace std;

#define MAX_CHAR_NUM 1024*512
#define MAXSIZE 1024*MAX_CHAR_NUM

unsigned int hash_all = 0;
unsigned int hash_rate = 0;
unsigned int t[64];

static const int endianTest = 1;
#define IsLittleEndian() (*(char *)&endianTest == 1)

#define LeftRotate(word, bits) ( (word) << (bits) | (word) >> (32 - (bits)) )

unsigned int *ReverseWord(unsigned int *word)
{
	unsigned char *byte, temp;

	byte = (unsigned char *)word;
	temp = byte[0];
	byte[0] = byte[3];
	byte[3] = temp;

	temp = byte[1];
	byte[1] = byte[2];
	byte[2] = temp;
	return word;

}
unsigned int T(int i)
{
	if (i >= 0 && i <= 15)
		return 0x79CC4519;
	else if (i >= 16 && i <= 63)
		return 0x7A879D8A;
	else
		return 0;
}


void caculT() {
	for (int i = 0; i < 64; i++) {
		t[i] = LeftRotate(T(i),i);
	}
	return ;
}

/*FF*/
unsigned int FF(unsigned int X, unsigned int Y, unsigned int Z, int i)
{
	if (i >= 0 && i <= 15)
		return X ^ Y ^ Z;
	else if (i >= 16 && i <= 63)
		return (X & Y) | (X & Z) | (Y & Z);
	else
		return 0;
}

/*GG*/
unsigned int GG(unsigned int X, unsigned int Y, unsigned int Z, int i)
{
	if (i >= 0 && i <= 15)
		return X ^ Y ^ Z;
	else if (i >= 16 && i <= 63)
		return (X & Y) | (~X & Z);
	else
		return 0;
}

/*P0*/
unsigned int P0(unsigned int X)
{
	return X ^ LeftRotate(X, 9) ^ LeftRotate(X, 17);
}

/*P1*/
unsigned int P1(unsigned int X)
{
	return X ^ LeftRotate(X, 15) ^ LeftRotate(X, 23);
}


void SM3Init(SM3::SM3Context *context) {
	context->intermediateHash[0] = 0x7380166F;
	context->intermediateHash[1] = 0x4914B2B9;
	context->intermediateHash[2] = 0x172442D7;
	context->intermediateHash[3] = 0xDA8A0600;
	context->intermediateHash[4] = 0xA96F30BC;
	context->intermediateHash[5] = 0x163138AA;
	context->intermediateHash[6] = 0xE38DEE4D;
	context->intermediateHash[7] = 0xB0FB0E4E;
}



void one_round(int i,unsigned int &A, unsigned int &B, unsigned int &C, unsigned int &D,
	unsigned int &E, unsigned int &F, unsigned int &G, unsigned int &H, unsigned int W[68],SM3::SM3Context *context)
{
	unsigned int SS1 = 0, SS2 = 0, TT1 = 0, TT2 = 0;

	if (i < 12) {
		W[i+4] = *(unsigned int *)(context->messageBlock + (i+4) * 4);
		if (IsLittleEndian())
			ReverseWord(W + i + 4);
	}
	else {
		/*P1*/
		W[i+4] = ((W[i - 12] ^ W[i - 5] ^ LeftRotate(W[i + 1], 15)) ^ LeftRotate((W[i - 12] ^ W[i - 5] ^ LeftRotate(W[i + 1], 15)), 15) ^ LeftRotate((W[i - 12] ^ W[i - 5] ^ LeftRotate(W[i + 1], 15)), 23))
			^ LeftRotate(W[i - 9], 7)
			^ W[i - 2];
		}

	TT2 = LeftRotate(A, 12);
	TT1 = TT2 + E + t[i];
	TT1 = LeftRotate(TT1, 7);
	TT2 ^= TT1;

	D = D + FF(A, B, C, i) + TT2 + (W[i] ^ W[i + 4]);
	H = H + GG(E, F, G, i) + TT1 + W[i];
	B = LeftRotate(B, 9);
	F = LeftRotate(F, 19);
	H = H ^ LeftRotate(H, 9) ^ LeftRotate(H, 17);
}

void SM3ProcessMessageBlock(SM3::SM3Context *context)
{
	int i;
	unsigned int W[68];

	unsigned int A, B, C, D, E, F, G, H;


	for (i = 0; i < 4; i++)
	{
		W[i] = *(unsigned int *)(context->messageBlock + i * 4);
		if (IsLittleEndian())
			ReverseWord(W + i);
		//        printf("%d: %x\n", i, W[i]);
	}

	A = context->intermediateHash[0];
	B = context->intermediateHash[1];
	C = context->intermediateHash[2];
	D = context->intermediateHash[3];
	E = context->intermediateHash[4];
	F = context->intermediateHash[5];
	G = context->intermediateHash[6];
	H = context->intermediateHash[7];
	for (i = 0; i <= 60; i+=4)
	{
		one_round(i, A, B, C, D, E, F, G, H, W, context);
		one_round(i+1, D, A, B, C, H, E, F, G, W, context);
		one_round(i+2, C, D, A, B, G, H, E, F, W, context);
		one_round(i+3, B, C, D, A, F, G, H, E, W, context);

	}
	context->intermediateHash[0] ^= A;
	context->intermediateHash[1] ^= B;
	context->intermediateHash[2] ^= C;
	context->intermediateHash[3] ^= D;
	context->intermediateHash[4] ^= E;
	context->intermediateHash[5] ^= F;
	context->intermediateHash[6] ^= G;
	context->intermediateHash[7] ^= H;
}

unsigned char *SM3::SM3Calc(const unsigned char *message,
	unsigned int messageLen, unsigned char digest[SM3_HASH_SIZE])
{
	SM3::SM3Context context;
	unsigned int i, remainder, bitLen;

	SM3Init(&context);ֵ
	hash_all = messageLen / 64 + 1;
	remainder = messageLen % 64;
	if (remainder > 55) {
		hash_all += 1;
	}

	for (i = 0; i < messageLen / 64; i++)
	{
		memcpy(context.messageBlock, message + i * 64, 64);
		hash_rate = i + 1;
		SM3ProcessMessageBlock(&context);
	}


	bitLen = messageLen * 8;
	if (IsLittleEndian())
		ReverseWord(&bitLen);
	memcpy(context.messageBlock, message + i * 64, remainder);
	context.messageBlock[remainder] = 0x80;
	if (remainder <= 55)
	{
		memset(context.messageBlock + remainder + 1, 0, 64 - remainder - 1 - 8 + 4);
		memcpy(context.messageBlock + 64 - 4, &bitLen, 4);
		hash_rate += 1;
		SM3ProcessMessageBlock(&context);
	}
	else
	{
		memset(context.messageBlock + remainder + 1, 0, 64 - remainder - 1);
		hash_rate += 1;
		SM3ProcessMessageBlock(&context);
		memset(context.messageBlock, 0, 64 - 4);
		memcpy(context.messageBlock + 64 - 4, &bitLen, 4);
		hash_rate += 1;
		SM3ProcessMessageBlock(&context);
	}


	if (IsLittleEndian())
		for (i = 0; i < 8; i++)
			ReverseWord(context.intermediateHash + i);
	memcpy(digest, context.intermediateHash, SM3_HASH_SIZE);

	return digest;
}

std::vector<uint32_t> SM3::call_hash_sm3(char *filepath)
{
	std::vector<uint32_t> hash_result(32, 0);
	std::ifstream infile;
	uint32_t FILESIZE = 0;
	unsigned char * buffer = new unsigned char[MAXSIZE];
	unsigned char hash_output[32];
	struct _stat info;
	_stat(filepath, &info);
	FILESIZE = info.st_size;
	infile.open(filepath, std::ifstream::binary);
	infile >> buffer;
	auto start = std::chrono::high_resolution_clock::now();
	SM3::SM3Calc(buffer, FILESIZE, hash_output);
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << "in millisecond time:";
	std::chrono::duration<double, std::ratio<1, 1000>> diff = end - start;
	std::cout << "Time is " << diff.count() << " ms\n";

	hash_result.assign(&hash_output[0], &hash_output[32]);

	delete[]buffer;
	return hash_result;
}


double progress() {
	return (double(hash_rate) / hash_all);
}


void CreatTxt(char* pathName, int length)
{
	ofstream fout(pathName);
	char char_list[] = "abcdefghijklmnopqrstuvwxyz";
	int n = 26;
	if (fout) { 
		for (int i = 0; i < length; i++)
		{
			fout << char_list[rand() % n];
		}

		fout.close(); 
	}
}

int main() {
	char filepath[] = "test.txt";
	CreatTxt(filepath, MAX_CHAR_NUM);
	std::vector<uint32_t> hash_result;
	caculT();
	hash_result = SM3::call_hash_sm3(filepath);
	for (int i = 0; i < 32; i++) {
		std::cout << std::hex << std::setw(2) << std::setfill('0') << hash_result[i];
		if (((i + 1) % 4) == 0) std::cout << " ";
	}
	std::cout << std::endl;

	double rate = progress();
	return 0;
}
