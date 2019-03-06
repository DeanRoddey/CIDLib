//
// FILE NAME: CIDXML_CharFlags.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/25/1999
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header file for the CIDXML_CharFlags.Cpp file, which
//  implements the TXMLCharFlags class. This class provides a number of very
//  key static methods that are used to check whether characters meet this
//  or that criteria. In XML, the types of chars that are legal at any one
//  point are strictly defined. Since it is too slow to do range compares
//  or individual character comparisons.
//
//  Instead, a 64K table is built. Each entry contains bit flags that
//  indicate the characteristics of that particular character. So a check
//  for a character just comes down to an index operation followed by a bit
//  wide logical op in most cases.
//
//  The table is precreated and hard coded into the program, in order to
//  avoid the overhead of creating it every time.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)


// ---------------------------------------------------------------------------
//  CLASS: TXMLCharFlags
// ---------------------------------------------------------------------------
class CIDXMLEXP TXMLCharFlags
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TBoolean bIsFirstNameChar
        (
            const   tCIDLib::TCh            chToCheck
        );

        static tCIDLib::TBoolean bIsMarkupTestChar
        (
            const   tCIDLib::TCh            chToCheck
        );

        static tCIDLib::TBoolean bIsNameChar
        (
            const   tCIDLib::TCh            chToCheck
        );

        static tCIDLib::TBoolean bIsPublicIDChar
        (
            const   tCIDLib::TCh            chToCheck
        );

        static tCIDLib::TBoolean bIsSpace
        (
            const   tCIDLib::TCh            chToCheck
        );

        static tCIDLib::TBoolean bIsSpecialCharData
        (
            const   tCIDLib::TCh            chToCheck
        );

        static tCIDLib::TBoolean bIsStartTagChar
        (
            const   tCIDLib::TCh            chToCheck
        );

        static tCIDLib::TBoolean bIsXMLChar
        (
            const   tCIDLib::TCh            chToCheck
        );



    private :
        // -------------------------------------------------------------------
        //  Static data members
        //
        //  s_ac1CharFlags
        //      This is the character flags array. Its contents are hard
        //      coded into the Cpp flag in the declaration. But it has to
        //      be visible here by name in order to allow for inline access
        //      below. These methods are called A LOT so inlining is very
        //      important.
        // -------------------------------------------------------------------
        static const tCIDLib::TCard1    s_ac1CharFlags[0x10000];


        // -------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // -------------------------------------------------------------------
        TXMLCharFlags();
        TXMLCharFlags(const TXMLCharFlags&);
        tCIDLib::TVoid operator=(const TXMLCharFlags&);
};

#pragma CIDLIB_POPPACK


// ---------------------------------------------------------------------------
//  TXMLCharFlags: Public static methods
// ---------------------------------------------------------------------------
inline tCIDLib::TBoolean
TXMLCharFlags::bIsFirstNameChar(const tCIDLib::TCh chToCheck)
{
    static const tCIDLib::TCard1 c1NameMask = kCIDXML::c1BaseChar
                                              | kCIDXML::c1LetterChar;

    // Do the obvious check first and return success if ok
    if ((s_ac1CharFlags[chToCheck] & c1NameMask) != 0)
        return kCIDLib::True;

    // Check the two special case chars
    return ((chToCheck == kCIDLib::chUnderscore) || (chToCheck == kCIDLib::chColon));
}

inline tCIDLib::TBoolean
TXMLCharFlags::bIsMarkupTestChar(const tCIDLib::TCh chToCheck)
{
    return (s_ac1CharFlags[chToCheck] & kCIDXML::c1MarkupTestChar) != 0;
}

inline tCIDLib::TBoolean TXMLCharFlags::bIsNameChar(const tCIDLib::TCh chToCheck)
{
    return (s_ac1CharFlags[chToCheck] & kCIDXML::c1NameChar) != 0;
}

inline tCIDLib::TBoolean TXMLCharFlags::bIsSpace(const tCIDLib::TCh chToCheck)
{
    return (s_ac1CharFlags[chToCheck] & kCIDXML::c1WhitespaceChar) != 0;
}

inline tCIDLib::TBoolean
TXMLCharFlags::bIsSpecialCharData(const tCIDLib::TCh chToCheck)
{
    return (s_ac1CharFlags[chToCheck] & kCIDXML::c1SpecialCharData) != 0;
}

inline tCIDLib::TBoolean
TXMLCharFlags::bIsStartTagChar(const tCIDLib::TCh chToCheck)
{
    return (s_ac1CharFlags[chToCheck] & kCIDXML::c1StartTagChar) != 0;
}

inline tCIDLib::TBoolean TXMLCharFlags::bIsXMLChar(const tCIDLib::TCh chToCheck)
{
    return (s_ac1CharFlags[chToCheck] & kCIDXML::c1XMLChar) != 0;
}

