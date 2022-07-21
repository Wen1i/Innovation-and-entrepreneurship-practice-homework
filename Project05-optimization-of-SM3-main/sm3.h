#ifndef HEADER_SM3_H
#define HEADER_SM3_H

#include <stdint.h>

#ifdef  __cplusplus
extern "C" {
#endif

typedef unsigned long u32;
typedef unsigned char u8;

#define SM3_LENGTH 32
#define SM3_BLOCK	64

//SM3 Flagֵ
#define SM3_FLAG 0x1

typedef struct _SM3_content
{
	u8	m_pbLastGroup[SM3_BLOCK];
	u32	m_dwLastLen;
	u32	m_dwGroupNum; 
	u32	m_AA;
	u32   m_BB;
	u32	m_CC;
	u32   m_DD;
	u32	m_EE;
	u32   m_FF;
	u32	m_GG;
	u32   m_HH;

	u8	m_ucFlag;
}SM3_content;

int SM3_HASH_INIT(SM3_content* pSm3Ctx);

int SM3_HASH_UPDATE(SM3_content* pSm3Ctx, 
					 u8* pbData, 
					 u32 dwDataLen);

int SM3_HASH_FINAL(u8* pbHashValue, 
					SM3_content* pSm3Ctx);

int SM3_HASH_r(SM3_content*	pSm3Ctx, u8* iv, u8* lastData, int* lastDataLen, int64_t *bitsHashProcessed);

#ifdef  __cplusplus
}
#endif

#endif
