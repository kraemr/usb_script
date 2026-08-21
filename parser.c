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

// Returns the number of found u16s, should be equal to 2 for our use case
int parse_comma_sep_u16(const char *s, uint16_t xy[2]) {
    xy[0] = 0;
    xy[1] = 0;
    int i = 0;     
    char* end_ptr = NULL;    
    while (*s && *s != ';') {
        size_t len = strcspn(s, ",;\n");
        xy[i] = (uint16_t)strtol(s, &end_ptr, 10); // x is always 0 y is always 1
        i++;
        // Jump to the delimiter (',' or ';')
        s += len;
        if (*s == ',') s++;
    }
    return i;
}


void zero_usb_command(UsbCommand *cmd) {
    memset(cmd,0,sizeof(USB_COMMAND));
}

void set_key(UsbState *ktx, unsigned char held, unsigned char val) {
  for (int i = 0; i < sizeof(ktx->keys); i++) {
    if (ktx->keys[i] == val && held == 0) {
      ktx->keys[i] = 0;
      return;
    } else if (held == 1 && ktx->keys[i] == 0) {
      ktx->keys[i] = val;
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


/*
  -1 means failure
  > -1 means index at which data starts
*/
int find_data_start(const char *input_ptr,uint16_t input_len,size_t *index) {
  size_t i = 0;
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
          return -1;
      }
      is_not_colon_space_comma = (input_ptr[i] != ' ' &&
          input_ptr[i] != ';' &&
          input_ptr[i] != ',');
    }
    while (is_not_colon_space_comma &&
           less_than_max_size &&
           in_bounds);
    return i;
}

PARSING_STATE handle_keyboard(const char *input, unsigned short input_len,
                          UsbState *kctx, ParseResult * result, size_t *index,
                          size_t *keys_index) {
  PARSING_STATE state = DUCK_KEY_ERR;
  UsbCommand* cmd = result->cmds;
  int i = find_data_start(&input[(*index)],input_len,index);
  if(i == -1 ) {
    return state;
  }
  for (size_t j = 0; j < DUCK_KEYS_COUNT; j++) {
    size_t len = strlen(DUCK_KEYS[j].key);      
    
    if (((*index) + len) > input_len) {
      continue;
    }

    int matches = memcmp(&input[(*index)], DUCK_KEYS[j].key, len) == 0;
    if (matches) {
      //TODO: Make these into constants
      uint8_t is_key_mod = j == 37 || j == 38 || j == 55 || j == 56 ||
                          j == 95 || j == 96 || j == 160 || j == 161;
      if (is_key_mod) {
        cmd->value.keyboard_cmd.modifier |= DUCK_KEYS[j].val;
      }
      // We only want to change key state if HOLD or RELEASE is specified
      else if (cmd->command == KB_HOLD || cmd->command == KB_RELEASE) {
        set_key(kctx, cmd->command == KB_HOLD, DUCK_KEYS[j].val);
        cmd->value.keyboard_cmd.keys[(*keys_index)] = DUCK_KEYS[j].val;
        result->count = 1;
        (*keys_index)++;
      } else {
        cmd->value.keyboard_cmd.keys[(*keys_index)] = DUCK_KEYS[j].val;
        result->count = 2;
        (*keys_index)++;
      } 
      state = EXPECT_COMMA_COLON;
      (*index) += len;
      break;
    }
  } 
  if(result->count == 2) {
    memcpy(&result->cmds[1], cmd, sizeof(UsbCommand));
    memset(&result->cmds[1].value.keyboard_cmd.keys, 0, sizeof(USB_COMMAND_VALUE));
    memcpy(&result->cmds[1].value.keyboard_cmd.keys, kctx->keys,sizeof(uint8_t) * 6);
  }
  return state;
}

PARSING_STATE handle_mouse_buttons(const char *input, unsigned short input_len,
                          UsbState * kctx, ParseResult * result, size_t *index) {
  PARSING_STATE state = DUCK_KEY_ERR;
  const int MOUSE_CMD_COUNT = (sizeof(MOUSE_CMDS) / sizeof(KeyPair));
  int i = find_data_start(&input[(*index)],input_len,index);
  UsbCommand* cmd = &result->cmds[0];
  cmd->type = MOUSE_BUTTONS;
  cmd->command = MOUSE_RELATIVE_MOVE;
  for (size_t j = 0; j < MOUSE_CMD_COUNT; j++) {
    size_t len = strlen(MOUSE_CMDS[j].key);   
    if (((*index) + len) > input_len) {
      continue;
    }
    int matches = memcmp(&input[(*index)], MOUSE_CMDS[j].key, len) == 0;
    if (matches && cmd->command == MOUSE_HOLD ) {
      // sets the bit for the mouse button in state
      set_mouse_button(kctx,MOUSE_CMDS[j].val);
      cmd->value.mouse_rel_cmd.buttons |= MOUSE_CMDS[j].val;
      state = EXPECT_COMMA_COLON;
      result->count = 1;
      (*index) += len;
    }else if(matches && cmd->command == MOUSE_RELEASE ) {
      // clears the bit set for the mouse button
      clear_mouse_button(kctx, MOUSE_CMDS[j].val);
      cmd->value.mouse_rel_cmd.buttons &= ~MOUSE_CMDS[j].val;
      state = EXPECT_COMMA_COLON;
    result->count = 1;
      (*index) += len;
    }else if(matches){
      cmd->value.mouse_rel_cmd.buttons |= MOUSE_CMDS[j].val;
      memcpy(&result->cmds[1], cmd, sizeof(UsbCommand));
      result->cmds[1].value.mouse_rel_cmd.buttons = kctx->mouse_buttons;
      state = EXPECT_COMMA_COLON;
      result->count = 2;
      (*index) += len;
    }
  }
  return state;
}


/*
  save x and y of the last mouse_move, so that clicking the mouse keeps the position
  TODO Error Handling
*/
PARSING_STATE handle_abs_mouse_move(const char *input,unsigned short input_len,UsbState * kctx, size_t *index,ParseResult * result) {
  PARSING_STATE state = DUCK_KEY_ERR;
  int res = find_data_start(&input[(*index)],input_len, index);
  uint16_t temp_arr[2] = {0};
  res = parse_comma_sep_u16(&input[(*index)], temp_arr);

  if(res == 2) {    
    kctx->mouse_x_abs = temp_arr[0];
    kctx->mouse_y_abs = temp_arr[1];

    result->cmds[0].command = MOUSE_ABSOLUTE_MOVE;
    result->cmds[0].type = MOUSE_ABS_MOVE;
    // keep the currently pressed buttons state
    result->cmds[0].value.mouse_abs_cmd.buttons = kctx->mouse_buttons;
    result->cmds[0].value.mouse_abs_cmd.x = kctx->mouse_x_abs;
    result->cmds[0].value.mouse_abs_cmd.y = kctx->mouse_y_abs;
    // Keep scroll wheel "state", for example if scroll wheel is Held down/up
    result->cmds[0].value.mouse_abs_cmd.horizontal_scroll = kctx->horizontal_scroll;
    result->cmds[0].value.mouse_abs_cmd.vertical_scroll = kctx->vertical_scroll;
  }
  result->count = 1;
  state = EXPECT_COMMA_COLON;
  return state;
}

/*
  Delta/Relative movements have no state to account for movements
  TODO Error Handling
*/
PARSING_STATE handle_rel_mouse_move(const char *input,unsigned short input_len,UsbState * kctx, size_t *index,ParseResult * result) {
  PARSING_STATE state = DUCK_KEY_ERR;
  int res = find_data_start(&input[(*index)],input_len, index);
  uint16_t temp_arr[2] = {0};
  res = parse_comma_sep_u16(&input[(*index)], temp_arr);
  
  if(res == 2) {    
    result->cmds[0].command = MOUSE_RELATIVE_MOVE;
    result->cmds[0].type = MOUSE_REL_MOVE;    
    result->cmds[0].value.mouse_rel_cmd.buttons = kctx->mouse_buttons;    
    // TODO: Throw error if out of bounds ? 
    result->cmds[0].value.mouse_rel_cmd.delta_x = temp_arr[0];
    result->cmds[0].value.mouse_rel_cmd.delta_y = temp_arr[1];
    // Keep scroll wheel "state", for example if scroll wheel is Held down/up
    result->cmds[0].value.mouse_rel_cmd.horizontal_scroll = kctx->horizontal_scroll;
    result->cmds[0].value.mouse_rel_cmd.vertical_scroll = kctx->vertical_scroll;
  }
  
  result->count = 1;
  state = EXPECT_COMMA_COLON;
  return state;
}


PARSING_STATE handle_delay(const char *input,unsigned short input_len, size_t *index,ParseResult * result) {
    PARSING_STATE state = DUCK_KEY_ERR;
    const int MAX_LEN_U32_AS_STR = 10;
    unsigned char num_str[MAX_LEN_U32_AS_STR+1];
    unsigned char success = 0;
    
    UsbCommand* cmd = result->cmds;

    for(int i =0; (i < input_len) && (i < MAX_LEN_U32_AS_STR); i++){
        if (input[i] == ';') {
            success = 1;
            num_str[i] = '\0';
            break;
        }else{
            num_str[i] = input[i];
        }
    }

    if(success) {
        result->count = 1;
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


PARSING_STATE handle_data(const char *input, unsigned short input_len,
                          UsbState *kctx, UsbCommand *cmd, size_t *index,ParseResult * result,
                          size_t *keys_index) {
  PARSING_STATE state = DUCK_KEY_ERR;
  switch (cmd->command) {
  case SET_LANG:
  case SWITCH_HID_MODE:
  case MOUSE_PRESS:
    state = handle_mouse_buttons(input,input_len,kctx,result,index);break;
  case MOUSE_HOLD:
    state = handle_mouse_buttons(input,input_len,kctx,result,index);break;
  case MOUSE_RELEASE:
    state = handle_mouse_buttons(input,input_len,kctx,result,index);break;
  case KB_PRESS:
    state = handle_keyboard(input,input_len,kctx,result,index,keys_index);break;
  case KB_HOLD:
    state = handle_keyboard(input,input_len,kctx,result,index,keys_index);break;
  case KB_RELEASE:
    state = handle_keyboard(input,input_len,kctx,result,index,keys_index);break;
  case RESTART:
    // noop
  case UNSUPPORTED:
    // noop
  case DELAY:
    state = handle_delay(input,input_len,index,result);break;
  case MOUSE_ABSOLUTE_MOVE:
    state = handle_abs_mouse_move(input,input_len,kctx,index,result);
  case MOUSE_RELATIVE_MOVE:
    state = handle_rel_mouse_move(input, input_len,kctx,index, result);
    break;
  }
  return state;
}


/*
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

      if (res == 0 && cmd->command == MOUSE_HOLD) {
        set_mouse_button(kctx, MOUSE_CMDS[j].val);
        cmd->value.mouse_rel_cmd.buttons |= MOUSE_CMDS[j].val;
        state = EXPECT_COMMA_COLON;
        (*index) += len;
        break;
      }
      else if(res == 0 && cmd->command == MOUSE_RELEASE) {
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

*/

/* 
Look ahead and find location of ;
If there is one:
parsing INSTEAD of char by character go by "block"
basically just split on a space

PRESS 0,1;        PRESS 0 ,   1 ;

block1:PRESS
block2:space
block3:0,1;

then it can be simplified to:
if (block1 == "PRESS" || "HOLD" || "RELEASE" ...) {
  handle_keywords
}

i = 0
for(block in blocks_except_1){
  if (block != SPACE) {
    break;
  }
  i++;
}
*/

void noop() {};

PARSING_STATE parse_line(const char *input, unsigned short input_len,
                         UsbState *kctx, ParseResult * result, size_t *index) {
  PARSING_STATE state = EXPECT_KEYWORD;
  size_t keys_index = KEYS_START;
  size_t done_at = 0;
  uint8_t return_found = 0;
  result->count=0;
  zero_usb_command(&result->cmds[0]);
  zero_usb_command(&result->cmds[1]);
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
      noop(); // so we dont need the C23 extension here
      
      state = handle_data(input, input_len, kctx, cmd, index,result, &keys_index);
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
