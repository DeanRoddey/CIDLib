//
// FILE NAME: Streams3_Widgets.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/01/1997
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  This file is part of a demonstration program of the CIDLib C++
//  Frameworks. Its contents are distributed 'as is', to provide guidance on
//  the use of the CIDLib system. However, these demos are not intended to
//  represent a full fledged applications. Any direct use of demo code in
//  user applications is at the user's discretion, and no warranties are
//  implied as to its correctness or applicability.
//
// DESCRIPTION:
//
//  This is the header file for the Streams3_Widgets.cpp module. This module
//  implements a set of simple classes that derive from a common class. These
//  classes implement the streamable interface, the polymorphic duplicability
//  interface, and the required RTTI functionality to be polymorphically
//  streamed.
//
//  These classes emulate a set of 'graphic widgets' such as circles, boxes,
//  lines, etc... They all must implement a Draw() method, which is a
//  pure virtual in the TBaseWidget class. But, since this is just a TTY
//  output program, the 'drawing' is just a matter of outputting the type
//  of the class plus its data information. The type is output using the
//  clsIsA() RTTI method.
//
//  Since TBaseWidget also mixes in MStreamable and MDuplicable, but does not
//  implement the interface, each derivative must implement them. MStreamable
//  allows the objects to be streamed to/from a binary stream. MDuplicable
//  supports polymorphic duplication. They all use the DefPolyDup() macro
//  to implement the default duplication code, which is just a call to their
//  own copy constructor.
//
//  TFilledBoxWidget is a second level derivative, so note how it calls its
//  parent's version of everything. The standard RTTI macro create a typedef
//  called TParent, which stands for the parent class. So it always calls
//  through this magic TParent class instead of hard coding its parent class.
//  And all of the Draw() methods call down to the base class first
//  because it outputs the type info, which is common to all of their output.
//
// CAVEATS/GOTCHAS:
//
//  1)  Because these are trivial examples, they do not follow the formal
//      guidelines for style. They just implement all of their functionality
//      inline, within the class definition. And they are more 'scrunched'
//      up that I would normally do so that they can all fit easily within
//      this one file.
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ----------------------------------------------------------------------------
//   CLASS: TBaseWidget
//  PREFIX: wid
// ----------------------------------------------------------------------------
class TBaseWidget : public TObject, public MStreamable, public MDuplicable
{
    public  :
        // --------------------------------------------------------------------
        //  Public constructors and destructors
        // --------------------------------------------------------------------
        ~TBaseWidget() {}

        // --------------------------------------------------------------------
        //  Public, virtual methods
        // --------------------------------------------------------------------
        virtual TTextOutStream& Draw(TTextOutStream& strmToWriteTo) const = 0;


    protected   :
        // --------------------------------------------------------------------
        //  Hidden constructors and operators
        // --------------------------------------------------------------------
        TBaseWidget() {}
        TBaseWidget(const TBaseWidget& toCopy) {}
        TBaseWidget& operator=(const TBaseWidget& toCopy) {return *this;}


        // --------------------------------------------------------------------
        //  Magic macros
        // --------------------------------------------------------------------
        RTTIDefs(TBaseWidget,TObject)
};


inline TTextOutStream& TBaseWidget::Draw(TTextOutStream& strmToWriteTo) const
{
    strmToWriteTo << L"Type: " << clsIsA() << L" - ";
    return strmToWriteTo;
}



// ----------------------------------------------------------------------------
//   CLASS: TLineWidget
//  PREFIX: wid
// ----------------------------------------------------------------------------
class TLineWidget : public TBaseWidget
{
    public  :
        // --------------------------------------------------------------------
        //  Public constructors and destructors
        // --------------------------------------------------------------------
        TLineWidget(const TPoint& pntFrom, const TPoint& pntTo) :
            m_pntFrom(pntFrom)
            , m_pntTo(pntTo)
        {}

        TLineWidget(const TLineWidget& widToCopy) :
            m_pntFrom(widToCopy.m_pntFrom)
            , m_pntTo(widToCopy.m_pntTo)
        {}

        ~TLineWidget() {}


        // --------------------------------------------------------------------
        //  Public operators
        // --------------------------------------------------------------------
        TLineWidget& operator=(const TLineWidget& widToAssign)
        {
            m_pntFrom = widToAssign.m_pntFrom;
            m_pntTo = widToAssign.m_pntTo;
            return *this;
        }

        // --------------------------------------------------------------------
        //  Public, inherited methods
        // --------------------------------------------------------------------
        TTextOutStream& Draw(TTextOutStream& strmToWriteTo) const
        {
            strmToWriteTo   << TParent::Draw(strmToWriteTo)
                            << L"{" << m_pntFrom
                            << L"," << m_pntTo << L"}";
            return strmToWriteTo;
        }


    protected   :
        // --------------------------------------------------------------------
        //  Hidden constructors
        // --------------------------------------------------------------------
        TLineWidget() :

            m_pntFrom(0, 0)
            , m_pntTo(1, 1)
        {}


        // --------------------------------------------------------------------
        //  Protected, implemented methods
        // --------------------------------------------------------------------
        tCIDLib::TVoid StreamFrom(TBinInStream& strmToReadFrom)
        {
            strmToReadFrom >> m_pntFrom >> m_pntTo;
        }

        tCIDLib::TVoid StreamTo(TBinOutStream& strmToWriteTo) const
        {
            strmToWriteTo << m_pntFrom << m_pntTo;
        }


    private :
        // --------------------------------------------------------------------
        //  Private data members
        //
        //  m_pntFrom
        //  m_pntTo
        //      The points that the line draws from and to.
        // --------------------------------------------------------------------
        TPoint  m_pntFrom;
        TPoint  m_pntTo;


        // --------------------------------------------------------------------
        //  Magic macros
        // --------------------------------------------------------------------
        RTTIDefs(TLineWidget,TBaseWidget)
        DefPolyDup(TLineWidget)
        BefriendFactory(TLineWidget)
};



// ----------------------------------------------------------------------------
//   CLASS: TCircleWidget
//  PREFIX: wid
// ----------------------------------------------------------------------------
class TCircleWidget : public TBaseWidget
{
    public  :
        // --------------------------------------------------------------------
        //  Public constructors and destructors
        // --------------------------------------------------------------------
        TCircleWidget(const TPoint& pntCenter, const tCIDLib::TCard4 c4Radius) :
            m_c4Radius(c4Radius)
            , m_pntCenter(pntCenter)
        {}

        TCircleWidget(const TCircleWidget& widToCopy) :
            m_c4Radius(widToCopy.m_c4Radius)
            , m_pntCenter(widToCopy.m_pntCenter)
        {}

        ~TCircleWidget() {}


        // --------------------------------------------------------------------
        //  Public operators
        // --------------------------------------------------------------------
        TCircleWidget& operator=(const TCircleWidget& widToAssign)
        {
            m_c4Radius = widToAssign.m_c4Radius;
            m_pntCenter = widToAssign.m_pntCenter;
            return *this;
        }

        // --------------------------------------------------------------------
        //  Public, inherited methods
        // --------------------------------------------------------------------
        TTextOutStream& Draw(TTextOutStream& strmToWriteTo) const
        {
            strmToWriteTo   << TParent::Draw(strmToWriteTo)
                            << L"Center/Radius: {" << m_pntCenter
                            << L"," << m_c4Radius << L"}";
            return strmToWriteTo;
        }


    protected   :
        // --------------------------------------------------------------------
        //  Hidden constructors
        // --------------------------------------------------------------------
        TCircleWidget() :

            m_c4Radius(1)
            , m_pntCenter(0, 0)
        {}


        // --------------------------------------------------------------------
        //  Protected, implemented methods
        // --------------------------------------------------------------------
        tCIDLib::TVoid StreamFrom(TBinInStream& strmToReadFrom)
        {
            strmToReadFrom >> m_pntCenter >> m_c4Radius;
        }

        tCIDLib::TVoid StreamTo(TBinOutStream& strmToWriteTo) const
        {
            strmToWriteTo << m_pntCenter << m_c4Radius;
        }


    private :
        // --------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4Radius
        //  m_pntCenter
        //      The center point and radius of the circle
        // --------------------------------------------------------------------
        tCIDLib::TCard4 m_c4Radius;
        TPoint          m_pntCenter;

        // --------------------------------------------------------------------
        //  Magic macros
        // --------------------------------------------------------------------
        RTTIDefs(TCircleWidget,TBaseWidget)
        DefPolyDup(TCircleWidget)
        BefriendFactory(TCircleWidget)
};



// ----------------------------------------------------------------------------
//   CLASS: TBoxWidget
//  PREFIX: wid
// ----------------------------------------------------------------------------
class TBoxWidget : public TBaseWidget
{
    public  :
        // --------------------------------------------------------------------
        //  Public constructors and destructors
        // --------------------------------------------------------------------
        TBoxWidget(const TPoint& pntUL, const TPoint& pntLR) :
            m_pntUL(pntUL)
            , m_pntLR(pntLR)
        {}

        TBoxWidget(const TBoxWidget& widToCopy) :
            m_pntUL(widToCopy.m_pntUL)
            , m_pntLR(widToCopy.m_pntLR)
        {}

        ~TBoxWidget() {}


        // --------------------------------------------------------------------
        //  Public operators
        // --------------------------------------------------------------------
        TBoxWidget& operator=(const TBoxWidget& widToAssign)
        {
            m_pntUL = widToAssign.m_pntUL;
            m_pntLR = widToAssign.m_pntLR;
            return *this;
        }

        // --------------------------------------------------------------------
        //  Public, inherited methods
        // --------------------------------------------------------------------
        TTextOutStream& Draw(TTextOutStream& strmToWriteTo) const
        {
            strmToWriteTo   << TParent::Draw(strmToWriteTo)
                            << L"Corners: {" << m_pntUL
                            << L"," << m_pntLR << L"}";
            return strmToWriteTo;
        }


    protected   :
        // --------------------------------------------------------------------
        //  Hidden constructors
        // --------------------------------------------------------------------
        TBoxWidget() :

            m_pntUL(0, 0)
            , m_pntLR(1, 1)
        {}


        // --------------------------------------------------------------------
        //  Protected, implemented methods
        // --------------------------------------------------------------------
        tCIDLib::TVoid StreamFrom(TBinInStream& strmToReadFrom)
        {
            strmToReadFrom >> m_pntUL >> m_pntLR;
        }

        tCIDLib::TVoid StreamTo(TBinOutStream& strmToWriteTo) const
        {
            strmToWriteTo << m_pntUL << m_pntLR;
        }


    private :
        // --------------------------------------------------------------------
        //  Private data members
        //
        //  m_pntUL
        //  m_pntLR
        //      The points that the line draws from and to.
        // --------------------------------------------------------------------
        TPoint  m_pntUL;
        TPoint  m_pntLR;

        // --------------------------------------------------------------------
        //  Magic macros
        // --------------------------------------------------------------------
        RTTIDefs(TBoxWidget,TBaseWidget)
        DefPolyDup(TBoxWidget)
        BefriendFactory(TBoxWidget)
};




// ----------------------------------------------------------------------------
//   CLASS: TFilledBoxWidget
//  PREFIX: wid
// ----------------------------------------------------------------------------
class TFilledBoxWidget : public TBoxWidget
{
    public  :
        // --------------------------------------------------------------------
        //  Public constructors and destructors
        // --------------------------------------------------------------------
        TFilledBoxWidget(   const   TPoint&     pntUL
                            , const TPoint&     pntLL
                            , const TRGBClr&    rgbFill) :
            TBoxWidget(pntUL, pntLL)
            , m_rgbFill(rgbFill)
        {}

        TFilledBoxWidget(const TFilledBoxWidget& widToCopy) :
            TBoxWidget(widToCopy)
            , m_rgbFill(widToCopy.m_rgbFill)
        {}

        ~TFilledBoxWidget() {}


        // --------------------------------------------------------------------
        //  Public operators
        // --------------------------------------------------------------------
        TFilledBoxWidget& operator=(const TFilledBoxWidget& widToAssign)
        {
            TParent::operator=(widToAssign);
            m_rgbFill = widToAssign.m_rgbFill;
            return *this;
        }

        // --------------------------------------------------------------------
        //  Public, inherited methods
        // --------------------------------------------------------------------
        TTextOutStream& Draw(TTextOutStream& strmToWriteTo) const
        {
            TParent::Draw(strmToWriteTo);
            strmToWriteTo << L", Fill Color: " << m_rgbFill;
            return strmToWriteTo;
        }


    protected   :
        // --------------------------------------------------------------------
        //  Hidden constructors
        // --------------------------------------------------------------------
        TFilledBoxWidget() :

            m_rgbFill(0xFF, 0xFF, 0xFF)
        {
        }


        // --------------------------------------------------------------------
        //  Protected, implemented methods
        // --------------------------------------------------------------------
        tCIDLib::TVoid StreamFrom(TBinInStream& strmToReadFrom)
        {
            TParent::StreamFrom(strmToReadFrom);
            strmToReadFrom >> m_rgbFill;
        }

        tCIDLib::TVoid StreamTo(TBinOutStream& strmToWriteTo) const
        {
            TParent::StreamTo(strmToWriteTo);
            strmToWriteTo << m_rgbFill;
        }


    private :
        // --------------------------------------------------------------------
        //  Private data members
        //
        //  m_rgbFill
        //      The fill color to use
        // --------------------------------------------------------------------
        TRGBClr     m_rgbFill;

        // --------------------------------------------------------------------
        //  Magic macros
        // --------------------------------------------------------------------
        RTTIDefs(TFilledBoxWidget,TBoxWidget)
        DefPolyDup(TFilledBoxWidget)
        BefriendFactory(TFilledBoxWidget)
};

