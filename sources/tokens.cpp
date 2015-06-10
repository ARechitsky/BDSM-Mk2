#include "BDSCompiler.h"
#include "tokens.h"

#include <string.h>

typedef struct {
	int id;
	const char *name;
} tokenType;

static tokenType tokenlist[TOK__LAST] = {
	{TOK_ADD, "ADD"},
	{TOK_BYTE, "BYTE"},
	{TOK_DIV, "DIV"},
	{TOK_DROP, "DROP"},
	{TOK_DUP, "DUP"},
	{TOK_DWORD, "DWORD"},
	{TOK_FLOAT, "FLOAT"},
	{TOK_GOTO, "GOTO"},
	{TOK_MUL, "MUL"},
	{TOK_NOP, "NOP"},
	{TOK_READ, "READ"},
	{TOK_ROT, "ROT"},
	{TOK_SUB, "SUB"},
	{TOK_WORD, "WORD"},
	{TOK_WRITE, "WRITE"}
};

bool BDSCompiler::is_valid_token(char *s)
{
	for (int x = 0; x < TOK__LAST; x ++)
	{
		if (!stricmp(tokenlist[x].name,s))
		{
			return true;
		}
	}
	return false;
}

int BDSCompiler::get_commandtoken(char *s)
{
	int x;
	for (x = 0; x < TOK__LAST; x ++)
	{
		if (!stricmp(tokenlist[x].name,s))
		{
			return tokenlist[x].id;
		}
	}
	return -1;
}
