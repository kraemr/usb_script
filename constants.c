#include "parser.h"
#include <stdint.h>

const KeyWordPair KEYWORDS[6] = {
    {"press",PRESS},
    {"PRESS",PRESS},
    {"hold",HOLD},
    {"HOLD",HOLD},
    {"release",RELEASE},
    {"RELEASE",RELEASE},    	
};

const uint8_t KEY_0 = 0x27;
const uint8_t KEY_1 = 0x1E;
const uint8_t KEY_2 = 0x1F;
const uint8_t KEY_3 = 0x20;
const uint8_t KEY_4 = 0x21;
const uint8_t KEY_5 = 0x22;
const uint8_t KEY_6 = 0x23;
const uint8_t KEY_7 = 0x24;
const uint8_t KEY_8 = 0x25;
const uint8_t KEY_9 = 0x26;

// Letters
const uint8_t KEY_A = 0x04;
const uint8_t KEY_B = 0x05;
const uint8_t KEY_C = 0x06;
const uint8_t KEY_D = 0x07;
const uint8_t KEY_E = 0x08;
const uint8_t KEY_F = 0x09;
const uint8_t KEY_G = 0x0A;
const uint8_t KEY_H = 0x0B;
const uint8_t KEY_I = 0x0C;
const uint8_t KEY_J = 0x0D;
const uint8_t KEY_K = 0x0E;
const uint8_t KEY_L = 0x0F;
const uint8_t KEY_M = 0x10;
const uint8_t KEY_N = 0x11;
const uint8_t KEY_O = 0x12;
const uint8_t KEY_P = 0x13;
const uint8_t KEY_Q = 0x14;
const uint8_t KEY_R = 0x15;
const uint8_t KEY_S = 0x16;
const uint8_t KEY_T = 0x17;
const uint8_t KEY_U = 0x18;
const uint8_t KEY_V = 0x19;
const uint8_t KEY_W = 0x1A;
const uint8_t KEY_X = 0x1B;
const uint8_t KEY_Y = 0x1C;
const uint8_t KEY_Z = 0x1D;

// Named Keys & Commands
const uint8_t AGAIN = 0x79;
const uint8_t ALT_LEFT = 0xE2;
const uint8_t ALT_RIGHT = 0xE6;
const uint8_t ALTERNATE_ERASE = 0x99;
const uint8_t APOSTROPHE = 0x34;
const uint8_t APPLICATION = 0x65;
const uint8_t ARROW_DOWN = 0x51;
const uint8_t ARROW_LEFT = 0x50;
const uint8_t ARROW_RIGHT = 0x4F;
const uint8_t ARROW_UP = 0x52;
const uint8_t BACKSLASH = 0x31;
const uint8_t BACKSPACE = 0x2A;
const uint8_t BRACKET_LEFT = 0x2F;
const uint8_t BRACKET_RIGHT = 0x30;
const uint8_t CANCEL = 0x9B;
const uint8_t CAPS_LOCK = 0x39;
const uint8_t CLEAR = 0x9C;
const uint8_t CLEAR_AGAIN = 0xA2;
const uint8_t COMMA = 0x36;
const uint8_t CONTROL_LEFT = 0xE0;
const uint8_t CONTROL_RIGHT = 0xE4;
const uint8_t COPY = 0x7C;
const uint8_t CRSEL_PROPS = 0xA3;
const uint8_t CUT = 0x7B;
const uint8_t DELETE = 0x4C;
const uint8_t END = 0x4D;
const uint8_t ENTER = 0x28;
const uint8_t EQUAL = 0x2E;
const uint8_t ESCAPE = 0x29;
const uint8_t EUROPE_1 = 0x32;
const uint8_t EUROPE_2 = 0x64;
const uint8_t EXECUTE = 0x74;
const uint8_t EXSEL = 0xA4;

// Function Keys
const uint8_t F1 = 0x3A;
const uint8_t F2 = 0x3B;
const uint8_t F3 = 0x3C;
const uint8_t F4 = 0x3D;
const uint8_t F5 = 0x3E;
const uint8_t F6 = 0x3F;
const uint8_t F7 = 0x40;
const uint8_t F8 = 0x41;
const uint8_t F9 = 0x42;
const uint8_t F10 = 0x43;
const uint8_t F11 = 0x44;
const uint8_t F12 = 0x45;
const uint8_t F13 = 0x68;
const uint8_t F14 = 0x69;
const uint8_t F15 = 0x6A;
const uint8_t F16 = 0x6B;
const uint8_t F17 = 0x6C;
const uint8_t F18 = 0x6D;
const uint8_t F19 = 0x6E;
const uint8_t F20 = 0x6F;
const uint8_t F21 = 0x70;
const uint8_t F22 = 0x71;
const uint8_t F23 = 0x72;
const uint8_t F24 = 0x73;

// Special & Navigation
const uint8_t FIND = 0x7E;
const uint8_t GRAVE = 0x35;
const uint8_t GUI_LEFT = 0xE3;
const uint8_t GUI_RIGHT = 0xE7;
const uint8_t HELP = 0x75;
const uint8_t HOME = 0x4A;
const uint8_t INSERT = 0x49;
const uint8_t KANJI1 = 0x87;
const uint8_t KANJI2 = 0x88;
const uint8_t KANJI3 = 0x89;
const uint8_t KANJI4 = 0x8A;
const uint8_t KANJI5 = 0x8B;
const uint8_t KANJI6 = 0x8C;
const uint8_t KANJI7 = 0x8D;
const uint8_t KANJI8 = 0x8E;
const uint8_t KANJI9 = 0x8F;

// Keypad
const uint8_t KEYPAD_0 = 0x62;
const uint8_t KEYPAD_1 = 0x59;
const uint8_t KEYPAD_2 = 0x5A;
const uint8_t KEYPAD_3 = 0x5B;
const uint8_t KEYPAD_4 = 0x5C;
const uint8_t KEYPAD_5 = 0x5D;
const uint8_t KEYPAD_6 = 0x5E;
const uint8_t KEYPAD_7 = 0x5F;
const uint8_t KEYPAD_8 = 0x60;
const uint8_t KEYPAD_9 = 0x61;
const uint8_t KEYPAD_ADD = 0x57;
const uint8_t KEYPAD_COMMA = 0x85;
const uint8_t KEYPAD_DECIMAL = 0x63;
const uint8_t KEYPAD_DIVIDE = 0x54;
const uint8_t KEYPAD_ENTER = 0x58;
const uint8_t KEYPAD_EQUAL = 0x67;
const uint8_t KEYPAD_EQUAL_SIGN = 0x86;
const uint8_t KEYPAD_MULTIPLY = 0x55;
const uint8_t KEYPAD_SUBTRACT = 0x56;

// System & International
const uint8_t LANG1 = 0x90;
const uint8_t LANG2 = 0x91;
const uint8_t LANG3 = 0x92;
const uint8_t LANG4 = 0x93;
const uint8_t LANG5 = 0x94;
const uint8_t LANG6 = 0x95;
const uint8_t LANG7 = 0x96;
const uint8_t LANG8 = 0x97;
const uint8_t LANG9 = 0x98;
const uint8_t LOCKING_CAPS_LOCK = 0x82;
const uint8_t LOCKING_NUM_LOCK = 0x83;
const uint8_t LOCKING_SCROLL_LOCK = 0x84;
const uint8_t MENU = 0x76;
const uint8_t MINUS = 0x2D;
const uint8_t MUTE = 0x7F;
const uint8_t NONE = 0x00;
const uint8_t NUM_LOCK = 0x53;
const uint8_t OPER = 0xA1;
const uint8_t OUT = 0xA0;
const uint8_t PAGE_DOWN = 0x4E;
const uint8_t PAGE_UP = 0x4B;
const uint8_t PASTE = 0x7D;
const uint8_t PAUSE = 0x48;
const uint8_t PERIOD = 0x37;
const uint8_t POWER = 0x66;
const uint8_t PRINT_SCREEN = 0x46;
const uint8_t PRIOR = 0x9D;
const uint8_t RETURN = 0x9E;
const uint8_t SCROLL_LOCK = 0x47;
const uint8_t SELECT = 0x77;
const uint8_t SEMICOLON = 0x33;
const uint8_t SEPARATOR = 0x9F;
const uint8_t SHIFT_LEFT = 0xE1;
const uint8_t SHIFT_RIGHT = 0xE5;
const uint8_t SLASH = 0x38;
const uint8_t SPACE = 0x2C;
const uint8_t STOP = 0x78;
const uint8_t SYSREQ_ATTENTION = 0x9A;
const uint8_t TAB = 0x2B;
const uint8_t UNDO = 0x7A;
const uint8_t VOLUME_DOWN = 0x81;
const uint8_t VOLUME_UP = 0x80;

const KeyPair DUCK_KEYS[170] = {
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