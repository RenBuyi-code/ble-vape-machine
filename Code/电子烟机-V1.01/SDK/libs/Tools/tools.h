#ifndef _TOOLS_h_
#define _TOOLS_h_

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

unsigned char compareValue(unsigned int a, unsigned int b, unsigned int n);
//char* strstr(char* src, char* sub);
void byte_cpy(unsigned char *dst,unsigned char *src,unsigned char num);
//void *memset(void *str, int c, unsigned int count);
unsigned char* Uint4BToStr( int value,unsigned char* str);
int HexToAsc(unsigned char aChar);
unsigned int String4ByteToInt(unsigned char buffer[],char offset);
uint8_t pack_string_to_buffer(char* str,char* buffer);
void StrToHex(unsigned char *pbDest, unsigned char *pbSrc, int nLen);

#endif

