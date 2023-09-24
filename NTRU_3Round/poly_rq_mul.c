#include "poly.h"
#include <stdio.h>
#include <stdlib.h>

void poly_Rq_mul(poly *r, const poly *a, const poly *b)
{
  int k,i;

  for(k=0; k<NTRU_N; k++)
  {
    r->coeffs[k] = 0;
    for(i=1; i<NTRU_N-k; i++)
      r->coeffs[k] += a->coeffs[k+i] * b->coeffs[NTRU_N-i];
    for(i=0; i<k+1; i++)
      r->coeffs[k] += a->coeffs[k-i] * b->coeffs[i];
  }
}

void poly_Rq_mul_DBG(poly* r, const poly* a, const poly* b, int DBG, int N_COEFFS)
{
    int k, i;

    if (DBG == 8) {
        for (int i = 0; i < N_COEFFS; i++) {
            printf("\n %d -> %d \t %d ", i, a->coeffs[i], b->coeffs[i]);
        }
    }
    for (k = 0; k < NTRU_N; k++)
    {
        r->coeffs[k] = 0;
        for (i = 1; i < NTRU_N - k; i++)
            r->coeffs[k] += a->coeffs[k + i] * b->coeffs[NTRU_N - i];
        for (i = 0; i < k + 1; i++)
            r->coeffs[k] += a->coeffs[k - i] * b->coeffs[i];
    }

}


