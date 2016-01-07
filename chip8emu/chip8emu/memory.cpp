
#include "memory.h"


Memory::Memory()
{
	mem = new unsigned char[0xfff];
}

Memory::~Memory()
{
	delete [] mem;
}