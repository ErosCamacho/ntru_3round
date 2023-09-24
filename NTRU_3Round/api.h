
#include "params.h"

#ifndef API_H
#define API_H

#if NTRU_N == 509
#define CRYPTO_SECRETKEYBYTES 935
#define CRYPTO_PUBLICKEYBYTES 699
#define CRYPTO_CIPHERTEXTBYTES 699
#define CRYPTO_BYTES 32

#define CRYPTO_ALGNAME "ntruhps2048509"

#elif NTRU_N == 677
#define CRYPTO_SECRETKEYBYTES 1234
#define CRYPTO_PUBLICKEYBYTES 930
#define CRYPTO_CIPHERTEXTBYTES 930
#define CRYPTO_BYTES 32

#define CRYPTO_ALGNAME "ntruhps2048677"

#elif NTRU_N == 821
#define CRYPTO_SECRETKEYBYTES 1590
#define CRYPTO_PUBLICKEYBYTES 1230
#define CRYPTO_CIPHERTEXTBYTES 1230
#define CRYPTO_BYTES 32

#define CRYPTO_ALGNAME "ntruhps4096821"

#elif NTRU_N == 701

#define CRYPTO_SECRETKEYBYTES 1450
#define CRYPTO_PUBLICKEYBYTES 1138
#define CRYPTO_CIPHERTEXTBYTES 1138
#define CRYPTO_BYTES 32

#define CRYPTO_ALGNAME "ntruhrss701"

#endif


#define crypto_kem_keypair CRYPTO_NAMESPACE(keypair)
int crypto_kem_keypair(unsigned char *pk, unsigned char *sk, int DBG);

#define crypto_kem_enc CRYPTO_NAMESPACE(enc)
int crypto_kem_enc(unsigned char *c, unsigned char *k, const unsigned char *pk, unsigned char* rm_seed);

#define crypto_kem_dec CRYPTO_NAMESPACE(dec)
int crypto_kem_dec(unsigned char *k, const unsigned char *c, const unsigned char *sk);

#endif
