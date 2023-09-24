
CC=/usr/bin/cc
LDFLAGS=-lcrypto -lpynq -lcma -lpthread -lm
CFLAGS = -DCRYPTO_NAMESPACE\(s\)=ntru_\#\#s

SOURCES = poly.c pack3.c packq.c poly_lift.c poly_mod.c poly_r2_inv.c poly_rq_mul.c poly_s3_inv.c cmov.c fips202.c sample.c sample_iid.c crypto_sort_int32.c rng.c kem.c owcpa.c Funciones_mat.c Funciones_gen_enc_dec.c Funciones_HW.c
HEADERS = params.h api.h api_bytes.h crypto_hash_sha3256.h cmov.h fips202.h poly.h kem.h owcpa.h sample.h crypto_sort_int32.h rng.h Funciones_mat.h Funciones_gen_enc_dec.h Funciones_HW.h

SOURCES_DEMO = $(SOURCES) Funciones_demo.c
HEADERS_DEMO = $(HEADERS) Funciones_demo.h

all: Test_509 Test_677 Test_701 Test_821 Demo_509 Demo_677 Demo_701 Demo_821
#all: Demo_509

#Test: $(HEADERS) $(SOURCES)
#	$(CC) -DCRYPTO_NAMESPACE\(s\)=ntru_\#\#s -o $@ $(SOURCES) $(LDFLAGS)

Test_509: $(HEADERS) $(SOURCES) Test.c
	$(CC) -DCRYPTO_NAMESPACE\(s\)=ntru_\#\#s -DN_VALUE=509 -o $@ $(SOURCES) Test.c $(LDFLAGS)

Test_677: $(HEADERS) $(SOURCES) Test.c
	$(CC) -DCRYPTO_NAMESPACE\(s\)=ntru_\#\#s -DN_VALUE=677 -o $@ $(SOURCES) Test.c $(LDFLAGS)

Test_821: $(HEADERS) $(SOURCES) Test.c
	$(CC) -DCRYPTO_NAMESPACE\(s\)=ntru_\#\#s -DN_VALUE=821 -o $@ $(SOURCES) Test.c $(LDFLAGS)

Test_701: $(HEADERS) $(SOURCES) Test.c
	$(CC) -DCRYPTO_NAMESPACE\(s\)=ntru_\#\#s -DN_VALUE=701 -o $@ $(SOURCES) Test.c $(LDFLAGS)

Demo_509: $(HEADERS_DEMO) $(SOURCES_DEMO) demo.c
	$(CC) -DCRYPTO_NAMESPACE\(s\)=ntru_\#\#s -DN_VALUE=509 -o $@ $(SOURCES_DEMO) demo.c $(LDFLAGS)

Demo_677: $(HEADERS_DEMO) $(SOURCES_DEMO) demo.c
	$(CC) -DCRYPTO_NAMESPACE\(s\)=ntru_\#\#s -DN_VALUE=677 -o $@ $(SOURCES_DEMO) demo.c $(LDFLAGS)

Demo_821: $(HEADERS_DEMO) $(SOURCES_DEMO) demo.c
	$(CC) -DCRYPTO_NAMESPACE\(s\)=ntru_\#\#s -DN_VALUE=821 -o $@ $(SOURCES_DEMO) demo.c $(LDFLAGS)

Demo_701: $(HEADERS_DEMO) $(SOURCES_DEMO) demo.c
	$(CC) -DCRYPTO_NAMESPACE\(s\)=ntru_\#\#s -DN_VALUE=701 -o $@ $(SOURCES_DEMO) demo.c $(LDFLAGS)

.PHONY: clean

clean:
	-rm Test Test_509 Test_677 Test_701 Test_821 Demo_509 Demo_677 Demo_701 Demo_821
