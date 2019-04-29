#pragma once
#include "stm32f1xx_hal.h"
#define __SIZE_OF_BUFFER 16

	
struct MyParsing
{
	uint8_t buffer_tmp[__SIZE_OF_BUFFER];									//local buffer
	uint8_t mode;														//mode 0/1 (0 - for default)
																		//0 - constant broadcast; 1 - broadcast on request
	//void(*clearBufferGlobal)(uint8_t *buffer_global);
	//void(*copyBuffer)(uint8_t *buffer_global, struct MyParsing *pars);	//copy global buffer to local
	uint8_t(*parsing)(uint8_t *buffer_global, struct MyParsing *pars, uint16_t *power, uint16_t *voltage, double *amperage);       	//parsing local buffer
	//void(*chekbuf)(struct MyParsing *pars);
	
	
	 /*
	 !!!!!!!!!!!!!!!!!!!!!!!!!
	 !to do setter and getter!
	 !!!!!!!!!!!!!!!!!!!!!!!!!
	 */
};

void myParsingInit(struct MyParsing *pars);								//config my struct
