/*
 * simple_hashing.cpp
 *
 *  Created on: Oct 8, 2014
 *      Author: mzohner
 */

#include "simple_hashing.h"

struct simple_hash_output simple_hashing(uint8_t* elements, uint32_t neles, uint32_t nbins,
		uint32_t ntasks, hs_t &hs) {
	sht_ctx* table;
	//uint8_t** bin_content;
	uint8_t *res_bins;
	uint32_t i, j, tmpneles;
	sheg_ctx* ctx;
	pthread_t* entry_gen_tasks;
	uint32_t *nelesinbin = new uint32_t[sizeof(uint32_t) * nbins];


	entry_gen_tasks = (pthread_t*) malloc(sizeof(pthread_t) * ntasks);
	ctx = (sheg_ctx*) malloc(sizeof(sheg_ctx) * ntasks);
	table = (sht_ctx*) malloc(sizeof(sht_ctx) * ntasks);

	uint32_t  *elements_to_hash_table = new uint32_t[neles * NUM_HASH_FUNCTIONS];
	uint32_t  *bin_to_elements_to_hash_table = new uint32_t[neles * NUM_HASH_FUNCTIONS];

	for(i = 0; i < ntasks; i++) {
		init_hash_table(table + i, ceil_divide(neles, ntasks), &hs);
	}

	//for(i = 0; i < nbins; i++)
	//	pthread_mutex_init(locks+i, NULL);

	//tmpbuf = (uint8_t*) malloc(table->outbytelen);

	for(i = 0; i < ntasks; i++) {
		ctx[i].elements = elements;
		ctx[i].table = table + i;
		ctx[i].startpos = i * ceil_divide(neles, ntasks);
		ctx[i].endpos = min(ctx[i].startpos + ceil_divide(neles, ntasks), neles);
		ctx[i].hs = &hs;

		//cout << "Thread " << i << " starting from " << ctx[i].startpos << " going to " << ctx[i].endpos << " for " << neles << " elements" << endl;
		if(pthread_create(entry_gen_tasks+i, NULL, gen_entries, (void*) (ctx+i))) {
			cerr << "Error in creating new pthread at simple hashing!" << endl;
			exit(0);
		}
	}

	for(i = 0; i < ntasks; i++) {
		if(pthread_join(entry_gen_tasks[i], NULL)) {
			cerr << "Error in joining pthread at simple hashing!" << endl;
			exit(0);
		}
	}

	uint8_t *hashed_elements = new uint8_t[neles*hs.outbytelen*sizeof(uint8_t)];
	uint32_t k = 0;
	for(i = 0; i < ntasks; i++) {
		uint32_t arr_size = (ctx[i].endpos-ctx[i].startpos)*hs.outbytelen;
		memcpy(hashed_elements+k,ctx[i].hashed_elements, arr_size);
		k = k + arr_size;
	}

	//for(i = 0, eleptr=elements; i < neles; i++, eleptr+=inbytelen) {
	//	insert_element(table, eleptr, tmpbuf);
	//}

	//malloc and copy simple hash table into hash table
	//bin_content = (uint8_t**) malloc(sizeof(uint8_t*) * nbins);
	//*nelesinbin = (uint32_t*) malloc(sizeof(uint32_t) * nbins);

	res_bins = new uint8_t[neles * NUM_HASH_FUNCTIONS * hs.outbytelen];

	uint32_t progress = 0;

	for(i = 0; i < hs.nbins; i++) {
		nelesinbin[i] = 0;
		for(j = 0; j < ntasks; j++) {
			tmpneles = (table +j)->bins[i].nvals;
			nelesinbin[i] += tmpneles;
			//bin_content[i] = (uint8_t*) malloc(nelesinbin[i] * table->outbytelen);
			memcpy(res_bins+progress*hs.outbytelen, (table + j)->bins[i].values, tmpneles * hs.outbytelen);

			for (uint32_t l = 0; l < tmpneles; l++) {
				bin_hash_ctx *c = (table +j)->bins[i].indexes + l;
				elements_to_hash_table[NUM_HASH_FUNCTIONS*c->index+c->hashFunctionNum] = progress + l;
				bin_to_elements_to_hash_table[progress + l]=NUM_HASH_FUNCTIONS*c->index+c->hashFunctionNum;
			}

			progress += tmpneles;
		}
		//right now only the number of elements in each bin is copied instead of the max bin size
	}

	for(j = 0; j < ntasks; j++)
		free_hash_table(table + j);
	free(table);
	free(entry_gen_tasks);
	free(ctx);

	//for(i = 0; i < nbins; i++)
	//	pthread_mutex_destroy(locks+i);
	//free(locks);

	struct simple_hash_output output;
	output.hashed_elements = hashed_elements;
	output.res_bins = res_bins;
	output.nelesinbin = nelesinbin;
	output.elements_to_hash_table = elements_to_hash_table;
	output.bin_to_elements_to_hash_table = bin_to_elements_to_hash_table;

	return output;
}

void *gen_entries(void *ctx_tmp) {
	//Insert elements in parallel, use lock to communicate
	uint8_t *tmpbuf, *eleptr;
	sheg_ctx* ctx = (sheg_ctx*) ctx_tmp;
	uint32_t i, inbytelen, *address;

	ctx->hashed_elements = (uint8_t*) malloc((ctx->endpos- ctx->startpos)* ctx->hs->outbytelen * sizeof(uint8_t));
	address = (uint32_t*) malloc(NUM_HASH_FUNCTIONS * sizeof(uint32_t));
	tmpbuf = (uint8_t*) calloc(ceil_divide(ctx->hs->outbitlen, 8), sizeof(uint8_t));	//for(i = 0; i < NUM_HASH_FUNCTIONS; i++) {
	//	tmpbuf[i] = (uint8_t*) malloc(ceil_divide(ctx->hs->outbitlen, 8));
	//}

	for(i = ctx->startpos, eleptr=ctx->elements, inbytelen=ctx->hs->inbytelen; i < ctx->endpos; i++, eleptr+=inbytelen) {
		insert_element(i, ctx->table, eleptr, address, tmpbuf, ctx->hs);
		memcpy(ctx->hashed_elements+(i-ctx->startpos)*ctx->hs->outbytelen, tmpbuf, ctx->hs->outbytelen*sizeof(uint8_t));
	}
	free(tmpbuf);
	free(address);
}

inline void insert_element(uint32_t index, sht_ctx* table, uint8_t* element, uint32_t* address, uint8_t* tmpbuf, hs_t* hs) {
	uint32_t i, j;
	bin_ctx* tmp_bin;

	hashElement(element, address, tmpbuf, hs);

	for(i = 0; i < NUM_HASH_FUNCTIONS; i++) {

		tmp_bin=table->bins + address[i];
		bin_hash_ctx bhc{index, i};
		//pthread_mutex_lock(locks + address[i]);
		memcpy(tmp_bin->values + tmp_bin->nvals * hs->outbytelen, tmpbuf, hs->outbytelen);
		tmp_bin->indexes[tmp_bin->nvals].index = index;
		tmp_bin->indexes[tmp_bin->nvals].hashFunctionNum = i;
		for(j = 0; j < i; j++) {
			if(address[i] == address[j]) {
				memset(tmp_bin->values + tmp_bin->nvals * hs->outbytelen, DUMMY_ENTRY_SERVER, hs->outbytelen);
			}
		}
		tmp_bin->nvals++;
		//TODO: or simply allocate a bigger block of memory: table->maxbinsize * 2, left out for efficiency reasons
		if(tmp_bin->nvals == table->maxbinsize) {
			increase_max_bin_size(table, hs->outbytelen);
		}
		//assert(tmp_bin->nvals < table->maxbinsize);
		/*cout << "Inserted into bin: " << address << ": " << (hex);
		for(uint32_t j = 0; j < table->outbytelen; j++) {
			cout << (unsigned int) tmpbuf[j];
		}
		cout << (dec) << endl;*/
		//pthread_mutex_unlock(locks + address[i]);
	}
}

void init_hash_table(sht_ctx* table, uint32_t nelements, hs_t* hs) {
	uint32_t i;

	//table->addrbitlen = ceil_log2(nbins);
	//table->addrbytelen = ceil_divide(table->addrbitlen, 8);
	//table->inbytelen = ceil_divide(inbitlen, 8);
	if(ceil_divide(nelements, hs->nbins) < 3)
		table->maxbinsize = 3*max(ceil_log2(nelements),3);
	else
		table->maxbinsize = 6*max((int) ceil_divide(nelements, hs->nbins), 3);
	//cout << "maxbinsize = " << table->maxbinsize << endl;
	//table->outbytelen = ceil_divide(getOutBitLen(inbitlen, nbins), 8);
	table->nbins = hs->nbins;

	table->bins = (bin_ctx*) calloc(hs->nbins, sizeof(bin_ctx));

	for(i = 0; i < hs->nbins; i++) {
		table->bins[i].values = (uint8_t*) malloc(table->maxbinsize * hs->outbytelen);
		table->bins[i].indexes = (bin_hash_ctx*) malloc(table->maxbinsize * sizeof(bin_hash_ctx));
	}
}

void free_hash_table(sht_ctx* table) {
	uint32_t i;
	//1. free the byte-pointers for the values in the bints
	for(i = 0; i < table->nbins; i++) {
		//if(table->bins[i].nvals > 0)
			free(table->bins[i].values);
			free(table->bins[i].indexes);
	}
	//2. free the bins
	free(table->bins);
	//3. free the actual table
	//free(table);
}

void increase_max_bin_size(sht_ctx* table, uint32_t valbytelen) {
	uint32_t new_maxsize = table->maxbinsize * 2;
	uint8_t* tmpvals;
	bin_hash_ctx* tmpindexes;
	for(uint32_t i = 0; i < table->nbins; i++) {
		tmpvals = table->bins[i].values;
		tmpindexes = table->bins[i].indexes;
		table->bins[i].values = (uint8_t*) malloc(new_maxsize * valbytelen);
		table->bins[i].indexes = (bin_hash_ctx*) malloc(new_maxsize * sizeof(bin_hash_ctx));
		memcpy(table->bins[i].values, tmpvals, table->bins[i].nvals * valbytelen);
		memcpy(table->bins[i].indexes, tmpindexes, table->bins[i].nvals * sizeof(bin_hash_ctx));
		free(tmpvals);
		free(tmpindexes);
	}
	table->maxbinsize = new_maxsize;
}
