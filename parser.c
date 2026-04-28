#include "parser.h"
#include <string.h>
#include <stdint.h>
#define DUCK_KEYS_COUNT sizeof(DUCK_KEYS) / sizeof(KeyPair)
#define KEYS_START 2 // index 2 is start of the keys, 0 is REPORT_ID (1 bein kb)
#define KEY_MOD_INDEX 1
#define PREALLOC_AMOUNT 64
#define TESTCASES_H
#include "testcases.h"

const KeyWordPair KEYWORDS[] = {
    {"press",PRESS},
    {"PRESS",PRESS},
    {"hold",HOLD},
    {"release",RELEASE},	
};
#define KEY_WORD_COUNT sizeof(KEYWORDS) / sizeof(KeyWordPair)

const KeyPair DUCK_KEYS[] = {
    // Numbers
    {"0", 0x27}, {"1", 0x1E}, {"2", 0x1F}, {"3", 0x20}, {"4", 0x21},
    {"5", 0x22}, {"6", 0x23}, {"7", 0x24}, {"8", 0x25}, {"9", 0x26},

    // Letters
    {"A", 0x04}, {"B", 0x05}, {"C", 0x06}, {"D", 0x07}, {"E", 0x08}, 
    {"F", 0x09}, {"G", 0x0A}, {"H", 0x0B}, {"I", 0x0C}, {"J", 0x0D}, 
    {"K", 0x0E}, {"L", 0x0F}, {"M", 0x10}, {"N", 0x11}, {"O", 0x12}, 
    {"P", 0x13}, {"Q", 0x14}, {"R", 0x15}, {"S", 0x16}, {"T", 0x17}, 
    {"U", 0x18}, {"V", 0x19}, {"W", 0x1A}, {"X", 0x1B}, {"Y", 0x1C}, {"Z", 0x1D},

    // Named Keys & Commands
    {"AGAIN",             0x79},
    {"ALT_LEFT",          0xE2},
    {"ALT_RIGHT",         0xE6},
    {"ALTERNATE_ERASE",   0x99},
    {"APOSTROPHE",        0x34},
    {"APPLICATION",       0x65},
    {"ARROW_DOWN",        0x51},
    {"ARROW_LEFT",        0x50},
    {"ARROW_RIGHT",       0x4F},
    {"ARROW_UP",          0x52},
    {"BACKSLASH",         0x31},
    {"BACKSPACE",         0x2A},
    {"BRACKET_LEFT",      0x2F},
    {"BRACKET_RIGHT",     0x30},
    {"CANCEL",            0x9B},
    {"CAPS_LOCK",         0x39},
    {"CLEAR",             0x9C},
    {"CLEAR_AGAIN",       0xA2},
    {"COMMA",             0x36},
    {"CONTROL_LEFT",      0xE0},
    {"CONTROL_RIGHT",     0xE4},
    {"COPY",              0x7C},
    {"CRSEL_PROPS",       0xA3},
    {"CUT",               0x7B},
    {"DELETE",            0x4C},
    {"END",               0x4D},
    {"ENTER",             0x28},
    {"EQUAL",             0x2E},
    {"ESCAPE",            0x29},
    {"EUROPE_1",          0x32},
    {"EUROPE_2",          0x64},
    {"EXECUTE",           0x74},
    {"EXSEL",             0xA4},

    // Function Keys
    {"F1", 0x3A}, {"F2", 0x3B}, {"F3", 0x3C}, {"F4", 0x3D}, {"F5", 0x3E}, 
    {"F6", 0x3F}, {"F7", 0x40}, {"F8", 0x41}, {"F9", 0x42}, {"F10", 0x43}, 
    {"F11", 0x44}, {"F12", 0x45}, {"F13", 0x68}, {"F14", 0x69}, {"F15", 0x6A}, 
    {"F16", 0x6B}, {"F17", 0x6C}, {"F18", 0x6D}, {"F19", 0x6E}, {"F20", 0x6F}, 
    {"F21", 0x70}, {"F22", 0x71}, {"F23", 0x72}, {"F24", 0x73},

    // Special & Navigation
    {"FIND",              0x7E},
    {"GRAVE",             0x35},
    {"GUI_LEFT",          0xE3},
    {"GUI_RIGHT",         0xE7},
    {"HELP",              0x75},
    {"HOME",              0x4A},
    {"INSERT",            0x49},
    {"KANJI1",            0x87},
    {"KANJI2",            0x88},
    {"KANJI3",            0x89},
    {"KANJI4",            0x8A},
    {"KANJI5",            0x8B},
    {"KANJI6",            0x8C},
    {"KANJI7",            0x8D},
    {"KANJI8",            0x8E},
    {"KANJI9",            0x8F},

    // Keypad
    {"KEYPAD_0",          0x62},
    {"KEYPAD_1",          0x59},
    {"KEYPAD_2",          0x5A},
    {"KEYPAD_3",          0x5B},
    {"KEYPAD_4",          0x5C},
    {"KEYPAD_5",          0x5D},
    {"KEYPAD_6",          0x5E},
    {"KEYPAD_7",          0x5F},
    {"KEYPAD_8",          0x60},
    {"KEYPAD_9",          0x61},
    {"KEYPAD_ADD",        0x57},
    {"KEYPAD_COMMA",      0x85},
    {"KEYPAD_DECIMAL",    0x63},
    {"KEYPAD_DIVIDE",     0x54},
    {"KEYPAD_ENTER",      0x58},
    {"KEYPAD_EQUAL",      0x67},
    {"KEYPAD_EQUAL_SIGN", 0x86},
    {"KEYPAD_MULTIPLY",   0x55},
    {"KEYPAD_SUBTRACT",   0x56},

    // System & International
    {"LANG1",             0x90}, {"LANG2", 0x91}, {"LANG3", 0x92},
    {"LANG4",             0x93}, {"LANG5", 0x94}, {"LANG6", 0x95},
    {"LANG7",             0x96}, {"LANG8", 0x97}, {"LANG9", 0x98},
    {"LOCKING_CAPS_LOCK", 0x82},
    {"LOCKING_NUM_LOCK",  0x83},
    {"LOCKING_SCROLL_LOCK",0x84},
    {"MENU",              0x76},
    {"MINUS",             0x2D},
    {"MUTE",              0x7F},
    {"NONE",              0x00},
    {"NUM_LOCK",          0x53},
    {"OPER",              0xA1},
    {"OUT",               0xA0},
    {"PAGE_DOWN",         0x4E},
    {"PAGE_UP",           0x4B},
    {"PASTE",             0x7D},
    {"PAUSE",             0x48},
    {"PERIOD",            0x37},
    {"POWER",             0x66},
    {"PRINT_SCREEN",      0x46},
    {"PRIOR",             0x9D},
    {"RETURN",            0x9E},
    {"SCROLL_LOCK",       0x47},
    {"SELECT",            0x77},
    {"SEMICOLON",         0x33},
    {"SEPARATOR",         0x9F},
    {"SHIFT_LEFT",        0xE1},
    {"SHIFT_RIGHT",       0xE5},
    {"SLASH",             0x38},
    {"SPACE",             0x2C},
    {"STOP",              0x78},
    {"SYSREQ_ATTENTION",  0x9A},
    {"TAB",               0x2B},
    {"UNDO",              0x7A},
    {"VOLUME_DOWN",       0x81},
    {"VOLUME_UP",         0x80}
};

unsigned char get_duck_key(char * key_name, size_t len) {
	for(int i = 0; i < DUCK_KEYS_COUNT; i++ ){
		if (strlen(DUCK_KEYS[i].key) != len) {
			continue;
		}
		if(!strncmp(key_name,DUCK_KEYS[i].key,len)){
			return DUCK_KEYS[i].val;
		}
	}
	return 0xFF;
}


void fill_usb_command(volatile UsbCommand* cmd) {
	// 0:1 means ReportIDKeyboard
	cmd->value[0] = 1;
	// 1 Contains OR ed keymodifiers
	cmd->value[1] = 0;
	// 6 keypresses possible to send at once
	cmd->value[2] = 0;
	cmd->value[3] = 0;
	cmd->value[4] = 0;
	cmd->value[5] = 0;
	cmd->value[6] = 0;
	cmd->value[7] = 0;
}
	

/*
STATE Machine
    EXPECT_KEYWORD -> expects next token to be whitespace or a keyword like press PRESS hold HOLD etc
    EXPECT_DATA -> expects next Token to be any of the DUCK_KEYS
    EXPECT_COMMA_COLON -> expects next token to be SEMICOLON or COMMA
    
    KEYWORD_FOUND_ERR -> next Token was not found in DUCK_KEYS
    DUCK_KEY_ERR -> next token was not SEMICOLON or COMMA
    COMMA_COLON_MISSING -> expected Comma or Semicolon
*/
PARSING_STATE parse_line(const char* input,unsigned short input_len,UsbCommand* cmd, size_t* index) {
	PARSING_STATE state = EXPECT_KEYWORD;
    size_t keys_index = KEYS_START;
    size_t done_at = 0;
	if (cmd == NULL) {
		return NO_REFERENCE_FOUND;
	}    
	fill_usb_command(cmd);
	while((*index) < input_len) {
        if(input[(*index)] == ' ') {
            (*index)++;
            continue;
        }
        
        #ifdef DEBUG
        printf("input[i]: %c\n",input[(*index)]);
        #endif

        if (state == EXPECT_KEYWORD) {
            for (size_t j = 0; j < KEY_WORD_COUNT; j++) {
                size_t len = strlen(KEYWORDS[j].keyword);
                int res = memcmp(&input[(*index)],KEYWORDS[j].keyword,strlen(KEYWORDS[j].keyword));
                if(res == 0) {
                    // Found Keyword 
                    cmd->command = KEYWORDS[j].cmd;
                    state = EXPECT_DATA;
                    #ifdef DEBUG
                    printf("FOUND KEYWORD %d\n",cmd->command);
                    #endif
                    (*index) += len;
                    break;
                }
            }

            if(state != EXPECT_DATA) {
                state = KEYWORD_FOUND_ERR;
                return state;
            }else{
                continue;
            }        
        }
        else if(state == EXPECT_DATA){
            // Could be heavily simplified with a hash map
            for(size_t j = 0; j < DUCK_KEYS_COUNT; j++) {
                size_t len = strlen(DUCK_KEYS[j].key);
                if( ((*index)+len) > input_len ) {
                    continue;
                }
                int res = memcmp(&input[(*index)],DUCK_KEYS[j].key,len);
                if(res == 0) {
                    cmd->value[keys_index] = DUCK_KEYS[j].val;                    
                    #ifdef DEBUG
                    printf("Found Value: %u\n", DUCK_KEYS[j].val);                
                    #endif
                    state = EXPECT_COMMA_COLON;
                    keys_index++;
                    (*index) += len;
                    break;
                }
            }

            if(state != EXPECT_COMMA_COLON) {
                state = DUCK_KEY_ERR;
                return state;
            }else{
                continue;
            }      
        }
        else if(state == EXPECT_COMMA_COLON){
            if(input[(*index)] == ',') {
                #ifdef DEBUG
                printf("FOUND Comma\n");
                #endif
                state = EXPECT_DATA;
                (*index)++;
            }else if(input[(*index)] == ';'){
                state = DONE;
                (*index)++;             
                done_at = (*index);
            }else{
                #ifdef DEBUG
                printf("COMMA IS MISSING\n");
                #endif
                state = COMMA_COLON_MISSING;
                return state;
            }                    
        }
        else{
            uint8_t return_found = input[(*index)] == '\r';
            // Check for CRLF
            if((*index+2) < input_len && return_found && input[(*index)+1] == '\n' ) {
                (*index)+=2;
                return state;
            }
            // Check for LF or CR
            else if( (input[(*index)] == '\n') ||  return_found) {
                (*index)++;
                return state;
            }else{
                (*index) = done_at;
                return state;
            }
            
        }
	}
    return state;	
}

void print_usb_command(volatile UsbCommand *cmd) {
    if (cmd == NULL) {
        printf("UsbCommand is NULL\n");
        return;
    }

    printf("UsbCommand Table:\n");
    printf("-------------------------------\n");
    printf("Command: %d\n", cmd->command);
    printf("Values (Hex): ");
    
    for (int i = 0; i < 8; i++) {
        // %02X prints hex with leading zeros (e.g., 0A instead of A)
        printf("%02X ", cmd->value[i]);
    }
    
    printf("\n-------------------------------\n");
}

PARSING_STATE parse_all_alloc(const char* input, size_t input_len ,UsbCommand** cmd_list, size_t* cmd_list_len) {    
    (*cmd_list) = malloc(PREALLOC_AMOUNT * sizeof(UsbCommand) );
    size_t index = 0;
    (*cmd_list_len) = 0;
    UsbCommand* cmd_list_ptr = (*cmd_list);
    PARSING_STATE state = DONE;

    while ( index < input_len ) {
        #ifdef DEBUG
            printf("STARTED PARSING LINE \n");
        #endif
        state = parse_line(input,input_len,&cmd_list_ptr[(*cmd_list_len)],&index);
        #ifdef DEBUG
            printf("ENDED PARSING LINE \n");
        #endif
        if(state == DONE) {
            (*cmd_list_len)++;                            
        }else {                    
            break;
        }
    }
    return state;
}

