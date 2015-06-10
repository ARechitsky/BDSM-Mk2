#pragma once

#define MAX_LINELENGTH 256

#define PS_OK 0
#define PS_ERROR 50
#define PS_FILE_ERROR -1

#include "growbuf.h"
#include <stdio.h>

class BDSCompiler
{
public:
	BDSCompiler();
	~BDSCompiler();
	int getLineCount();
   	int getCodeSize();
	const char *getByteCode();
	int Open(const char *filename);
	int Save(const char *filename);

private:
	bool is_valid_token(char *s);
	int get_commandtoken(char *s);
	int parseScript();
	int doParse(const char *str);

	FILE *fp;
	short int linecnt;
	GrowBuf byte_code;
};

