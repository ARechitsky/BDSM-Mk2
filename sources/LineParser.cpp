#include "LineParser.h"

#include <stdlib.h>
#include <string.h>

bool isAlpha(char c)
{
	if((c>='a' && c<='z') || (c>='A' && c<='Z'))
		return true;
	return false;
}

bool isNumDot(char c)
{
	if((c>='0' && c<='9') || c=='.' || c=='-')
		return true;
	return false;
}

LineParser::LineParser()
{
	m_command = 0;
	m_type = 'B';
	m_const = false;
}

LineParser::~LineParser()
{
}

int LineParser::parse(char *line)
{
	clean();
	while (*line == ' ' || *line == '\t') line++;

	int nc=0;
	char *p = line;
	if (isAlpha(*line)) //Text
    {
        m_const = false;
        while (isAlpha(*line))
        {
            line++;
            nc++;
        }
    }
    else if (isNumDot(*line)) //Number
    {
        m_const = true;
        while (isNumDot(*line))
        {
            line++;
            nc++;
        }
    }
    else //Can't understand, just ignore
    {
		m_command = 0;
		m_type = 'B';
		m_const = false;
		return 0;
    }

	m_command = new char[nc+1];
	for (int i = 0; i<nc; i++)
	{
		m_command[i] = p[i];
	}
	m_command[nc] = 0;

	if (*m_command == '\0') return -1;

	while (*line == ' ' || *line == '\t') line++;

	switch (*line)
	{
	case 'c':
	case 'C':
		m_type = 'C';
		break;
	case 'b':
	case 'B':
		m_type = 'B';
		break;
	case 'w':
	case 'W':
		m_type = 'W';
		break;
	case 'd':
	case 'D':
		m_type = 'D';
		break;
	case 'f':
	case 'F':
		m_type = 'F';
		break;
	}

	return 0;
}

void LineParser::clean()
{
	if(m_command)
	{
		free(m_command);
	}
	m_command = 0;
	m_const = false;
	m_type = 'B';
}

char *LineParser::getcommand()
{
	if (!m_command)
		return 0;
	return m_command;
}

bool LineParser::getconstflag()
{
	return m_const;
}

char LineParser::gettype()
{
	return m_type;
}
