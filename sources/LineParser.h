#pragma once

class LineParser
{
public:
	LineParser();
	~LineParser();

	int parse(char *line);
	char *getcommand();
	char gettype();
	bool getconstflag();

private:
	void clean();

	char *m_command;
	char m_type;
	bool m_const;
};

