/**
 \file 		parse_options.h
 \author 	michael.zohner@ec-spride.de

 \brief		Parse Options Implementation
 */

#ifndef UTIL_PARSE_OPTIONS_H_
#define UTIL_PARSE_OPTIONS_H_

#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <stdint.h>

enum etype {
	T_NUM, T_STR, T_FLAG, T_DOUBLE
};

typedef struct {
	void* val;
	etype type;
	char opt_name;
	std::string help_str;
	bool required;
	bool set;
} parsing_ctx;

int32_t parse_options(int32_t* argcp, char*** argvp, parsing_ctx* options, uint32_t nops);
void print_usage(std::string progname, parsing_ctx* options, uint32_t nops);

#endif /* PARSE_OPTIONS_H_ */
