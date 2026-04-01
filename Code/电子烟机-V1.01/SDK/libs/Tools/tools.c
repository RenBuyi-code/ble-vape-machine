#include "tools.h"


//char* strstr(char* src, char* sub)
//{
//  char *bp;
//  char *sp;
//  if (!src || !sub)
//  {
//    return src;
//  }
//  /* 遍历src字符串  */
//  while (*src)
//  {
//    /* 用来遍历子串 */
//    bp = src;
//    sp = sub;
//    do
//    {
//      if (!*sp)  /*到了sub的结束位置，返回src位置   */
//        return src;
//    } while (*bp++ == *sp++);
//    src++;
//  }
//  return 0;
//}


void byte_cpy(unsigned char *dst,unsigned char *src,unsigned char num) //简化了很多使用的时候注意
{
  uint8_t i =0;
  for ( ; i < num; i++)
  {
    dst[i] = src[i];
  }
  
}

//void *memset(void *str, int c, unsigned int count)     
//{     
//  
//  void *s = str;     
//  while (count --)     
//  {     
//    *(char *) s = (char) c;     
//    s = (char *) s + 1;     
//  }     
//  return str;     
//}  

/**
* a在b的+/-n 范围内,不扩展
*/
uint8_t compareValue(unsigned int a, unsigned int b, unsigned int n)
{
  if (b > 0xFFFF - n && (a <= 0xFFFF || a <= b + n) && a >= b - n)
  {
    return 1;
  }
  else if (a <= b + n && a >= b - n)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

uint8_t* Uint4BToStr(int value,unsigned char* str)
{
 
   memset(str,0,5);
   str[3] = value%10+48;
   str[2] = value/10%10+48;
   str[1] = value/100%10+48;
   str[0] = value/1000%10+48;
    
   
  return str;
}


void StrToHex(unsigned char *pbDest, unsigned char *pbSrc, int nLen)//字符串转HEX数组
{
	char h1,h2;
	unsigned char s1,s2;
	int i;

		for (i=0; i<nLen; i++)
		{
			h1 = pbSrc[2*i];
			h2 = pbSrc[2*i+1];

			s1 = toupper(h1) - 0x30; //十六进制 0x30   ,    dec十进制 48	,   图形 0
			if (s1 > 9)
				s1 -= 7;

			s2 = toupper(h2) - 0x30;
			if (s2 > 9)
				s2 -= 7;

			pbDest[i] = s1*16 + s2;
		}
}


//int strlen(const char* src)
//{
//  
//  int len = 0;
//  
//  
//  
//  while(*src++ != '\0')
//    
//    len ++;
//  
//  return len;
//  
//}


int HexToAsc(unsigned char aChar){
  char ret = aChar;
  
  if((aChar>=0x30)&&(aChar<=0x39)) //0 - 9
    ret = ret-0x30;
  else if((aChar>=0x41)&&(aChar<=0x46))//大写字母
    ret = ret-0x37;
  else if((aChar>=0x61)&&(aChar<=0x66))//小写字母
    ret = ret-0x57;
  else ret = 0xff;
  
  return ret; 
} 



unsigned int  String4ByteToInt(unsigned char buffer[],char offset)
{
          unsigned int v = 0;
         
          //v += (buffer[offset]-48)*1000;
          v += (buffer[offset+1]-48)*100;
          v += (buffer[offset+2]-48)*10;
          v += (buffer[offset+3]-48);
          return v;
}

uint8_t pack_string_to_buffer(char* str,char* buffer)
{
	int len = 0;
	len = strlen(str);
	
	if(len < 20)
 {
		sprintf(buffer,"%s\r\n",str);
	  str[strlen(str)+1] = '\0';
	  
	  return 1;
 }
 return 0;

}



