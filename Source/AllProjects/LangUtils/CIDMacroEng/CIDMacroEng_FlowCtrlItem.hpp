//
// FILE NAME: CIDMacroEng_FlowCtrlItem.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/08/2003
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header file for the CIDMacroEng_FlowCtrlItem.cpp file, which
//  implements the TMEngFlowCtrlItem class. This class is used to push items
//  onto a flow control management stack. When a flow control item is seen,
//  an item is made and pushed. This allows us to correctly handle nesting of
//  flow control statements, insuring that they are all closed correctly, and
//  that we don't have any open at the end of the method and so forth.
//
//  It also stores the opcode index, since the end of some control structures
//  involves a jump back to the start. And, some are forward jumps, so we can
//  update the forward jump offset before we pop it off.
//
//  These items are used in a strict LIFO stack mode.
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
//  CLASS: TMEngFlowCtrlItem
// PREFIX: mefci
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngFlowCtrlItem : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngFlowCtrlItem();

        TMEngFlowCtrlItem
        (
            const   tCIDMacroEng::EFlowTypes   eType
            , const tCIDLib::TCard4         c4Offset
            , const tCIDLib::TCard4         c4LineNum
        );

        TMEngFlowCtrlItem
        (
            const   tCIDMacroEng::EFlowTypes   eType
            , const tCIDLib::TCard4         c4Offset
            , const tCIDLib::TCard2         c2Id
            , const tCIDLib::TCard4         c4LineNum
        );

        TMEngFlowCtrlItem
        (
            const   tCIDMacroEng::EFlowTypes   eType
            , const tCIDLib::TCard4         c4Offset1
            , const tCIDLib::TCard4         c4Offset2
            , const tCIDLib::TCard4         c4LineNum
        );

        TMEngFlowCtrlItem
        (
            const   TMEngFlowCtrlItem&      mefciToCopy
        );

        ~TMEngFlowCtrlItem();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngFlowCtrlItem& operator=
        (
            const   TMEngFlowCtrlItem&      mefciToAssign
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddBreakOffset
        (
            const   tCIDLib::TCard4         c4Offset
        );

        tCIDLib::TBoolean bIsLoopedType() const;

        tCIDLib::TCard2 c2Id() const;

        tCIDLib::TCard4 c4BreakCount() const;

        tCIDLib::TCard4 c4BreakOffsetAt
        (
            const   tCIDLib::TCard4         c4At
        )   const;

        tCIDLib::TCard4 c4LineNum() const;

        tCIDLib::TCard4 c4Offset1() const;

        tCIDLib::TCard4 c4Offset1
        (
            const   tCIDLib::TCard4         c4ToSet
        );

        tCIDLib::TCard4 c4Offset2() const;

        tCIDLib::TCard4 c4Offset2
        (
            const   tCIDLib::TCard4         c4ToSet
        );

        tCIDMacroEng::EFlowTypes eType() const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c2Id
        //      Sometimes we need to remember the id of something until we get
        //      to the end of the loop.
        //
        //  m_c4LineNum
        //      In some cases we want to give an indication of where the
        //      problem was if an error in flow control occurs, so they have
        //      to give us the line number of where this flow control item
        //      was found.
        //
        //  m_c4Offset1
        //      The IP of where this control structure starts, i.e. the offset
        //      of the first opcode of the control structure.
        //
        //  m_c4Offset2
        //      An extra offset that can be used to remember another IP
        //      where something needs to be updated. It might not always be
        //      be used.
        //
        //  m_eType
        //      Indicates the type of item this is, e.g. while, if, else, or
        //      so forth.
        //
        //  m_fcolBreaks
        //      This stores a list of the indexes of any jumps that we put
        //      out due to Break commands. These need to be updated at the
        //      end of the block to jump to the end of the block.
        // -------------------------------------------------------------------
        tCIDLib::TCard2                 m_c2Id;
        tCIDLib::TCard4                 m_c4LineNum;
        tCIDLib::TCard4                 m_c4Offset1;
        tCIDLib::TCard4                 m_c4Offset2;
        tCIDMacroEng::EFlowTypes           m_eType;
        TFundVector<tCIDLib::TCard4>    m_fcolBreaks;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngFlowCtrlItem,TObject)
};

#pragma CIDLIB_POPPACK


