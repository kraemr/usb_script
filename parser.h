#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

typedef enum USB_COMMAND {
  SET_LANG,
  SWITCH_HID_MODE, // Switches Hid mode from keyboard to controller for example

  MOUSE_PRESS,
  MOUSE_HOLD,
  MOUSE_RELEASE,

  KB_PRESS,
  KB_HOLD,
  KB_RELEASE,

  RESTART, // When put at the end, program gets run again after finished, good
           // for commands that need to be sent repeatedly
  UNSUPPORTED,
  DELAY,
  MOUSE_ABSOLUTE_MOVE,
  MOUSE_RELATIVE_MOVE,
} USB_COMMAND;

typedef struct KeyWordPair {
  const char *keyword;
  USB_COMMAND cmd;
} KeyWordPair;

typedef struct {
  unsigned char keys[6];
  uint8_t mouse_buttons;
  uint8_t vertical_scroll;
  uint8_t horizontal_scroll;

  // DO NOT change order of mouse_x and mouse_y here!!
  int16_t mouse_x_abs;
  int16_t mouse_y_abs;
} UsbState;

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
  DONE_PRESS,
} PARSING_STATE;

typedef struct {
  uint8_t buttons;
  int16_t x;
  int16_t y;
  int8_t vertical_scroll;
  int8_t horizontal_scroll;
} MOUSE_ABS_MOVE_CMD;

typedef struct {
  uint8_t buttons;
  int8_t delta_x;
  int8_t delta_y;
  int8_t vertical_scroll;
  int8_t horizontal_scroll;
} MOUSE_REL_MOVE_CMD;

typedef struct USB_COMMAND_KEYBOARD {
  uint8_t modifier;
  uint8_t keys[6];
} USB_COMMAND_KEYBOARD;

typedef union USB_COMMAND_VALUE {
  uint32_t delay;
  USB_COMMAND_KEYBOARD keyboard_cmd;
  MOUSE_ABS_MOVE_CMD mouse_abs_cmd;
  MOUSE_REL_MOVE_CMD mouse_rel_cmd;
} USB_COMMAND_VALUE;

typedef enum USB_COMMAND_VALUE_TYPE {
  KEYBOARD,
  WAIT,
  MOUSE_ABS_MOVE,
  MOUSE_REL_MOVE,
  MOUSE_BUTTONS,
} USB_COMMAND_VALUE_TYPE;

typedef struct UsbCommand {
  USB_COMMAND command;
  USB_COMMAND_VALUE value;
  USB_COMMAND_VALUE_TYPE type;
} UsbCommand;

typedef struct __attribute__((packed)) {
  const char *key;
  uint8_t val;
} KeyPair;

typedef struct ParseResult {
  uint8_t count;
  UsbCommand cmds[2];
} ParseResult;

extern const KeyPair DUCK_KEYS[170];
extern const KeyPair MOUSE_CMDS[6];
extern const KeyWordPair KEYWORDS[9];
extern PARSING_STATE parse_all_alloc(const char *input, size_t input_len,
                                     UsbState *ctx, UsbCommand **cmd_list,
                                     size_t *cmd_list_len);
PARSING_STATE parse_line(const char *input, unsigned short input_len,
                         UsbState *kctx, ParseResult *result, size_t *index);
extern void set_key_index(UsbState *ktx, unsigned char held, size_t index);
#endif