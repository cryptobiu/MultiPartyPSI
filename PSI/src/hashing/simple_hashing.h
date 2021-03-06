/*
 * simle_hashing.h
 *
 *  Created on: Oct 8, 2014
 *      Author: mzohner
 */

#ifndef SIMLE_HASHING_H_
#define SIMLE_HASHING_H_

#include "hashing_util.h"

struct bin_hash_ctx {
	uint32_t index;
	uint32_t hashFunctionNum;
};

struct bin_ctx {
	//hash-values of all elements mapped to this bin
	uint8_t* values;
	bin_hash_ctx* indexes;
	//number of elements stored in this bin
	uint32_t nvals;
};

typedef struct simple_hash_table_ctx {
	//pointer to the bins in the hash table
	bin_ctx* bins;
	//number bins in the hash table
	uint32_t nbins;
	//max bin size
	uint32_t maxbinsize;
	//uint32_t addrbitlen;
	//uint32_t addrbytelen;
	//uint32_t inbytelen;
	//uint32_t outbytelen;
} sht_ctx;

typedef struct simple_hash_entry_gen_ctx {
	sht_ctx* table;
	//input elements
	uint8_t* hashed_elements;
	uint8_t* elements;
	uint32_t startpos;
	uint32_t endpos;
	//uint32_t inbytelen;
	hs_t* hs;
} sheg_ctx;

typedef struct simple_hash_output {
	uint8_t *hashed_elements;
	uint8_t *res_bins;
	uint32_t *nelesinbin;
	uint32_t *elements_to_hash_table;
	uint32_t *bin_to_elements_to_hash_table;
} simple_hash_output;

//returns a cuckoo hash table with the first dimension being the bins and the second dimension being the pointer to the elements
struct simple_hash_output simple_hashing(uint8_t* elements, uint32_t neles, uint32_t nbins,
		uint32_t ntasks, hs_t &hs);
//routine for generating the entries, is invoked by the threads
void *gen_entries(void *ctx);
void init_hash_table(sht_ctx* table, uint32_t nelements, hs_t* hs);
void increase_max_bin_size(sht_ctx* table, uint32_t valbytelen);
void free_hash_table(sht_ctx* table);
inline void insert_element(uint32_t index, sht_ctx* table, uint8_t* element, uint32_t* address, uint8_t* tmpbuf, hs_t* hs);

#endif /* SIMLE_HASHING_H_ */
