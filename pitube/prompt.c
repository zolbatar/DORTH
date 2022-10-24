#include <stdio.h>
#include "../prompt.h"
#include "../beebScreen/beebScreen.h"

void prompt()
{
	printf("Welcome to Dorth! @thezolbatar\n");
	uint8_t buffer[512];
	uint32_t size;
	_swi(OS_ReadLine, _INR(0, 4) | _OUTR(1, 1), &buffer, 512 - 1, 0x0, 0xff, 0, &size);
	buffer[size] = 0;
	printf("%s\n", buffer);
	printf("Entered characters are %s ", buffer);
}