#include "poly.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pynq_api.h"
#include "api.h"
#include "cmov.h"
#include "crypto_hash_sha3256.h"
#include "kem.h"
#include "owcpa.h"
#include "params.h"
#include "rng.h"
#include "sample.h"
#include "Funciones_mat.h"
#include "Funciones_HW.h"

// Funciones copiadas de owcpa //
static int owcpa_check_ciphertext(const unsigned char* ciphertext)
{
    /* A ciphertext is log2(q)*(n-1) bits packed into bytes.  */
    /* Check that any unused bits of the final byte are zero. */

    uint16_t t = 0;

    t = ciphertext[NTRU_CIPHERTEXTBYTES - 1];
    t &= 0xff << (8 - (7 & (NTRU_LOGQ * NTRU_PACK_DEG)));

    /* We have 0 <= t < 256 */
    /* Return 0 on success (t=0), 1 on failure */
    return (int)(1 & ((~t + 1) >> 15));
}
static int owcpa_check_r(const poly* r)
{
    /* A valid r has coefficients in {0,1,q-1} and has r[N-1] = 0 */
    /* Note: We may assume that 0 <= r[i] <= q-1 for all i        */

    int i;
    uint32_t t = 0;
    uint16_t c;
    for (i = 0; i < NTRU_N - 1; i++)
    {
        c = r->coeffs[i];
        t |= (c + 1) & (NTRU_Q - 4);  /* 0 iff c is in {-1,0,1,2} */
        t |= (c + 2) & 4;  /* 1 if c = 2, 0 if c is in {-1,0,1} */
    }
    t |= r->coeffs[NTRU_N - 1]; /* Coefficient n-1 must be zero */

    /* We have 0 <= t < 2^16. */
    /* Return 0 on success (t=0), 1 on failure */
    return (int)(1 & ((~t + 1) >> 31));
}
#ifdef NTRU_HPS
static int owcpa_check_m(const poly* m)
{
    /* Check that m is in message space, i.e.                  */
    /*  (1)  |{i : m[i] = 1}| = |{i : m[i] = 2}|, and          */
    /*  (2)  |{i : m[i] != 0}| = NTRU_WEIGHT.                  */
    /* Note: We may assume that m has coefficients in {0,1,2}. */

    int i;
    uint32_t t = 0;
    uint16_t ps = 0;
    uint16_t ms = 0;
    for (i = 0; i < NTRU_N; i++)
    {
        ps += m->coeffs[i] & 1;
        ms += m->coeffs[i] & 2;
    }
    t |= ps ^ (ms >> 1);   /* 0 if (1) holds */
    t |= ms ^ NTRU_WEIGHT; /* 0 if (1) and (2) hold */

    /* We have 0 <= t < 2^16. */
    /* Return 0 on success (t=0), 1 on failure */
    return (int)(1 & ((~t + 1) >> 31));
}
#endif

// Funciones propias del sistema criptografico //
void gen_keys(unsigned char* pk, unsigned char* sk, int DBG) {

    unsigned long long tic = 0, toc;
    unsigned long long ini_tiempo_total = 0, fin_tiempo_total;
    unsigned char seed[NTRU_SAMPLE_FG_BYTES];

    int i;

    poly x1, x2, x3, x4, x5;

    poly* f = &x1, * g = &x2, * invf_mod3 = &x3;
    poly* gf = &x3, * invgf = &x4, * tmp = &x5;
    poly* invh = &x3, * h = &x3;

    ini_tiempo_total = PYNQ_Wtime();

    // sample_fg //
    tic = PYNQ_Wtime();
        randombytes(seed, NTRU_SAMPLE_FG_BYTES);
        sample_fg(f, g, seed);
    toc = PYNQ_Wtime() - tic;
    if (DBG == 2) printf(" \t sample_fg(f,g,seed) \t \t ... \t (%6llu us.) \n ", toc);

    // poly_S3_inv //
    tic = PYNQ_Wtime();
        poly_S3_inv(invf_mod3, f);
        poly_S3_tobytes(sk, f);
        poly_S3_tobytes(sk + NTRU_PACK_TRINARY_BYTES, invf_mod3);
    toc = PYNQ_Wtime() - tic;
    if (DBG == 2) printf(" \t poly_S3_inv(invf_mod3, f) \t ... \t (%6llu us.) \n ", toc);

    // poly_Z3_to_Zq // 
    tic = PYNQ_Wtime();
        /* Lift coeffs of f and g from Z_p to Z_q */
        poly_Z3_to_Zq(f);
        poly_Z3_to_Zq(g);
    toc = PYNQ_Wtime() - tic;
    if (DBG == 2) printf(" \t poly_Z3_to_Zq \t \t \t ... \t (%6llu us.) \n ", toc);

    #ifdef NTRU_HRSS
        /* g = 3*(x-1)*g */
        for (i = NTRU_N - 1; i > 0; i--)
            g->coeffs[i] = 3 * (g->coeffs[i - 1] - g->coeffs[i]);
        g->coeffs[0] = -(3 * g->coeffs[0]);
    #endif

    #ifdef NTRU_HPS
        /* g = 3*g */
        for (i = 0; i < NTRU_N; i++)
            g->coeffs[i] = 3 * g->coeffs[i];
    #endif

    // poly_Rq_mul //
    tic = PYNQ_Wtime();
        poly_Rq_mul(gf, g, f);
    toc = PYNQ_Wtime() - tic;
    if (DBG == 2) printf(" \t poly_Rq_mul(gf, g, f) \t \t ... \t (%6llu us.) \n ", toc);

    // poly_Rq_inv(invgf, gf) //
    tic = PYNQ_Wtime();
        poly_Rq_inv(invgf, gf);
    toc = PYNQ_Wtime() - tic;
    if (DBG == 2) printf(" \t poly_Rq_inv(invgf, gf) \t ... \t (%6llu us.) \n ", toc);

    // poly_Rq_mul / poly_Sq_mul // 
    tic = PYNQ_Wtime();
        poly_Rq_mul(tmp, invgf, f);
        poly_Sq_mul(invh, tmp, f);
        poly_Sq_tobytes(sk + 2 * NTRU_PACK_TRINARY_BYTES, invh);
    toc = PYNQ_Wtime() - tic;
    if (DBG == 2) printf(" \t poly_Rq_mul / poly_Sq_mul \t ... \t (%6llu us.) \n ", toc);

    // poly_Rq_mul / poly_Rq_mul // 
    tic = PYNQ_Wtime();
        poly_Rq_mul(tmp, invgf, g);
        poly_Rq_mul(h, tmp, g);
        poly_Rq_sum_zero_tobytes(pk, h);
    toc = PYNQ_Wtime() - tic;
    if (DBG == 2) printf(" \t poly_Rq_mul / poly_Sq_mul \t ... \t (%6llu us.) \n ", toc);
    
    // randombytes sk //
    tic = PYNQ_Wtime();
        randombytes(sk + NTRU_OWCPA_SECRETKEYBYTES, NTRU_PRFKEYBYTES);
    toc = PYNQ_Wtime() - tic;
    if (DBG == 2) printf(" \t randombytes sk \t \t ... \t (%6llu us.) \n ", toc);

    fin_tiempo_total = PYNQ_Wtime() - ini_tiempo_total;
    if (DBG == 1 || DBG == 2) printf(" Generacion claves (pk / sk) \t ... \t (%6llu us.) \n ", fin_tiempo_total);
}
void gen_rhmss(uint16_t* r, poly* poly_r, uint16_t* h, poly* poly_h, poly* poly_m, unsigned char* pk, unsigned char* ss, int DBG, int N_COEFFS, unsigned char* rm_seed) {
    
    unsigned char rm[NTRU_OWCPA_MSGBYTES];
    //unsigned char rm_seed[NTRU_SAMPLE_RM_BYTES];
    unsigned long long tic = 0, toc;

    tic = PYNQ_Wtime();
        // Samplear r / m
        //randombytes(rm_seed, NTRU_SAMPLE_RM_BYTES);
        sample_rm(poly_r, poly_m, rm_seed);

        // Generacion ss
        poly_S3_tobytes(rm, poly_r);
        poly_S3_tobytes(rm + NTRU_PACK_TRINARY_BYTES, poly_m);
        crypto_hash_sha3256(ss, rm, NTRU_OWCPA_MSGBYTES);

        // Generacion r
        poly_Z3_to_Zq(poly_r);
        poly_to_array(r, poly_r);
        num_ones_minus(r, DBG);

        // Generacion h
        poly_Rq_sum_zero_frombytes(poly_h, pk);
        poly_to_array(h, poly_h);

    toc = PYNQ_Wtime() - tic;
    if (DBG == 1 || DBG == 2) printf(" Generacion r / h / ss (SW) \t ... \t (%6llu us.) \n ", toc);

    if (DBG == 7) {
        printf("\n rm: ");
        for (unsigned long long i = 0; i < NTRU_OWCPA_MSGBYTES; i++) printf("%02X", rm[i]);
        printf("\n");

        printf("\n ss: ");
        for (unsigned long long i = 0; i < CRYPTO_BYTES; i++) printf("%02X", ss[i]);
        printf("\n");

        for (int i = 0; i < N_COEFFS; i++) {
            printf("\n %d -> %d \t %d \t %d \t %d", i, poly_r->coeffs[i], r[i], poly_h->coeffs[i], h[i]);
        }
    }

}
void enc_ntru_3round(unsigned char* c, poly* poly_r, poly* poly_h, poly* poly_m, unsigned long long* tiempo_mult, unsigned long long* tiempo_enc, int DBG, int N_COEFFS) {

    unsigned long long tic = 0, toc;
    poly x1, x2;
    poly* liftm = &x1;
    poly* ct = &x2;

    tic = PYNQ_Wtime();
        poly_Rq_mul_DBG(ct, poly_r, poly_h, DBG, N_COEFFS);
    toc = PYNQ_Wtime() - tic;
    if (DBG == 2) printf(" \t Mult. v. NTRU 3 Ronda (SW) \t ... \t (%6llu us.) \n ", toc);
    //for (int i = 0; i < 10; i++) printf("%d ", ct->coeffs[i] % 2048);
    *tiempo_mult = toc;
    *tiempo_enc = toc;
  
    tic = PYNQ_Wtime();
        poly_lift(liftm, poly_m);
        for (int i = 0; i < NTRU_N; i++)
            ct->coeffs[i] = ct->coeffs[i] + liftm->coeffs[i];
        poly_Rq_sum_zero_tobytes(c, ct);
    toc = PYNQ_Wtime() - tic;
    if (DBG == 2) printf(" \t Resto Enc. v3Ronda (SW) \t ... \t (%6llu us.) \n ", toc);
    *tiempo_enc += toc;

    if (DBG == 1 || DBG == 2) printf(" CIFRADO 3Ronda (SW) \t \t ... \t (%6llu us.) \n ", *tiempo_enc);
}
void enc_ntru_libntru(unsigned char* c, uint16_t* r, uint16_t* h, poly* poly_m, unsigned long long* tiempo_mult, unsigned long long* tiempo_enc, int DBG, int N_COEFFS) {

    unsigned long long tic = 0, toc;
    uint16_t e[NTRU_N];
    poly x1, x2;
    poly* liftm = &x1;
    poly* ct = &x2;

    tic = PYNQ_Wtime();
        array_mult_v16bits(e, r, h, DBG);
    toc = PYNQ_Wtime() - tic;
    if (DBG == 2) printf(" \t Mult. v. NTRU Encrypt (SW) \t ... \t (%6llu us.) \n ", toc);
    *tiempo_mult = toc;
    *tiempo_enc = toc;

    tic = PYNQ_Wtime();
        array_to_poly(e, ct);
        poly_lift(liftm, poly_m);
        for (int i = 0; i < NTRU_N; i++)
            ct->coeffs[i] = ct->coeffs[i] + liftm->coeffs[i];
        poly_Rq_sum_zero_tobytes(c, ct);
    toc = PYNQ_Wtime() - tic;
    if (DBG == 2) printf(" \t Resto Enc. vEncrypt (SW) \t ... \t (%6llu us.) \n ", toc);
    *tiempo_enc += toc;

    if (DBG == 1 || DBG == 2) printf(" CIFRADO LIBNTRU (SW) \t \t ... \t (%6llu us.) \n ", *tiempo_enc);

}
void enc_ntru_hw(unsigned char* c, uint16_t* r, uint16_t* h, poly* poly_m, unsigned long long* tiempo_mult, unsigned long long* tiempo_enc, PYNQ_SHARED_MEMORY shared_memory_1, PYNQ_SHARED_MEMORY shared_memory_2, PYNQ_AXI_DMA dma, int DBG, int N_COEFFS) {

    unsigned long long tic = 0, toc;
    uint16_t e_HW[NTRU_N];
    poly x1, x2;
    poly* liftm = &x1;
    poly* ct = &x2;

    tic = PYNQ_Wtime();
        ntru_mult_ms2xs(r, h, e_HW, shared_memory_1, shared_memory_2, dma, DBG);
    toc = PYNQ_Wtime() - tic;
    if (DBG == 2) printf(" \t Mult. HARDWARE (HW) \t \t ... \t (%6llu us.) \n ", toc);
    *tiempo_mult = toc;
    *tiempo_enc = toc;

    tic = PYNQ_Wtime();
        array_to_poly(e_HW, ct);
        poly_lift(liftm, poly_m);
        for (int i = 0; i < NTRU_N; i++)
            ct->coeffs[i] = ct->coeffs[i] + liftm->coeffs[i];
        poly_Rq_sum_zero_tobytes(c, ct);
    toc = PYNQ_Wtime() - tic;
    if (DBG == 2) printf(" \t Resto Enc. v. HW (SW) \t \t ... \t (%6llu us.) \n ", toc);
    *tiempo_enc += toc;

    if (DBG == 1 || DBG == 2) printf(" CIFRADO HW (HW/SW) \t \t ... \t (%6llu us.) \n ", *tiempo_enc);

}
void dec_ntru_3round(unsigned char* ss_dec, const unsigned char* ct, const unsigned char* sk, unsigned long long* tiempo_dec, int DBG, int N_COEFFS) {
    
    int i, fail;
    unsigned char rm[NTRU_OWCPA_MSGBYTES];
    unsigned char buf[NTRU_PRFKEYBYTES + NTRU_CIPHERTEXTBYTES];

    unsigned long long tic = 0, toc;

    poly x1, x2, x3, x4;

    poly* c = &x1, * f = &x2, * cf = &x3;
    poly* mf = &x2, * finv3 = &x3, * m = &x4;
    poly* liftm = &x2, * invh = &x3, * r = &x4;
    poly* b = &x1;
    
    // poly_Rq_sum_zero / poly_S3_from / poly Z3_to_Zq //
    tic = PYNQ_Wtime();
        poly_Rq_sum_zero_frombytes(c, ct);
        poly_S3_frombytes(f, sk);
        poly_Z3_to_Zq(f);
    toc = PYNQ_Wtime() - tic;
    if (DBG == 2) printf(" \t Rq_sum / S3_from / Z3_to_Zq  \t ... \t (%6llu us.) \n ", toc);
    *tiempo_dec = toc;

    // Rq_mul / Rq_to_S3 //
    tic = PYNQ_Wtime();
        poly_Rq_mul(cf, c, f);
        poly_Rq_to_S3(mf, cf);
    toc = PYNQ_Wtime() - tic;
    if (DBG == 2) printf(" \t Rq_mul / Rq_to_R3 \t \t ... \t (%6llu us.) \n ", toc);
    *tiempo_dec += toc;

    if (DBG == 7) {
        printf("\n cf: ");
        for (unsigned long long i = 0; i < NTRU_N; i++) printf("%u ", cf->coeffs[i] % 2048);
        printf("\n");
    }

    // S3_from / S3_mul / S3_to //
    tic = PYNQ_Wtime();
        poly_S3_frombytes(finv3, sk + NTRU_PACK_TRINARY_BYTES);
        poly_S3_mul_DBG(m, mf, finv3, DBG, N_COEFFS);
        poly_S3_tobytes(rm + NTRU_PACK_TRINARY_BYTES, m);
    toc = PYNQ_Wtime() - tic;
    if (DBG == 2) printf(" \t S3_from / S3_mul / S3_to \t ... \t (%6llu us.) \n ", toc);
    *tiempo_dec += toc;

    // CHECK FAIL //
    tic = PYNQ_Wtime();
        fail = 0;

        /* Check that the unused bits of the last byte of the ciphertext are zero */
        fail |= owcpa_check_ciphertext(ct);

        /* For the IND-CCA2 KEM we must ensure that c = Enc(h, (r,m)).             */
        /* We can avoid re-computing r*h + Lift(m) as long as we check that        */
        /* r (defined as b/h mod (q, Phi_n)) and m are in the message space.       */
        /* (m can take any value in S3 in NTRU_HRSS) */
    #ifdef NTRU_HPS
        fail |= owcpa_check_m(m);
    #endif
    toc = PYNQ_Wtime() - tic;
    if (DBG == 2) printf(" \t check_ct / check_m \t \t ... \t (%6llu us.) \n ", toc);
    *tiempo_dec += toc;

    tic = PYNQ_Wtime();
        /* b = c - Lift(m) mod (q, x^n - 1) */
        poly_lift(liftm, m);
        for (i = 0; i < NTRU_N; i++)
            b->coeffs[i] = c->coeffs[i] - liftm->coeffs[i];
    toc = PYNQ_Wtime() - tic;
    if (DBG == 2) printf(" \t poly_liftm \t \t \t ... \t (%6llu us.) \n ", toc);
    *tiempo_dec += toc;

    tic = PYNQ_Wtime();
        /* r = b / h mod (q, Phi_n) */
        poly_Sq_frombytes(invh, sk + 2 * NTRU_PACK_TRINARY_BYTES);
        poly_Sq_mul_DBG(r, b, invh, DBG, N_COEFFS);
    toc = PYNQ_Wtime() - tic;
    if (DBG == 2) printf(" \t Sq_from / Sq_mul \t \t ... \t (%6llu us.) \n ", toc);
    *tiempo_dec += toc;

    if (DBG == 7) {
        printf("\n r: ");
        for (unsigned long long i = 0; i < NTRU_N; i++) printf("%u ", r->coeffs[i]);
        printf("\n");
    }

    tic = PYNQ_Wtime();
        /* NOTE: Our definition of r as b/h mod (q, Phi_n) follows Figure 4 of     */
        /*   [Sch18] https://eprint.iacr.org/2018/1174/20181203:032458.            */
        /* This differs from Figure 10 of Saito--Xagawa--Yamakawa                  */
        /*   [SXY17] https://eprint.iacr.org/2017/1005/20180516:055500             */
        /* where r gets a final reduction modulo p.                                */
        /* We need this change to use Proposition 1 of [Sch18].                    */

        /* Proposition 1 of [Sch18] shows that re-encryption with (r,m) yields c.  */
        /* if and only if fail==0 after the following call to owcpa_check_r        */
        /* The procedure given in Fig. 8 of [Sch18] can be skipped because we have */
        /* c(1) = 0 due to the use of poly_Rq_sum_zero_{to,from}bytes.             */
        fail |= owcpa_check_r(r);
    toc = PYNQ_Wtime() - tic;
    if (DBG == 2) printf(" \t check_r \t \t \t ... \t (%6llu us.) \n ", toc);
    *tiempo_dec += toc;

    // trinary_Zq_to_Z3 / S3_to //
    tic = PYNQ_Wtime();
        poly_trinary_Zq_to_Z3(r);
        poly_S3_tobytes(rm, r);
    toc = PYNQ_Wtime() - tic;
    if (DBG == 2) printf(" \t tri_Zq_to_Z3 / S3_to \t \t ... \t (%6llu us.) \n ", toc);
    *tiempo_dec += toc;

    // hash_sha3256_ss_dec //
    tic = PYNQ_Wtime();
        crypto_hash_sha3256(ss_dec, rm, NTRU_OWCPA_MSGBYTES);
    toc = PYNQ_Wtime() - tic;
    if (DBG == 2) printf(" \t tri_Zq_to_Z3 / S3_to \t \t ... \t (%6llu us.) \n ", toc);
    *tiempo_dec += toc;

    // shake_secret_PRF_ct //
    tic = PYNQ_Wtime();
        /* shake(secret PRF key || input ciphertext) */
        for (i = 0; i < NTRU_PRFKEYBYTES; i++)
            buf[i] = sk[i + NTRU_OWCPA_SECRETKEYBYTES];
        for (i = 0; i < NTRU_CIPHERTEXTBYTES; i++)
            buf[NTRU_PRFKEYBYTES + i] = ct[i];
        crypto_hash_sha3256(rm, buf, NTRU_PRFKEYBYTES + NTRU_CIPHERTEXTBYTES);
    toc = PYNQ_Wtime() - tic;
    if (DBG == 2) printf(" \t shake_secret_PRF_ct \t \t ... \t (%6llu us.) \n ", toc);
    *tiempo_dec += toc;

    // cmov_ss_dec //
    tic = PYNQ_Wtime();
        cmov(ss_dec, rm, NTRU_SHAREDKEYBYTES, (unsigned char)fail);
    toc = PYNQ_Wtime() - tic;
    if (DBG == 2) printf(" \t cmov_ss_dec \t \t \t ... \t (%6llu us.) \n ", toc);
    *tiempo_dec += toc;

    if (DBG == 1 || DBG == 2) printf(" DESCIFRADO (SW) \t \t ... \t (%6llu us.) \n ", *tiempo_dec);

    if (DBG == 7) {
        printf("\n rm_dec: ");
        for (unsigned long long i = 0; i < NTRU_OWCPA_MSGBYTES; i++) printf("%02X", rm[i]);
        printf("\n");

        printf("\n ss_dec: ");
        for (unsigned long long i = 0; i < CRYPTO_BYTES; i++) printf("%02X", ss_dec[i]);
        printf("\n");

        printf("\n fail: %d", fail);
    }
}

void dec_ntru_hw(unsigned char* ss_dec, const unsigned char* ct, const unsigned char* sk, unsigned long long* tiempo_dec, PYNQ_SHARED_MEMORY shared_memory_1, PYNQ_SHARED_MEMORY shared_memory_2, PYNQ_AXI_DMA dma, int DBG, int N_COEFFS) {

    int i, fail;
    unsigned char rm[NTRU_OWCPA_MSGBYTES];
    unsigned char buf[NTRU_PRFKEYBYTES + NTRU_CIPHERTEXTBYTES];

    unsigned long long tic = 0, toc;

    uint16_t c_array[NTRU_N];
    uint16_t f_array[NTRU_N];
    uint16_t cf_array[NTRU_N];

    poly x1, x2, x3, x4;

    poly* c = &x1, * f = &x2, * cf = &x3;
    poly* mf = &x2, * finv3 = &x3, * m = &x4;
    poly* liftm = &x2, * invh = &x3, * r = &x4;
    poly* b = &x1;

    // poly_Rq_sum_zero / poly_S3_from / poly Z3_to_Zq //
    tic = PYNQ_Wtime();
        poly_Rq_sum_zero_frombytes(c, ct);
        poly_S3_frombytes(f, sk);
        poly_Z3_to_Zq(f);
    toc = PYNQ_Wtime() - tic;
    if (DBG == 2) printf(" \t Rq_sum / S3_from / Z3_to_Zq  \t ... \t (%6llu us.) \n ", toc);
    *tiempo_dec = toc;

    // Rq_mul / Rq_to_S3 //
    tic = PYNQ_Wtime();
        poly_to_array(c_array, c);
        poly_to_array(f_array, f);
        //poly_Rq_mul(cf, c, f);
        ntru_mult_ms2xs(f_array, c_array, cf_array, shared_memory_1, shared_memory_2, dma, DBG);
        array_to_poly(cf_array, cf);
        poly_Rq_to_S3(mf, cf);
    toc = PYNQ_Wtime() - tic;
    if (DBG == 2) printf(" \t Rq_mul (HW) / Rq_to_R3 \t ... \t (%6llu us.) \n ", toc);
    *tiempo_dec += toc;
    if (DBG == 7) {
        printf("\n cf: ");
        for (unsigned long long i = 0; i < NTRU_N; i++) printf("%u ", cf->coeffs[i]);
        printf("\n");
    }

    // S3_from / S3_mul / S3_to //
    tic = PYNQ_Wtime();
        poly_S3_frombytes(finv3, sk + NTRU_PACK_TRINARY_BYTES);
        poly_S3_mul(m, mf, finv3);
        //poly_S3_mul_HW(m, mf, finv3, shared_memory_1, shared_memory_2, dma, DBG);
        poly_S3_tobytes(rm + NTRU_PACK_TRINARY_BYTES, m);
    toc = PYNQ_Wtime() - tic;
    if (DBG == 2) printf(" \t S3_from / S3_mul / S3_to \t ... \t (%6llu us.) \n ", toc);
    *tiempo_dec += toc;

    // CHECK FAIL //
    tic = PYNQ_Wtime();
    fail = 0;

    /* Check that the unused bits of the last byte of the ciphertext are zero */
    fail |= owcpa_check_ciphertext(ct);

    /* For the IND-CCA2 KEM we must ensure that c = Enc(h, (r,m)).             */
    /* We can avoid re-computing r*h + Lift(m) as long as we check that        */
    /* r (defined as b/h mod (q, Phi_n)) and m are in the message space.       */
    /* (m can take any value in S3 in NTRU_HRSS) */
#ifdef NTRU_HPS
    fail |= owcpa_check_m(m);
#endif
    toc = PYNQ_Wtime() - tic;
    if (DBG == 2) printf(" \t check_ct / check_m \t \t ... \t (%6llu us.) \n ", toc);
    *tiempo_dec += toc;

    tic = PYNQ_Wtime();
    /* b = c - Lift(m) mod (q, x^n - 1) */
    poly_lift(liftm, m);
    for (i = 0; i < NTRU_N; i++)
        b->coeffs[i] = c->coeffs[i] - liftm->coeffs[i];
    toc = PYNQ_Wtime() - tic;
    if (DBG == 2) printf(" \t poly_liftm \t \t \t ... \t (%6llu us.) \n ", toc);
    *tiempo_dec += toc;

    tic = PYNQ_Wtime();
    /* r = b / h mod (q, Phi_n) */
        poly_Sq_frombytes(invh, sk + 2 * NTRU_PACK_TRINARY_BYTES);
        poly_Sq_mul_DBG(r, b, invh, DBG, N_COEFFS);
        //poly_Sq_mul_HW(r, b, invh, shared_memory_1, shared_memory_2, dma, DBG);
    toc = PYNQ_Wtime() - tic;
    if (DBG == 2) printf(" \t Sq_from / Sq_mul \t \t ... \t (%6llu us.) \n ", toc);
    *tiempo_dec += toc;

    tic = PYNQ_Wtime();
    /* NOTE: Our definition of r as b/h mod (q, Phi_n) follows Figure 4 of     */
    /*   [Sch18] https://eprint.iacr.org/2018/1174/20181203:032458.            */
    /* This differs from Figure 10 of Saito--Xagawa--Yamakawa                  */
    /*   [SXY17] https://eprint.iacr.org/2017/1005/20180516:055500             */
    /* where r gets a final reduction modulo p.                                */
    /* We need this change to use Proposition 1 of [Sch18].                    */

    /* Proposition 1 of [Sch18] shows that re-encryption with (r,m) yields c.  */
    /* if and only if fail==0 after the following call to owcpa_check_r        */
    /* The procedure given in Fig. 8 of [Sch18] can be skipped because we have */
    /* c(1) = 0 due to the use of poly_Rq_sum_zero_{to,from}bytes.             */
    fail |= owcpa_check_r(r);
    toc = PYNQ_Wtime() - tic;
    if (DBG == 2) printf(" \t check_r \t \t \t ... \t (%6llu us.) \n ", toc);
    *tiempo_dec += toc;

    // trinary_Zq_to_Z3 / S3_to //
    tic = PYNQ_Wtime();
        poly_trinary_Zq_to_Z3(r);
        poly_S3_tobytes(rm, r);
    toc = PYNQ_Wtime() - tic;
    if (DBG == 2) printf(" \t tri_Zq_to_Z3 / S3_to \t \t ... \t (%6llu us.) \n ", toc);
    *tiempo_dec += toc;

    // hash_sha3256_ss_dec //
    tic = PYNQ_Wtime();
    crypto_hash_sha3256(ss_dec, rm, NTRU_OWCPA_MSGBYTES);
    toc = PYNQ_Wtime() - tic;
    if (DBG == 2) printf(" \t tri_Zq_to_Z3 / S3_to \t \t ... \t (%6llu us.) \n ", toc);
    *tiempo_dec += toc;

    // shake_secret_PRF_ct //
    tic = PYNQ_Wtime();
    /* shake(secret PRF key || input ciphertext) */
    for (i = 0; i < NTRU_PRFKEYBYTES; i++)
        buf[i] = sk[i + NTRU_OWCPA_SECRETKEYBYTES];
    for (i = 0; i < NTRU_CIPHERTEXTBYTES; i++)
        buf[NTRU_PRFKEYBYTES + i] = ct[i];
    crypto_hash_sha3256(rm, buf, NTRU_PRFKEYBYTES + NTRU_CIPHERTEXTBYTES);
    toc = PYNQ_Wtime() - tic;
    if (DBG == 2) printf(" \t shake_secret_PRF_ct \t \t ... \t (%6llu us.) \n ", toc);
    *tiempo_dec += toc;

    // cmov_ss_dec //
    tic = PYNQ_Wtime();
    cmov(ss_dec, rm, NTRU_SHAREDKEYBYTES, (unsigned char)fail);
    toc = PYNQ_Wtime() - tic;
    if (DBG == 2) printf(" \t cmov_ss_dec \t \t \t ... \t (%6llu us.) \n ", toc);
    *tiempo_dec += toc;

    if (DBG == 1 || DBG == 2) printf(" DESCIFRADO HW \t \t ... \t (%6llu us.) \n ", *tiempo_dec);

    if (DBG == 7) {
        printf("\n rm_dec: ");
        for (unsigned long long i = 0; i < NTRU_OWCPA_MSGBYTES; i++) printf("%02X", rm[i]);
        printf("\n");

        printf("\n ss_dec: ");
        for (unsigned long long i = 0; i < CRYPTO_BYTES; i++) printf("%02X", ss_dec[i]);
        printf("\n");

        printf("\n fail: %d", fail);
    }
}