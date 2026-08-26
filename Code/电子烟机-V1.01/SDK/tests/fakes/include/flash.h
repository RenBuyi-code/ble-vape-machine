#ifndef _FLASH_H_
#define _FLASH_H_

#include <stdint.h>

void Write_NVR_Flash(uint32_t address, uint8_t *buffer, uint32_t length);
void Read_NVR_Flash(uint32_t address, uint8_t *buffer, uint32_t length);

#endif
