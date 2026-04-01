
#ifndef _FLASH_H_
#define _FLASH_H_

#include <stdint.h>
#include "BK3432_reg.h"


enum 
{
	FLASH_SPACE_TYPE_MAIN = 0x00,
	FLASH_SPACE_TYPE_NVR = 0x01,
	FLASH_SPACE_TYPE_INVALID = 0XFF,
};


#define FLASH_TYPE_EMBEN       		 FLASH_SPACE_TYPE_NVR
#define FLASH_MAIN_SIZE              0x00028000
#define FLASH_NVR_SIZE               0x00000200
#define FLASH_WRITE_ECC_DATA0 0XA3
#define FLASH_WRITE_ECC_DATA1 0X56

#define FLASH_SECT_SIZE1         0x00000200  //  512 Byte
#define FLASH_SECT_SIZE2         0x00001000  //  4 KByte
#define FLASH_BOUNDARY256B       0x00000100  // 256 Byte


void flash_init(void);

void flash_erase_sector(uint8_t type,uint32_t address);

void flash_erase(uint8_t flash_type, uint32_t address, uint32_t size);

void flash_write(uint8_t flash_space, uint32_t address, uint32_t length, uint8_t *buffer);

void flash_read(uint8_t flash_space, uint32_t address, uint32_t length, uint8_t *buffer);



void Write_NVR_Flash(uint32_t address, uint8_t *buffer,uint32_t length);
void Read_NVR_Flash(uint32_t address, uint8_t *buffer,uint32_t length);

#endif





