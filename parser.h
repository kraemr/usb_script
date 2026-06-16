#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

typedef enum USB_COMMAND {
	SET_LANG,
	SWITCH_HID_MODE, // Switches Hid mode from keyboard to controller for example
	PRESS, // Press a button and instantly release it after
	HOLD, // Hold a button
	RELEASE,// Release a button that is held
	RESTART, // When put at the end, program gets run again after finished, good for commands that need to be sent repeatedly 
	UNSUPPORTED,
	DELAY,
}USB_COMMAND;

typedef struct KeyWordPair {
    const char * keyword;
    USB_COMMAND cmd;
}KeyWordPair;

typedef struct {
    unsigned char keys[6];
}KeysContext;

typedef enum PARSING_STATE {
	EXPECT_KEYWORD,
	EXPECT_DATA,
	EXPECT_COMMA_COLON,
	EXPECT_POSSIBLE_NEWLINE,
	KEYWORD_FOUND_ERR,
	DUCK_KEY_ERR,
	COMMA_COLON_MISSING,
	LINE_TOO_LONG,
	NO_REFERENCE_FOUND,
	DONE,
}PARSING_STATE;

typedef union USB_COMMAND_VALUE {
	unsigned char keys[8];
	unsigned int delay;
}USB_COMMAND_VALUE;

typedef enum USB_COMMAND_VALUE_TYPE {
	KEYBOARD,
	WAIT,
}USB_COMMAND_VALUE_TYPE;

typedef struct UsbCommand {
	USB_COMMAND command;
	USB_COMMAND_VALUE value;
	USB_COMMAND_VALUE_TYPE type;
}UsbCommand;

typedef struct __attribute__((packed)) {
    const char* key;
    unsigned char val;
} KeyPair;

extern const KeyPair DUCK_KEYS[170];
extern const KeyWordPair KEYWORDS[8];
extern PARSING_STATE parse_line(const char* input,unsigned short input_len,KeysContext* kctx,UsbCommand* cmd, size_t* index);
extern PARSING_STATE parse_all_alloc(const char* input, size_t input_len,KeysContext* ctx ,UsbCommand** cmd_list, size_t* cmd_list_len);
extern void set_key_index(KeysContext* ktx,unsigned char held, size_t index);
#endif