#include"def.h"

uint16_t
lwip_htons(uint16_t n)
{
	  return (uint16_t)PP_HTONS(n);
}
