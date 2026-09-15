

#include "parser.h"
#include <stddef.h>
#include <stdio.h>
void print_kb_usb_command(UsbCommand *cmd) {
  if (cmd == NULL) {
    printf("UsbCommand is NULL\n");
    return;
  }
  printf("UsbCommand Table:\n");
  printf("-------------------------------\n");
  printf("Command: %d\n", cmd->command);
  printf("Values (Hex): ");
  for (int i = 0; i < 6; i++) {
    printf("%02X ", cmd->value.keyboard_cmd.keys[i]);
  }
  printf("\n-------------------------------\n");
}

static void print_bits_8(uint8_t val) {
  for (int i = 7; i >= 0; i--) {
    putchar((val & (1 << i)) ? '1' : '0');
  }
}

void print_m_abs_usb_command(UsbCommand *cmd) {
  if (cmd == NULL) {
    printf("UsbCommand is NULL\n");
    return;
  }
  printf("x:%d y:%d \n", cmd->value.mouse_abs_cmd.x,
         cmd->value.mouse_abs_cmd.y);
}

void print_m_rel_usb_command(UsbCommand *cmd) {
  if (cmd == NULL) {
    printf("UsbCommand is NULL\n");
    return;
  }
  printf("delta x:%d delta y:%d \n", cmd->value.mouse_rel_cmd.delta_x,
         cmd->value.mouse_rel_cmd.delta_y);
  printf("buttons: ");
  print_bits_8(cmd->value.mouse_rel_cmd.buttons);
  printf("\n");
}

void print_usb_state(UsbState *state) {
  if (!state) {
    printf("UsbState: NULL\n");
    return;
  }

  printf("=== UsbState ===\n");

  /* 1. Print Active Key Codes */
  printf("Keys [6]           : [ ");
  for (int i = 0; i < 6; i++) {
    printf("0x%02X ", state->keys[i]);
  }
  printf("]\n");

  /* 2. Print Mouse Buttons (Raw Hex + Decimal + Bitmask) */
  printf("Mouse Buttons      : 0x%02X (%u) | Bits: 0b", state->mouse_buttons,
         state->mouse_buttons);
  print_bits_8(state->mouse_buttons);
  printf("\n");

  /* 3. Print Scroll States */
  printf("Vertical Scroll    : %d (0x%02X)\n", (int8_t)state->vertical_scroll,
         state->vertical_scroll);

  printf("Horizontal Scroll  : %d (0x%02X)\n", (int8_t)state->horizontal_scroll,
         state->horizontal_scroll);

  /* 4. Print Absolute Pointer Coordinates */
  printf("Mouse X (Abs)      : %d\n", state->mouse_x_abs);
  printf("Mouse Y (Abs)      : %d\n", state->mouse_y_abs);

  printf("================\n");
}

typedef struct {
  const char *name;
  const char *payload;
  ParseResult results[16];
  int results_len;
} TestCase;

const TestCase SMALL_DELAY = {.name = "SMALL_DELAY",
                              .payload = "delay 1000;", // 1000ms delay
                              .results =
                                  {
                                      {.count = 1,
                                       .cmds = {{.command = DELAY,
                                                 .type = WAIT,
                                                 .value =
                                                     {
                                                         .delay = 1000,
                                                     }}}

                                      },
                                  },
                              .results_len = 1};

const TestCase MOUSE_PRESS_LEFT_ONLY = {
    .name = "MOUSE_PRESS_LEFT_ONLY",
    .payload = "mouse_press mouse_left;", // 1000ms delay
    .results =
        {
            {.count = 2,
             .cmds = {{.command = MOUSE_PRESS,
                       .type = MOUSE_BUTTONS,
                       .value = {.mouse_rel_cmd =
                                     {
                                         .buttons = 0x1,
                                         .delta_x = 0,
                                         .delta_y = 0,
                                         .vertical_scroll = 0,
                                         .horizontal_scroll = 0,
                                     }}},
                      {.command = MOUSE_PRESS,
                       .type = MOUSE_BUTTONS,
                       .value = {.mouse_rel_cmd =
                                     {
                                         .buttons = 0b00000000,
                                         .delta_x = 0,
                                         .delta_y = 0,
                                         .vertical_scroll = 0,
                                         .horizontal_scroll = 0,
                                     }}}}

            },
        },
    .results_len = 1};

const TestCase MOUSE_PRESS_RIGHT_ONLY = {
    .name = "MOUSE_PRESS_RIGHT_ONLY",
    .payload = "mouse_press mouse_right;", // 1000ms delay
    .results =
        {
            {.count = 2,
             .cmds = {{.command = MOUSE_PRESS,
                       .type = MOUSE_BUTTONS,
                       .value = {.mouse_rel_cmd =
                                     {
                                         .buttons = 0x2,
                                         .delta_x = 0,
                                         .delta_y = 0,
                                         .vertical_scroll = 0,
                                         .horizontal_scroll = 0,
                                     }}},
                      {.command = MOUSE_PRESS,
                       .type = MOUSE_BUTTONS,
                       .value = {.mouse_rel_cmd =
                                     {
                                         .buttons = 0b00000000,
                                         .delta_x = 0,
                                         .delta_y = 0,
                                         .vertical_scroll = 0,
                                         .horizontal_scroll = 0,
                                     }}}}

            },
        },
    .results_len = 1};

const TestCase MOUSE_PRESS_ALL = {
    .name = "TEST_MOUSE_PRESS",
    .payload =
        "mouse_press mouse_left,mouse_right,mouse_middle;", // 1000ms delay
    .results =
        {
            {.count = 2,
             .cmds = {{.command = MOUSE_PRESS,
                       .type = MOUSE_BUTTONS,
                       .value = {.mouse_rel_cmd =
                                     {
                                         .buttons = 0x1 | 0x2 | 0x4,
                                         .delta_x = 0,
                                         .delta_y = 0,
                                         .vertical_scroll = 0,
                                         .horizontal_scroll = 0,
                                     }}},
                      {.command = MOUSE_PRESS,
                       .type = MOUSE_BUTTONS,
                       .value = {.mouse_rel_cmd =
                                     {
                                         .buttons = 0b00000000,
                                         .delta_x = 0,
                                         .delta_y = 0,
                                         .vertical_scroll = 0,
                                         .horizontal_scroll = 0,
                                     }}}}

            },
        },
    .results_len = 1};

const TestCase MOUSE_MOVE_REL = {
    .name = "MOUSE_MOVE_REL",
    .payload = "move_rel 100,101;", // 1000ms delay
    .results =
        {
            {.count = 1,
             .cmds = {{.command = MOUSE_RELATIVE_MOVE,
                       .type = MOUSE_REL_MOVE,
                       .value = {.mouse_rel_cmd =
                                     {
                                         .buttons = 0x00,
                                         .delta_x = 100,
                                         .delta_y = 101,
                                         .vertical_scroll = 0,
                                         .horizontal_scroll = 0,
                                     }}}}

            },
        },
    .results_len = 1};

const TestCase MOUSE_MOVE_ABS = {
    .name = "MOUSE_MOVE_ABS",
    .payload = "move_abs 100,101;", // 1000ms delay
    .results =
        {
            {.count = 1,
             .cmds = {{.command = MOUSE_ABSOLUTE_MOVE,
                       .type = MOUSE_ABS_MOVE,
                       .value = {.mouse_abs_cmd =
                                     {
                                         .buttons = 0x00,
                                         .x = 100,
                                         .y = 101,
                                         .vertical_scroll = 0,
                                         .horizontal_scroll = 0,
                                     }}}}

            },
        },
    .results_len = 1};

const TestCase KEYBOARD_PRESS =
    {
        .name = "KEYBOARD_PRESS",
        .payload = "press 0,1,2,3,4;",
        .results =
            {
                {.count = 2,
                 .cmds = {{.command = 5,
                           .type = KEYBOARD,
                           .value = {.keyboard_cmd = {.modifier = 0,
                                                      .keys = {0x27, 0x1E, 0x1F,
                                                               0x20, 0x21,
                                                               0x0}}}},
                          {.command = 5,
                           .type = KEYBOARD,
                           .value = {.keyboard_cmd = {.modifier = 0,
                                                      .keys = {0x0, 0x0, 0x0,
                                                               0x0, 0x0, 0x0}}}}}},
            },
        .results_len = 1};

const TestCase KEYBOARD_HOLD = {
    .name = "KEYBOARD_HOLD",
    .payload = "hold 0,1,2,3,4;",
    .results =
        {
            {.count = 1,
             .cmds =
                 {
                     {.command = KB_HOLD,
                      .type = KEYBOARD,
                      .value = {.keyboard_cmd = {.modifier = 0,
                                                 .keys = {0x27, 0x1E, 0x1F,
                                                          0x20, 0x21, 0x0}}}},
                 }},
        },
    .results_len = 1};

const TestCase KEYBOARD_RELEASE =
    {.name = "KEYBOARD_RELEASE",
     .payload = "hold 0,1,2,3,4;release 0,2,3,4;",
     .results = {{.count = 1,
                  .cmds =
                      {
                          {.command = KB_HOLD,
                           .type = KEYBOARD,
                           .value = {.keyboard_cmd = {.modifier = 0,
                                                      .keys = {0x27, 0x1E, 0x1F,
                                                               0x20, 0x21,
                                                               0x0}}}},
                      }},
                 {.count = 1,
                  .cmds =
                      {
                          {.command = KB_RELEASE,
                           .type = KEYBOARD,
                           .value = {.keyboard_cmd = {.modifier = 0,
                                                      .keys = {0x0, 0x1E,
                                                               0x0, 0x0, 0x0, 0x0}}}},

                      }}},
     .results_len = 2};

const TestCase
    KEYBOARD_HOLD_PRESS =
        {.name = "KEYBOARD_HOLD_PRESS",
         .payload = "hold 0,1;press 2,3,4;",
         .results =
             {
                 {.count = 1,
                  .cmds =
                      {
                          {.command = KB_HOLD,
                           .type = KEYBOARD,
                           .value = {.keyboard_cmd = {.modifier = 0,
                                                      .keys = {0x27, 0x1E, 0, 0,
                                                               0, 0}}}},
                      }},

                 {.count = 2,
                  .cmds = {{.command = 5,
                            .type = KEYBOARD,
                            .value = {.keyboard_cmd = {.modifier = 0,
                                                       .keys = {0x27, 0x1E,
                                                                0x1F, 0x20, 0x21, 0x00}}}},
                           {.command = 5,
                            .type = KEYBOARD,
                            .value = {.keyboard_cmd = {.modifier = 0,
                                                       .keys =
                                                           {
                                                               0x27, 0x1E, 0x0,
                                                               0x0, 0x0, 0x0}}}}}},
             },
         .results_len = 2};

const TestCase KEYBOARD_TESTCASES[] = {
    SMALL_DELAY,    MOUSE_PRESS_LEFT_ONLY, MOUSE_PRESS_ALL,
    MOUSE_MOVE_REL, MOUSE_MOVE_ABS,        KEYBOARD_PRESS,
    KEYBOARD_HOLD,  KEYBOARD_RELEASE,      KEYBOARD_HOLD_PRESS};

/*
    Closely simulate how the file is read on the pico from sd
*/
int run_test_case(const TestCase *tcase) {
  FILE *fp = fopen("/tmp/PicoTestCase", "w");
  fwrite(tcase->payload, strlen(tcase->payload), 1, fp);
  fclose(fp);

  UsbState state = {0};
  char buf[4096] = {0};
  fp = fopen("/tmp/PicoTestCase", "r");
  int i = 0;
  int something_failed = 0;

  while (1) {
    memset(buf, 0, 4096);
    char *s = fgets(buf, 4096, fp);

    if (s == NULL) {
      break;
    }

    size_t cmds_len = 0;
    size_t position_in_buffer = 0;
    ParseResult result = {0};

    while (position_in_buffer < strlen(s)) {
      PARSING_STATE ret =
          parse_line(buf, strlen(s), &state, &result, &position_in_buffer);
      if (i < tcase->results_len &&
          memcmp(&tcase->results[i], &result, sizeof(ParseResult)) != 0) {
        printf("########## IS ###########\n");
        print_m_rel_usb_command(&result.cmds[0]);

        printf("########## SHOULD BE ###########\n");
        print_m_rel_usb_command(&tcase->results[i].cmds[0]);
        something_failed = 1;
      }
      if (ret == DONE) {
        i++;
      } else {
        break;
      }
    }
  }
  fclose(fp);
  printf("Testcase: %s %s\n", tcase->name,
         something_failed ? "failed" : "succeeded");
  return 1;
}

int main(int argc, char *argv[]) {
  size_t count = (sizeof(KEYBOARD_TESTCASES) / sizeof(TestCase));
  for (int i = 0; i < count; i++)
    run_test_case(&KEYBOARD_TESTCASES[i]);
}
