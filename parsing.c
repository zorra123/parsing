#include "parsing.h"

#define __SIZE_OF_SET_PARAM 	4   //3+1 3 as number and  1 for :
#define __SIZE_OF_SET 				12
#define __SIZE_OF_SET_HELP 		2
#define __SIZE_OF_MODE 				5
#define __SIZE_OF_MODE_HELP 	2
#define __SIZE_OF_OFF 				3
#define __SIZE_OF_HELP 				1

#define __OFFSET_FOR_COMMAND_SET 				__SIZE_OF_BUFFER + (i - __SIZE_OF_SET)
#define __OFFSET_FOR_COMMAND_SET_HELP		__SIZE_OF_BUFFER + (i - __SIZE_OF_SET_HELP)
#define __OFFSET_FOR_COMMAND_MODE 			__SIZE_OF_BUFFER + (i - __SIZE_OF_MODE)
#define __OFFSET_FOR_COMMAND_MODE_HELP 	__SIZE_OF_BUFFER + (i - __SIZE_OF_MODE_HELP)
#define __OFFSET_FOR_COMMAND_OFF 				__SIZE_OF_BUFFER + (i - __SIZE_OF_OFF)
#define __OFFSET_FOR_COMMAND_HELP 			__SIZE_OF_BUFFER + (i - __SIZE_OF_HELP)

#define __MAX_POWER 		600
#define __MAX_VOLTAGE 	600
#define __MAX_AMPERAGE 	8.0


volatile static	enum { 
	IDLE,
	COMMAND_GROUP_S,
	COMMAND_GROUP_M,
	COMMAND_SET,
	COMMAND_SET_HELP,
	COMMAND_MODE,
	COMMAND_MODE_HELP,
	COMMAND_OFF,
	COMMAND_HELP,
	ERROR_
} state = IDLE;

void clearBufferGlobal(uint8_t *buffer_global);
void copyBuffer(uint8_t *buffer_global, struct MyParsing *pars);
uint8_t parsing(uint8_t *buffer_global, struct MyParsing *pars, uint16_t *power, uint16_t *voltage, double *amperage);	
void chekbuf(struct MyParsing *pars);
void myParsingInit(struct MyParsing *pars)
{
	pars->mode = 0;
	for (uint8_t i = 0; i < __SIZE_OF_BUFFER; i++)
	{
		pars->buffer_tmp[i] = 0;
	}
	//pars->clearBufferGlobal = &clearBufferGlobal;
	//pars->copyBuffer = &copyBuffer;
	pars->parsing = &parsing;
	//pars->chekbuf = &chekbuf;
}

void clearBufferGlobal(uint8_t *buffer_global)
{
	
	//while(1)
	//{
		//to do it
	//}
}
void copyBuffer(uint8_t *buffer_global, struct MyParsing *pars)
{
	/*
	while (1)
	{
		//to do
		//block global buffer
	}*/
	for (uint8_t i = 0; i < __SIZE_OF_BUFFER; i++)
	{
		pars->buffer_tmp[i] = buffer_global[i];
	}
	//to do clear global buffer
	clearBufferGlobal(buffer_global); 
	
	/*
	while (1)
	{
		//to do
		//unlcok global buffer
	}*/
	
}
uint8_t symbolProcessing(uint8_t ch)
{
	uint8_t OUT_FlagCommandRecognized 	= 0;	//0- parsing is processing, 1-success, 2 - fail
	
	switch (state)
	{
	case IDLE:
		{
			switch (ch)
			{
			case 's':
				{
					state = COMMAND_GROUP_S;
					break;
				}
			case 'm':
				{
					state =  COMMAND_GROUP_M;
					break;
				}
			case 'o':
				{
					state =  COMMAND_OFF;
					break;
				}
			case '?':
				{
					state =	COMMAND_HELP;
					break;
				}				
			}
			break;
		}
	case COMMAND_GROUP_S: 
		{
			if (ch == '?')
				state = COMMAND_SET_HELP;
			else if ((ch >= '0' && ch <= '9') || ch == 'x')
				state = COMMAND_SET;
			else state = IDLE;								//it was trash
			break;
		}
	case COMMAND_SET:
		{
			if (!((ch >= '0' && ch <= ':') || ch == 'x' || ch == '\n' || ch == '.'))
			{
				state = ERROR_;
				OUT_FlagCommandRecognized = 2; 							//ERROR
			}
			else if (ch == '\n')
			{
				OUT_FlagCommandRecognized = 1;
			}
			break;
		}
	case COMMAND_SET_HELP:
		{
			if (!(ch == '?'))
			{
				state = ERROR_;
				OUT_FlagCommandRecognized = 2;  							//ERROR
			}
			else 
				OUT_FlagCommandRecognized = 1;
				
			break;
		}
	case COMMAND_GROUP_M: 
		{
			if (ch == '?')
				state = COMMAND_MODE_HELP;
			else if (ch == 'o')
			{					
				state = COMMAND_MODE;
			}
			else state = IDLE; 								//it was trash
			break;
		}
	case COMMAND_MODE:
		{
			if (!(ch == 'd' || ch == 'e' || ch == '1' || ch == '0' || ch == '\n'))
			{
				state = ERROR_;
				OUT_FlagCommandRecognized = 2;  							//ERROR
			}
			else if (ch == '\n')
			{
				OUT_FlagCommandRecognized = 1;
			}
			break;
		}
	case COMMAND_MODE_HELP:
		{
			if (!(ch == '?'))
			{
				state = ERROR_;
				OUT_FlagCommandRecognized = 2;   							//ERROR
			}
			else 
				OUT_FlagCommandRecognized = 1;
			break;
		}
	case COMMAND_OFF:
		{
			
		}
	}
	return OUT_FlagCommandRecognized;
	
}
void expandBuffer(struct MyParsing *pars, uint8_t start_command, uint8_t end_command, uint8_t lenght)
{
	uint8_t offset = __SIZE_OF_BUFFER - start_command;
	uint8_t num_tmp;
	uint8_t tmp[2];
	uint8_t *iterator = &tmp[0];
	for(uint8_t i = start_command; i <__SIZE_OF_BUFFER;i++)
	{
		num_tmp = __SIZE_OF_BUFFER - (i + offset);
		if (i + offset > __SIZE_OF_BUFFER)
			num_tmp = i + offset - __SIZE_OF_BUFFER;
		*iterator = pars->buffer_tmp[num_tmp];
		pars->buffer_tmp[num_tmp] = pars->buffer_tmp[i];
		while (num_tmp <= lenght)
		{
			num_tmp += offset;
			iterator++;
			*iterator = pars->buffer_tmp[num_tmp];
			iterator--;
			pars->buffer_tmp[num_tmp] = *iterator;
			*iterator = tmp[1];
		}
	}
}
void chekbuf(struct MyParsing *pars)
{
	for (uint8_t i = 0; i < __SIZE_OF_BUFFER; i++)
	{
		if (pars->buffer_tmp[i] == '\n')
		{
			if ((i - __SIZE_OF_SET) < 0 && pars->buffer_tmp[__OFFSET_FOR_COMMAND_SET] == 's') //if true expand buffer (command set)
			{
				//to do 
				expandBuffer(pars, __OFFSET_FOR_COMMAND_SET, i, __SIZE_OF_SET);
				break;
			}
			else if ((i - __SIZE_OF_SET_HELP) < 0 && pars->buffer_tmp[__OFFSET_FOR_COMMAND_SET_HELP] == 's')//if true expand buffer (command set help)
			{				
				//to do 
				expandBuffer(pars, __OFFSET_FOR_COMMAND_SET_HELP, i, __SIZE_OF_SET_HELP);
				break;
			}
			else if((i - __SIZE_OF_MODE) < 0 &&pars->buffer_tmp[__OFFSET_FOR_COMMAND_MODE] == 'm')//if true expand buffer (command mode)
			{
				//to do
				expandBuffer(pars, __OFFSET_FOR_COMMAND_MODE, i, __SIZE_OF_MODE);
				break;
			}
			else if((i - __SIZE_OF_MODE_HELP) < 0 && pars->buffer_tmp[__OFFSET_FOR_COMMAND_MODE_HELP] == 'm')//if true expand buffer (command mode help)
			{				
				//to do 
				expandBuffer(pars, __OFFSET_FOR_COMMAND_MODE_HELP, i, __SIZE_OF_MODE_HELP);
				break;
			}
			else if((i - __SIZE_OF_OFF) < 0 &&pars->buffer_tmp[__OFFSET_FOR_COMMAND_OFF] == 'o')//if true expand buffer (command off)
			{
				//to do 
				expandBuffer(pars, __OFFSET_FOR_COMMAND_OFF, i, __SIZE_OF_OFF);
				break;
			}
			else if((i - __SIZE_OF_HELP) < 0 && pars->buffer_tmp[__OFFSET_FOR_COMMAND_HELP] == '?')//if true expand buffer (command set help)
			{				
				//to do 
				expandBuffer(pars, __OFFSET_FOR_COMMAND_HELP, i, __SIZE_OF_HELP);
				break;
			}
		}
	}
}
uint8_t set(struct MyParsing *pars, uint16_t *power, uint16_t *voltage, double *amperage)
{
	uint16_t new_power = *power;
	uint16_t new_voltage = *voltage; 
	double new_amperage = *amperage;
	uint16_t tmp = 0;
	double tmp_amp = 0;
	uint8_t i = 0;
	uint8_t flag = 0; 				//flag for xxx
	
	
	while (pars->buffer_tmp[i++] != 's') ;
	for (uint8_t y = 0; y < 3; y++, i += 4)// 3 becouse 3 param in command set
	{	//check that 3 digits are numbers (integer for 1st and 2nd param)
		if ((y==0||y==1)&&(pars->buffer_tmp[i] >= 48 && pars->buffer_tmp[i] <= 57)&&(pars->buffer_tmp[i + 1] >= 48 && pars->buffer_tmp[i + 1] <= 57)&&(pars->buffer_tmp[i + 2] >= 48 && pars->buffer_tmp[i + 2] <= 57)&&pars->buffer_tmp[i + 3] == ':') 
		{		
			tmp = (100 * (pars->buffer_tmp[i] - 48)) + (10 * (pars->buffer_tmp[i + 1] - 48)) + (pars->buffer_tmp[i + 2] - 48);
		}
		//check that 3 digits are numbers (double for 3d param)
		else if((y == 2) && (pars->buffer_tmp[i] >= 48 && pars->buffer_tmp[i] <= 57) && (pars->buffer_tmp[i + 1] == '.') && (pars->buffer_tmp[i + 2] >= 48 && pars->buffer_tmp[i + 2] <= 57) && (pars->buffer_tmp[i + 3] == '\n'))
		{
			tmp_amp = (double)(pars->buffer_tmp[i] - 48) + (double)(0.1*(pars->buffer_tmp[i + 2] - 48));
		}
		else if(pars->buffer_tmp[i] == 'x'&&pars->buffer_tmp[i + 1] == 'x'&&pars->buffer_tmp[i + 2] == 'x')
		{
			flag = 1;
		}
		else
		{
			return 0;
		}
		switch (y)
		{
		case 0:
			{
				if (!flag)
				{
					if (tmp <= __MAX_POWER)
					{
						new_power = tmp;							
					}
					else
					{
						return 0;
					}
				}
				break;
			}
		case 1:
			{
				if (!flag)
				{
					if (tmp <= __MAX_VOLTAGE)
					{
						new_voltage = tmp;							
					}
					else
					{
						return 0;
					}
				}					
				break;
			}
		case 2:
			{
				if (!flag)
				{
					if (tmp_amp <= __MAX_AMPERAGE)
					{
						new_amperage = tmp_amp;						
					}
					else
					{
						return 0;
					}
				}	
				break;
			}
		}
		
	}
	*power = new_power;
	*voltage = new_voltage;
	*amperage = new_amperage;
	return 1;	
}
void mode(struct MyParsing *pars)
{
	uint8_t i = 0;
	while (pars->buffer_tmp[i++] != 'e') ;
	pars->mode = pars->buffer_tmp[i] - 48;
}
uint8_t parsing(uint8_t *buffer_global, struct MyParsing *pars, uint16_t *power, uint16_t *voltage, double *amperage)
{
	uint8_t command_recognized	= 0;
	uint8_t ch					= 0;
	copyBuffer(buffer_global, pars);
	chekbuf(pars);
	for (uint8_t i = 0; i < __SIZE_OF_BUFFER; i++)
	{
		ch = pars->buffer_tmp[i];
		command_recognized = symbolProcessing(ch);
		if (command_recognized == 1)
		{
			switch (state)
			{
			case COMMAND_SET: 
				{
					if (!set(pars, power, voltage, amperage))
						//error handling
						return 0;
					break;
				}		
			case COMMAND_SET_HELP: 
				{
					//to do
					break;
				}	
			case COMMAND_MODE:
				{
					mode(pars);
					break;
				}
			case COMMAND_MODE_HELP:
				{
					//to do
					break;
				}
				
			}
			state = IDLE;
			break;
		}
		else if (command_recognized == 2)
		{			
			state = IDLE;
			//error handling
			return 0;
		}
	}
	
	for (uint8_t i = 0; i < __SIZE_OF_BUFFER; i++)
	{
		pars->buffer_tmp[i] = 0;
	}
	return 1;
}	
