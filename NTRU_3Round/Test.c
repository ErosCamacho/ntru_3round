#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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

int main(int argc, char** argv){
	uint16_t r[NTRU_N];
	uint16_t h[NTRU_N];
	uint16_t e[NTRU_N];
	uint16_t e_HW[NTRU_N];
	poly x1, x2, x3, x4, x5, x6;
	poly* poly_r = &x1;
	poly* poly_m = &x4;
	poly* poly_h = &x2;
	poly* poly_e = &x3;
	poly* liftm = &x5;

	int DBG = 1;
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
	unsigned char pk[CRYPTO_PUBLICKEYBYTES], sk[CRYPTO_SECRETKEYBYTES];
	unsigned char ss[CRYPTO_BYTES], ss_dec[CRYPTO_BYTES], ss_dec_hw[CRYPTO_BYTES];
	unsigned char ss_enc_kem[CRYPTO_BYTES], ss_dec_kem[CRYPTO_BYTES];
	unsigned char c[CRYPTO_CIPHERTEXTBYTES], c_libntru[CRYPTO_CIPHERTEXTBYTES], c_hw[CRYPTO_CIPHERTEXTBYTES];
	unsigned char ct[CRYPTO_CIPHERTEXTBYTES];

	unsigned long long tic = 0, toc;
	unsigned long long tiempo_mult_3round;
	unsigned long long tiempo_mult_libntru;
	unsigned long long tiempo_mult_hw;
	unsigned long long tiempo_enc_3round;
	unsigned long long tiempo_enc_libntru;
	unsigned long long tiempo_enc_hw;
	unsigned long long tiempo_dec_3round;
	unsigned long long tiempo_dec_hw;
	unsigned long long tiempo_total_mult_1 = 0;
	unsigned long long tiempo_total_mult_2 = 0;
	unsigned long long tiempo_total_mult_hw = 0;
	unsigned long long tiempo_total_enc_1 = 0;
	unsigned long long tiempo_total_enc_2 = 0;
	unsigned long long tiempo_total_enc_hw = 0;
	unsigned long long tiempo_total_dec = 0;
	unsigned long long tiempo_total_dec_hw = 0;

	FILE* fichero;

	for (int arg = 1; arg < argc; arg++) {
		if (argv[arg][0] == '-') {
			if (argv[arg][1] == 'h') {
				printf("\nUsage: ./Test [-h] [-hh][-n] [-M] [-y] [-d] [-c]   \n\n");

				printf("\n -h : Show the help.");
				printf("\n -hh : Show the extended help.");
				printf("\n -n : Number of test to perform.");
				printf("\n -M : Paralelization coefficient.");
				printf("\n -y : max_coef parameter.");
				printf("\n");

				printf("\n -d : DBG");
				printf("\n -c : number of coefficiente in DBG.");

				if (argv[arg][2] == 'h') {
					printf("\nTEMPORAL ANALYSIS : \n");
					//printf("DBG == 0: Se muestra cada 20 test. Minimizar la impresion por pantalla \n");
					printf("DBG == 0: Minimize the print in window \n");
					printf("DBG == 1: Show the time in each part of the algorithm \n");
					printf("DBG == 2: Show the extended evaluation of time \n");
					printf("\nFUNCTION ANALYSIS : \n");
					printf("DBG == 3: Show the coefficients of SW and HW \n");
					printf("DBG == 4: Show the multiplication operation in SW. \n");
					printf("DBG == 5: Show the multiplication operation in HW. \n");
					printf("DBG == 6: Show the public key. \n");
					//printf("DBG == 7: Se muestra la semilla, numero de 1's y -1's en r. Tambien muestra los coeficientes de r y h. \n");
					printf("DBG == 7: Show the seed and he coefficients of r and h. \n");
					printf("DBG == 8: Show the multiplication operation in SW 3 ROUND. \n");
					printf("DBG == 9: Show the cuphertext of 3 ROUND, LIBNTRU, HW. \n");
					printf("DBG == 10: Show the hash of rm. \n");
					printf("DBG == 11: ONLY FOR PERFORMING THE SEED ANALYSIS. It generates the file r.txt \n");
				}
				return 0;
			}
			else if (argv[arg][1] == 'd') {
				if (arg < argc - 1) DBG = atoi(argv[arg + 1]);
			}
			else if (argv[arg][1] == 'n') {
				if (arg < argc - 1) N_TEST = atoi(argv[arg + 1]);
			}
			else if (argv[arg][1] == 'c') {
				if (arg < argc - 1) N_COEFFS = atoi(argv[arg + 1]);
			}
			else if (argv[arg][1] == 'M') {
				if (arg < argc - 1) M = atoi(argv[arg + 1]);
			}
			else if (argv[arg][1] == 'y') {
				if (arg < argc - 1) max_cycles = atoi(argv[arg + 1]);
			}
			else {
				printf("\nunknow option: %s\n", argv[arg]);
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

	// ------- ANALISIS DE LA SEMILLA ------ //

	if (DBG == 11) {
		FILE* fichero;
		fichero = fopen("r.txt", "wt");

		for (int i = 0; i < 1E6; i++) {
			unsigned char rm_seed[NTRU_SAMPLE_RM_BYTES];
			randombytes(rm_seed, NTRU_SAMPLE_RM_BYTES);

			poly x1, x2;
			poly* poly_r = &x1;
			poly* poly_m = &x2;
			sample_rm(poly_r, poly_m, rm_seed);

			uint16_t r_analysis[NTRU_N];
			int r_exit[3];
			poly_Z3_to_Zq(poly_r);
			poly_to_array(r_analysis, poly_r);
			num_ones_minus_exit(r_analysis, r_exit, DBG);

			fprintf(fichero, "%d %d %d \n", r_exit[0], r_exit[1], r_exit[2]);
			printf("%d \n", i);
		}
		fclose(fichero);
	}

	printf(" \n --- STARTING TESTS --- \n ");

	if (DBG == 1) {
		char str[30] = "result_test/result_";
		char buf[3];
		sprintf(buf, "%d_", max_cycles);
		strcat(str, buf);
		sprintf(buf, "%d", M);
		strcat(str, buf);
		strcat(str, ".txt");
		fichero = fopen(str, "wt");
		fprintf(fichero, "ID \t MULT(SW) \t MULT(HW) \t ACEL. MULT. \t ENC(SW) \t ENC(HW) \t ACEL. ENC. \t DEC(SW) \t DEC(HW) \t ACEL. DEC. \n");
	}
	
	for (int n_test = 1; n_test <= N_TEST; n_test++) {

		if (DBG == 1 || DBG == 2) printf("\n --- TEST %d --- \n ", n_test);

		// -------- GENERACION CLAVES ------------------- // 

		// Generar Clave Publica y Privada
		gen_keys(pk, sk, DBG);

		// --------- CIFRADO SOFTWARE --------------------- //

		unsigned char rm_seed[NTRU_SAMPLE_RM_BYTES];
		randombytes(rm_seed, NTRU_SAMPLE_RM_BYTES);

		// Generar r, h y m a partir de semillas
		gen_rhmss(r, poly_r, h, poly_h, poly_m, pk, ss, DBG, N_COEFFS, rm_seed);

		crypto_kem_enc(ct, ss_enc_kem, pk, rm_seed);

		enc_ntru_3round(c, poly_r, poly_h, poly_m, &tiempo_mult_3round, &tiempo_enc_3round, DBG, N_COEFFS);
		tiempo_total_mult_1 += tiempo_mult_3round;
		tiempo_total_enc_1 += tiempo_enc_3round;

		enc_ntru_libntru(c_libntru, r, h, poly_m, &tiempo_mult_libntru, &tiempo_enc_libntru, DBG, N_COEFFS);
		tiempo_total_mult_2 += tiempo_mult_libntru;
		tiempo_total_enc_2 += tiempo_enc_libntru;

		// --------- CIFRADO HARDWARE --------------------- //

		enc_ntru_hw(c_hw, r, h, poly_m, &tiempo_mult_hw, &tiempo_enc_hw, shm_1, shm_2, dma, DBG, N_COEFFS);
		tiempo_total_mult_hw += tiempo_mult_hw;
		tiempo_total_enc_hw += tiempo_enc_hw;

		// --------- DESCIFRADO SOFTWARE --------------------- //

		crypto_kem_dec(ss_dec_kem, ct, sk);

		dec_ntru_3round(ss_dec, c, sk, &tiempo_dec_3round, DBG, N_COEFFS);
		tiempo_total_dec += tiempo_dec_3round;

		// --------- DESCIFRADO HARDWARE --------------------- //

		dec_ntru_hw(ss_dec_hw, c_hw, sk, &tiempo_dec_hw, shm_1, shm_2, dma, DBG, N_COEFFS);
		tiempo_total_dec_hw += tiempo_dec_hw;

		// ---------- CHECKEAR IGUALDADES CIFRADO -------------- //

		if (DBG == 10) {
			printf("\n ss_enc_kem: ");
			for (unsigned long long i = 0; i < CRYPTO_BYTES; i++) printf("%02X", ss_enc_kem[i]);
			printf("\n");
			printf("\n ss_dec_kem: ");
			for (unsigned long long i = 0; i < CRYPTO_BYTES; i++) printf("%02X", ss_dec_kem[i]);
			printf("\n");
			printf("\n ss: ");
			for (unsigned long long i = 0; i < CRYPTO_BYTES; i++) printf("%02X", ss[i]);
			printf("\n");
			printf("\n ss_dec: ");
			for (unsigned long long i = 0; i < CRYPTO_BYTES; i++) printf("%02X", ss_dec[i]);
			printf("\n");
			printf("\n ss_dec_hw: ");
			for (unsigned long long i = 0; i < CRYPTO_BYTES; i++) printf("%02X", ss_dec_hw[i]);
			printf("\n");
		}

		if (DBG == 9) {
			printf("\n c_kem: ");
			for (unsigned long long i = 0; i < CRYPTO_CIPHERTEXTBYTES; i++) printf("%02X", ct[i]);
			printf("\n");
			printf("\n c: ");
			for (unsigned long long i = 0; i < CRYPTO_CIPHERTEXTBYTES; i++) printf("%02X", c[i]);
			printf("\n");
			printf("\n c_libntru: ");
			for (unsigned long long i = 0; i < CRYPTO_CIPHERTEXTBYTES; i++) printf("%02X", c_libntru[i]);
			printf("\n");
			printf("\n c_HW: ");
			for (unsigned long long i = 0; i < CRYPTO_CIPHERTEXTBYTES; i++) printf("%02X", c_hw[i]);
			printf("\n");
		}


		tic = PYNQ_Wtime();
			comp_SW = comprobar_igualdad_c(ct, c);
			if (comp_SW != 0) fallos_enc++;
			comp_HW = comprobar_igualdad_c(ct, c_hw);
			if (comp_HW != 0) fallos_enc_HW++;
		toc = PYNQ_Wtime() - tic;
		if (DBG == 2 && comp_SW == 0) printf("\n Check Equality  ENC. (SW) \t ... \t (%6llu us.) OK \n", toc);
		if (DBG == 2 && comp_SW != 0) printf("\n Check Equality ENC. (SW) \t ... \t (%6llu us.) FAIL \n", toc);
		if (DBG == 2 && comp_HW == 0) printf(" Check Equality ENC. (HW) \t ... \t (%6llu us.) OK \n ", toc);
		if (DBG == 2 && comp_HW != 0) printf(" Check Equality ENC. (HW) \t ... \t (%6llu us.) FAIL \n ", toc);

		comp_crypto		= comprobar_igualdad_crypto(ss_dec_kem, ss_dec);
		comp_crypto_HW	= comprobar_igualdad_crypto(ss_dec_kem, ss_dec_hw);
		if (comp_crypto		!= 0) fallos_crypto++;
		if (comp_crypto_HW	!= 0) fallos_crypto_HW++;
		if ((DBG == 1 || DBG == 2) && comp_crypto == 0 && comp_crypto_HW == 0) printf("\n Everything works OK \n");
		if ((DBG == 1 || DBG == 2) && comp_crypto != 0 && comp_crypto_HW == 0) printf("\n HW works OK \n");
		if ((DBG == 1 || DBG == 2) && comp_crypto == 0 && comp_crypto_HW != 0) printf("\n SW works OK \n");
		if ((DBG == 1 || DBG == 2) && comp_crypto != 0 && comp_crypto_HW != 0) printf("\n Cryptosystem FAIL \n");

		// ---------- IMPRESION COEFICIENTES ------------ //
		if (DBG == 3 || DBG == 5) {
			printf("\n i -> poly_r->coeffs[i] \t poly_h->coeffs[i] \t poly_e->coeffs[i] \t e[i] \t e_HW[i]");
			for (int i = 0; i < N_COEFFS; i++) {
				printf("\n %d -> %d \t %d \t %d \t %d \t %d", i, poly_r->coeffs[i], poly_h->coeffs[i], poly_e->coeffs[i], e[i], e_HW[i]);
			}
		}

		// ---- REDUCCION DE ESCRITO EN PANTALLA -------- //
		if (DBG == 0) {
			if(n_test % 20 == 0 || n_test == 1) printf("\n %d ", n_test);
		}

		// ---- ALMACENAMIENTO FICHERO ----- //
		if (DBG == 1) {
			fprintf(fichero, "%d \t %6llu \t %6llu \t %f \t %6llu \t %6llu \t %f \t %6llu \t %6llu \t %f \n", n_test, tiempo_mult_3round, tiempo_mult_hw, (float)tiempo_mult_3round / (float)tiempo_mult_hw, tiempo_enc_3round, tiempo_enc_hw, (float)tiempo_enc_3round / (float)tiempo_enc_hw, tiempo_dec_3round, tiempo_dec_hw, (float)tiempo_dec_3round / (float)tiempo_dec_hw);
		}

	}


	// --------- IMPRESION DE VALORES MEDIOS ------------ //
	printf("\n\n ### --  RESULTS  -- ### \n");
	printf("\n AVERAGE VALUES \n \n");
	printf(" Mult. v. NTRU 3 Round (SW) \t ... \t (%6llu us.) \n", tiempo_total_mult_1 / N_TEST);
	printf(" Mult. v. NTRU Encrypt (SW) \t ... \t (%6llu us.) \n", tiempo_total_mult_2 / N_TEST);
	printf(" Mult. v. NTRU HARDWARE (HW) \t ... \t (%6llu us.) \n \n", tiempo_total_mult_hw / N_TEST);
	printf(" ENC. v. NTRU 3 Round (SW) \t ... \t (%6llu us.) \n", tiempo_total_enc_1 / N_TEST);
	printf(" ENC. v. NTRU Encrypt (SW) \t ... \t (%6llu us.) \n", tiempo_total_enc_2 / N_TEST);
	printf(" ENC. v. NTRU HARDWARE (HW) \t ... \t (%6llu us.) \n \n", tiempo_total_enc_hw / (unsigned long long)N_TEST);
	printf(" DEC. v. NTRU 3 Round (SW) \t ... \t (%6llu us.) \n", tiempo_total_dec / (unsigned long long)N_TEST);
	printf(" DEC. v. NTRU HARDWARE (HW) \t ... \t (%6llu us.) \n \n", tiempo_total_dec_hw / (unsigned long long)N_TEST);
	printf(" ACCELERATION Mult.:  %f \n",	(float)tiempo_total_mult_1 / (float)tiempo_total_mult_hw);
	printf(" ACCELERATION ENC:  %f \n",		(float)tiempo_total_enc_1 / (float)tiempo_total_enc_hw);
	printf(" ACCELERATION DEC:  %f \n \n",	(float)tiempo_total_dec / (float)tiempo_total_dec_hw);
	printf(" FAILS_ENC_SW: %d \n", fallos_enc);
	printf(" FAILS_ENC_HW: %d \n", fallos_enc_HW);
	printf(" FAILS_CRYPTO_SW: %d \n", fallos_crypto);
	printf(" FAILS_CRYPTO_HW: %d \n", fallos_crypto_HW);

	PYNQ_closeDMA(&dma);
	PYNQ_freeSharedMemory(&shm_1);
	PYNQ_freeSharedMemory(&shm_2);

	return 0;
}