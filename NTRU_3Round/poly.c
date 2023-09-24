#include "poly.h"
#include <stdio.h>
#include <stdlib.h>
#include "Funciones_mat.h"
#include "Funciones_HW.h"
#include "pynq_api.h"

/* Map {0, 1, 2} -> {0,1,q-1} in place */
void poly_Z3_to_Zq(poly *r)
{
  int i;
  for(i=0; i<NTRU_N; i++)
    r->coeffs[i] = r->coeffs[i] | ((-(r->coeffs[i]>>1)) & (NTRU_Q-1));
}

/* Map {0, 1, q-1} -> {0,1,2} in place */
void poly_trinary_Zq_to_Z3(poly *r)
{
  int i;
  for(i=0; i<NTRU_N; i++)
  {
    r->coeffs[i] = MODQ(r->coeffs[i]);
    r->coeffs[i] = 3 & (r->coeffs[i] ^ (r->coeffs[i]>>(NTRU_LOGQ-1)));
  }
}

void poly_Sq_mul(poly *r, const poly *a, const poly *b)
{
  poly_Rq_mul(r, a, b);
  poly_mod_q_Phi_n(r);
}

void poly_Sq_mul_DBG(poly* r, const poly* a, const poly* b, int DBG, int N_COEFFS)
{
    poly_Rq_mul_DBG(r, a, b, DBG, N_COEFFS);
    poly_mod_q_Phi_n(r);
}

void poly_Sq_mul_HW(poly* r, const poly* a, const poly* b, PYNQ_SHARED_MEMORY shared_memory_1, PYNQ_SHARED_MEMORY shared_memory_2, PYNQ_AXI_DMA dma, int DBG)
{
    uint16_t a_array[NTRU_N];
    uint16_t b_array[NTRU_N];
    uint16_t r_array[NTRU_N];

    poly_to_array(a_array, a);
    poly_to_array(b_array, b);
    //poly_Rq_mul(r, a, b);
    ntru_mult_ms2xs(b_array, a_array, r_array, shared_memory_1, shared_memory_2, dma, DBG);

    if (DBG == 12) {
        printf("\n a_array: ");
        for (unsigned long long i = 0; i < NTRU_N; i++) printf("%d ", a_array[i]);
        printf("\n");
        printf("\n b_array: ");
        for (unsigned long long i = 0; i < NTRU_N; i++) printf("%d ", b_array[i]);
        printf("\n");
        printf("\n r_array: ");
        for (unsigned long long i = 0; i < NTRU_N; i++) printf("%d ", r_array[i]);
        printf("\n");

        printf("\n r: ");
        for (unsigned long long i = 0; i < NTRU_N; i++) printf("%d ", r->coeffs[i]);
        printf("\n");
    }

    array_to_poly(r_array, r);
    poly_mod_q_Phi_n(r);
}

void poly_S3_mul(poly *r, const poly *a, const poly *b)
{
  poly_Rq_mul(r, a, b);
  poly_mod_3_Phi_n(r);
}

void poly_S3_mul_DBG(poly* r, const poly* a, const poly* b, int DBG, int N_COEFFS)
{
    poly_Rq_mul_DBG(r, a, b, DBG, N_COEFFS);
    poly_mod_3_Phi_n(r);
}

void poly_S3_mul_HW(poly* r, const poly* a, const poly* b, PYNQ_SHARED_MEMORY shared_memory_1, PYNQ_SHARED_MEMORY shared_memory_2, PYNQ_AXI_DMA dma, int DBG)
{
    uint16_t a_array[NTRU_N];
    uint16_t b_array[NTRU_N];
    uint16_t r_array[NTRU_N];

    poly_to_array(a_array, a);
    poly_to_array(b_array, b);
    //poly_Rq_mul(r, a, b);
    ntru_mult_ms2xs(b_array, a_array, r_array, shared_memory_1, shared_memory_2, dma, DBG);

    if (DBG == 11) {
        printf("\n a_array: ");
        for (unsigned long long i = 0; i < NTRU_N; i++) printf("%d ", a_array[i]);
        printf("\n");
        printf("\n b_array: ");
        for (unsigned long long i = 0; i < NTRU_N; i++) printf("%d ", b_array[i]);
        printf("\n");
        printf("\n r_array: ");
        for (unsigned long long i = 0; i < NTRU_N; i++) printf("%d ", r_array[i]);
        printf("\n");

        printf("\n r: ");
        for (unsigned long long i = 0; i < NTRU_N; i++) printf("%d ", r->coeffs[i]);
        printf("\n");
    }
    array_to_poly(r_array, r);
    poly_mod_3_Phi_n(r);

}

static void poly_R2_inv_to_Rq_inv(poly *r, const poly *ai, const poly *a)
{
#if NTRU_Q <= 256 || NTRU_Q >= 65536
#error "poly_R2_inv_to_Rq_inv in poly.c assumes 256 < q < 65536"
#endif

  int i;
  poly b, c;
  poly s;

  // for 0..4
  //    ai = ai * (2 - a*ai)  mod q
  for(i=0; i<NTRU_N; i++)
    b.coeffs[i] = -(a->coeffs[i]);

  for(i=0; i<NTRU_N; i++)
    r->coeffs[i] = ai->coeffs[i];

  poly_Rq_mul(&c, r, &b);
  c.coeffs[0] += 2; // c = 2 - a*ai
  poly_Rq_mul(&s, &c, r); // s = ai*c

  poly_Rq_mul(&c, &s, &b);
  c.coeffs[0] += 2; // c = 2 - a*s
  poly_Rq_mul(r, &c, &s); // r = s*c

  poly_Rq_mul(&c, r, &b);
  c.coeffs[0] += 2; // c = 2 - a*r
  poly_Rq_mul(&s, &c, r); // s = r*c

  poly_Rq_mul(&c, &s, &b);
  c.coeffs[0] += 2; // c = 2 - a*s
  poly_Rq_mul(r, &c, &s); // r = s*c
}

void poly_Rq_inv(poly *r, const poly *a)
{
  poly ai2;
  poly_R2_inv(&ai2, a);
  poly_R2_inv_to_Rq_inv(r, &ai2, a);
}

