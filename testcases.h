#include "parser.h"
typedef struct TestCase {
	const char* name;
	const char* str;
	UsbCommand expected_commands[8];
	size_t commands_count;
	PARSING_STATE expected_state;
}TestCase;