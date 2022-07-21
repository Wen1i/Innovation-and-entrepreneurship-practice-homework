#include "sm3.h"
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <emmintrin.h>
#include <smmintrin.h>
/*
#define u32_2_u8(dwValue, pbBuffer) { (pbBuffer)[3] = (u8)(dwValue), \
	                              (pbBuffer)[2] = (u8)((dwValue)>>8); \
				      (pbBuffer)[1] = (u8)((dwValue)>>16); \
				      (pbBuffer)[0] = (u8)((dwValue)>>24); }
*/
#define u32_2_u8(dwValue, pbBuffer) 
{
	

#define T0 (0x79cc4519)
#define T1 (0x7a879d8a)
#define FF0(X, Y, Z) ( (X) ^ (Y) ^ (Z) )
#define FF1(X, Y, Z) ( ((X)&(Y)) | ((X)&(Z)) | ((Y)&(Z)) )
#define GG0(X, Y, Z) ( (X) ^ (Y) ^ (Z) )
#define GG1(X, Y, Z) ( ((X)&(Y)) | ((~(X))&(Z)) )
#define remove_bit(X, cnt)  ( ( (X)<<((cnt)&31) ) | ( (X)>>(32-((cnt)&31)) ) )
#define P0(X)		 ( (X) ^ remove_bit(X,  9) ^ remove_bit(X, 17))
#define P1(X)		 ( (X) ^ remove_bit(X, 15) ^ remove_bit(X, 23))

static int OP_one_group(u8 *pbData, SM3_content* pSm3Ctx);

int SM3_HASH_INIT(SM3_content* sm3_content)
{
	sm3_content->m_dwLastLen = 0x00;
	sm3_content->m_dwGroupNum = 0x00;
	sm3_content->m_AA = 0x7380166f;
	sm3_content->m_BB = 0x4914b2b9;
	sm3_content->m_CC = 0x172442d7;
	sm3_content->m_DD = 0xda8a0600;
	sm3_content->m_EE = 0xa96f30bc;
	sm3_content->m_FF = 0x163138aa;
	sm3_content->m_GG = 0xe38dee4d;
	sm3_content->m_HH = 0xb0fb0e4e;

	return 1;
}

int SM3_HASH_UPDATE(SM3_content*	sm3_content, 
					 u8*			data_op, 
					 u32			data_len)
{
	if ( (data_len + sm3_content->m_dwLastLen) < 0x40 )
	{
		memcpy(sm3_content->m_pbLastGroup + sm3_content->m_dwLastLen, data_op, data_len);
		sm3_content->m_dwLastLen += data_len;
	}
	else 
	{
		u32 dwCopyLen;
		dwCopyLen = 0x40 - sm3_content->m_dwLastLen;
		memcpy(sm3_content->m_pbLastGroup + sm3_content->m_dwLastLen, data_op, dwCopyLen);
		data_op += dwCopyLen;
		data_len -= dwCopyLen;
		OP_one_group(sm3_content->m_pbLastGroup, sm3_content);
		sm3_content->m_dwGroupNum++;

		while ( data_len >= 0x40 )
		{
			OP_one_group(data_op, sm3_content);
			data_len -= 0x40;
			data_op += 0x40;
			sm3_content->m_dwGroupNum++;
		}
		memcpy(sm3_content->m_pbLastGroup, data_op, data_len);
		sm3_content->m_dwLastLen = data_len;
	}
	return 1;
}

int SM3_HASH_r(SM3_content*	sm3_content, u8* iv, u8* last_data, int* data_len, int64_t *bitsHashProcessed)
{
	u8 *temp_val_hash = 0;
	*data_len = sm3_content->m_dwLastLen;
	memcpy(last_data, sm3_content->m_pbLastGroup, sm3_content->m_dwLastLen);
	
	temp_val_hash = iv;
	u32_2_u8(sm3_content->m_AA, temp_val_hash);
	temp_val_hash += 4;
	u32_2_u8(sm3_content->m_BB, temp_val_hash);
	temp_val_hash += 4;
	u32_2_u8(sm3_content->m_CC, temp_val_hash);
	temp_val_hash += 4;
	u32_2_u8(sm3_content->m_DD, temp_val_hash);
	temp_val_hash += 4;
	u32_2_u8(sm3_content->m_EE, temp_val_hash);
	temp_val_hash += 4;
	u32_2_u8(sm3_content->m_FF, temp_val_hash);
	temp_val_hash += 4;
	u32_2_u8(sm3_content->m_GG, temp_val_hash);
	temp_val_hash += 4;
	u32_2_u8(sm3_content->m_HH, temp_val_hash);

	*bitsHashProcessed = sm3_content->m_dwGroupNum * 64 * 8;
	return 1;
}

int SM3_HASH_FINAL(u8*		val_hash, 
				   SM3_content*	sm3_content)
{
	u8	last_group[0x40*2];
	u32	last_len;
	u32	data_len;
	u32	dwOffset;

	last_len = sm3_content->m_dwLastLen;
	if (last_len>=56)
	{
		dwOffset = 0x40;
	}
	else
	{
		dwOffset = 0x00;
	}
	memset(last_group, 0x00, sizeof(last_group));
	memcpy(last_group, sm3_content->m_pbLastGroup, last_len);
	last_group[last_len] = 0x80;
	data_len = sm3_content->m_dwGroupNum * 0x40 + last_len;
	data_len <<= 3;
	last_group[dwOffset+63] = (u8)(data_len);
	last_group[dwOffset+62] = (u8)(data_len>>8);
	last_group[dwOffset+61] = (u8)(data_len>>16);
	last_group[dwOffset+60] = (u8)(data_len>>24);

	OP_one_group(last_group, sm3_content);

	if ( 0x40 == dwOffset )
	{
		OP_one_group(last_group+0x40, sm3_content);
	}

	u32_2_u8(sm3_content->m_AA, val_hash);
	val_hash += 4;
	u32_2_u8(sm3_content->m_BB, val_hash);
	val_hash += 4;
	u32_2_u8(sm3_content->m_CC, val_hash);
	val_hash += 4;
	u32_2_u8(sm3_content->m_DD, val_hash);
	val_hash += 4;
	u32_2_u8(sm3_content->m_EE, val_hash);
	val_hash += 4;
	u32_2_u8(sm3_content->m_FF, val_hash);
	val_hash += 4;
	u32_2_u8(sm3_content->m_GG, val_hash);
	val_hash += 4;
	u32_2_u8(sm3_content->m_HH, val_hash);

	return 1;
}


static int OP_one_group(u8 *data_op, SM3_content* sm3_content)
{
	u32 W1[68], W2[64];
	u32 A, B, C, D, E, F, G, H;
	u32 SS1, SS2, TT1, TT2;
	int j;

	for (j=0; j<16; j++)
	{
		W1[j] = (data_op[j*4]<<24) | (data_op[j*4+1]<<16) | (data_op[j*4+2]<<8) | data_op[j*4+3];
	}
	for ( ; j<68; j++)
	{
		W1[j] = P1(W1[j-16]^W1[j-9]^remove_bit(W1[j-3], 15)) ^ remove_bit(W1[j-13], 7) ^ W1[j-6];
	}
	for (j=0; j<64; j++)
	{
		W2[j] = W1[j] ^ W1[j+4];
	}

	A = sm3_content->m_AA;
	B = sm3_content->m_BB;
	C = sm3_content->m_CC;
	D = sm3_content->m_DD;
	E = sm3_content->m_EE;
	F = sm3_content->m_FF;
	G = sm3_content->m_GG;
	H = sm3_content->m_HH;
	for (j=0; j<16; j++)
	{
		SS1 = remove_bit(remove_bit(A, 12) + E + remove_bit(T0, j), 7);
		SS2 = SS1 ^ remove_bit(A, 12);
		TT1 = FF0(A, B, C) + D + SS2 + W2[j];
		TT2 = GG0(E, F, G) + H + SS1 + W1[j];
		D = C;
		C = remove_bit(B, 9);
		B = A;
		A = TT1;
		H = G;
		G = remove_bit(F, 19);
		F = E;
		E = P0(TT2);
	}
	for ( ; j<64; j++)
	{
		SS1 = remove_bit(remove_bit(A, 12) + E + remove_bit(T1, j), 7);
		SS2 = SS1 ^ remove_bit(A, 12);
		TT1 = FF1(A, B, C) + D + SS2 + W2[j];
		TT2 = GG1(E, F, G) + H + SS1 + W1[j];
		D = C;
		C = remove_bit(B, 9);
		B = A;
		A = TT1;
		H = G;
		G = remove_bit(F, 19);
		F = E;
		E = P0(TT2);
	}
	sm3_content->m_AA ^= A;
	sm3_content->m_BB ^= B;
	sm3_content->m_CC ^= C;
	sm3_content->m_DD ^= D;
	sm3_content->m_EE ^= E;
	sm3_content->m_FF ^= F;
	sm3_content->m_GG ^= G;
	sm3_content->m_HH ^= H;

	return 1;
}
int main()
{

	unsigned char bHashData[64] = { 0x61,0x62,0x63,0x64,0x61,0x62,0x63,0x64,0x61,0x62,0x63,0x64,0x61,0x62,0x63,0x64,
		0x61,0x62,0x63,0x64,0x61,0x62,0x63,0x64,0x61,0x62,0x63,0x64,0x61,0x62,0x63,0x64,
		0x61,0x62,0x63,0x64,0x61,0x62,0x63,0x64,0x61,0x62,0x63,0x64,0x61,0x62,0x63,0x64,
		0x61,0x62,0x63,0x64,0x61,0x62,0x63,0x64,0x61,0x62,0x63,0x64,0x61,0x62,0x63,0x64 };

	unsigned char bHashStdResult[32] = { 0xde,0xbe,0x9f,0xf9,0x22,0x75,0xb8,0xa1,0x38,0x60,0x48,0x89,0xc1,0x8e,0x5a,0x4d,
		0x6f,0xdb,0x70,0xe5,0x38,0x7e,0x57,0x65,0x29,0x3d,0xcb,0xa3,0x9c,0x0c,0x57,0x32 };
	unsigned char pbHashRet[32] = { 0 };
	int iRet = 0;
	SM3_content softSM3;
	memset(&softSM3, 0, sizeof(softSM3));
	SM3_HASH_INIT(&softSM3);

	SM3_HASH_UPDATE(&softSM3, bHashData, sizeof(bHashData));

	SM3_HASH_FINAL(pbHashRet, &softSM3);
	if (0 != memcmp(bHashStdResult, pbHashRet, sizeof(bHashStdResult)))
	{
		printf("SM3 hash failed...\n");
		return -1;
	}

	return 0;
}
