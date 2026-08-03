#ifndef AESKEY_H
#define AESKEY_H

#include "aesopt.h"
aes_set_block_size
aes_rval aes_set_encrypt_key(const unsigned char in_key[], unsigned int klen, aes_ctx cx[1])
aes_rval aes_set_decrypt_key(const unsigned char in_key[], unsigned int klen, aes_ctx cx[1])

#endif
