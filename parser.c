#include "parser.h"
#include <stddef.h>
#include <string.h>
#include <stdint.h>


#define DUCK_KEYS_COUNT sizeof(DUCK_KEYS) / sizeof(KeyPair)
// index 2 is start of the keys, 0 is REPORT_ID (1 being report id for keyb)
#define KEYS_START 2 
// The index into the .value part of UsbCommand indicating key modifiers like CTRL,SHIFT ...
#define KEY_MOD_INDEX 1
// This defines the count of UsbCommands allocated in parse_all_alloc
#define PREALLOC_AMOUNT 64
// 32 Keys being able to be held is probably a sane Default, who in their right mind wants to hold 170 keys?
// If you want you can obviously change it anyways
#define MAX_KEYS_HELD 32
#define KEY_WORD_COUNT sizeof(KEYWORDS) / sizeof(KeyWordPair)

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
            const char* input_ptr = &input[(*index)];
            for(size_t j = 0; j < DUCK_KEYS_COUNT; j++) {
                size_t len = strlen(DUCK_KEYS[j].key);
                if( ((*index)+len) > input_len ) {
                    continue;
                }
                size_t str_len_until_comma_whitespace_semi = 0;
                while(
                    (input_ptr[str_len_until_comma_whitespace_semi]!=' ' 
                    && input_ptr[str_len_until_comma_whitespace_semi]!=';'
                    && input_ptr[str_len_until_comma_whitespace_semi]!=','
                    )
                    && str_len_until_comma_whitespace_semi < 20
                    && (*index)+str_len_until_comma_whitespace_semi < input_len
                )
                {
                    str_len_until_comma_whitespace_semi++;
                }

                if(len != str_len_until_comma_whitespace_semi){
                    continue;
                }


               // printf("str_len_until_comma_whitespace_semi: %zu\n",str_len_until_comma_whitespace_semi);
                int res = memcmp(&input[(*index)],DUCK_KEYS[j].key,len);
               // printf("failed to find %s \n", &input[(*index)]);
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

