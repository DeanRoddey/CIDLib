//
// FILE NAME: CIDBuild_ResCompiler_Menu.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/10/2000
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
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDBuild.hpp"
#include    <memory.h>


enum EBaseMenuFlds
{
    EBaseMenuFld_Symbol
    , EBaseMenuFld_TextSym
    , EBaseMenuFlds_Count
    , EBaseMenuFlds_Min     = 0
};
EnumIncMacro(EBaseMenuFlds)



// ---------------------------------------------------------------------------
//  TResCompiler: Private, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TCard1*
TResCompiler::pc1ParseMenuItemFlds( tCIDLib::TCard1* const  pc1Start
                                    , TBinFile&             bflBinOut
                                    , TTextFile&            tflResHpp)
{
    static tCIDLib::TBoolean bInitDone = kCIDLib::False;
    static TBlkField ablkfInfo[EBaseMenuFlds_Count] =
    {
        TBlkField(L"SYMBOL"         , kCIDLib::True , 2, 2 )
        , TBlkField(L"TEXTSYM"      , kCIDLib::True , 1, 1 )
    };

    //
    //  If this is the first time through, then do some more init. We have
    //  to setup up the types of the various values
    //
    if (!bInitDone)
    {
        ablkfInfo[EBaseMenuFld_Symbol].eValType(0, TBlkField::EValTypes::CppName);
        ablkfInfo[EBaseMenuFld_Symbol].eValType(1, TBlkField::EValTypes::Cardinal);
        ablkfInfo[EBaseMenuFld_TextSym].eValType(0, TBlkField::EValTypes::CppName);

        // And indicate that we've done this init
        bInitDone = kCIDLib::True;
    }

    // Use the block parser class to do the grunt work parsing
    TBlkParser blkpInfo
    (
        ablkfInfo
        , EBaseMenuFlds_Count
        , m_plsplInput
        , L"END ITEM"
    );
    blkpInfo.Parse();

    //
    //  The return pointer will be the start, plus the size of the menu item
    //  itself, plus the text size. So do what we know now and we'll add the
    //  text size when we see it.
    //
    tCIDLib::TCard1* pc1Ret = pc1Start + sizeof(tCIDCtrls::TRawMenuItem);

    // Get our pointer to our menu item, and set the type
    tCIDCtrls::TRawMenuItem* prmiCur = reinterpret_cast<tCIDCtrls::TRawMenuItem*>(pc1Start);
    prmiCur->eType = tCIDCtrls::EMenuItemTypes::ActionItem;

    for (EBaseMenuFlds eFld = EBaseMenuFlds_Min; eFld < EBaseMenuFlds_Count; eFld++)
    {
        const TBlkField& blkfCur = ablkfInfo[eFld];

        // If this one wasn't provided, then try again
        if (!blkfCur.bFound())
            continue;

        if (eFld == EBaseMenuFld_Symbol)
        {
            // Write out the the symbol to the header, if needed
            if (m_bGenHeaders)
            {
                tflResHpp   << L"    constexpr tCIDLib::TCard4  "
                            << blkfCur.strField(0)
                            << L" = " << blkfCur.strField(1) << L";\n";
            }

            // Store the numeric command value in the item
            prmiCur->c4Id = blkfCur.strField(1).c4AsCard();
        }
         else if (eFld == EBaseMenuFld_TextSym)
        {
            //
            //  Its the message id for the text of the title bar. So get the
            //  symbol name out and look it up in the message id info. If its
            //  there, then save the message id.
            //
            if (!bFindMsgSymbol(blkfCur.strField(0).pszBuffer(), prmiCur->midText))
            {
                stdOut  << L"(Line " << blkfCur.c4LineAt()
                        << L") Text id '" << blkfCur.strField(0)
                        << L"' is not defined for project "
                        << m_projiSrc.strProjectName() << kCIDBuild::EndLn;
                throw tCIDBuild::EErrors::FileFormat;
            }
        }
         else
        {
            stdOut  << tCIDLib::TInt4(eFld)
                    << L" is not a valid menu field type" << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::Internal;
        }
    }

    return pc1Ret;
}


tCIDLib::TCard1*
TResCompiler::pc1ParseSubMenu(          tCIDCtrls::TRawMenuDesc&  rmdToFill
                                , const tCIDLib::TCard1* const  pc1End
                                ,       tCIDLib::TCard1* const  pc1Start
                                ,       TBinFile&               bflBinOut
                                ,       TTextFile&              tflResHpp)
{
    const tCIDLib::TCard4 c4MaxFlds = 2;
    TBldStr strReadBuf;
    TBldStr astrFlds[c4MaxFlds];

    // If we max out, then throw
    if (pc1Start + sizeof(tCIDCtrls::TRawMenuItem) >= pc1End)
    {
        stdOut  << L"(Line " << m_plsplInput->c4CurLine()
                    << L") Menu has too manu items" << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::Full;
    }

    // Remember our index, which we'll need at the below
    const tCIDLib::TCard2 c2OurIndex = rmdToFill.c2Count;

    //
    //  Look at the current pointer as a menu item. This is what we'll keep
    //  updated for what we find at this level. Set our type to submenu.
    //
    tCIDCtrls::TRawMenuItem* prmiSub
                        = reinterpret_cast<tCIDCtrls::TRawMenuItem*>(pc1Start);
    prmiSub->eType = tCIDCtrls::EMenuItemTypes::SubMenu;

    //
    //  And also start our return pointer at the same point. We'll bump this
    //  up as we go, so that it'll point to the byte after our last used byte
    //  when we are done (i.e. where the next thing would start if any.)
    //
    tCIDLib::TCard1* pc1Ret = pc1Start;

    //
    //  If our index is zero, then we have no symbol stuff since this is
    //  the top level menu. Else we have to process the symbol and text
    //  symbol. But we still make space for it, because we need a magic
    //  sub-menu item to hold the top level items.
    //
    if (c2OurIndex != 0)
    {
        // Pull out the symbol information and store it
        if (!m_plsplInput->bReadLine(strReadBuf))
        {
            stdOut  << L"(Line " << m_plsplInput->c4CurLine()
                    << L") Expected SYMBOL=" << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::UnexpectedEOF;
        }

        if (!strReadBuf.bStartsWith(L"SYMBOL="))
        {
            stdOut  << L"(Line " << m_plsplInput->c4CurLine()
                    << L") Expected SYMBOL= first in sub-menu definition"
                    << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::FileFormat;
        }
        strReadBuf.Cut(7);

        // Pull out the two fields
        if (c4SplitFields(strReadBuf, astrFlds, 2, m_plsplInput->c4CurLine()) != 2)
        {
            stdOut  << L"(Line " << m_plsplInput->c4CurLine()
                    << L") Expected symbol name and resource id" << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::FileFormat;
        }

        // Write out the the symbol to the header, if needed
        if (m_bGenHeaders)
        {
            tflResHpp   << L"    constexpr tCIDLib::TCard4  "
                        << astrFlds[0]
                        << L" = " << astrFlds[1] << L";\n";
        }

        //
        //  Ok, now move the return value up past our item, and put the
        //  command value into the item.
        //
        pc1Ret += sizeof(tCIDCtrls::TRawMenuItem);
        prmiSub->c4Id = astrFlds[1].c4AsCard();

        // Pull out the text symbol information and store it
        if (!m_plsplInput->bReadLine(strReadBuf))
        {
            stdOut  << L"(Line " << m_plsplInput->c4CurLine()
                    << L") Expected TEXTSYM=" << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::UnexpectedEOF;
        }

        if (!strReadBuf.bStartsWith(L"TEXTSYM="))
        {
            stdOut  << L"(Line " << m_plsplInput->c4CurLine()
                    << L") Expected TEXTSYM="
                    << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::FileFormat;
        }
        strReadBuf.Cut(8);

        // Try to find this text symbol and store it
        if (!bFindMsgSymbol(strReadBuf.pszBuffer(), prmiSub->midText))
        {
            stdOut  << L"(Line " << m_plsplInput->c4CurLine()
                    << L") Text id '" << strReadBuf
                    << L"' is not defined for project "
                    << m_projiSrc.strProjectName() << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::FileFormat;
        }
    }
     else
    {
        pc1Ret += sizeof(tCIDCtrls::TRawMenuItem);
    }

    tCIDCtrls::TRawMenuItem* prmiCur;
    while (true)
    {
        if (!m_plsplInput->bReadLine(strReadBuf))
        {
            stdOut  << L"(Line " << m_plsplInput->c4CurLine()
                    << L") Expected menu entry or END MENU" << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::UnexpectedEOF;
        }

        // Watch for the end of this block
        if (!c2OurIndex)
        {
            if (strReadBuf == L"END MENU")
                break;
        }
         else
        {
            if (strReadBuf == L"END SUBMENU")
                break;
        }

        // Bump the total count of items
        rmdToFill.c2Count++;

        //
        //  Ok we will see the start of either a separator, menu item, or
        //  submenu at this point.
        //
        if (strReadBuf == L"SEPARATOR")
        {
            // And get a pointer to new item and set the type and sibling offset
            prmiCur = reinterpret_cast<tCIDCtrls::TRawMenuItem*>(pc1Ret);
            prmiCur->eType = tCIDCtrls::EMenuItemTypes::Decoration;

            //
            //  Calc and store the next offset. Its not really required for
            //  later decoding, but it helps for debugging. In this case its
            //  just a raw menu item, since there's no trailing notification
            //  text.
            //
            prmiCur->c2NextOfs = sizeof(tCIDCtrls::TRawMenuItem);

            // We have no text, so bump the current pointer by one item
            pc1Ret += prmiCur->c2NextOfs;
        }
         else if (strReadBuf == L"SUBMENU=")
        {
            //
            //  Recurse to parse this submenu. We get back the pointer just
            //  past the end of the sub-content, which is where we pick back
            //  up again.
            //
            pc1Ret = pc1ParseSubMenu(rmdToFill, pc1End, pc1Ret, bflBinOut, tflResHpp);
        }
         else if (strReadBuf == L"ITEM=")
        {
            // And parse out the standard menu fields into the new entry
            pc1Ret = pc1ParseMenuItemFlds(pc1Ret, bflBinOut, tflResHpp);
        }
         else
        {
            stdOut  << L"(Line " << m_plsplInput->c4CurLine()
                    << L") Expected SEPARATOR=, SUBMENU=, ITEM="
                       L" or TEXTSYM=" << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::FileFormat;
        }

        // Bump up the count of children for this submenu
        prmiSub->c2SubCount++;

        // Watch for buffer overflow
        if (pc1Ret >= pc1End)
        {
            stdOut  << L"(Line " << m_plsplInput->c4CurLine()
                        << L") Menu has too manu items" << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::Full;
        }
    }

    // Store the offset to the next sibling
    prmiSub->c2NextOfs = pc1Ret - pc1Start;

    // Return the buffer pointer where we left it
    return pc1Ret;
}


tCIDLib::TVoid
TResCompiler::ProcessMenu(TBinFile& bflBinOut, TTextFile& tflResHpp)
{
    //
    //  Create a buffer to write into. We can't know ahead of time how long
    //  it will need to be, but its so completely unlikely to be longer than
    //  this that we'll use a buffer of that size. Any menu that overflowed
    //  this buffer would be grotesquely large.
    //
    const tCIDLib::TCard4   c4BufSz = 1024 * 16;
    tCIDLib::TCard1         ac1Buf[c4BufSz];

    // Calc the end of the buffer, so we can watch for overflow
    const tCIDLib::TCard1*  pc1End = ac1Buf + c4BufSz;

    // Zero it out so unused fields are all zero
    memset(ac1Buf, 0, c4BufSz);

    // We look at the buffer via the raw menu description structure
    tCIDCtrls::TRawMenuDesc* prmdFill = reinterpret_cast<tCIDCtrls::TRawMenuDesc*>(ac1Buf);
    prmdFill->c2Count = 0;

    // Create a pointer we can work up through the buffer as we go
    tCIDLib::TCard1* pc1Cur
                    = reinterpret_cast<tCIDLib::TCard1*>(&prmdFill->armiList[0]);

    // The resource item map that we'll fill in here
    tCIDLib::TResItemMap rimCur = {0};
    rimCur.eType = tCIDLib::EResTypes::Menu;
    rimCur.c4At = bflBinOut.c4CurPos();

    //
    //  The first thing we must see is the SYMBOL field. So check for that
    //  and process it if found.
    //
    TBldStr strReadBuf;
    if (!m_plsplInput->bReadLine(strReadBuf))
    {
        stdOut  << L"(Line " << m_plsplInput->c4CurLine()
                << L") Expected SYMBOL= first in menu definition" << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::UnexpectedEOF;
    }

    if (!strReadBuf.bStartsWith(L"SYMBOL="))
    {
        stdOut  << L"(Line " << m_plsplInput->c4CurLine()
                << L") Expected SYMBOL= first in menu definition" << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::FileFormat;
    }
    strReadBuf.Cut(7);

    // Pull out the two fields
    TBldStr astrFlds[2];
    if (c4SplitFields(strReadBuf, astrFlds, 2, m_plsplInput->c4CurLine()) != 2)
    {
        stdOut  << L"(Line " << m_plsplInput->c4CurLine()
                << L") Expected symbol name and resource id" << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::FileFormat;
    }

    // The second field should be a legal number
    rimCur.ridRes = astrFlds[1].c4AsCard();

    // Output the header field for this id
    if (m_bGenHeaders)
    {
        tflResHpp   << L"    constexpr tCIDLib::TResId  "
                    << astrFlds[0] << L" = " << rimCur.ridRes << L";\n";
    }

    //
    //  And now call a recursive method which will parse all of our children,
    //  and their children, and so on.
    //
    pc1Cur = pc1ParseSubMenu(*prmdFill, pc1End, pc1Cur, bflBinOut, tflResHpp);

    //
    //  If we didn't get any items, do nothing. Else, write out the data
    //  for this menu.
    //
    if (!prmdFill->c2Count)
        return;

    // Now we can calc the size of the resource
    rimCur.c4Size = pc1Cur - ac1Buf;

    // Seek back and write out our item map, then seek back to the end
    bflBinOut.SeekTo
    (
        sizeof(tCIDLib::TResHeader)
        + (sizeof(tCIDLib::TResItemMap) * m_reshOut.c4ResCount)
    );
    bflBinOut.c4Write(&rimCur, sizeof(rimCur));
    bflBinOut.c4SeekToEnd();

    // And now write out our menu resource
    bflBinOut.c4Write(prmdFill, rimCur.c4Size);

    // Bump up the overall resource count
    m_reshOut.c4ResCount++;
}

