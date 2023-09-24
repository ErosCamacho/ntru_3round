#include "poly.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pynq_api.h"
#include "api.h"
#include "Funciones_mat.h"

void array_to_poly(uint16_t* in, poly* out) {
	for (int i = 0; i < NTRU_N; i++) {
		out->coeffs[i] = in[i];
	}
}

void poly_to_array(uint16_t* out, const poly* in) {
	for (int i = 0; i < NTRU_N; i++) {
		out[i] = in->coeffs[i];
	}
}

void array_aleatorio(uint16_t* ar) {
	srand(time(NULL));
	for (int i = 0; i < NTRU_N; i++) {
		ar[i] = rand() % 2048;
	}
}

void array_aleatorio_dr_rapido(uint16_t* ar, int dr, int DBG) {
	for (int i = 0; i < dr; i++) {
		ar[i] = 1;
	}
	for (int i = dr; i < 2*dr; i++) {
		ar[i] = -1;
	}
}

void num_ones_minus(uint16_t* ar, int DBG) {
	int num_ones = 0;
	int num_minus = 0;
	for (int i = 0; i < NTRU_N; i++) {
		if (ar[i] == 1) num_ones++;
		if (ar[i] == 2047) num_minus++;
	}
	if(DBG == 7) printf("\n \n %d %d %d \n \n", num_ones, num_minus, num_ones + num_minus);
}

void num_ones_minus_exit(uint16_t* ar, int* ar_exit, int DBG) {
	int num_ones = 0;
	int num_minus = 0;
	for (int i = 0; i < NTRU_N; i++) {
		if (ar[i] == 1) num_ones++;
		if (ar[i] == (1 << NTRU_LOGQ) - 1) num_minus++;
	}
	ar_exit[0] = num_ones;
	ar_exit[1] = num_minus;
	ar_exit[2] = num_ones + num_minus;
	if (DBG == 7) printf("\n \n %d %d %d \n \n", num_ones, num_minus, num_ones + num_minus);
}

void array_aleatorio_dr(uint16_t* ar, int dr, int DBG) {
	srand(time(NULL));
	uint16_t pos_ones[dr];
	uint16_t pos_minus[dr];
	int exit = 0;
	int c = 0;
	for (int i = 0; i < dr; i++) {
		pos_ones[i] = rand() % NTRU_N;
		pos_minus[i] = rand() % NTRU_N;
	}

	for (int i = 0; i < dr; i++) {
		printf("\n %d -> %d \t %d ", i, pos_ones[i], pos_minus[i]);
	}
	
	do {
		exit = 1;
		// Compruebo que ningun elemento se repite en los mismos arrays
		int i = 0;
		int j = 0;
		do {
			do {
				printf("\n %d %d %d %d %d", i, j, pos_ones[i], pos_ones[j], c);
				if (pos_ones[i] == pos_ones[j]) {
					pos_ones[j] = rand() % NTRU_N; i = 0; j = 0; c = 1; // Empieza el bucle de nuevo 
				}
				if (pos_minus[i] == pos_minus[j]) {
					pos_minus[j] = rand() % NTRU_N; i = 0; j = 0; c = 1;// Empieza el bucle de nuevo 
				}
				if (c == 1) {
					j = 0;
					c = 0;
				}
				else j++;
			} while (j < dr);
			i++;
			j = 0;
		} while (i < dr);
		printf("Estoy aqui");
		do {
			do {
				if (pos_ones[i] == pos_minus[j]) {
					pos_minus[j] = rand() % NTRU_N; i = 0; j = 0; exit = 0; // Empieza el bucle de nuevo 
				}
				j++;
			} while (j < dr);
			i++;
			j = 0;
		} while (i < dr);


		//for (int i = 0; i < dr; i++) {
		//	for (int j = 0; j < dr; j++) {
		//		if (pos_ones[i] == pos_ones[j]) {
		//			pos_ones[j] = rand() % NTRU_N; i = 0; j = 0; // Empieza el bucle de nuevo 
		//		}
		//		if (pos_minus[i] == pos_minus[j]) {
		//			pos_minus[j] = rand() % NTRU_N; i = 0; j = 0; // Empieza el bucle de nuevo 
		//		}
		//	}
		//}
		//// Compruebo que no se repite ningun elemento 
		//for (int i = 0; i < dr; i++) {
		//	for (int j = 0; j < dr; j++) {
		//		if (pos_ones[i] == pos_minus[j]) {
		//			pos_minus[j] = rand() % NTRU_N; i = 0; j = 0; exit = 0; // Empieza el bucle de nuevo 
		//		}
		//	}
		//}
		printf("Estoy aqui");
	} while (exit == 0);

	for (int i = 0; i < dr; i++) {
		ar[pos_ones[i]] = 1;
		ar[pos_minus[i]] = -1;
	}

	if (DBG == 6) {
		for (int i = 0; i < dr; i++) {
			printf("\n %d -> %d \t %d ", i, pos_ones[i], pos_minus[i]);
		}
	}


}

void array_aleatorio_r(uint16_t* ar) {
	srand(time(NULL));
	int num;
	for (int i = 0; i < NTRU_N; i++) {
		num = rand() % 4;
		if (num == 0)	ar[i] = 1;
		else if (num == 1) ar[i] = -1;
		else ar[i] = -1;
	}
}


void array_mult(uint16_t* e, const uint16_t* r, const uint16_t* h, int DBG) {
	int ind;
	for (int i = 0; i < NTRU_N; i++) {
		e[i] = 0;
	}

	for (int i = 0; i < NTRU_N; i++) {
		if (r[i] == 1) {
			for (int j = 0; j < NTRU_N; j++) {
				ind = (i + j) % NTRU_N;
				if(DBG == 4) printf("\n 1: %d \t %d \t %d \t %d", i, j, ind, e[i]);
				e[ind] = e[ind] + h[j];
			}
		}
		else if (r[i] == 2047) {
			for (int j = 0; j < NTRU_N; j++) {
				ind = (i + j) % NTRU_N;
				if(DBG == 4) printf("\n -1: %d \t %d \t %d \t %d", i, j, ind, e[i]);
				e[ind] = e[ind] - h[j];
			}
		}
	}
}

void array_mult_v16bits(uint16_t* e, const uint16_t* r, const uint16_t* h, int DBG) {
	int ind;
	for (int i = 0; i < NTRU_N; i++) {
		e[i] = 0;
	}

	for (int i = 0; i < NTRU_N; i++) {
		for (int j = 0; j < NTRU_N; j++) {
			ind = (i + j) % NTRU_N;
			e[ind] = e[ind] + (r[i]*h[j]);
			if (DBG == 4 && r[i] != 0) {
				uint16_t result = r[i] * h[j];
				printf("\n %d %d -> %d \t %d \t %d ", i, j, r[i], h[j], result);
			}
		}
	}
}

void sleep_pynq_ms(int ms) {
	unsigned long long tic = 0, toc;
	tic = PYNQ_Wtime();
	do { toc = PYNQ_Wtime() - tic; } while (toc * 1000 <= ms);
}

int comprobar_igualdad(const poly* poly_e, const uint16_t* e) {
	int comp = 0;
	for (int i = 0; i < NTRU_N; i++) {
		if (poly_e->coeffs[i] != e[i]) comp++;
	}
	return comp;
}

int comprobar_igualdad_c(const unsigned char* c, const unsigned char* c_libntru) {
	int comp = 0;
	for (int i = 0; i < CRYPTO_CIPHERTEXTBYTES; i++) {
		if (c[i] != c_libntru[i]) comp++;
	}
	return comp;
}

int comprobar_igualdad_crypto(const unsigned char* ss, const unsigned char* ss_dec) {
	int comp = 0;
	for (int i = 0; i < CRYPTO_BYTES; i++) {
		if (ss[i] != ss_dec[i]) comp++;
	}
	return comp;
}