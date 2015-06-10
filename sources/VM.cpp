#include "VM.h"
#include "byte_code.h"
#include "tokens.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

VM::VM()
{
	action coms[16] = {&VM::com_rot,
	&VM::com_dup,
	&VM::com_drop,
	&VM::com_const,
	&VM::com_read,
	&VM::com_write,
	&VM::com_byte,
	&VM::com_word,
	&VM::com_dword,
	&VM::com_float,
	&VM::com_add,
	&VM::com_sub,
	&VM::com_mul,
	&VM::com_div,
	&VM::com_goto,
	&VM::com_nop};
	commands = new action [16];
	for (int i=0; i<16; i++)
	{
		commands[i] = coms[i];
	}
}


VM::~VM()
{
}

int VM::execute_one()
{
    char line = m_code[m_lines[m_ip]];
	int command_code = (line & COMMAND_MASK) >> 4;
	if (command_code > MAX_COMMAND)
	{
		return 50;
	}
	action command = commands[command_code];

	if (command_code == TOK_CONST)
    {
        memcpy(m_constval, &m_code[m_lines[m_ip]+1], 4);
    }

 	m_type = line & TYPE_MASK;
	switch (m_type)
	{
	case TYPE_DWORD:
    case TYPE_FLOAT:
		m_typemask = 0xFFFFFFFF;
		m_typesize = 4;
		break;
	case TYPE_WORD:
		m_typemask = 0x0000FFFF;
		m_typesize = 2;
		break;
	default:
		m_typemask = 0x000000FF;
		m_typesize = 1;
	}

	(this->*command)();
	return 0;
}

int VM::execute(ushort linecount, const char *byte_code, ushort codesize)
{
	m_sp = 0;
	m_ip = 0;
	memset(m_constval, 0, 4);
	memset(m_memory, 0, 65536);
	m_code = byte_code;
	m_code_size = codesize;
	m_linecount = linecount;
	m_lines = new ushort [m_linecount];
	for (int i = 0,ip = 0; i < m_code_size; ++i, ++ip)
    {
        m_lines[ip] = i;
        int command_code = (m_code[i] & COMMAND_MASK) >> 4;
        if (command_code == TOK_CONST)
        {
            i+=4;
        }
    }
	while (m_ip < m_linecount)
	{
		int res = execute_one();
		if (res) return res;
	}
	return 0;
}

int VM::executeFile(const char *filename)
{
	FILE *fp = fopen(filename, "rb");
	if (!fp) return -1;
	ushort linecount;
	fread(&linecount, sizeof(linecount), 1, fp);
	ushort size = 0;
	int tmp;
	while ((tmp = fgetc(fp))!=EOF) size++;
	fflush(fp);
	char *code = new char [size];
	fread(&linecount, sizeof(linecount), 1, fp);
	fread(code, sizeof(char), size, fp);
	return execute(linecount, code, size);
}

void *VM::pop(int size)
{
    m_sp -= size;
    return &m_memory[m_sp];
}

void VM::push(void *data, int size)
{
    memcpy(&m_memory[m_sp], data, size);
    m_sp += size;
}

void VM::com_rot()
{
    ushort depth;
    memcpy(&depth, pop(2), 2);
    char *buf1 = new char [m_typesize];
    char *buf2 = new char [m_typesize*(depth-1)];
    memcpy(buf1, &m_memory[m_sp-m_typesize*depth], m_typesize);
    memcpy(buf2, &m_memory[m_sp-m_typesize*(depth-1)], m_typesize*(depth-1));
    memcpy(&m_memory[m_sp-m_typesize*depth], buf2, m_typesize*(depth-1));
    memcpy(&m_memory[m_sp-m_typesize], buf1, m_typesize);
	m_ip++;
}

void VM::com_dup()
{
    memcpy(&m_memory[m_sp], &m_memory[m_sp-m_typesize], m_typesize);
    m_sp += m_typesize;
	m_ip++;
}

void VM::com_drop()
{
    m_sp -= m_typesize;
	m_ip++;
}

void VM::com_const()
{
    push(m_constval, m_typesize);
	m_ip++;
}

void VM::com_read()
{
	if (m_type == TYPE_CHAR)
	{
	    char c = getchar();
	    push(&c, 1);
	}
	else if (m_type == TYPE_FLOAT)
	{
		char str1[30];
		gets(str1);
		float f = atof(str1);
		push(&f, 4);
	}
	else
	{
		char str[15];
		gets(str);
		int i = atoi(str);
		push(&i, m_typesize);
	}
	m_ip++;
}

void VM::com_write()
{
	if (m_type == TYPE_CHAR)
	{
	    char c = *((char *) pop(1));
	    putchar(c);
	}
	else if (m_type == TYPE_FLOAT)
	{
		float f = *((float *) pop(4));
		printf("%f ", f);
	}
	else
	{
		int i = (*((int *) pop(m_typesize))) & m_typemask;
		printf("%d ", i);
	}
	m_ip++;
}

void VM::com_byte()
{
    int val;
	if (m_type == TYPE_FLOAT)
	{
		float f = *((float *) pop(4));
		val = int(f);
	}
	else
	{
		val = *((int *) pop(m_typesize));
	}
	val &= 0xFF;
	push(&val, 1);
	m_ip++;
}

void VM::com_word()
{
    int val;
	if (m_type == TYPE_FLOAT)
	{
		float f = *((float *) pop(4));
		val = int(f);
	}
	else
	{
		val = *((int *) pop(m_typesize));
	}
	val &= 0xFFFF;
	push(&val, 2);
	m_ip++;
}

void VM::com_dword()
{
    int val;
	if (m_type == TYPE_FLOAT)
	{
		float f = *((float *) pop(4));
		val = int(f);
	}
	else
	{
		val = *((int *) pop(m_typesize));
	}
	val &= 0xFFFFFF;
	push(&val, 4);
	m_ip++;
}

void VM::com_float()
{
	if (m_type != TYPE_FLOAT)
	{
        float val = float((*((int *) pop(m_typesize))) & m_typemask);
        push(&val, 4);
	}
	m_ip++;
}

void VM::com_add()
{
	if (m_type == TYPE_FLOAT)
	{
		float f1 = *((float *) pop(4));
		float f2 = *((float *) pop(4));
		float f3 = f1 + f2;
		push(&f3, 4);
	}
	else
	{
	    int i1 = (*((int *) pop(m_typesize))) & m_typemask;
	    int i2 = (*((int *) pop(m_typesize))) & m_typemask;
	    int i3 = (i1 + i2) & m_typemask;
		push(&i3, m_typesize);
	}
	m_ip++;
}

void VM::com_sub()
{
	if (m_type == TYPE_FLOAT)
	{
		float f1 = *((float *) pop(4));
		float f2 = *((float *) pop(4));
		float f3 = f1 - f2;
		push(&f3, 4);
	}
	else
	{
	    int i1 = (*((int *) pop(m_typesize))) & m_typemask;
	    int i2 = (*((int *) pop(m_typesize))) & m_typemask;
	    int i3 = (i1 - i2) & m_typemask;
		push(&i3, m_typesize);
	}
	m_ip++;
}

void VM::com_mul()
{
	if (m_type == TYPE_FLOAT)
	{
		float f1 = *((float *) pop(4));
		float f2 = *((float *) pop(4));
		float f3 = f1 * f2;
		push(&f3, 4);
	}
	else
	{
	    int i1 = (*((int *) pop(m_typesize))) & m_typemask;
	    int i2 = (*((int *) pop(m_typesize))) & m_typemask;
	    int i3 = (i1 * i2) & m_typemask;
		push(&i3, m_typesize);
	}
	m_ip++;
}

void VM::com_div()
{
	if (m_type == TYPE_FLOAT)
	{
		float f1 = *((float *) pop(4));
		float f2 = *((float *) pop(4));
		float f3 = f1 / f2;
		push(&f3, 4);
	}
	else
	{
	    int i1 = (*((int *) pop(m_typesize))) & m_typemask;
	    int i2 = (*((int *) pop(m_typesize))) & m_typemask;
	    int i3 = (i1 / i2) & m_typemask;
		push(&i3, m_typesize);
	}
	m_ip++;
}

void VM::com_goto()
{
    ushort next = *((ushort *) pop(2));
    bool go;
	if (m_type == TYPE_FLOAT)
	{
		float f = *((float *) pop(4));
		go = (f != 0);
	}
	else
	{
	    int i = (*((int *) pop(m_typesize))) & m_typemask;
		go = (i != 0);
	}
	if (go)
    {
        m_ip = next;
    }
    else
    {
        m_ip++;
    }
}

void VM::com_nop()
{
    m_ip++;
}
