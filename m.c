

#include "parser.h"
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
    printf("x:%d y:%d \n",cmd->value.mouse_abs_cmd.x,cmd->value.mouse_abs_cmd.y);
}


void print_m_rel_usb_command(UsbCommand *cmd) {
    if (cmd == NULL) {
        printf("UsbCommand is NULL\n");
        return;
    }
    printf("delta x:%d delta y:%d \n",cmd->value.mouse_rel_cmd.delta_x,cmd->value.mouse_rel_cmd.delta_y);
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
    printf("Mouse Buttons      : 0x%02X (%u) | Bits: 0b", 
           state->mouse_buttons, 
           state->mouse_buttons);
    print_bits_8(state->mouse_buttons);
    printf("\n");

    /* 3. Print Scroll States */
    printf("Vertical Scroll    : %d (0x%02X)\n", 
           (int8_t)state->vertical_scroll, 
           state->vertical_scroll);
           
    printf("Horizontal Scroll  : %d (0x%02X)\n", 
           (int8_t)state->horizontal_scroll, 
           state->horizontal_scroll);

    /* 4. Print Absolute Pointer Coordinates */
    printf("Mouse X (Abs)      : %d\n", state->mouse_x_abs);
    printf("Mouse Y (Abs)      : %d\n", state->mouse_y_abs);
    
    printf("================\n");
}

int main(int argc, char* argv[]) {
    int i = 4;
    size_t index = 0;
    UsbState state = {0};
    
    while(1) {    
        ParseResult result = {0};
        PARSING_STATE parser_state = parse_line(argv[1], strlen(argv[1]), &state, &result, &index);
        printf("state: %u \n", parser_state );
        if(result.cmds->type == KEYBOARD) {
            for(int i = 0; i < result.count; i++) {
                print_kb_usb_command(&result.cmds[i]);
            }
        }else if(result.cmds->type == MOUSE_REL_MOVE || result.cmds->type == MOUSE_BUTTONS) {
            for(int i = 0; i < result.count; i++) {
                print_m_rel_usb_command(&result.cmds[i]);
            }
        }else if(result.cmds->type == MOUSE_ABS_MOVE){
            for(int i = 0; i < result.count; i++) {
                print_m_abs_usb_command(&result.cmds[i]);
            }
        }   
        print_usb_state(&state);
        if (parser_state != DONE) break;
    }

	
}