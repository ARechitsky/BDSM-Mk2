#include "growbuf.h"

#include <cstdlib> // for malloc/free
#include <cstring> // for memcpy
#include <cstdio> // for f*
#include <algorithm> // for std::min

using namespace std;

GrowBuf::GrowBuf() { m_alloc=m_used=m_zero=0; m_s=NULL; m_bs=32768; }
GrowBuf::~GrowBuf() { free(m_s); }

void GrowBuf::set_zeroing(int zero) { m_zero=zero; }

int GrowBuf::add(const void *data, int len)
{
	if (len<=0) return 0;
	resize(m_used+len);
	memcpy((char*)m_s+m_used-len,data,len);
	return m_used-len;
}

void GrowBuf::resize(int newlen)
{
	int os=m_alloc;
	int ou=m_used;
	m_used=newlen;
	if (newlen > m_alloc)
	{
		void *n;
		m_alloc = newlen*2 + m_bs;
		n = realloc(m_s, m_alloc);
		if (!n)
		{
			printf("\nack! realloc(%d) failed, trying malloc(%d)!\n",m_alloc,newlen);
			m_alloc=newlen; // try to malloc the minimum needed
			n=malloc(m_alloc);
			if (!n)
			{
				printf("\nInternal compiler error #12345: GrowBuf realloc/malloc(%d) failed.\n",m_alloc);
				exit(0);
			}
			memcpy(n,m_s,min(newlen,os));
			free(m_s);
		}
		m_s=n;
	}
	if (m_zero && m_used > ou)
		memset((char*)m_s + ou, 0, m_used - ou);
	if (!m_used && m_alloc > 2*m_bs) // only free if you resize to 0 and we're > 64k
	{
		m_alloc=0;
		free(m_s);
		m_s=NULL;
	}
}

int GrowBuf::getlen() const { return m_used; }
void *GrowBuf::get() const { return m_s; }
