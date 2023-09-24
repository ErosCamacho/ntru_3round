#include "kem.h"
#include "params.h"
#include "api.h"

void well_vec(unsigned char* ar, const size_t size);
void gen_files();
void save_keys(unsigned char* pk, unsigned char* sk);
void save_ct(unsigned char* ct);
void save_ss(unsigned char* ss);
void load_ct(unsigned char* ct);
void load_pk_key(unsigned char* pk);
void load_sk_key(unsigned char* sk);
void char_to_hex(unsigned char in0, unsigned char in1, unsigned char* out);
int check_hex(unsigned char in);

