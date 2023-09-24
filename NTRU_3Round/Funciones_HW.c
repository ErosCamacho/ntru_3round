#include "poly.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pynq_api.h"

void load_ntru_hw(int DBG, int M, int max_cycles) {

	unsigned long long tic = 0, toc;
	int Status;
	
	char bitstream_file[80] = "/home/xilinx/NTRU_3Round/bit/"; 
	char M_str[3];
	char max_str[3];
	sprintf(M_str, "%d", M);
	sprintf(max_str, "%d", max_cycles);
	strcat(bitstream_file, max_str);
	strcat(bitstream_file, "/");
	strcat(bitstream_file, M_str);
	strcat(bitstream_file, "/BD_wrapper.bit");
	
	//char bitstream_file[80] = "/home/xilinx/NTRU_3Round/bit/BD_wrapper.bit";

	FILE* file;

	if (!(file = fopen(bitstream_file, "r"))) {
		printf("\n   Bitstream not available. Bye ...\n\n");
	}
	fclose(file);

	tic = PYNQ_Wtime();
		Status = PYNQ_loadBitstream(bitstream_file);
		if (Status != PYNQ_SUCCESS) {
			printf(" Failed Loading !!!\n");
		}
	toc = PYNQ_Wtime() - tic;
	if(DBG == 1) printf(" Load Bitsream (HW) \t \t ... \t (%6llu us.) \n ", toc);
}

void ntru_mult_ms2xs(uint16_t* a, uint16_t* b, uint16_t* c,
	PYNQ_SHARED_MEMORY shared_memory_1,
	PYNQ_SHARED_MEMORY shared_memory_2,
	PYNQ_AXI_DMA dma, int DBG) {

	uint16_t N = NTRU_N;
	uint16_t* r = a;
	uint16_t* h = b;

	unsigned long long tic = 0, toc;

	int* di = (int*)shared_memory_1.pointer;
	int* eo = (int*)shared_memory_2.pointer;

	for (int i = 0; i < NTRU_N; i++) {
		if (r[i] == ((1 << NTRU_LOGQ) - 1)) r[i] = 3;
	}

	/* Generate coefficients */
	tic = PYNQ_Wtime();
	for (int i = 0; i < NTRU_N; i++) {
		di[i] = (unsigned int)(h[i] << 4) + (r[i] << 2);
	}
	toc = PYNQ_Wtime() - tic;
	if (DBG == 3) printf(" Generando Coeficientes (HW) \t ... \t (%6llu us.) \n ", toc);

	/* Operate */
	tic = PYNQ_Wtime();
	PYNQ_writeDMA(&dma, &shared_memory_1, 0, sizeof(int) * N);
	PYNQ_readDMA(&dma, &shared_memory_2, 0, sizeof(int) * N);
	PYNQ_waitForDMAComplete(&dma, AXI_DMA_WRITE);
	PYNQ_waitForDMAComplete(&dma, AXI_DMA_READ);

	for (int i = 0; i < NTRU_N; i++) {
		c[i] = eo[i];
	}

	if (DBG == 5) {
		printf("\n i -> a[i] \t b[i] \t b[i] << 4 \t a[i] << 2 \t  di[i] \t c[i]");
		for (int i = 0; i < 10; i++) {
			printf("\n %d -> %d \t %d \t %d \t \t %d \t \t %d \t %d", i, r[i], h[i], h[i] << 4, r[i] << 2, di[i], c[i]);
		}
	}

	toc = PYNQ_Wtime() - tic;
	if (DBG == 3) printf(" Operando (HW) \t \t ... \t (%6llu us.) \n ", toc);

}
