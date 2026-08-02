#include "parser.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define DUCK_KEYS_COUNT sizeof(DUCK_KEYS) / sizeof(KeyPair)
// index 2 is start of the keys, 0 is REPORT_ID (1 being report id for keyb)
#define KEYS_START 1
// The index into the .value part of UsbCommand indicating key modifiers like
// CTRL,SHIFT ...
#define KEY_MOD_INDEX 0
// This defines the count of UsbCommands allocated in parse_all_alloc
#define PREALLOC_AMOUNT 256
// 32 Keys being able to be held is probably a sane Default, who in their right
// mind wants to hold 170 keys? If you want you can obviously change it anyways
#define MAX_KEYS_HELD 32
#define KEY_WORD_COUNT sizeof(KEYWORDS) / sizeof(KeyWordPair)

unsigned char get_duck_key(char *key_name, size_t len) {
  for (int i = 0; i < DUCK_KEYS_COUNT; i++) {
    if (strlen(DUCK_KEYS[i].key) != len) {
      continue;
    }
    if (!strncmp(key_name, DUCK_KEYS[i].key, len)) {
      return DUCK_KEYS[i].val;
    }
  }
  return 0xFF;
}

void fill_usb_command(UsbCommand *cmd) {
    cmd->type = KEYBOARD;
    cmd->command = PRESS;
    // 0:1 means ReportIDKeyboard
    cmd->value.keys[0] = 0;
    // 1 Contains OR ed keymodifiers
    cmd->value.keys[1] = 0;
    // 6 keypresses possible to send at once
    cmd->value.keys[2] = 0;
    cmd->value.keys[3] = 0;
    cmd->value.keys[4] = 0;
    cmd->value.keys[5] = 0;
    cmd->value.keys[6] = 0;
}

// later change it to bit indexed
void set_key(UsbState *ktx, unsigned char held, unsigned char val) {
  //printf("{%x %x %x %x %x %x} %x %x\n",ktx->keys[0],ktx->keys[1],ktx->keys[2],ktx->keys[3],ktx->keys[4],ktx->keys[5] ,held , val);
  for (int i = 0; i < sizeof(ktx->keys); i++) {
    if (ktx->keys[i] == val && held == 0) {
      ktx->keys[i] = 0;
      //printf("after {%x %x %u %x %x %x} %x %x\n",ktx->keys[0],ktx->keys[1],ktx->keys[2],ktx->keys[3],ktx->keys[4],ktx->keys[5] ,held , val);
      return;
    } else if (held == 1 && ktx->keys[i] == 0) {
      ktx->keys[i] = val;
      //printf("after {%x %x %x %x %x %x} %x %x\n",ktx->keys[0],ktx->keys[1],ktx->keys[2],ktx->keys[3],ktx->keys[4],ktx->keys[5] ,held , val);
      return;
    }
  }
}

void set_mouse_button(UsbState *ktx, uint8_t val) {
  ktx->mouse_buttons |= val;
}

void clear_mouse_button(UsbState *ktx, uint8_t val) {
  ktx->mouse_buttons &= ~val;
}

void set_mouse_abs_pos(UsbState *ktx, uint16_t x, uint16_t y) {
  ktx->mouse_x_abs = x;
  ktx->mouse_y_abs = y;
}

void set_mouse_scroll(UsbState *ktx, uint8_t scroll) {
  ktx->horizontal_scroll = scroll;
}

PARSING_STATE handle_keyword(const char *input, size_t *index,
                             UsbCommand *cmd) {
  PARSING_STATE state =
      KEYWORD_FOUND_ERR; // set to err if loop doesnt find keyword return
  for (size_t j = 0; j < KEY_WORD_COUNT; j++) {
    size_t len = strlen(KEYWORDS[j].keyword);
    int res = memcmp(&input[(*index)], KEYWORDS[j].keyword,
                     strlen(KEYWORDS[j].keyword));
    if (res == 0) {
      cmd->command = KEYWORDS[j].cmd;
      state = EXPECT_DATA;
      (*index) += len;
      break;
    }
  }
  return state;
}

PARSING_STATE handle_data(const char *input, unsigned short input_len,
                          UsbState *kctx, UsbCommand *cmd, size_t *index,
                          size_t *keys_index) {
  PARSING_STATE state = DUCK_KEY_ERR;
  const char *input_ptr = &input[(*index)];

  for (size_t j = 0; j < DUCK_KEYS_COUNT; j++) {
    size_t len = strlen(DUCK_KEYS[j].key);      
    if (((*index) + len) > input_len) {
      continue;
    }

    size_t i = 0;
    uint8_t could_be_mouse = (input[(*index)] == 'm' || input[(*index)] == 'M') && j < 6;
    
    
    uint8_t is_not_colon_space_comma = (input_ptr[i] != ' ' &&
            input_ptr[i] != ';' &&
            input_ptr[i] != ',');
    
    uint8_t less_than_max_size =  i < 20;
    uint8_t in_bounds =  (*index) + i < input_len;
    do
    {
        i++;
        less_than_max_size =  i < 20;
        in_bounds =  (*index) + i < input_len;
        if (!less_than_max_size || !in_bounds) {
            return state;
        }
        is_not_colon_space_comma = (input_ptr[i] != ' ' &&
            input_ptr[i] != ';' &&
            input_ptr[i] != ',');
    }
    while (is_not_colon_space_comma &&
           less_than_max_size &&
           in_bounds);

    /*if (len != i) {
      continue;
    }*/
    
    uint8_t was_mouse = 0;
    if(could_be_mouse) {
      len = strlen(MOUSE_CMDS[j].key);
      int res = memcmp(&input[(*index)], MOUSE_CMDS[j].key, len);
      was_mouse = res == 0;      
      if (was_mouse) {
        // so we dont move when user specifies to just press left mouse for example, because absolute would also move the mouse always
        cmd->type = MOUSE_REL_MOVE; 
        cmd->value.mouse_rel_cmd.delta_x = 0;
        cmd->value.mouse_rel_cmd.delta_y = 0;        
      }

      if (res == 0 && cmd->command == HOLD) {
        set_mouse_button(kctx, MOUSE_CMDS[j].val);
        cmd->value.mouse_rel_cmd.buttons |= MOUSE_CMDS[j].val;
        state = EXPECT_COMMA_COLON;
        (*index) += len;
        break;
      }
      else if(res == 0 && cmd->command == RELEASE) {
        clear_mouse_button(kctx, MOUSE_CMDS[j].val);
        cmd->value.mouse_rel_cmd.buttons |= MOUSE_CMDS[j].val;
        state = EXPECT_COMMA_COLON;
        (*index) += len;
        break;
      }
      else if(res == 0){
        cmd->value.mouse_rel_cmd.buttons |= MOUSE_CMDS[j].val;
        state = EXPECT_COMMA_COLON;
        (*index) += len;
        break;
      }      
    }

    if(!was_mouse){
      int res = memcmp(&input[(*index)], DUCK_KEYS[j].key, len);
      if (res == 0) {
        uint8_t is_key_mod = j == 37 || j == 38 || j == 55 || j == 56 ||
                          j == 95 || j == 96 || j == 160 || j == 161;
        if (is_key_mod) {
          cmd->value.keys[KEY_MOD_INDEX] |= DUCK_KEYS[j].val;
        }
        // We only want to change key state if HOLD or RELEASE is specified
        else if (cmd->command == HOLD || cmd->command == RELEASE) {
          set_key(kctx, cmd->command == HOLD, DUCK_KEYS[j].val);
          cmd->value.keys[(*keys_index)] = DUCK_KEYS[j].val;
          (*keys_index)++;
        } else {
          cmd->value.keys[(*keys_index)] = DUCK_KEYS[j].val;
          (*keys_index)++;
        } 
        state = EXPECT_COMMA_COLON;
        (*index) += len;
        printf("after %c %zu\n", input[(*index)], len);
        break;
      }
    }
  }
  return state;
}



PARSING_STATE handle_delay(const char *input,unsigned short input_len, size_t *index,
                        UsbCommand *cmd) {
    PARSING_STATE state = DUCK_KEY_ERR;
    const int MAX_LEN_U32_AS_STR = 10;
    unsigned char num_str[MAX_LEN_U32_AS_STR+1];
    const char *input_ptr = &input[(*index)];
    unsigned char success = 0;

    for(int i =0; (i < input_len) && (i < MAX_LEN_U32_AS_STR); i++){
        if (input_ptr[i] == ';') {
            success = 1;
            num_str[i] = '\0';
            break;
        }else{
            num_str[i] = input_ptr[i];
        }
    }

    if(success) {
        const char* ptr = (const char*)&num_str[0];
        int value = strtoul(ptr,NULL,10);
        cmd->command     =  DELAY;
        cmd->type        =  WAIT;
        cmd->value.delay =  value;
        (*index) += strnlen(ptr,MAX_LEN_U32_AS_STR+1);
        state = EXPECT_COMMA_COLON;
    }
    return state;
}


PARSING_STATE parse_line(const char *input, unsigned short input_len,
                         UsbState *kctx, ParseResult * result, size_t *index) {
  PARSING_STATE state = EXPECT_KEYWORD;
  size_t keys_index = KEYS_START;
  size_t done_at = 0;
  uint8_t return_found = 0;
  result->count=0;
  fill_usb_command(&result->cmds[0]);
  fill_usb_command(&result->cmds[1]);
  UsbCommand* cmd = &result->cmds[0];

  if (cmd == NULL) {
    return NO_REFERENCE_FOUND;
  }

  while ((*index) < input_len) {
    if (input[(*index)] == ' ') {
      (*index)++;
      continue;
    }
    
    switch (state) {
    case EXPECT_KEYWORD:
      state = handle_keyword(input, index, cmd);
      if (state != EXPECT_DATA) {
        state = KEYWORD_FOUND_ERR;
        return state;
      }

      break;
    case EXPECT_DATA:
      if (cmd->command == DELAY) {
        state = handle_delay(input, input_len, index, cmd);
      }else if(cmd->command == PRESS){
        state = handle_data(input, input_len, kctx, cmd, index, &keys_index);
        
        if(cmd->type == MOUSE_REL_MOVE) {
          UsbCommand* cmd2 = &result->cmds[1];
          result->count = 2;
          memcpy(cmd2, cmd, sizeof(UsbCommand));  
        }else{
          UsbCommand* cmd2 = &result->cmds[1];
          result->count = 2;
          memcpy(&result->cmds[1].value.keys[1],kctx->keys,sizeof(uint8_t) * 6);
        }

        
      }
      else{
        state = handle_data(input, input_len, kctx, cmd, index, &keys_index);
      }
      if (state != EXPECT_COMMA_COLON) {
        state = DUCK_KEY_ERR;
        return state;
      }
      break;
    case EXPECT_COMMA_COLON:
      if (input[(*index)] == ',') {
        (*index)++;
        state = EXPECT_DATA;
      } else if (input[(*index)] == ';') {        
        (*index)++;
        state = (*index) >= input_len ? DONE : EXPECT_POSSIBLE_NEWLINE;
        done_at = (*index);
      } else {
        state = COMMA_COLON_MISSING;
        return state;
      }
      break;
    case EXPECT_POSSIBLE_NEWLINE:
      return_found = input[(*index)] == '\r';
      // Check for CRLF
      if ((*index + 2) < input_len && return_found &&
          input[(*index) + 1] == '\n') {
        (*index) += 2;
        return DONE;
      }
      // Check for LF or CR
      else if ((input[(*index)] == '\n') || return_found) {
        (*index)++;
        return DONE;
      } else {
        (*index) = done_at;
        return DONE;
      }
      break;
    case KEYWORD_FOUND_ERR:break;
    case DUCK_KEY_ERR:break;
    case COMMA_COLON_MISSING:break;
    case LINE_TOO_LONG:break;
    case NO_REFERENCE_FOUND:break;
    case DONE:break;
    case DONE_PRESS:break;
    }

  }
  return state;
}

PARSING_STATE parse_all_alloc(const char *input, size_t input_len,
                              UsbState *ctx, UsbCommand **cmd_list,
                              size_t *cmd_list_len) {
  (*cmd_list) = malloc(PREALLOC_AMOUNT * sizeof(UsbCommand));
  size_t index = 0;
  (*cmd_list_len) = 0;
  UsbCommand *cmd_list_ptr = (*cmd_list);
  PARSING_STATE state = DONE;
  ParseResult result;
  while (index < input_len) {
    state = parse_line(input, input_len, ctx,&result,&index);
    cmd_list_ptr[(*cmd_list_len)] = result.cmds[0];
    if (state == DONE) {
      if(result.count == 2) {
        (*cmd_list_len)++;
        cmd_list_ptr[(*cmd_list_len)] = result.cmds[1];
      }
      (*cmd_list_len)++;
    } else {
      printf("got state %u\n", state);
      break;
    }
  }
  return state;
}
