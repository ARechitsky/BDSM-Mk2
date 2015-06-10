#pragma once

enum
{
	//Stack control
	TOK_ROT,
	TOK_DUP,
	TOK_DROP,

	//Input/output
	TOK_CONST,
	TOK_READ,
	TOK_WRITE,

    //Types
	TOK_BYTE,
	TOK_WORD,
	TOK_DWORD,
	TOK_FLOAT,

	//Arithmetic
	TOK_ADD,
	TOK_SUB,
	TOK_MUL,
	TOK_DIV,

	//Management
	TOK_GOTO,
	TOK_NOP,

	TOK__LAST
};
