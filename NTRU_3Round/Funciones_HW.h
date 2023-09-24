
void load_ntru_hw(int DBG, int M, int max_cycles);
void ntru_mult_ms2xs(uint16_t* a, uint16_t* b, uint16_t* c,
	PYNQ_SHARED_MEMORY shared_memory_1,
	PYNQ_SHARED_MEMORY shared_memory_2,
	PYNQ_AXI_DMA dma, int DBG);