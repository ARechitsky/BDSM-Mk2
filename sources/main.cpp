#include "BDSCompiler.h"
#include "VM.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void error(int err)
{
	switch(err)
	{
	case -2:
		printf("Usage: bdsvm -(p|b) file\n");
		printf("\t--plain or -p for read plain text\n");
		printf("\t--byte or -b for read byte-code\n");
		break;
	case -1:
		printf("File error\n");
		break;
	case 50:
		printf("Syntax error\n");
		break;
	default:
		printf("Unknown error\n");
	}
	system("pause");
	exit(0);
}

int main(int argc, char **argv)
{
//        BDSCompiler a;
//        VM b;
//        int res = a.Open("3.txt");
//        if(res) error(res);
//        a.Save("3.hex");
//        res = b.execute(a.getLineCount(), a.getByteCode());
//        if(res) error(res);
	if (argc != 3) error(-2);
	if (!stricmp(argv[1],"-p") || !stricmp(argv[1],"--plain"))
	{
		BDSCompiler a;
		VM b;
		int res = a.Open(argv[2]);
		if(res) error(res);
		res = b.execute(a.getLineCount(), a.getByteCode(), a.getCodeSize());
		if(res) error(res);
	}
	else if (!stricmp(argv[1],"-b") || !stricmp(argv[1],"--byte"))
	{
		VM b;
		int res = b.executeFile(argv[2]);
		if(res) error(res);
	}
	else
	{
		error(-2);
	}
	printf("\n");
	system("pause");
	return 0;
}
