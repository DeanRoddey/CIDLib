//
// FILE NAME: CIDMath_FArea.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/10/1996
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
//  This file implements the TFArea class, a 2D rectangular area in a
//  floating point coordinate space.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDMath_.hpp"


// ---------------------------------------------------------------------------
//  Do our standard members macros
// ---------------------------------------------------------------------------
RTTIDecls(TFArea,TObject)



// ---------------------------------------------------------------------------
//   CLASS: TFArea
//  PREFIX: farea
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFArea: Constructors and Destructor
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//  Constructors
// ---------------------------------------------------------------------------
TFArea::TFArea() :

    m_f8Bottom(0.0)
    , m_f8Left(0.0)
    , m_f8Right(0.0)
    , m_f8Top(0.0)
{
}

TFArea::TFArea(const TArea& areaFrom, const tCIDLib::TFloat8& f8ScaleBy) :

    m_f8Bottom(tCIDLib::TFloat8(areaFrom.i4Bottom()) * f8ScaleBy)
    , m_f8Left(tCIDLib::TFloat8(areaFrom.i4Left()) * f8ScaleBy)
    , m_f8Right(tCIDLib::TFloat8(areaFrom.i4Right()) * f8ScaleBy)
    , m_f8Top(tCIDLib::TFloat8(areaFrom.i4Top()) * f8ScaleBy)
{
}

TFArea::TFArea( const   tCIDLib::TFloat8&   f8Left
                , const tCIDLib::TFloat8&   f8Right
                , const tCIDLib::TFloat8&   f8Bottom
                , const tCIDLib::TFloat8&   f8Top) :

    m_f8Bottom(f8Bottom)
    , m_f8Left(f8Left)
    , m_f8Right(f8Right)
    , m_f8Top(f8Top)
{
    AdjustCoordinates();
}


TFArea::TFArea(const T2DVector& vecLL, const T2DVector& vecUR) :

    m_f8Bottom(vecLL.f8X())
    , m_f8Left(vecLL.f8Y())
    , m_f8Right(vecUR.f8X())
    , m_f8Top(vecUR.f8Y())
{
    AdjustCoordinates();
}


TFArea::~TFArea()
{
}


// ---------------------------------------------------------------------------
//  TFArea: Public operators
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TFArea::operator==(const TFArea& fareaSrc) const
{
    if (&fareaSrc != this)
    {
        if ((m_f8Bottom != fareaSrc.m_f8Bottom)
        ||  (m_f8Left != fareaSrc.m_f8Left)
        ||  (m_f8Right != fareaSrc.m_f8Right)
        ||  (m_f8Top != fareaSrc.m_f8Top))
        {
            return kCIDLib::False;
        }
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean TFArea::operator!=(const TFArea& fareaSrc) const
{
    return !operator==(fareaSrc);
}


// ---------------------------------------------------------------------------
//  TFArea: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TFloat8 TFArea::f8Area() const
{
    return ((m_f8Right - m_f8Left) * (m_f8Top - m_f8Bottom));
}


tCIDLib::TFloat8 TFArea::f8Bottom() const
{
    return m_f8Bottom;
}

tCIDLib::TFloat8 TFArea::f8Bottom(const tCIDLib::TFloat8& f8New)
{
    if (f8New > m_f8Top)
    {
        facCIDMath().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMathErrs::errcFArea_BadCoord
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TString(L"Bottom")
            , TFloat(f8New)
            , TString(L"top")
            , TFloat(m_f8Top)
        );
    }
    m_f8Bottom = f8New;
    return f8New;
}


tCIDLib::TFloat8 TFArea::f8CX() const
{
    return m_f8Right - m_f8Left;
}

tCIDLib::TFloat8 TFArea::f8CXDiv(const tCIDLib::TCard4 c4DivBy) const
{
    return (m_f8Right - m_f8Left) / tCIDLib::TFloat8(c4DivBy);
}


tCIDLib::TFloat8 TFArea::f8CY() const
{
    return m_f8Top - m_f8Bottom;
}

tCIDLib::TFloat8 TFArea::f8CYDiv(const tCIDLib::TCard4 c4DivBy) const
{
    return (m_f8Top - m_f8Bottom) / tCIDLib::TFloat8(c4DivBy);
}


tCIDLib::TFloat8 TFArea::f8Left() const
{
    return m_f8Left;
}

tCIDLib::TFloat8 TFArea::f8Left(const tCIDLib::TFloat8& f8New)
{
    if (f8New > m_f8Right)
    {
        facCIDMath().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMathErrs::errcFArea_BadCoord
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TString(L"Left")
            , TFloat(f8New)
            , TString(L"right")
            , TFloat(m_f8Right)
        );
    }
    m_f8Left = f8New;
    return f8New;
}


tCIDLib::TFloat8 TFArea::f8Right() const
{
    return m_f8Right;
}

tCIDLib::TFloat8 TFArea::f8Right(const tCIDLib::TFloat8& f8New)
{
    if (f8New < m_f8Left)
    {
        facCIDMath().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMathErrs::errcFArea_BadCoord
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TString(L"Right")
            , TFloat(f8New)
            , TString(L"left")
            , TFloat(m_f8Left)
        );
    }
    m_f8Right = f8New;
    return f8New;
}


tCIDLib::TFloat8 TFArea::f8Top() const
{
    return m_f8Top;
}

tCIDLib::TFloat8 TFArea::f8Top(const tCIDLib::TFloat8& f8New)
{
    if (f8New < m_f8Bottom)
    {
        facCIDMath().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMathErrs::errcFArea_BadCoord
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TString(L"Top")
            , TFloat(f8New)
            , TString(L"bottom")
            , TFloat(m_f8Bottom)
        );
    }
    m_f8Top = f8New;
    return f8New;
}


tCIDLib::TVoid
TFArea::SetAll( const   tCIDLib::TFloat8&   f8Left
                , const tCIDLib::TFloat8&   f8Right
                , const tCIDLib::TFloat8&   f8Bottom
                , const tCIDLib::TFloat8&   f8Top)
{
    m_f8Bottom  = f8Bottom;
    m_f8Left    = f8Left;
    m_f8Right   = f8Right;
    m_f8Top     = f8Top;

    AdjustCoordinates();
}


tCIDLib::TVoid TFArea::SetLowerRight(const T2DVector& vecToSet)
{
    f8Right(vecToSet.f8X());
    f8Bottom(vecToSet.f8Y());
}


tCIDLib::TVoid TFArea::SetOrg(const T2DVector& vecToSet)
{
    f8Left(vecToSet.f8X());
    f8Top(vecToSet.f8Y());
}


T2DVector TFArea::vecLL() const
{
    return T2DVector(m_f8Left, m_f8Bottom);
}


T2DVector TFArea::vecLR() const
{
    return T2DVector(m_f8Right, m_f8Bottom);
}


T2DVector TFArea::vecUL() const
{
    return T2DVector(m_f8Left, m_f8Top);
}


T2DVector TFArea::vecUR() const
{
    return T2DVector(m_f8Right, m_f8Top);
}


// ---------------------------------------------------------------------------
//  TFArea: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TFArea::FormatTo(TTextOutStream& strmToWriteTo) const
{
    // Save the stream state and set up a new one
    TStreamJanitor janWrite(&strmToWriteTo);
    strmToWriteTo   << TTextOutStream::Width(0)
                    << TTextOutStream::Precision(4)
                    << TTextOutStream::Justify(tCIDLib::EHJustify::Right)
                    << TTextOutStream::Fill(kCIDLib::chSpace);

    strmToWriteTo   << L"{"
                    << m_f8Left << L","
                    << m_f8Bottom << L"-"
                    << m_f8Right << L","
                    << m_f8Top
                    << L"}";
}


tCIDLib::TVoid TFArea::StreamFrom(TBinInStream& strmToReadFrom)
{
    strmToReadFrom  >> m_f8Bottom
                    >> m_f8Left
                    >> m_f8Right
                    >> m_f8Top;
}

tCIDLib::TVoid TFArea::StreamTo(TBinOutStream& strmToWriteTo) const
{
    strmToWriteTo   << m_f8Bottom
                    << m_f8Left
                    << m_f8Right
                    << m_f8Top;
}



// ---------------------------------------------------------------------------
//  TFArea: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  This is called after the coordinates are set by the outside world. It will
//  flip them as needed to keep the left and bottom less than the top and
//  right. Its only called when they are all set at once. When individual
//  members are set, its illegal to do them wrong.
//
tCIDLib::TVoid TFArea::AdjustCoordinates()
{
    if (m_f8Bottom > m_f8Top)
        tCIDLib::Swap(m_f8Bottom, m_f8Top);

    if (m_f8Left > m_f8Right)
        tCIDLib::Swap(m_f8Left, m_f8Right);
}
