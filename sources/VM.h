#pragma once

typedef unsigned char uchar;
typedef unsigned short int ushort;
typedef unsigned int uint;
class VM;
typedef void(VM::*action)(void);

class VM
{
public:
	VM();
	~VM();

	int executeFile(const char *filename);
	int execute(ushort linecount, const char *byte_code, ushort codesize);
private:
	void init();
	int execute_one();

	void com_rot();
	void com_dup();
	void com_drop();
	void com_const();
	void com_read();
	void com_write();
	void com_byte();
	void com_word();
	void com_dword();
	void com_float();
	void com_add();
	void com_sub();
	void com_mul();
	void com_div();
	void com_goto();
	void com_nop();

	void *pop(int size);
	void push(void *data, int size);

	const char *m_code;
	ushort m_code_size;
	ushort *m_lines;
	ushort m_linecount;
	char m_memory[65536];
	ushort m_sp;
	ushort m_ip;
	uint m_typemask;
	uint m_typesize;
	char m_type;
	char m_constval[4];
	action *commands;
};

