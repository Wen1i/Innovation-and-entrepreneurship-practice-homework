#pragma once

#define SM3_HASH_SIZE 32
namespace SM3 {
	typedef struct SM3Context {
		unsigned int intermediateHash[SM3_HASH_SIZE / 4];
		unsigned char messageBlock[64];
	} SM3Context;

	unsigned char *SM3Calc(const unsigned char *message,
		unsigned int messageLen, unsigned char digest[SM3_HASH_SIZE]);

	std::vector<uint32_t> call_hash_sm3(char *filepath);

	double progress();
}
