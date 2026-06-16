#include "parser.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#define DUCK_KEYS_COUNT sizeof(DUCK_KEYS) / sizeof(KeyPair)
// index 2 is start of the keys, 0 is REPORT_ID (1 being report id for keyb)
#define KEYS_START 2
// The index into the .value part of UsbCommand indicating key modifiers like
// CTRL,SHIFT ...
#define KEY_MOD_INDEX 1
// This defines the count of UsbCommands allocated in parse_all_alloc
#define PREALLOC_AMOUNT 128
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
    cmd->value.keys[0] = 1;
    // 1 Contains OR ed keymodifiers
    cmd->value.keys[1] = 0;
    // 6 keypresses possible to send at once
    cmd->value.keys[2] = 0;
    cmd->value.keys[3] = 0;
    cmd->value.keys[4] = 0;
    cmd->value.keys[5] = 0;
    cmd->value.keys[6] = 0;
    cmd->value.keys[7] = 0;
}

// later change it to bit indexed
void set_key(KeysContext *ktx, unsigned char held, unsigned char val) {
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
#ifdef DEBUG
      printf("FOUND KEYWORD %d\n", cmd->command);
#endif
      (*index) += len;
      break;
    }
  }
  return state;
}

PARSING_STATE handle_data(const char *input, unsigned short input_len,
                          KeysContext *kctx, UsbCommand *cmd, size_t *index,
                          size_t *keys_index) {
  PARSING_STATE state = DUCK_KEY_ERR;
  const char *input_ptr = &input[(*index)];
  for (size_t j = 0; j < DUCK_KEYS_COUNT; j++) {
    size_t len = strlen(DUCK_KEYS[j].key);    
    if (((*index) + len) > input_len) {
      continue;
    }

    size_t i = 0;
    uint8_t is_not_colon_space_comma = (input_ptr[i] != ' ' &&
            input_ptr[i] != ';' &&
            input_ptr[i] != ',');
    uint8_t less_than_max_size =  i < 20;
    uint8_t in_bounds =  (*index) + i < input_len;
    do
    {
        ++i;
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

    if (len != i) {
      continue;
    }

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
#ifdef DEBUG
      printf("Found Value: %u\n", DUCK_KEYS[j].val);
#endif
      state = EXPECT_COMMA_COLON;
      (*index) += len;
      break;
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
        printf("%d\n", value );
        cmd->command     =  DELAY;
        cmd->type        =  WAIT;
        cmd->value.delay =  value;
        (*index) += strnlen(ptr,MAX_LEN_U32_AS_STR+1);
        state = EXPECT_COMMA_COLON;
    }
    return state;
}


PARSING_STATE parse_line(const char *input, unsigned short input_len,
                         KeysContext *kctx, UsbCommand *cmd, size_t *index) {
  PARSING_STATE state = EXPECT_KEYWORD;
  size_t keys_index = KEYS_START;
  size_t done_at = 0;
  if (cmd == NULL) {
    return NO_REFERENCE_FOUND;
  }
  fill_usb_command(cmd);
  while ((*index) < input_len) {
    if (input[(*index)] == ' ') {
      (*index)++;
      continue;
    }
#ifdef DEBUG
    printf("input[i]: %c\n", input[(*index)]);
#endif

    if (state == EXPECT_KEYWORD) {
      state = handle_keyword(input, index, cmd);
      if (state != EXPECT_DATA) {
        state = KEYWORD_FOUND_ERR;
        return state;
      } else {
        continue;
      }
    } else if (state == EXPECT_DATA) {
      if (cmd->command == DELAY) {
        state = handle_delay(input, input_len, index, cmd);
      }else{
        state = handle_data(input, input_len, kctx, cmd, index, &keys_index);
      }
      if (state != EXPECT_COMMA_COLON) {
        state = DUCK_KEY_ERR;
        return state;
      } else {
        continue;
      }
    } else if (state == EXPECT_COMMA_COLON) {
      if (input[(*index)] == ',') {
#ifdef DEBUG
        printf("FOUND Comma\n");
#endif
        state = EXPECT_DATA;
        (*index)++;
      } else if (input[(*index)] == ';') {
        state = DONE;
        (*index)++;
        done_at = (*index);
      } else {
#ifdef DEBUG
        printf("COMMA IS MISSING\n");
#endif
        state = COMMA_COLON_MISSING;
        return state;
      }
    } else {
      uint8_t return_found = input[(*index)] == '\r';
      // Check for CRLF
      if ((*index + 2) < input_len && return_found &&
          input[(*index) + 1] == '\n') {
        (*index) += 2;
        return state;
      }
      // Check for LF or CR
      else if ((input[(*index)] == '\n') || return_found) {
        (*index)++;
        return state;
      } else {
        (*index) = done_at;
        return state;
      }
    }
  }
  return state;
}

PARSING_STATE parse_all_alloc(const char *input, size_t input_len,
                              KeysContext *ctx, UsbCommand **cmd_list,
                              size_t *cmd_list_len) {
  (*cmd_list) = malloc(PREALLOC_AMOUNT * sizeof(UsbCommand));
  size_t index = 0;
  (*cmd_list_len) = 0;
  UsbCommand *cmd_list_ptr = (*cmd_list);
  PARSING_STATE state = DONE;
  while (index < input_len) {
#ifdef DEBUG
    printf("STARTED PARSING LINE \n");
#endif
    state = parse_line(input, input_len, ctx, &cmd_list_ptr[(*cmd_list_len)],
                       &index);
#ifdef DEBUG
    printf("ENDED PARSING LINE \n");
#endif
    if (state == DONE) {
      (*cmd_list_len)++;
    } else {
      break;
    }
  }
  return state;
}
