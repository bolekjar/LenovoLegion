// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */

#pragma once


namespace LenovoLegionDaemon {

constexpr const char* KEY_EN_UNUSED             = "";
constexpr const char* ZONE_EN_KEYBOARD          = "Keyboard";

constexpr const char* KEY_EN_ESCAPE             = "Key: Escape";
constexpr const char* KEY_EN_F1                 = "Key: F1";
constexpr const char* KEY_EN_F2                 = "Key: F2";
constexpr const char* KEY_EN_F3                 = "Key: F3";
constexpr const char* KEY_EN_F4                 = "Key: F4";
constexpr const char* KEY_EN_F5                 = "Key: F5";
constexpr const char* KEY_EN_F6                 = "Key: F6";
constexpr const char* KEY_EN_F7                 = "Key: F7";
constexpr const char* KEY_EN_F8                 = "Key: F8";
constexpr const char* KEY_EN_F9                 = "Key: F9";
constexpr const char* KEY_EN_F10                = "Key: F10";
constexpr const char* KEY_EN_F11                = "Key: F11";
constexpr const char* KEY_EN_F12                = "Key: F12";
constexpr const char* KEY_EN_PRINT_SCREEN       = "Key: Print Screen";
constexpr const char* KEY_EN_SCROLL_LOCK        = "Key: Scroll Lock";
constexpr const char* KEY_EN_PAUSE_BREAK        = "Key: Pause/Break";
constexpr const char* KEY_EN_POWER              = "Key: Power";

constexpr const char* KEY_EN_BACK_TICK          = "Key: `";
constexpr const char* KEY_EN_1                  = "Key: 1";
constexpr const char* KEY_EN_2                  = "Key: 2";
constexpr const char* KEY_EN_3                  = "Key: 3";
constexpr const char* KEY_EN_4                  = "Key: 4";
constexpr const char* KEY_EN_5                  = "Key: 5";
constexpr const char* KEY_EN_6                  = "Key: 6";
constexpr const char* KEY_EN_7                  = "Key: 7";
constexpr const char* KEY_EN_8                  = "Key: 8";
constexpr const char* KEY_EN_9                  = "Key: 9";
constexpr const char* KEY_EN_0                  = "Key: 0";
constexpr const char* KEY_EN_MINUS              = "Key: -";
constexpr const char* KEY_EN_PLUS               = "Key: +";
constexpr const char* KEY_EN_EQUALS             = "Key: =";
constexpr const char* KEY_EN_BACKSPACE          = "Key: Backspace";
constexpr const char* KEY_EN_INSERT             = "Key: Insert";
constexpr const char* KEY_EN_HOME               = "Key: Home";
constexpr const char* KEY_EN_PAGE_UP            = "Key: Page Up";

constexpr const char* KEY_EN_TAB                = "Key: Tab";
constexpr const char* KEY_EN_Q                  = "Key: Q";
constexpr const char* KEY_EN_W                  = "Key: W";
constexpr const char* KEY_EN_E                  = "Key: E";
constexpr const char* KEY_EN_R                  = "Key: R";
constexpr const char* KEY_EN_T                  = "Key: T";
constexpr const char* KEY_EN_Y                  = "Key: Y";
constexpr const char* KEY_EN_U                  = "Key: U";
constexpr const char* KEY_EN_I                  = "Key: I";
constexpr const char* KEY_EN_O                  = "Key: O";
constexpr const char* KEY_EN_P                  = "Key: P";
constexpr const char* KEY_EN_LEFT_BRACKET       = "Key: [";
constexpr const char* KEY_EN_RIGHT_BRACKET      = "Key: ]";
constexpr const char* KEY_EN_BACK_SLASH         = "Key: \\";
constexpr const char* KEY_EN_ANSI_BACK_SLASH    = "Key: \\ (ANSI)";
constexpr const char* KEY_EN_DELETE             = "Key: Delete";
constexpr const char* KEY_EN_END                = "Key: End";
constexpr const char* KEY_EN_PAGE_DOWN          = "Key: Page Down";

constexpr const char* KEY_EN_CAPS_LOCK          = "Key: Caps Lock";
constexpr const char* KEY_EN_A                  = "Key: A";
constexpr const char* KEY_EN_S                  = "Key: S";
constexpr const char* KEY_EN_D                  = "Key: D";
constexpr const char* KEY_EN_F                  = "Key: F";
constexpr const char* KEY_EN_G                  = "Key: G";
constexpr const char* KEY_EN_H                  = "Key: H";
constexpr const char* KEY_EN_J                  = "Key: J";
constexpr const char* KEY_EN_K                  = "Key: K";
constexpr const char* KEY_EN_L                  = "Key: L";
constexpr const char* KEY_EN_SEMICOLON          = "Key: ;";
constexpr const char* KEY_EN_QUOTE              = "Key: '";
constexpr const char* KEY_EN_POUND              = "Key: #";
constexpr const char* KEY_EN_ANSI_ENTER         = "Key: Enter";
constexpr const char* KEY_EN_ISO_ENTER          = "Key: Enter (ISO)";

constexpr const char* KEY_EN_LEFT_SHIFT         = "Key: Left Shift";
constexpr const char* KEY_EN_ISO_BACK_SLASH     = "Key: \\ (ISO)";
constexpr const char* KEY_EN_Z                  = "Key: Z";
constexpr const char* KEY_EN_X                  = "Key: X";
constexpr const char* KEY_EN_C                  = "Key: C";
constexpr const char* KEY_EN_V                  = "Key: V";
constexpr const char* KEY_EN_B                  = "Key: B";
constexpr const char* KEY_EN_N                  = "Key: N";
constexpr const char* KEY_EN_M                  = "Key: M";
constexpr const char* KEY_EN_COMMA              = "Key: ,";
constexpr const char* KEY_EN_PERIOD             = "Key: .";
constexpr const char* KEY_EN_FORWARD_SLASH      = "Key: /";
constexpr const char* KEY_EN_RIGHT_SHIFT        = "Key: Right Shift";
constexpr const char* KEY_EN_UP_ARROW           = "Key: Up Arrow";

constexpr const char* KEY_EN_LEFT_CONTROL       = "Key: Left Control";
constexpr const char* KEY_EN_LEFT_WINDOWS       = "Key: Left Windows";
constexpr const char* KEY_EN_LEFT_FUNCTION      = "Key: Left Fn";
constexpr const char* KEY_EN_LEFT_ALT           = "Key: Left Alt";
constexpr const char* KEY_EN_SPACE              = "Key: Space";
constexpr const char* KEY_EN_RIGHT_ALT          = "Key: Right Alt";
constexpr const char* KEY_EN_RIGHT_FUNCTION     = "Key: Right Fn";
constexpr const char* KEY_EN_RIGHT_WINDOWS      = "Key: Right Windows";
constexpr const char* KEY_EN_MENU               = "Key: Menu";
constexpr const char* KEY_EN_RIGHT_CONTROL      = "Key: Right Control";
constexpr const char* KEY_EN_LEFT_ARROW         = "Key: Left Arrow";
constexpr const char* KEY_EN_DOWN_ARROW         = "Key: Down Arrow";
constexpr const char* KEY_EN_RIGHT_ARROW        = "Key: Right Arrow";

constexpr const char* KEY_EN_NUMPAD_LOCK        = "Key: Num Lock";
constexpr const char* KEY_EN_NUMPAD_DIVIDE      = "Key: Number Pad /";
constexpr const char* KEY_EN_NUMPAD_TIMES       = "Key: Number Pad *";
constexpr const char* KEY_EN_NUMPAD_MINUS       = "Key: Number Pad -";
constexpr const char* KEY_EN_NUMPAD_PLUS        = "Key: Number Pad +";
constexpr const char* KEY_EN_NUMPAD_PERIOD      = "Key: Number Pad .";
constexpr const char* KEY_EN_NUMPAD_ENTER       = "Key: Number Pad Enter";
constexpr const char* KEY_EN_NUMPAD_0           = "Key: Number Pad 0";
constexpr const char* KEY_EN_NUMPAD_1           = "Key: Number Pad 1";
constexpr const char* KEY_EN_NUMPAD_2           = "Key: Number Pad 2";
constexpr const char* KEY_EN_NUMPAD_3           = "Key: Number Pad 3";
constexpr const char* KEY_EN_NUMPAD_4           = "Key: Number Pad 4";
constexpr const char* KEY_EN_NUMPAD_5           = "Key: Number Pad 5";
constexpr const char* KEY_EN_NUMPAD_6           = "Key: Number Pad 6";
constexpr const char* KEY_EN_NUMPAD_7           = "Key: Number Pad 7";
constexpr const char* KEY_EN_NUMPAD_8           = "Key: Number Pad 8";
constexpr const char* KEY_EN_NUMPAD_9           = "Key: Number Pad 9";

constexpr const char* KEY_EN_MEDIA_PLAY_PAUSE   = "Key: Media Play/Pause";
constexpr const char* KEY_EN_MEDIA_PREVIOUS     = "Key: Media Previous";
constexpr const char* KEY_EN_MEDIA_NEXT         = "Key: Media Next";
constexpr const char* KEY_EN_MEDIA_STOP         = "Key: Media Stop";
constexpr const char* KEY_EN_MEDIA_MUTE         = "Key: Media Mute";
constexpr const char* KEY_EN_MEDIA_VOLUME_DOWN  = "Key: Media Volume -";
constexpr const char* KEY_EN_MEDIA_VOLUME_UP    = "Key: Media Volume +";

constexpr const char* KEY_JP_RO                 = "Key: _";
constexpr const char* KEY_JP_EJ                 = "Key: E/J";
constexpr const char* KEY_JP_ZENKAKU            = "Key: 半角/全角";
constexpr const char* KEY_JP_KANA               = "Key: かな";
constexpr const char* KEY_JP_HENKAN             = "Key: 変換";
constexpr const char* KEY_JP_MUHENKAN           = "Key: 無変換";
constexpr const char* KEY_JP_YEN                = "Key: ¥";
constexpr const char* KEY_JP_AT                 = "Key: @";
constexpr const char* KEY_JP_CHEVRON            = "Key: ^";
constexpr const char* KEY_JP_COLON              = "Key: :";

constexpr const char* KEY_KR_HAN                = "Key: 한/영";
constexpr const char* KEY_KR_HANJA              = "Key: 한자";

constexpr const char* KEY_NORD_AAL              = "Key: Å";
constexpr const char* KEY_NORD_A_OE             = "Key: Ä Ø";
constexpr const char* KEY_NORD_O_AE             = "Key: Ö Æ";
constexpr const char* KEY_NORD_HALF             = "Key: § ½";
constexpr const char* KEY_NORD_HYPHEN           = "Key: - _";
constexpr const char* KEY_NORD_PLUS_QUESTION    = "Key: + ?";
constexpr const char* KEY_NORD_ACUTE_GRAVE      = "Key: ´ `";
constexpr const char* KEY_NORD_DOTS_CARET       = "Key: ¨ ^";
constexpr const char* KEY_NORD_QUOTE            = "Key: ' *";
constexpr const char* KEY_NORD_ANGLE_BRACKET    = "Key: < >";

constexpr const char* KEY_DE_ESZETT             = "Key: ß";
constexpr const char* KEY_DE_DIAERESIS_A        = "Key: Ä";
constexpr const char* KEY_DE_DIAERESIS_O        = "Key: Ö";
constexpr const char* KEY_DE_DIAERESIS_U        = "Key: Ü";

constexpr const char* KEY_FR_SUPER_2            = "Key: ²";
constexpr const char* KEY_FR_AMPERSAND          = "Key: &";
constexpr const char* KEY_FR_ACUTE_E            = "Key: é";
constexpr const char* KEY_FR_DOUBLEQUOTE        = "Key: \"";
constexpr const char* KEY_FR_LEFT_PARENTHESIS   = "Key: (";
constexpr const char* KEY_FR_GRAVE_E            = "Key: è";
constexpr const char* KEY_FR_UNDERSCORE         = "Key: _";
constexpr const char* KEY_FR_CEDILLA_C          = "Key: ç";
constexpr const char* KEY_FR_GRAVE_A            = "Key: à";
constexpr const char* KEY_FR_RIGHT_PARENTHESIS  = "Key: )";
constexpr const char* KEY_FR_DOLLAR             = "Key: $";
constexpr const char* KEY_FR_GRAVE_U            = "Key: ù";
constexpr const char* KEY_FR_ASTERIX            = "Key: *";
constexpr const char* KEY_FR_EXCLAIMATION       = "Key: !";

constexpr const char* KEY_ES_OPEN_QUESTION_MARK = "Key: ¿/¡";
constexpr const char* KEY_ES_TILDE              = "Key: ´/¨";
constexpr const char* KEY_ES_ENIE               = "Key: Ñ";

}
