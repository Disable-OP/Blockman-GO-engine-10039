#ifndef _AESCRYPT_H
#define _AESCRYPT_H

aes_rval aes_decrypt_block(const unsigned char in_blk[], unsigned char out_blk[], const aes_ctx cx[1]);
aes_rval aes_encrypt_block(const unsigned char in_blk[], unsigned char out_blk[], const aes_ctx cx[1]);

#endif
