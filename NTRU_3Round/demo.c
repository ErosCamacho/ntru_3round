#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "poly.h"
#include "Funciones_mat.h"
#include "time.h"
#include "pynq_api.h"
#include "Funciones_HW.h"
#include "sample.h"
#include "poly.h"
#include "rng.h"
#include "api.h"
#include "kem.h"
#include "Funciones_gen_enc_dec.h"
#include "Funciones_demo.h"

int main(int argc, char** argv){
	uint16_t r[NTRU_N];
	uint16_t h[NTRU_N];
	poly x1, x2, x3, x4, x5, x6;
	poly* poly_r = &x1;
	poly* poly_m = &x4;
	poly* poly_h = &x2;
	poly* poly_e = &x3;
	poly* liftm = &x5;

	int DBG = 0;
	int N_TEST = 1;
	int N_COEFFS = 10;
	int M = 1;
	int max_cycles = NTRU_N;
	int comp_SW = 0;
	int comp_HW = 0;
	int fallos_enc = 0;
	int fallos_enc_HW = 0;
	int fallos_dec = 0;
	int fallos_dec_HW = 0;
	int comp_crypto = 0;
	int comp_crypto_HW = 0;
	int fallos_crypto = 0;
	int fallos_crypto_HW = 0;

	//unsigned char rm_seed[NTRU_SAMPLE_RM_BYTES];

	unsigned char	*ct, *c_hw;
	unsigned char	*ss, *ss_hw;
	unsigned char	* pk;
	unsigned char	* sk;
	//unsigned char       ct[CRYPTO_CIPHERTEXTBYTES], ss[CRYPTO_BYTES], ss1[CRYPTO_BYTES];
	//unsigned char       pk[CRYPTO_PUBLICKEYBYTES], sk[CRYPTO_SECRETKEYBYTES];

	ct			= malloc(sizeof(unsigned char) * CRYPTO_CIPHERTEXTBYTES);
	c_hw		= malloc(sizeof(unsigned char) * CRYPTO_CIPHERTEXTBYTES);
	ss			= malloc(sizeof(unsigned char) * CRYPTO_BYTES);
	ss_hw		= malloc(sizeof(unsigned char) * CRYPTO_BYTES);

	pk = malloc(sizeof(unsigned char) * CRYPTO_PUBLICKEYBYTES);
	sk = malloc(sizeof(unsigned char) * CRYPTO_SECRETKEYBYTES);

	unsigned long long tic = 0, toc;
	unsigned long long tiempo_mult_3round;
	unsigned long long tiempo_mult_hw;
	unsigned long long tiempo_enc_3round;
	unsigned long long tiempo_enc_hw;
	unsigned long long tiempo_dec_3round;
	unsigned long long tiempo_dec_hw;

	int k = 0;
	int e = 0;
	int d = 0;
	int v = 0;

	for (int arg = 1; arg < argc; arg++) {

		if (argv[arg][0] == '-') {
			if (argv[arg][1] == 'h') {
				printf("\n Usage: ./ntru_demo [-h] [-k] [-e] [-d] \n");
				printf("\n -h : Show the help.");
				printf("\n -k : Key Generation");
				printf("\n -e : Encryption");
				printf("\n -d : Decryption");
				printf("\n -v : Verbose level");
				printf("\n \t 1 : Show only functions.");
				printf("\n \t 2 : Show intermediate results.");
				printf("\n \t 3 : Show keys.");
				printf("\n \n");

				return 0;
			}
			else if (argv[arg][1] == 'k') {
				k = 1;
			}
			else if (argv[arg][1] == 'e') {
				e = 1;
			}
			else if (argv[arg][1] == 'd') {
				d = 1;
			}
			else if (argv[arg][1] == 'v') {
				if (arg < argc - 1) v = atoi(argv[arg + 1]);
			}
			else {
				printf("\n Unknow option: %s\n", argv[arg]);
			}
		}
	}

	printf(" \n --- HARDWARE INITIALIZATION --- \n ");

	// ----- INICIALIZACION HARDWARE ----- //
	load_ntru_hw(DBG, M, max_cycles);
	PYNQ_SHARED_MEMORY shm_1, shm_2;
	PYNQ_allocatedSharedMemory(&shm_1, sizeof(int) * NTRU_N, 1);
	PYNQ_allocatedSharedMemory(&shm_2, sizeof(int) * NTRU_N, 1);
	PYNQ_AXI_DMA dma;
	PYNQ_openDMA(&dma, 0x40400000);

	// -------- GENERACION RANDOM ------------------- //

	unsigned char seed[48];
	unsigned char entropy_input[48];

	srand(time(NULL));

	for (int i = 0; i < 48; i++)
		entropy_input[i] = rand() * i;

	randombytes_init(entropy_input, NULL, 256);
	for (int i = 0; i < 100; i++) {
		randombytes(seed, 48);
	}

	randombytes_init(seed, NULL, 256);
	
	// -------- GENERACION CLAVES ------------------- // 

	// Generar Clave Publica y Privada

	if (k) {

		gen_files();

		if (v >= 1) printf("\n Generating keys ... ");

		gen_keys(pk, sk, DBG);

		if (v >= 1) printf("\t Complete ");

		if (v >= 1) printf("\n Storing keys ... ");

		save_keys(pk, sk);

		if (v >= 1) printf("\t Complete ");

		if (v >= 3) {
			printf("\n sk: "); 
			
			int mod = 32;

			printf("\n");

			int ind = 0;

			for (int i = 0; i < (int)ceil((double)CRYPTO_SECRETKEYBYTES / (double)mod); i++) {
				for (int j = 0; j < mod; j++) {
					ind = i * mod + j;
					if (ind < CRYPTO_SECRETKEYBYTES) printf("%02x", sk[ind]);
					else printf("  ");
				}
				printf("\n");
			}
		}
		if (v >= 3) {
			printf("\n pk: ");

			int mod = 32;

			printf("\n");

			int ind = 0;

			for (int i = 0; i < (int)ceil((double)CRYPTO_PUBLICKEYBYTES / (double)mod); i++) {
				for (int j = 0; j < mod; j++) {
					ind = i * mod + j;
					if (ind < CRYPTO_PUBLICKEYBYTES) printf("%02x", pk[ind]);
					else printf("  ");
				}
				printf("\n");
			}
		}
	}

	if (e) {

		if (v >= 1) printf("\n Loading public key ... ");

		load_pk_key(pk);

		if (v >= 1) printf("\t Complete ");
		if (v >= 3) {
			printf("\n pk: "); for (int i = 0; i < CRYPTO_PUBLICKEYBYTES; i++) printf("%02x", pk[i]);
		}

		if (v >= 1) printf("\n Generating shared secret and ciphertext in SW and HW/SW... ");

		// --------- CIFRADO SOFTWARE --------------------- //

		unsigned char rm_seed[NTRU_SAMPLE_RM_BYTES];
		randombytes(rm_seed, NTRU_SAMPLE_RM_BYTES);

		// Generar r, h y m a partir de semillas
		gen_rhmss(r, poly_r, h, poly_h, poly_m, pk, ss_hw, DBG, N_COEFFS, rm_seed);

		enc_ntru_3round(ct, poly_r, poly_h, poly_m, &tiempo_mult_3round, &tiempo_enc_3round, DBG, N_COEFFS);

		// --------- CIFRADO HARDWARE --------------------- //

		enc_ntru_hw(c_hw, r, h, poly_m, &tiempo_mult_hw, &tiempo_enc_hw, shm_1, shm_2, dma, DBG, N_COEFFS);

		if (v >= 1) {
			printf("\t Complete ");
			printf("\n");
			printf("\n      Time SW: \t %6llu us.", tiempo_enc_3round);
			printf("\n      Time HW: \t %6llu us.", tiempo_enc_hw);
			printf("\n Acceleration: \t %f", (float)tiempo_enc_3round / (float)tiempo_enc_hw);
			printf("\n");
		}

		if (v >= 1) printf("\n Storing ciphertext ... ");

		save_ct(ct);

		if (v >= 1) printf("\t Complete ");

		if (v >= 1) {
			printf("\n ss: "); for (int i = 0; i < CRYPTO_BYTES; i++) printf("%02x", ss_hw[i]);
		}
		if (v >= 2) {
			int mod = 24;

			printf("\n");

			int ind = 0;

			for (int i = 0; i < (int)ceil((double)CRYPTO_CIPHERTEXTBYTES / (double)mod); i++) {
				for (int j = 0; j < mod; j++) {
					ind = i * mod + j;
					if(ind < CRYPTO_CIPHERTEXTBYTES) printf("%02x", ct[ind]);
					else printf("  ");
				}
				printf("\t");
				for (int j = 0; j < mod; j++) {
					ind = i * mod + j;
					if (ind < CRYPTO_CIPHERTEXTBYTES) printf("%02x", c_hw[ind]);
					else printf("  ");
				}
				printf("\n");
			}

			
		}
	}

	if (d) {

		if (v >= 1) printf("\n Loading secret key ... ");

		load_sk_key(sk);

		if (v >= 1) printf("\t Complete ");
		if (v >= 1) printf("\n Loading ciphertext ... ");

		load_ct(ct);

		if (v >= 1) printf("\t Complete ");
		if (v >= 2) {
			printf("\n ct: ");

			int mod = 32;

			printf("\n");

			int ind = 0;

			for (int i = 0; i < (int)ceil((double)CRYPTO_CIPHERTEXTBYTES / (double)mod); i++) {
				for (int j = 0; j < mod; j++) {
					ind = i * mod + j;
					if (ind < CRYPTO_CIPHERTEXTBYTES) printf("%02x", ct[ind]);
					else printf("  ");
				}
				printf("\n");
			}

		}
		if (v >= 3) {
			printf("\n sk: ");

			int mod = 32;

			printf("\n");

			int ind = 0;

			for (int i = 0; i < (int)ceil((double)CRYPTO_SECRETKEYBYTES / (double)mod); i++) {
				for (int j = 0; j < mod; j++) {
					ind = i * mod + j;
					if (ind < CRYPTO_SECRETKEYBYTES) printf("%02x", sk[ind]);
					else printf("  ");
				}
				printf("\n");
			}
		}

		if (v >= 1) printf("\n Recovering shared secret in SW and HW/SW... ");

		dec_ntru_3round(ss, ct, sk, &tiempo_dec_3round, DBG, N_COEFFS);

		dec_ntru_hw(ss_hw, ct, sk, &tiempo_dec_hw, shm_1, shm_2, dma, DBG, N_COEFFS);

		if (v >= 1) {
			printf("\t Complete ");
			printf("\n");
			printf("\n      Time SW: \t %6llu us.", tiempo_dec_3round);
			printf("\n      Time HW: \t %6llu us.", tiempo_dec_hw);
			printf("\n Acceleration: \t %f", (float)tiempo_dec_3round / (float)tiempo_dec_hw);
			printf("\n");
		}

		if (v >= 1) {
			printf("\n ss_SW: "); for (int i = 0; i < CRYPTO_BYTES; i++) printf("%02x", ss[i]);
			printf("\n ss_HW: "); for (int i = 0; i < CRYPTO_BYTES; i++) printf("%02x", ss_hw[i]);
		}
	}

	printf("\n \n");

	return 0;

}