//
// FILE NAME: CIDKernel_Unicode.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/06/1996
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
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
    const   tCIDLib::TCh    chNull          = 0x00;
    const   tCIDLib::TCh    chBell          = 0x07;
    const   tCIDLib::TCh    chBS            = 0x08;
    const   tCIDLib::TCh    chTab           = 0x09;
    const   tCIDLib::TCh    chLF            = 0x0A;
    const   tCIDLib::TCh    chVTab          = 0x0B;
    const   tCIDLib::TCh    chFF            = 0x0C;
    const   tCIDLib::TCh    chCR            = 0x0D;
    const   tCIDLib::TCh    chEOF           = 0x1A;
    const   tCIDLib::TCh    chEsc           = 0x1B;
    const   tCIDLib::TCh    chSpace         = 0x20;
    const   tCIDLib::TCh    chDel           = 0x7F;

    // Standard punctuation stuff
    const   tCIDLib::TCh    chExclamation   = 0x21;
    const   tCIDLib::TCh    chQuotation     = 0x22;
    const   tCIDLib::TCh    chPoundSign     = 0x23;
    const   tCIDLib::TCh    chDollarSign    = 0x24;
    const   tCIDLib::TCh    chPercentSign   = 0x25;
    const   tCIDLib::TCh    chAmpersand     = 0x26;
    const   tCIDLib::TCh    chApostrophe    = 0x27;
    const   tCIDLib::TCh    chOpenParen     = 0x28;
    const   tCIDLib::TCh    chCloseParen    = 0x29;
    const   tCIDLib::TCh    chAsterisk      = 0x2A;
    const   tCIDLib::TCh    chPlusSign      = 0x2B;
    const   tCIDLib::TCh    chComma         = 0x2C;
    const   tCIDLib::TCh    chHyphenMinus   = 0x2D;
    const   tCIDLib::TCh    chPeriod        = 0x2E;
    const   tCIDLib::TCh    chForwardSlash  = 0x2F;
    const   tCIDLib::TCh    chColon         = 0x3A;
    const   tCIDLib::TCh    chSemiColon     = 0x3B;
    const   tCIDLib::TCh    chLessThan      = 0x3C;
    const   tCIDLib::TCh    chEquals        = 0x3D;
    const   tCIDLib::TCh    chGreaterThan   = 0x3E;
    const   tCIDLib::TCh    chQuestionMark  = 0x3F;
    const   tCIDLib::TCh    chAtSign        = 0x40;
    const   tCIDLib::TCh    chOpenBracket   = 0x5B;
    const   tCIDLib::TCh    chBackSlash     = 0x5C;
    const   tCIDLib::TCh    chCloseBracket  = 0x5D;
    const   tCIDLib::TCh    chCircumflex    = 0x5E; // aka carat
    const   tCIDLib::TCh    chUnderscore    = 0x5F;
    const   tCIDLib::TCh    chGrave         = 0x60;
    const   tCIDLib::TCh    chOpenBrace     = 0x7B;
    const   tCIDLib::TCh    chVerticalBar   = 0x7C;
    const   tCIDLib::TCh    chCloseBrace    = 0x7D;
    const   tCIDLib::TCh    chTilde         = 0x7E;


    // Capital Latin letters
    const   tCIDLib::TCh    chLatin_A       = 0x41;
    const   tCIDLib::TCh    chLatin_B       = 0x42;
    const   tCIDLib::TCh    chLatin_C       = 0x43;
    const   tCIDLib::TCh    chLatin_D       = 0x44;
    const   tCIDLib::TCh    chLatin_E       = 0x45;
    const   tCIDLib::TCh    chLatin_F       = 0x46;
    const   tCIDLib::TCh    chLatin_G       = 0x47;
    const   tCIDLib::TCh    chLatin_H       = 0x48;
    const   tCIDLib::TCh    chLatin_I       = 0x49;
    const   tCIDLib::TCh    chLatin_J       = 0x4A;
    const   tCIDLib::TCh    chLatin_K       = 0x4B;
    const   tCIDLib::TCh    chLatin_L       = 0x4C;
    const   tCIDLib::TCh    chLatin_M       = 0x4D;
    const   tCIDLib::TCh    chLatin_N       = 0x4E;
    const   tCIDLib::TCh    chLatin_O       = 0x4F;
    const   tCIDLib::TCh    chLatin_P       = 0x50;
    const   tCIDLib::TCh    chLatin_Q       = 0x51;
    const   tCIDLib::TCh    chLatin_R       = 0x52;
    const   tCIDLib::TCh    chLatin_S       = 0x53;
    const   tCIDLib::TCh    chLatin_T       = 0x54;
    const   tCIDLib::TCh    chLatin_U       = 0x55;
    const   tCIDLib::TCh    chLatin_V       = 0x56;
    const   tCIDLib::TCh    chLatin_W       = 0x57;
    const   tCIDLib::TCh    chLatin_X       = 0x58;
    const   tCIDLib::TCh    chLatin_Y       = 0x59;
    const   tCIDLib::TCh    chLatin_Z       = 0x5A;

    // Lower case Latin letters
    const   tCIDLib::TCh    chLatin_a       = 0x61;
    const   tCIDLib::TCh    chLatin_b       = 0x62;
    const   tCIDLib::TCh    chLatin_c       = 0x63;
    const   tCIDLib::TCh    chLatin_d       = 0x64;
    const   tCIDLib::TCh    chLatin_e       = 0x65;
    const   tCIDLib::TCh    chLatin_f       = 0x66;
    const   tCIDLib::TCh    chLatin_g       = 0x67;
    const   tCIDLib::TCh    chLatin_h       = 0x68;
    const   tCIDLib::TCh    chLatin_i       = 0x69;
    const   tCIDLib::TCh    chLatin_j       = 0x6A;
    const   tCIDLib::TCh    chLatin_k       = 0x6B;
    const   tCIDLib::TCh    chLatin_l       = 0x6C;
    const   tCIDLib::TCh    chLatin_m       = 0x6D;
    const   tCIDLib::TCh    chLatin_n       = 0x6E;
    const   tCIDLib::TCh    chLatin_o       = 0x6F;
    const   tCIDLib::TCh    chLatin_p       = 0x70;
    const   tCIDLib::TCh    chLatin_q       = 0x71;
    const   tCIDLib::TCh    chLatin_r       = 0x72;
    const   tCIDLib::TCh    chLatin_s       = 0x73;
    const   tCIDLib::TCh    chLatin_t       = 0x74;
    const   tCIDLib::TCh    chLatin_u       = 0x75;
    const   tCIDLib::TCh    chLatin_v       = 0x76;
    const   tCIDLib::TCh    chLatin_w       = 0x77;
    const   tCIDLib::TCh    chLatin_x       = 0x78;
    const   tCIDLib::TCh    chLatin_y       = 0x79;
    const   tCIDLib::TCh    chLatin_z       = 0x7A;

    // Arabic digits
    const   tCIDLib::TCh    chDigit0        = 0x30;
    const   tCIDLib::TCh    chDigit1        = 0x31;
    const   tCIDLib::TCh    chDigit2        = 0x32;
    const   tCIDLib::TCh    chDigit3        = 0x33;
    const   tCIDLib::TCh    chDigit4        = 0x34;
    const   tCIDLib::TCh    chDigit5        = 0x35;
    const   tCIDLib::TCh    chDigit6        = 0x36;
    const   tCIDLib::TCh    chDigit7        = 0x37;
    const   tCIDLib::TCh    chDigit8        = 0x38;
    const   tCIDLib::TCh    chDigit9        = 0x39;


    // Greek capital letters
    const   tCIDLib::TCh    chGreek_Alpha   = 0x0391;
    const   tCIDLib::TCh    chGreek_Beta    = 0x0392;
    const   tCIDLib::TCh    chGreek_Gamma   = 0x0393;
    const   tCIDLib::TCh    chGreek_Delta   = 0x0394;
    const   tCIDLib::TCh    chGreek_Epsilon = 0x0395;
    const   tCIDLib::TCh    chGreek_Zeta    = 0x0396;
    const   tCIDLib::TCh    chGreek_Eta     = 0x0397;
    const   tCIDLib::TCh    chGreek_Theta   = 0x0398;
    const   tCIDLib::TCh    chGreek_Iota    = 0x0399;
    const   tCIDLib::TCh    chGreek_Kappa   = 0x039A;
    const   tCIDLib::TCh    chGreek_Lambda  = 0x039B;
    const   tCIDLib::TCh    chGreek_Mu      = 0x039C;
    const   tCIDLib::TCh    chGreek_Nu      = 0x039D;
    const   tCIDLib::TCh    chGreek_Xi      = 0x039E;
    const   tCIDLib::TCh    chGreek_Omicron = 0x039F;
    const   tCIDLib::TCh    chGreek_Pi      = 0x03A0;
    const   tCIDLib::TCh    chGreek_Rho     = 0x03A1;
    const   tCIDLib::TCh    chGreek_Sigma   = 0x03A3;
    const   tCIDLib::TCh    chGreek_Tau     = 0x03A4;
    const   tCIDLib::TCh    chGreek_Upsilon = 0x03A5;
    const   tCIDLib::TCh    chGreek_Phi     = 0x03A6;
    const   tCIDLib::TCh    chGreek_Chi     = 0x03A7;
    const   tCIDLib::TCh    chGreek_Psi     = 0x03A8;
    const   tCIDLib::TCh    chGreek_Omega   = 0x03A9;

    // Greek lower case letters
    const   tCIDLib::TCh    chGreek_alpha   = 0x03B1;
    const   tCIDLib::TCh    chGreek_beta    = 0x03B2;
    const   tCIDLib::TCh    chGreek_gamma   = 0x03B3;
    const   tCIDLib::TCh    chGreek_delta   = 0x03B4;
    const   tCIDLib::TCh    chGreek_epsilon = 0x03B5;
    const   tCIDLib::TCh    chGreek_zeta    = 0x03B6;
    const   tCIDLib::TCh    chGreek_eta     = 0x03B7;
    const   tCIDLib::TCh    chGreek_theta   = 0x03B8;
    const   tCIDLib::TCh    chGreek_iota    = 0x03B9;
    const   tCIDLib::TCh    chGreek_kappa   = 0x03BA;
    const   tCIDLib::TCh    chGreek_lambda  = 0x03BB;
    const   tCIDLib::TCh    chGreek_mu      = 0x03BC;
    const   tCIDLib::TCh    chGreek_nu      = 0x03BD;
    const   tCIDLib::TCh    chGreek_xi      = 0x03BE;
    const   tCIDLib::TCh    chGreek_omicron = 0x03BF;
    const   tCIDLib::TCh    chGreek_pi      = 0x03C0;
    const   tCIDLib::TCh    chGreek_rho     = 0x03C1;
    const   tCIDLib::TCh    chGreek_sigma   = 0x03C3;
    const   tCIDLib::TCh    chGreek_tau     = 0x03C4;
    const   tCIDLib::TCh    chGreek_upsilon = 0x03C5;
    const   tCIDLib::TCh    chGreek_phi     = 0x03C6;
    const   tCIDLib::TCh    chGreek_chi     = 0x03C7;
    const   tCIDLib::TCh    chGreek_psi     = 0x03C8;
    const   tCIDLib::TCh    chGreek_omega   = 0x03C9;


    //
    //  These are defined formatting characters.
    //
    const   tCIDLib::TCh    chLineSeparator = 0x2028;
    const   tCIDLib::TCh    chParaSeparator = 0x2029;

    //
    //  These are special characters used to indicate special circumstances
    //  to readers/writes of Unicode text files.
    //
    const   tCIDLib::TCh    chReplacement   = 0xFFFD;
    const   tCIDLib::TCh    chInvalid       = 0xFFFF;
    const   tCIDLib::TCh    chUniBOM        = 0xFEFF;
    const   tCIDLib::TCh    chSwappedUniBOM = 0xFFFE;
}
