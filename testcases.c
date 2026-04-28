#include "testcases.h"
#include "parser.h"
#include <stddef.h>

const TestCase testcases[] = {
    {
        "testcase_normal_keypress",
        "press 0,1,2,3,4,5;",
        {
            {PRESS,1,0,0x27,0x1E,0x1F,0x20,0x21,0x22}
        },
        1,
        DONE,
    },
    {
        "testcase_one_key",
        "press 0;",
        {
            {PRESS,1,0,0x27,0,0,0,0,0}
        },
        1,
        DONE,
    },
    {
        "multiple_key_press_one_line",
        "press 0,1;press 2,3;",
        {
            {PRESS,1,0,0x27,0x1E,0,0,0,0},
            {PRESS,1,0,0x1F,0x20,0,0,0,0}
        },
        2,
        DONE,
    },
    {
        "multiple_key_press_mult_line",
        "press 0,1;\npress 2,3;",
        {
            {PRESS,1,0,0x27,0x1E,0,0,0,0},
            {PRESS,1,0,0x1F,0x20,0,0,0,0}
        },
        2,
        DONE,
    },
    {
        "multiple_key_press_mult_line_CRLF",
        "press 0,1;\r\npress 2,3;",
        {
            {PRESS,1,0,0x27,0x1E,0,0,0,0},
            {PRESS,1,0,0x1F,0x20,0,0,0,0}
        },
        2,
        DONE,
    },
    {
        "multiple_key_press_mult_line_CR",
        "press 0,1;\rpress 2,3;",
        {
            {PRESS,1,0,0x27,0x1E,0,0,0,0},
            {PRESS,1,0,0x1F,0x20,0,0,0,0}
        },
        2,
        DONE,
    },

        
};


const char * SUCCESS = "passed";
const char * FAILURE = "failure";

/*
    returns 1 if the test passed, 0 if not
*/
int check_test_passes(TestCase* testcase,UsbCommand* cmds,size_t cmds_count,PARSING_STATE state) {
    int cmd_count_matches = cmds_count == testcase->commands_count;
    if(!cmd_count_matches) {
        printf("Testcase %s cmds_count mismatch \n",testcase->name);
        return 0;
    }
    int state_matches = state == testcase->expected_state;
    if(!state_matches) {
        printf("Testcase %s state_matches mismatch \n",testcase->name);
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
            break;
        }

    }

    if(!cmds_match) {
        printf("Testcase %s cmds_match mismatch \n",testcase->name);
        return 0;
    }
    return cmd_count_matches && state_matches && cmds_match;
}

int main(){        
    size_t count = (sizeof(testcases) / sizeof(TestCase));
    printf("Testcases: \n");
    for(size_t i = 0; i < count; i++ ) {
        UsbCommand* cmds = NULL;
        size_t cmds_count = 0;
        TestCase test = testcases[i];
        PARSING_STATE state =  parse_all_alloc(test.str,strlen(test.str),&cmds,&cmds_count);                 
        int test_result = check_test_passes(&test,cmds,cmds_count,state);
        const char* result_str = test_result ? SUCCESS : FAILURE;
        printf("Testcase %s %s\n",test.name,result_str);
        free(cmds);        
    }    
}
