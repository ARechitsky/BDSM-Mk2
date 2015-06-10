#include "BDSCompiler.h"
#include "LineParser.h"
#include "tokens.h"
#include "byte_code.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

BDSCompiler::BDSCompiler(void)
{
	fp = 0;
	linecnt = 0;
	byte_code.resize(0);
}


BDSCompiler::~BDSCompiler(void)
{
}

int BDSCompiler::parseScript()
{
	char str[MAX_LINELENGTH];

	for (;;)
	{
        char *p=str;
		*p=0;
		fgets(str,MAX_LINELENGTH,fp);
		if (feof(fp)&&!str[0]) break;

		// remove trailing whitespace
		while (*p) p++;
		if (p > str) p--;
		while (p >= str && (*p == '\r' || *p == '\n' || *p == ' ' || *p == '\t')) p--;
		*++p=0;

		int ret=doParse(str);

		if (ret != PS_OK) return ret;
	}

	return PS_OK;
}

int BDSCompiler::doParse(const char *str)
{
	LineParser line;
	int res = line.parse((char *)str);
	if (res)
	{
		return PS_ERROR;
	}

	if(!line.getcommand())
	{
		return PS_OK;
	}

    int tkid, int_tmp;
    float flt_tmp;
    char constVal[4];
    char type = line.gettype();
    bool isConst = line.getconstflag();

	if (!isConst)
    {
        tkid = get_commandtoken(line.getcommand());
        if (tkid == -1)
        {
            return PS_ERROR;
        }
    }
    else
    {
        tkid = TOK_CONST;
        switch (type)
        {
        case 'F':
            flt_tmp = atof(line.getcommand());
            memcpy(constVal, &flt_tmp, 4);
            break;
        default:
            int_tmp = atoi(line.getcommand());
            memcpy(constVal, &int_tmp, 4);
            break;
        }
    }

	char command_code;
	command_code = tkid << 4;
	command_code |= RESERVED;
	switch (type)
	{
	case 'C':
		command_code |= TYPE_CHAR;
		break;
	case 'W':
		command_code |= TYPE_WORD;
		break;
	case 'D':
		command_code |= TYPE_DWORD;
		break;
	case 'F':
		command_code |= TYPE_FLOAT;
		break;
	default:
		command_code |= TYPE_BYTE;
		break;
	}

	byte_code.add(&command_code, 1);
	if (isConst)
    {
        byte_code.add(constVal, 4);
    }
	linecnt++;

	return PS_OK;
}

int BDSCompiler::getLineCount()
{
	return linecnt;
}

int BDSCompiler::getCodeSize()
{
    return byte_code.getlen();
}

const char *BDSCompiler::getByteCode()
{
	return (const char *)byte_code.get();
}

int BDSCompiler::Open(const char *filename)
{
	fp = fopen(filename, "r");
	if (!fp)
	{
		return PS_FILE_ERROR;
	}
	int res = parseScript();
	fclose(fp);
	return res;
}

int BDSCompiler::Save(const char *filename)
{
	FILE *f = fopen(filename, "wb");
	if (!fp)
	{
		return PS_FILE_ERROR;
	}
	fwrite(&linecnt, sizeof(linecnt), 1, f);
	fwrite(getByteCode(), 1, byte_code.getlen(), f);
	fclose(f);
	return 0;
}
