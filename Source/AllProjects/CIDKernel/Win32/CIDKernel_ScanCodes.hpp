//
// FILE NAME: CIDKernel_ScanCodes.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/06/1995
//
// COPYRIGHT: Charmed Quark Systems, Ltd @ 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and
//  the author (Dean Roddey.) It is licensed under the MIT Open Source
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This file contains constant definitions for the scan codes of keys on the
//  host system. For those very few CIDLib apps that might need to do raw
//  keyboard input, these should be used in order to maintain portability.
//
//  Internally these are used by the TKrnlConsole class to do raw console
//  input and output.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  These are the key scan codes for the letters of the alphabet. They
//  are not preceeded by any leading bytes.
// ---------------------------------------------------------------------------
constexpr tCIDLib::TCh Key_a            = tCIDLib::TCh(97);
constexpr tCIDLib::TCh Key_b            = tCIDLib::TCh(98);
constexpr tCIDLib::TCh Key_c            = tCIDLib::TCh(99);
constexpr tCIDLib::TCh Key_d            = tCIDLib::TCh(100);
constexpr tCIDLib::TCh Key_e            = tCIDLib::TCh(101);
constexpr tCIDLib::TCh Key_f            = tCIDLib::TCh(102);
constexpr tCIDLib::TCh Key_g            = tCIDLib::TCh(103);
constexpr tCIDLib::TCh Key_h            = tCIDLib::TCh(104);
constexpr tCIDLib::TCh Key_i            = tCIDLib::TCh(105);
constexpr tCIDLib::TCh Key_j            = tCIDLib::TCh(106);
constexpr tCIDLib::TCh Key_k            = tCIDLib::TCh(107);
constexpr tCIDLib::TCh Key_l            = tCIDLib::TCh(108);
constexpr tCIDLib::TCh Key_m            = tCIDLib::TCh(109);
constexpr tCIDLib::TCh Key_n            = tCIDLib::TCh(110);
constexpr tCIDLib::TCh Key_o            = tCIDLib::TCh(111);
constexpr tCIDLib::TCh Key_p            = tCIDLib::TCh(112);
constexpr tCIDLib::TCh Key_q            = tCIDLib::TCh(113);
constexpr tCIDLib::TCh Key_r            = tCIDLib::TCh(114);
constexpr tCIDLib::TCh Key_s            = tCIDLib::TCh(115);
constexpr tCIDLib::TCh Key_t            = tCIDLib::TCh(116);
constexpr tCIDLib::TCh Key_u            = tCIDLib::TCh(117);
constexpr tCIDLib::TCh Key_v            = tCIDLib::TCh(118);
constexpr tCIDLib::TCh Key_w            = tCIDLib::TCh(119);
constexpr tCIDLib::TCh Key_x            = tCIDLib::TCh(120);
constexpr tCIDLib::TCh Key_y            = tCIDLib::TCh(121);
constexpr tCIDLib::TCh Key_z            = tCIDLib::TCh(122);

constexpr tCIDLib::TCh Key_A            = tCIDLib::TCh(65);
constexpr tCIDLib::TCh Key_B            = tCIDLib::TCh(66);
constexpr tCIDLib::TCh Key_C            = tCIDLib::TCh(67);
constexpr tCIDLib::TCh Key_D            = tCIDLib::TCh(68);
constexpr tCIDLib::TCh Key_E            = tCIDLib::TCh(69);
constexpr tCIDLib::TCh Key_F            = tCIDLib::TCh(70);
constexpr tCIDLib::TCh Key_G            = tCIDLib::TCh(71);
constexpr tCIDLib::TCh Key_H            = tCIDLib::TCh(72);
constexpr tCIDLib::TCh Key_I            = tCIDLib::TCh(73);
constexpr tCIDLib::TCh Key_J            = tCIDLib::TCh(74);
constexpr tCIDLib::TCh Key_K            = tCIDLib::TCh(75);
constexpr tCIDLib::TCh Key_L            = tCIDLib::TCh(76);
constexpr tCIDLib::TCh Key_M            = tCIDLib::TCh(77);
constexpr tCIDLib::TCh Key_N            = tCIDLib::TCh(78);
constexpr tCIDLib::TCh Key_O            = tCIDLib::TCh(79);
constexpr tCIDLib::TCh Key_P            = tCIDLib::TCh(80);
constexpr tCIDLib::TCh Key_Q            = tCIDLib::TCh(81);
constexpr tCIDLib::TCh Key_R            = tCIDLib::TCh(82);
constexpr tCIDLib::TCh Key_S            = tCIDLib::TCh(83);
constexpr tCIDLib::TCh Key_T            = tCIDLib::TCh(84);
constexpr tCIDLib::TCh Key_U            = tCIDLib::TCh(85);
constexpr tCIDLib::TCh Key_V            = tCIDLib::TCh(86);
constexpr tCIDLib::TCh Key_W            = tCIDLib::TCh(87);
constexpr tCIDLib::TCh Key_X            = tCIDLib::TCh(88);
constexpr tCIDLib::TCh Key_Y            = tCIDLib::TCh(89);
constexpr tCIDLib::TCh Key_Z            = tCIDLib::TCh(90);



// ---------------------------------------------------------------------------
//  These are the key scan codes for the numbers. They are not
//  preceeded by any leading bytes.
// ---------------------------------------------------------------------------
constexpr tCIDLib::TCh Key_0            = tCIDLib::TCh(48);
constexpr tCIDLib::TCh Key_1            = tCIDLib::TCh(49);
constexpr tCIDLib::TCh Key_2            = tCIDLib::TCh(50);
constexpr tCIDLib::TCh Key_3            = tCIDLib::TCh(51);
constexpr tCIDLib::TCh Key_4            = tCIDLib::TCh(52);
constexpr tCIDLib::TCh Key_5            = tCIDLib::TCh(53);
constexpr tCIDLib::TCh Key_6            = tCIDLib::TCh(54);
constexpr tCIDLib::TCh Key_7            = tCIDLib::TCh(55);
constexpr tCIDLib::TCh Key_8            = tCIDLib::TCh(56);
constexpr tCIDLib::TCh Key_9            = tCIDLib::TCh(57);



// ---------------------------------------------------------------------------
//  These are the key scan codes for the punctuation marks. They are
//  not preceeded by any leading bytes.
// ---------------------------------------------------------------------------
constexpr tCIDLib::TCh Key_Exclamation  = tCIDLib::TCh(33);
constexpr tCIDLib::TCh Key_DoubleQuote  = tCIDLib::TCh(34);
constexpr tCIDLib::TCh Key_Pound        = tCIDLib::TCh(35);
constexpr tCIDLib::TCh Key_Dollar       = tCIDLib::TCh(36);
constexpr tCIDLib::TCh Key_Percent      = tCIDLib::TCh(37);
constexpr tCIDLib::TCh Key_Ampersand    = tCIDLib::TCh(38);
constexpr tCIDLib::TCh Key_SingleQuote  = tCIDLib::TCh(39);
constexpr tCIDLib::TCh Key_OpenParen    = tCIDLib::TCh(40);
constexpr tCIDLib::TCh Key_CloseParen   = tCIDLib::TCh(41);
constexpr tCIDLib::TCh Key_Asterisk     = tCIDLib::TCh(42);
constexpr tCIDLib::TCh Key_Plus         = tCIDLib::TCh(43);
constexpr tCIDLib::TCh Key_Comma        = tCIDLib::TCh(44);
constexpr tCIDLib::TCh Key_Minus        = tCIDLib::TCh(45);
constexpr tCIDLib::TCh Key_Period       = tCIDLib::TCh(46);
constexpr tCIDLib::TCh Key_ForwardSlash = tCIDLib::TCh(47);
constexpr tCIDLib::TCh Key_Colon        = tCIDLib::TCh(58);
constexpr tCIDLib::TCh Key_SemiColon    = tCIDLib::TCh(59);
constexpr tCIDLib::TCh Key_LessThan     = tCIDLib::TCh(60);
constexpr tCIDLib::TCh Key_Equal        = tCIDLib::TCh(61);
constexpr tCIDLib::TCh Key_GreaterThan  = tCIDLib::TCh(62);
constexpr tCIDLib::TCh Key_Question     = tCIDLib::TCh(63);
constexpr tCIDLib::TCh Key_At           = tCIDLib::TCh(64);
constexpr tCIDLib::TCh Key_OpenBracket  = tCIDLib::TCh(91);
constexpr tCIDLib::TCh Key_BackSlash    = tCIDLib::TCh(92);
constexpr tCIDLib::TCh Key_CloseBracket = tCIDLib::TCh(93);
constexpr tCIDLib::TCh Key_Caret        = tCIDLib::TCh(94);
constexpr tCIDLib::TCh Key_OpenBrace    = tCIDLib::TCh(123);
constexpr tCIDLib::TCh Key_CloseBrace   = tCIDLib::TCh(125);
constexpr tCIDLib::TCh Key_Tilde        = tCIDLib::TCh(126);


// ---------------------------------------------------------------------------
//  These are the scan codes for the non-extended cursor movement
//  keys. They are not preceeded by any leading bytes.
// ---------------------------------------------------------------------------
constexpr tCIDLib::TCh Key_CtrlC        = tCIDLib::TCh(3);
constexpr tCIDLib::TCh Key_BS           = tCIDLib::TCh(8);
constexpr tCIDLib::TCh Key_Tab          = tCIDLib::TCh(9);
constexpr tCIDLib::TCh Key_Enter        = tCIDLib::TCh(13);
constexpr tCIDLib::TCh Key_BackTab      = tCIDLib::TCh(15);
constexpr tCIDLib::TCh Key_Escape       = tCIDLib::TCh(27);
constexpr tCIDLib::TCh Key_Space        = tCIDLib::TCh(32);


// ---------------------------------------------------------------------------
//  Codes for the shift keys
// ---------------------------------------------------------------------------
constexpr tCIDLib::TCh Key_Alt          = tCIDLib::TCh(56);
constexpr tCIDLib::TCh Key_Ctrl         = tCIDLib::TCh(29);
constexpr tCIDLib::TCh Key_Shift        = tCIDLib::TCh(42);


// ---------------------------------------------------------------------------
//  These are prefix characters which are received before extended key codes.
// ---------------------------------------------------------------------------
constexpr tCIDLib::TCh Key_PadPrefix    = tCIDLib::TCh(0);
constexpr tCIDLib::TCh Key_ExtPrefix    = tCIDLib::TCh(224);


// ---------------------------------------------------------------------------
//  These are the scan codes for the keypad style keys. These are all
//  prefixed by the pad key prefix value if they are on the keypad. And
//  by the extended prefix if from the other versions of those keys.
// ---------------------------------------------------------------------------
constexpr tCIDLib::TCh Key_Home         = tCIDLib::TCh(71);
constexpr tCIDLib::TCh Key_Up           = tCIDLib::TCh(72);
constexpr tCIDLib::TCh Key_PageUp       = tCIDLib::TCh(73);
constexpr tCIDLib::TCh Key_Left         = tCIDLib::TCh(75);
constexpr tCIDLib::TCh Key_Right        = tCIDLib::TCh(77);
constexpr tCIDLib::TCh Key_End          = tCIDLib::TCh(79);
constexpr tCIDLib::TCh Key_Down         = tCIDLib::TCh(80);
constexpr tCIDLib::TCh Key_PageDown     = tCIDLib::TCh(81);
constexpr tCIDLib::TCh Key_Insert       = tCIDLib::TCh(82);
constexpr tCIDLib::TCh Key_Delete       = tCIDLib::TCh(83);


// ---------------------------------------------------------------------------
//  Only available on the keypad, and prefixed by the pad prefix.
// ---------------------------------------------------------------------------
constexpr tCIDLib::TCh Key_Center       = tCIDLib::TCh(76);
