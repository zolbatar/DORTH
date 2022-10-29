#include <string>
#include "../Console/Console.h"

extern bool console_active;
extern Console console;

extern "C" void console_print(const char* buffer)
{
	if (console_active)
	{
		console.WriteString(buffer);
	}
	else
	{
		printf("%s", buffer);
	}
}