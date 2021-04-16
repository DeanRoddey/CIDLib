//
// FILE NAME: CIDKernel_Unicode.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/06/1996
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
//  This header contains the constants for a lot of the Unicode characters
//  that people will need to refer to and there is no convenient way to
//  do so within the actual source code such that its portable. Or, in
//  many cases, its just more self documenting and readable to use these
//  named values.
//
//  There are WAY too many to have them all defined, or even to provide
//  a majority of them all the time. So, if there are others provided, they
//  will be in specialized headers that are not normally included and must
//  be used explicitly by code that wants to use them.
//
//  This file is included within the kCIDLib namespace because its just
//  constants, but there were enough of them that we want to keep them
//  in a separate file.
//
//  These are stored in a native wide character character type, so they
//  might not be 2 byte unsigned values.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


namespace kCIDLib
{
    // Standard control characters
    constexpr   tCIDLib::TCh    chNull          = 0x00;
    constexpr   tCIDLib::TCh    chBell          = 0x07;
    constexpr   tCIDLib::TCh    chBS            = 0x08;
    constexpr   tCIDLib::TCh    chTab           = 0x09;
    constexpr   tCIDLib::TCh    chLF            = 0x0A;
    constexpr   tCIDLib::TCh    chVTab          = 0x0B;
    constexpr   tCIDLib::TCh    chFF            = 0x0C;
    constexpr   tCIDLib::TCh    chCR            = 0x0D;
    constexpr   tCIDLib::TCh    chEOF           = 0x1A;
    constexpr   tCIDLib::TCh    chEsc           = 0x1B;
    constexpr   tCIDLib::TCh    chSpace         = 0x20;
    constexpr   tCIDLib::TCh    chDel           = 0x7F;

    // Standard punctuation stuff
    constexpr   tCIDLib::TCh    chExclamation   = 0x21;
    constexpr   tCIDLib::TCh    chQuotation     = 0x22;
    constexpr   tCIDLib::TCh    chPoundSign     = 0x23;
    constexpr   tCIDLib::TCh    chDollarSign    = 0x24;
    constexpr   tCIDLib::TCh    chPercentSign   = 0x25;
    constexpr   tCIDLib::TCh    chAmpersand     = 0x26;
    constexpr   tCIDLib::TCh    chApostrophe    = 0x27;
    constexpr   tCIDLib::TCh    chOpenParen     = 0x28;
    constexpr   tCIDLib::TCh    chCloseParen    = 0x29;
    constexpr   tCIDLib::TCh    chAsterisk      = 0x2A;
    constexpr   tCIDLib::TCh    chPlusSign      = 0x2B;
    constexpr   tCIDLib::TCh    chComma         = 0x2C;
    constexpr   tCIDLib::TCh    chHyphenMinus   = 0x2D;
    constexpr   tCIDLib::TCh    chPeriod        = 0x2E;
    constexpr   tCIDLib::TCh    chForwardSlash  = 0x2F;
    constexpr   tCIDLib::TCh    chColon         = 0x3A;
    constexpr   tCIDLib::TCh    chSemiColon     = 0x3B;
    constexpr   tCIDLib::TCh    chLessThan      = 0x3C;
    constexpr   tCIDLib::TCh    chEquals        = 0x3D;
    constexpr   tCIDLib::TCh    chGreaterThan   = 0x3E;
    constexpr   tCIDLib::TCh    chQuestionMark  = 0x3F;
    constexpr   tCIDLib::TCh    chAtSign        = 0x40;
    constexpr   tCIDLib::TCh    chOpenBracket   = 0x5B;
    constexpr   tCIDLib::TCh    chBackSlash     = 0x5C;
    constexpr   tCIDLib::TCh    chCloseBracket  = 0x5D;
    constexpr   tCIDLib::TCh    chCircumflex    = 0x5E; // aka carat
    constexpr   tCIDLib::TCh    chUnderscore    = 0x5F;
    constexpr   tCIDLib::TCh    chGrave         = 0x60;
    constexpr   tCIDLib::TCh    chOpenBrace     = 0x7B;
    constexpr   tCIDLib::TCh    chVerticalBar   = 0x7C;
    constexpr   tCIDLib::TCh    chCloseBrace    = 0x7D;
    constexpr   tCIDLib::TCh    chTilde         = 0x7E;


    // Capital Latin letters
    constexpr   tCIDLib::TCh    chLatin_A       = 0x41;
    constexpr   tCIDLib::TCh    chLatin_B       = 0x42;
    constexpr   tCIDLib::TCh    chLatin_C       = 0x43;
    constexpr   tCIDLib::TCh    chLatin_D       = 0x44;
    constexpr   tCIDLib::TCh    chLatin_E       = 0x45;
    constexpr   tCIDLib::TCh    chLatin_F       = 0x46;
    constexpr   tCIDLib::TCh    chLatin_G       = 0x47;
    constexpr   tCIDLib::TCh    chLatin_H       = 0x48;
    constexpr   tCIDLib::TCh    chLatin_I       = 0x49;
    constexpr   tCIDLib::TCh    chLatin_J       = 0x4A;
    constexpr   tCIDLib::TCh    chLatin_K       = 0x4B;
    constexpr   tCIDLib::TCh    chLatin_L       = 0x4C;
    constexpr   tCIDLib::TCh    chLatin_M       = 0x4D;
    constexpr   tCIDLib::TCh    chLatin_N       = 0x4E;
    constexpr   tCIDLib::TCh    chLatin_O       = 0x4F;
    constexpr   tCIDLib::TCh    chLatin_P       = 0x50;
    constexpr   tCIDLib::TCh    chLatin_Q       = 0x51;
    constexpr   tCIDLib::TCh    chLatin_R       = 0x52;
    constexpr   tCIDLib::TCh    chLatin_S       = 0x53;
    constexpr   tCIDLib::TCh    chLatin_T       = 0x54;
    constexpr   tCIDLib::TCh    chLatin_U       = 0x55;
    constexpr   tCIDLib::TCh    chLatin_V       = 0x56;
    constexpr   tCIDLib::TCh    chLatin_W       = 0x57;
    constexpr   tCIDLib::TCh    chLatin_X       = 0x58;
    constexpr   tCIDLib::TCh    chLatin_Y       = 0x59;
    constexpr   tCIDLib::TCh    chLatin_Z       = 0x5A;

    // Lower case Latin letters
    constexpr   tCIDLib::TCh    chLatin_a       = 0x61;
    constexpr   tCIDLib::TCh    chLatin_b       = 0x62;
    constexpr   tCIDLib::TCh    chLatin_c       = 0x63;
    constexpr   tCIDLib::TCh    chLatin_d       = 0x64;
    constexpr   tCIDLib::TCh    chLatin_e       = 0x65;
    constexpr   tCIDLib::TCh    chLatin_f       = 0x66;
    constexpr   tCIDLib::TCh    chLatin_g       = 0x67;
    constexpr   tCIDLib::TCh    chLatin_h       = 0x68;
    constexpr   tCIDLib::TCh    chLatin_i       = 0x69;
    constexpr   tCIDLib::TCh    chLatin_j       = 0x6A;
    constexpr   tCIDLib::TCh    chLatin_k       = 0x6B;
    constexpr   tCIDLib::TCh    chLatin_l       = 0x6C;
    constexpr   tCIDLib::TCh    chLatin_m       = 0x6D;
    constexpr   tCIDLib::TCh    chLatin_n       = 0x6E;
    constexpr   tCIDLib::TCh    chLatin_o       = 0x6F;
    constexpr   tCIDLib::TCh    chLatin_p       = 0x70;
    constexpr   tCIDLib::TCh    chLatin_q       = 0x71;
    constexpr   tCIDLib::TCh    chLatin_r       = 0x72;
    constexpr   tCIDLib::TCh    chLatin_s       = 0x73;
    constexpr   tCIDLib::TCh    chLatin_t       = 0x74;
    constexpr   tCIDLib::TCh    chLatin_u       = 0x75;
    constexpr   tCIDLib::TCh    chLatin_v       = 0x76;
    constexpr   tCIDLib::TCh    chLatin_w       = 0x77;
    constexpr   tCIDLib::TCh    chLatin_x       = 0x78;
    constexpr   tCIDLib::TCh    chLatin_y       = 0x79;
    constexpr   tCIDLib::TCh    chLatin_z       = 0x7A;

    // Arabic digits
    constexpr   tCIDLib::TCh    chDigit0        = 0x30;
    constexpr   tCIDLib::TCh    chDigit1        = 0x31;
    constexpr   tCIDLib::TCh    chDigit2        = 0x32;
    constexpr   tCIDLib::TCh    chDigit3        = 0x33;
    constexpr   tCIDLib::TCh    chDigit4        = 0x34;
    constexpr   tCIDLib::TCh    chDigit5        = 0x35;
    constexpr   tCIDLib::TCh    chDigit6        = 0x36;
    constexpr   tCIDLib::TCh    chDigit7        = 0x37;
    constexpr   tCIDLib::TCh    chDigit8        = 0x38;
    constexpr   tCIDLib::TCh    chDigit9        = 0x39;


    // Greek capital letters
    constexpr   tCIDLib::TCh    chGreek_Alpha   = 0x0391;
    constexpr   tCIDLib::TCh    chGreek_Beta    = 0x0392;
    constexpr   tCIDLib::TCh    chGreek_Gamma   = 0x0393;
    constexpr   tCIDLib::TCh    chGreek_Delta   = 0x0394;
    constexpr   tCIDLib::TCh    chGreek_Epsilon = 0x0395;
    constexpr   tCIDLib::TCh    chGreek_Zeta    = 0x0396;
    constexpr   tCIDLib::TCh    chGreek_Eta     = 0x0397;
    constexpr   tCIDLib::TCh    chGreek_Theta   = 0x0398;
    constexpr   tCIDLib::TCh    chGreek_Iota    = 0x0399;
    constexpr   tCIDLib::TCh    chGreek_Kappa   = 0x039A;
    constexpr   tCIDLib::TCh    chGreek_Lambda  = 0x039B;
    constexpr   tCIDLib::TCh    chGreek_Mu      = 0x039C;
    constexpr   tCIDLib::TCh    chGreek_Nu      = 0x039D;
    constexpr   tCIDLib::TCh    chGreek_Xi      = 0x039E;
    constexpr   tCIDLib::TCh    chGreek_Omicron = 0x039F;
    constexpr   tCIDLib::TCh    chGreek_Pi      = 0x03A0;
    constexpr   tCIDLib::TCh    chGreek_Rho     = 0x03A1;
    constexpr   tCIDLib::TCh    chGreek_Sigma   = 0x03A3;
    constexpr   tCIDLib::TCh    chGreek_Tau     = 0x03A4;
    constexpr   tCIDLib::TCh    chGreek_Upsilon = 0x03A5;
    constexpr   tCIDLib::TCh    chGreek_Phi     = 0x03A6;
    constexpr   tCIDLib::TCh    chGreek_Chi     = 0x03A7;
    constexpr   tCIDLib::TCh    chGreek_Psi     = 0x03A8;
    constexpr   tCIDLib::TCh    chGreek_Omega   = 0x03A9;

    // Greek lower case letters
    constexpr   tCIDLib::TCh    chGreek_alpha   = 0x03B1;
    constexpr   tCIDLib::TCh    chGreek_beta    = 0x03B2;
    constexpr   tCIDLib::TCh    chGreek_gamma   = 0x03B3;
    constexpr   tCIDLib::TCh    chGreek_delta   = 0x03B4;
    constexpr   tCIDLib::TCh    chGreek_epsilon = 0x03B5;
    constexpr   tCIDLib::TCh    chGreek_zeta    = 0x03B6;
    constexpr   tCIDLib::TCh    chGreek_eta     = 0x03B7;
    constexpr   tCIDLib::TCh    chGreek_theta   = 0x03B8;
    constexpr   tCIDLib::TCh    chGreek_iota    = 0x03B9;
    constexpr   tCIDLib::TCh    chGreek_kappa   = 0x03BA;
    constexpr   tCIDLib::TCh    chGreek_lambda  = 0x03BB;
    constexpr   tCIDLib::TCh    chGreek_mu      = 0x03BC;
    constexpr   tCIDLib::TCh    chGreek_nu      = 0x03BD;
    constexpr   tCIDLib::TCh    chGreek_xi      = 0x03BE;
    constexpr   tCIDLib::TCh    chGreek_omicron = 0x03BF;
    constexpr   tCIDLib::TCh    chGreek_pi      = 0x03C0;
    constexpr   tCIDLib::TCh    chGreek_rho     = 0x03C1;
    constexpr   tCIDLib::TCh    chGreek_sigma   = 0x03C3;
    constexpr   tCIDLib::TCh    chGreek_tau     = 0x03C4;
    constexpr   tCIDLib::TCh    chGreek_upsilon = 0x03C5;
    constexpr   tCIDLib::TCh    chGreek_phi     = 0x03C6;
    constexpr   tCIDLib::TCh    chGreek_chi     = 0x03C7;
    constexpr   tCIDLib::TCh    chGreek_psi     = 0x03C8;
    constexpr   tCIDLib::TCh    chGreek_omega   = 0x03C9;


    //
    //  These are defined formatting characters.
    //
    constexpr   tCIDLib::TCh    chLineSeparator = 0x2028;
    constexpr   tCIDLib::TCh    chParaSeparator = 0x2029;

    //
    //  These are special characters used to indicate special circumstances
    //  to readers/writes of Unicode text files.
    //
    constexpr   tCIDLib::TCh    chReplacement   = 0xFFFD;
    constexpr   tCIDLib::TCh    chInvalid       = 0xFFFF;
    constexpr   tCIDLib::TCh    chUniBOM        = 0xFEFF;
    constexpr   tCIDLib::TCh    chSwappedUniBOM = 0xFFFE;
}
