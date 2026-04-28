#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum USB_COMMAND {
	SET_LANG,
	SWITCH_HID_MODE, // Switches Hid mode from keyboard to controller for example
	PRESS, // Press a button and instantly release it after
	HOLD, // Hold a button
	RELEASE,// Release a button that is held
	RESTART, // When put at the end, program gets run again after finished, good for commands that need to be sent repeatedly 
	UNSUPPORTED,
}USB_COMMAND;

typedef struct KeyWordPair {
    const char * keyword;
    USB_COMMAND cmd;
}KeyWordPair;


/*
STATE Machine
	EXPECT_KEYWORD -> expects next token to be whitespace or a keyword like press PRESS hold HOLD etc
	EXPECT_DATA -> expects next Token to be any of the DUCK_KEYS
	EXPECT_COMMA_COLON -> expects next token to be SEMICOLON or COMMA
	
	KEYWORD_FOUND_ERR -> next Token was not found in DUCK_KEYS
	DUCK_KEY_ERR -> next token was not SEMICOLON or COMMA
	COMMA_COLON_MISSING -> expected Comma or Semicolon
*/

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

typedef struct UsbCommand {
	USB_COMMAND command;
	unsigned char value[8];
}UsbCommand;

typedef struct {
    const char* key;
    unsigned char val;
} KeyPair;
extern const KeyPair DUCK_KEYS[170];
extern const KeyWordPair KEYWORDS[6];
extern PARSING_STATE parse_line(const char* input,unsigned short input_len,UsbCommand* cmd, size_t* index);
extern PARSING_STATE parse_all_alloc(const char* input, size_t input_len ,UsbCommand** cmd_list, size_t* cmd_list_len);

#endif