#ifdef TEST
#include "parser.h"
#include <stddef.h>
#include <stdint.h>
const char * SUCCESS = "passed";
const char * FAILURE = "failure";

void print_usb_command(UsbCommand *cmd) {
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

typedef struct TestCase {
    const char* name;
    const char* str;
    UsbCommand expected_commands[128];
    KeysContext ctx;
    size_t commands_count;
    PARSING_STATE expected_state;
}TestCase;

const TestCase testcases[] = {
    {
        "testcase_normal_keypress",
        "press 0,1,2,3,4,5;",
        {
            {PRESS,1,0,0x27,0x1E,0x1F,0x20,0x21,0x22},
            {RELEASE,1,0,0,0,0,0,0,0},
        },
        {
            {0,0,0,0,0,0}
        },
        2,
        DONE,
    },
    {
        "testcase_one_key",
        "press 0;",
        {
            {PRESS,1,0,0x27,0,0,0,0,0},
            {RELEASE,1,0,0,0,0,0,0,0},
        },
        {
            {0}
        },
        2,
        DONE,
    },
    {
        "multiple_key_press_one_line",
        "press 0,1;press 2,3;",
        {
            {PRESS,1,0,0x27,0x1E,0,0,0,0},
            {RELEASE,1,0,0,0,0,0,0,0},
            {PRESS,1,0,0x1F,0x20,0,0,0,0},
            {RELEASE,1,0,0,0,0,0,0,0},
        },
        {
            {0}
        },
        4,
        DONE,
    },
    {
        "multiple_key_press_mult_line",
        "press 0,1;\npress 2,3;",
        {
            {PRESS,1,0,0x27,0x1E,0,0,0,0},
            {RELEASE,1,0,0,0,0,0,0,0},
            {PRESS,1,0,0x1F,0x20,0,0,0,0},
            {RELEASE,1,0,0,0,0,0,0,0},
        },
       {
            {0}
        },
        4,
        DONE,
    },
    {
        "multiple_key_press_mult_line_CRLF",
        "press 0,1;\r\npress 2,3;",
        {
            {PRESS,1,0,0x27,0x1E,0,0,0,0},
            {RELEASE,1,0,0,0,0,0,0,0},
            {PRESS,1,0,0x1F,0x20,0,0,0,0},
            {RELEASE,1,0,0,0,0,0,0,0},
        },
        {
            {0}
        },
        4,
        DONE,
    },
    {
        "multiple_key_press_mult_line_CR",
        "press 0,1;\rpress 2,3;",
        {
            {PRESS,1,0,0x27,0x1E,0,0,0,0},
            {RELEASE,1,0,0,0,0,0,0,0},
            {PRESS,1,0,0x1F,0x20,0,0,0,0},
            {RELEASE,1,0,0,0,0,0,0,0},
        },
        {
            {0}
        },
        4,
        DONE,
    },
    {
        "multiple_key_press_mult_line_CRLF",
        "press 0,1;\r\npress 2,3;",
        {
            {PRESS,1,0,0x27,0x1E,0,0,0,0},
            {RELEASE,1,0,0,0,0,0,0,0},
            {PRESS,1,0,0x1F,0x20,0,0,0,0},
            {RELEASE,1,0,0,0,0,0,0,0},
        },
        {
            {0}
        },
        4,
        DONE,
    },
    {
        "all_arrow_keys",
        "press ARROW_LEFT,ARROW_RIGHT,ARROW_UP,ARROW_DOWN;",
        {
            {PRESS,1,0,0x50,0x4F,0x52,0x51},
            {RELEASE,1,0,0,0,0,0,0,0},
        },
        {0},
        2,
        DONE,
    },
    {
    "all_possible_keys",
    "press 0,1;\npress 2,3;\npress 4,5;\npress 6,7;\npress 8,9;\n"
    "press A,B;\npress C,D;\npress E,F;\npress G,H;\npress I,J;\n"
    "press K,L;\npress M,N;\npress O,P;\npress Q,R;\npress S,T;\n"
    "press U,V;\npress W,X;\npress Y,Z;\npress AGAIN,ALT_LEFT;\n"
    "press ALT_RIGHT,ALTERNATE_ERASE;\npress APOSTROPHE,APPLICATION;\n"
    "press ARROW_DOWN,ARROW_LEFT;\npress ARROW_RIGHT,ARROW_UP;\n"
    "press BACKSLASH,BACKSPACE;\npress BRACKET_LEFT,BRACKET_RIGHT;\n"
    "press CANCEL,CAPS_LOCK;\npress CLEAR,CLEAR_AGAIN;\n"
    "press COMMA,CONTROL_LEFT;\npress CONTROL_RIGHT,COPY;\n"
    "press CRSEL_PROPS,CUT;\npress DELETE,END;\n"
    "press ENTER,EQUAL;\npress ESCAPE,EUROPE_1;\n"
    "press EUROPE_2,EXECUTE;\npress EXSEL,F1;\n"
    "press F2,F3;\npress F4,F5;\npress F6,F7;\n"
    "press F8,F9;\npress F10,F11;\npress F12,F13;\n"
    "press F14,F15;\npress F16,F17;\npress F18,F19;\n"
    "press F20,F21;\npress F22,F23;\npress F24,FIND;\n"
    "press GRAVE,GUI_LEFT;\npress GUI_RIGHT,HELP;\n"
    "press HOME,INSERT;\npress KANJI1,KANJI2;\n"
    "press KANJI3,KANJI4;\npress KANJI5,KANJI6;\n"
    "press KANJI7,KANJI8;\npress KANJI9,KEYPAD_0;\n"
    "press KEYPAD_1,KEYPAD_2;\npress KEYPAD_3,KEYPAD_4;\n"
    "press KEYPAD_5,KEYPAD_6;\npress KEYPAD_7,KEYPAD_8;\n"
    "press KEYPAD_9,KEYPAD_ADD;\npress KEYPAD_COMMA,KEYPAD_DECIMAL;\n"
    "press KEYPAD_DIVIDE,KEYPAD_ENTER;\npress KEYPAD_EQUAL,KEYPAD_EQUAL_SIGN;\n"
    "press KEYPAD_MULTIPLY,KEYPAD_SUBTRACT;\npress LANG1,LANG2;\n"
    "press LANG3,LANG4;\npress LANG5,LANG6;\n"
    "press LANG7,LANG8;\npress LANG9,LOCKING_CAPS_LOCK;\n"
    "press LOCKING_NUM_LOCK,LOCKING_SCROLL_LOCK;\n"
    "press MENU,MINUS;\npress MUTE,NONE;\n"
    "press NUM_LOCK,OPER;\npress OUT,PAGE_DOWN;\n"
    "press PAGE_UP,PASTE;\npress PAUSE,PERIOD;\n"
    "press POWER,PRINT_SCREEN;\npress PRIOR,RETURN;\n"
    "press SCROLL_LOCK,SELECT;\npress SEMICOLON,SEPARATOR;\n"
    "press SHIFT_LEFT,SHIFT_RIGHT;\npress SLASH,SPACE;\n"
    "press STOP,SYSREQ_ATTENTION;\npress TAB,UNDO;\n"
    "press VOLUME_DOWN,VOLUME_UP;",
    {
        // Numbers
        {PRESS, 1, 0, 0x27, 0x1E, 0, 0, 0, 0}, // 0, 1
        {PRESS, 1, 0, 0x1F, 0x20, 0, 0, 0, 0}, // 2, 3
        {PRESS, 1, 0, 0x21, 0x22, 0, 0, 0, 0}, // 4, 5
        {PRESS, 1, 0, 0x23, 0x24, 0, 0, 0, 0}, // 6, 7
        {PRESS, 1, 0, 0x25, 0x26, 0, 0, 0, 0}, // 8, 9

        // Letters
        {PRESS, 1, 0, 0x04, 0x05, 0, 0, 0, 0}, // A, B
        {PRESS, 1, 0, 0x06, 0x07, 0, 0, 0, 0}, // C, D
        {PRESS, 1, 0, 0x08, 0x09, 0, 0, 0, 0}, // E, F
        {PRESS, 1, 0, 0x0A, 0x0B, 0, 0, 0, 0}, // G, H
        {PRESS, 1, 0, 0x0C, 0x0D, 0, 0, 0, 0}, // I, J
        {PRESS, 1, 0, 0x0E, 0x0F, 0, 0, 0, 0}, // K, L
        {PRESS, 1, 0, 0x10, 0x11, 0, 0, 0, 0}, // M, N
        {PRESS, 1, 0, 0x12, 0x13, 0, 0, 0, 0}, // O, P
        {PRESS, 1, 0, 0x14, 0x15, 0, 0, 0, 0}, // Q, R
        {PRESS, 1, 0, 0x16, 0x17, 0, 0, 0, 0}, // S, T
        {PRESS, 1, 0, 0x18, 0x19, 0, 0, 0, 0}, // U, V
        {PRESS, 1, 0, 0x1A, 0x1B, 0, 0, 0, 0}, // W, X
        {PRESS, 1, 0, 0x1C, 0x1D, 0, 0, 0, 0}, // Y, Z

        // Named Keys & Commands
        {PRESS, 1, 0 | 0b00000100, 0x79, 0, 0, 0, 0, 0}, // AGAIN, ALT_LEFT
        {PRESS, 1, 0 | 0b01000000, 0x99, 0, 0, 0, 0, 0}, // ALT_RIGHT, ALTERNATE_ERASE
        {PRESS, 1, 0, 0x34, 0x65, 0, 0, 0, 0}, // APOSTROPHE, APPLICATION
        {PRESS, 1, 0, 0x51, 0x50, 0, 0, 0, 0}, // ARROW_DOWN, ARROW_LEFT
        {PRESS, 1, 0, 0x4F, 0x52, 0, 0, 0, 0}, // ARROW_RIGHT, ARROW_UP
        {PRESS, 1, 0, 0x31, 0x2A, 0, 0, 0, 0}, // BACKSLASH, BACKSPACE
        {PRESS, 1, 0, 0x2F, 0x30, 0, 0, 0, 0}, // BRACKET_LEFT, BRACKET_RIGHT
        {PRESS, 1, 0, 0x9B, 0x39, 0, 0, 0, 0}, // CANCEL, CAPS_LOCK
        {PRESS, 1, 0, 0x9C, 0xA2, 0, 0, 0, 0}, // CLEAR, CLEAR_AGAIN
        {PRESS, 1, 0 | 0b00000001, 0x36, 0, 0, 0, 0, 0}, // COMMA, CONTROL_LEFT
        {PRESS, 1, 0 | 0b00010000, 0x7c, 0, 0, 0, 0, 0}, // CONTROL_RIGHT, COPY
        {PRESS, 1, 0, 0xA3, 0x7B, 0, 0, 0, 0}, // CRSEL_PROPS, CUT
        {PRESS, 1, 0, 0x4C, 0x4D, 0, 0, 0, 0}, // DELETE, END
        {PRESS, 1, 0, 0x28, 0x2E, 0, 0, 0, 0}, // ENTER, EQUAL
        {PRESS, 1, 0, 0x29, 0x32, 0, 0, 0, 0}, // ESCAPE, EUROPE_1
        {PRESS, 1, 0, 0x64, 0x74, 0, 0, 0, 0}, // EUROPE_2, EXECUTE
        {PRESS, 1, 0, 0xA4, 0x3A, 0, 0, 0, 0}, // EXSEL, F1

        // Function Keys
        {PRESS, 1, 0, 0x3B, 0x3C, 0, 0, 0, 0}, // F2, F3
        {PRESS, 1, 0, 0x3D, 0x3E, 0, 0, 0, 0}, // F4, F5
        {PRESS, 1, 0, 0x3F, 0x40, 0, 0, 0, 0}, // F6, F7
        {PRESS, 1, 0, 0x41, 0x42, 0, 0, 0, 0}, // F8, F9
        {PRESS, 1, 0, 0x43, 0x44, 0, 0, 0, 0}, // F10, F11
        {PRESS, 1, 0, 0x45, 0x68, 0, 0, 0, 0}, // F12, F13
        {PRESS, 1, 0, 0x69, 0x6A, 0, 0, 0, 0}, // F14, F15
        {PRESS, 1, 0, 0x6B, 0x6C, 0, 0, 0, 0}, // F16, F17
        {PRESS, 1, 0, 0x6D, 0x6E, 0, 0, 0, 0}, // F18, F19
        {PRESS, 1, 0, 0x6F, 0x70, 0, 0, 0, 0}, // F20, F21
        {PRESS, 1, 0, 0x71, 0x72, 0, 0, 0, 0}, // F22, F23
        {PRESS, 1, 0, 0x73, 0x7E, 0, 0, 0, 0}, // F24, FIND

        // Special & Navigation
        {PRESS, 1, 0 | 0b00001000, 0x35, 0, 0, 0, 0, 0}, // GRAVE, GUI_LEFT
        {PRESS, 1, 0 | 0b10000000, 0x75, 0, 0, 0, 0, 0}, // GUI_RIGHT, HELP
        {PRESS, 1, 0, 0x4A, 0x49, 0, 0, 0, 0}, // HOME, INSERT
        {PRESS, 1, 0, 0x87, 0x88, 0, 0, 0, 0}, // KANJI1, KANJI2
        {PRESS, 1, 0, 0x89, 0x8A, 0, 0, 0, 0}, // KANJI3, KANJI4
        {PRESS, 1, 0, 0x8B, 0x8C, 0, 0, 0, 0}, // KANJI5, KANJI6
        {PRESS, 1, 0, 0x8D, 0x8E, 0, 0, 0, 0}, // KANJI7, KANJI8
        {PRESS, 1, 0, 0x8F, 0x62, 0, 0, 0, 0}, // KANJI9, KEYPAD_0

        // Keypad
        {PRESS, 1, 0, 0x59, 0x5A, 0, 0, 0, 0}, // KEYPAD_1, KEYPAD_2
        {PRESS, 1, 0, 0x5B, 0x5C, 0, 0, 0, 0}, // KEYPAD_3, KEYPAD_4
        {PRESS, 1, 0, 0x5D, 0x5E, 0, 0, 0, 0}, // KEYPAD_5, KEYPAD_6
        {PRESS, 1, 0, 0x5F, 0x60, 0, 0, 0, 0}, // KEYPAD_7, KEYPAD_8
        {PRESS, 1, 0, 0x61, 0x57, 0, 0, 0, 0}, // KEYPAD_9, KEYPAD_ADD
        {PRESS, 1, 0, 0x85, 0x63, 0, 0, 0, 0}, // KEYPAD_COMMA, KEYPAD_DECIMAL
        {PRESS, 1, 0, 0x54, 0x58, 0, 0, 0, 0}, // KEYPAD_DIVIDE, KEYPAD_ENTER
        {PRESS, 1, 0, 0x67, 0x86, 0, 0, 0, 0}, // KEYPAD_EQUAL, KEYPAD_EQUAL_SIGN
        {PRESS, 1, 0, 0x55, 0x56, 0, 0, 0, 0}, // KEYPAD_MULTIPLY, KEYPAD_SUBTRACT

        // System & International
        {PRESS, 1, 0, 0x90, 0x91, 0, 0, 0, 0}, // LANG1, LANG2
        {PRESS, 1, 0, 0x92, 0x93, 0, 0, 0, 0}, // LANG3, LANG4
        {PRESS, 1, 0, 0x94, 0x95, 0, 0, 0, 0}, // LANG5, LANG6
        {PRESS, 1, 0, 0x96, 0x97, 0, 0, 0, 0}, // LANG7, LANG8
        {PRESS, 1, 0, 0x98, 0x82, 0, 0, 0, 0}, // LANG9, LOCK_CAPS
        {PRESS, 1, 0, 0x83, 0x84, 0, 0, 0, 0}, // LOCK_NUM, LOCK_SCROLL
        {PRESS, 1, 0, 0x76, 0x2D, 0, 0, 0, 0}, // MENU, MINUS
        {PRESS, 1, 0, 0x7F, 0x00, 0, 0, 0, 0}, // MUTE, NONE
        {PRESS, 1, 0, 0x53, 0xA1, 0, 0, 0, 0}, // NUM_LOCK, OPER
        {PRESS, 1, 0, 0xA0, 0x4E, 0, 0, 0, 0}, // OUT, PAGE_DOWN
        {PRESS, 1, 0, 0x4B, 0x7D, 0, 0, 0, 0}, // PAGE_UP, PASTE
        {PRESS, 1, 0, 0x48, 0x37, 0, 0, 0, 0}, // PAUSE, PERIOD
        {PRESS, 1, 0, 0x66, 0x46, 0, 0, 0, 0}, // POWER, PRINT_SCREEN
        {PRESS, 1, 0, 0x9D, 0x9E, 0, 0, 0, 0}, // PRIOR, RETURN
        {PRESS, 1, 0, 0x47, 0x77, 0, 0, 0, 0}, // SCROLL_LOCK, SELECT
        {PRESS, 1, 0, 0x33, 0x9F, 0, 0, 0, 0}, // SEMICOLON, SEPARATOR
        {PRESS, 1, 0b00000010 | 0b00100000, 0, 0, 0, 0, 0, 0}, // SHIFT_LEFT, SHIFT_RIGHT
        {PRESS, 1, 0, 0x38, 0x2C, 0, 0, 0, 0}, // SLASH, SPACE
        {PRESS, 1, 0, 0x78, 0x9A, 0, 0, 0, 0}, // STOP, SYSREQ
        {PRESS, 1, 0, 0x2B, 0x7A, 0, 0, 0, 0}, // TAB, UNDO
        {PRESS, 1, 0, 0x81, 0x80, 0, 0, 0, 0}  // VOLUME_DOWN, VOLUME_UP
    },
    {
        {0}
    },
    85,
    DONE
    },

    {
        "zero_one_held",
        "HOLD 0,1;",
        {
            {HOLD,1,0,0x27,0x1E,0,0,0},
        },
        {0x27,0x1E},
        1,
        DONE,
    },
    {
        "zeron_one_held_one_release",
        "HOLD 0,1;\nRELEASE 1;",
        {
            {HOLD,   1,0,0x27,0x1E,0,0,0},
            {RELEASE,1,0,0x27,0,0,0,0},
        },
        {0x27,0},
        2,
        DONE,
    },


};

/*
    returns 1 if the test passed, 0 if not
*/
int check_test_passes(TestCase* testcase,UsbCommand* cmds,KeysContext* ctx,size_t cmds_count,PARSING_STATE state) {

    int state_matches = state == testcase->expected_state;
    if(!state_matches) {
        printf("Testcase %s state_matches mismatch \n",testcase->name);
        return 0;
    }

    int cmd_count_matches = cmds_count == testcase->commands_count;
    if(!cmd_count_matches) {
        printf("Testcase %s cmds_count mismatch exepcted: %zu, got %zu \n",testcase->name,testcase->commands_count,cmds_count);
        //uint8_t pressed = send_hid_keyboard_report(&cmd->value[2],keymod);
        return 0;
    }


    int cmds_match = 1;
    for(int i = 0; i < cmds_count;i++) {
        UsbCommand* test_cmd = &testcase->expected_commands[i];
        UsbCommand* cmd = &cmds[i];
        if (cmd->command != test_cmd->command) {
            cmds_match = 0;
            break;
        }

        if( memcmp(cmd->value,test_cmd->value,sizeof(test_cmd->value)) != 0  ) {
            cmds_match = 0;
            uint8_t* keys = &test_cmd->value[2];
            printf("expect kb input %d %u %u %u %u %u %u\n",test_cmd->value[1],keys[0],keys[1],keys[2],keys[3],keys[4],keys[5]);
            keys = &cmd->value[2];
            printf("got kb input %d %u %u %u %u %u %u\n",cmds->value[1],keys[0],keys[1],keys[2],keys[3],keys[4],keys[5]);
            break;
        }
    }
    int key_state_matches = memcmp(ctx->keys,testcase->ctx.keys,6 * sizeof(uint8_t) ) == 0;
    if(!key_state_matches) {
        printf("Testcase %s key_state_matches mismatch \n",testcase->name);
        return 0;
    }

    if(!cmds_match) {
        printf("Testcase %s cmds_match mismatch \n",testcase->name);
        return 0;
    }
    return cmd_count_matches && state_matches && cmds_match && key_state_matches;
}

void execute_payload(UsbCommand* cmd, KeysContext* ctx){
    uint8_t keys_i = 0;
    uint8_t keymod = cmd->value[1];
    uint8_t report_type = cmd->value[0];
    uint8_t* keys = &cmd->value[2];
    //uint8_t pressed = send_hid_keyboard_report(&cmd->value[2],keymod);
    printf("sending kb input %d %u %u %u %u %u %u\n",keymod,keys[0],keys[1],keys[2],keys[3],keys[4],keys[5]);
}

int main(){        
    size_t count = (sizeof(testcases) / sizeof(TestCase));
    
    printf("Testcases: \n");

    for(size_t i = 0; i < count; i++ ) {
        UsbCommand* cmds = NULL;
        KeysContext ctx = {0};
        size_t cmds_count = 0;
        TestCase test = testcases[i];
        PARSING_STATE state =  parse_all_alloc(test.str,strlen(test.str),&ctx,&cmds,&cmds_count);                 
        int test_result = check_test_passes(&test,cmds,&ctx,cmds_count,state);
        const char* result_str = test_result ? SUCCESS : FAILURE;
        printf("Testcase %s %s\n",test.name,result_str);
        //print_usb_command(cmds);
        free(cmds);        
    }    


    size_t index = 0;
    size_t cmd_list_len = 0;
    UsbCommand cmd;
    PARSING_STATE state = DONE;
    KeysContext ctx= {0};
    const char* input = "HOLD 0,1;RELEASE 0;PRESS 2;";
    while ( index < strlen(input) ) {
        state = parse_line(input,strlen(input),&ctx,&cmd,&index,state);
        execute_payload(&cmd,&ctx);
        printf("%d \n",state);
        if( state == DONE_PRESS){
            state = parse_line(input,strlen(input),&ctx,&cmd,&index,state);
            execute_payload(&cmd,&ctx);
            cmd_list_len++;                            
        }

        if(state == DONE) {
            cmd_list_len++;                            
        }else {                    
            break;
        }
    }

}
#endif