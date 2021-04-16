//
// FILE NAME: Streams4_Widgets.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/07/1997
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
//  This header file defines some 'widget' classes for this sample to use. It
//  implements a set of simple classes that derive from a common class. These
//  classes implement the streamable interface, the polymorphic duplicability
//  interface, and the required RTTI functionality to be polymorphically
//  streamed.
//
//  These classes emulate a set of 'graphic widgets' such as circles, boxes,
//  lines, etc... They all must implement a Draw() method, which is a
//  pure virtual in the TBaseWidget class. But, since this is just a console
//  output program, the 'drawing' is just a matter of outputting the type
//  of the class plus its member data. The type is output using the clsIsA()
//  RTTI method.
//
//  Since TBaseWidget also mixes in MStreamable and MDuplicable, but does not
//  implement the interface, each derivative must implement them. MStreamable
//  allows the objects to be streamed to/from a binary stream. MDuplicable
//  supports polymorphic duplication. They all use the DefPolyDup() macro
//  to implement the default duplication code, which is just a call to their
//  own copy constructor.
//
//  They must have a default ctor, because the polymorphic streaming code
//  needs to be able to dynamically gen one up when streaming back in. For our
//  purposes we just let the compiler generate a protected one for us. We could
//  make them protected and use the BefriendFactory macro to make those available
//  to the RTTI support for the class.
//
//  TFilledCircleWidget is a second level derivative, so note how it calls its
//  parent's version of everything. The standard RTTI macro creates an alias
//  called TParent, which stands for the parent class. So it always calls
//  through this magic TParent class instead of hard coding its parent class.
//  And all of the Draw() methods call down to the base class first
//  because it outputs the type info, which is common to all of their output.
//
// CAVEATS/GOTCHAS:
//
//  1)  Because these are trivial examples, they are fairly lax in terms of
//      style relative to other CIDLib stuff.
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
        //  Public, virtual methods
        // --------------------------------------------------------------------
        virtual TTextOutStream& Draw(TTextOutStream& strmToWriteTo) const = 0
        {
            strmToWriteTo << L"Type: " << clsIsA() << L" - ";
            return strmToWriteTo;
        }


    protected   :
        // --------------------------------------------------------------------
        //  Magic macros
        // --------------------------------------------------------------------
        RTTIDefs(TBaseWidget,TObject)
};



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
        TLineWidget() = default;
        TLineWidget(const TPoint& pntFrom, const TPoint& pntTo) :

            m_pntFrom(pntFrom)
            , m_pntTo(pntTo)
        {}


        // --------------------------------------------------------------------
        //  Public, inherited methods
        // --------------------------------------------------------------------
        TTextOutStream& Draw(TTextOutStream& strmToWriteTo) const final
        {
            TParent::Draw(strmToWriteTo) << L"{" << m_pntFrom << L"," << m_pntTo << L"}";
            return strmToWriteTo;
        }


    protected   :
        // --------------------------------------------------------------------
        //  Protected, implemented methods
        // --------------------------------------------------------------------
        tCIDLib::TVoid StreamFrom(TBinInStream& strmToReadFrom) final
        {
            strmToReadFrom >> m_pntFrom >> m_pntTo;
        }

        tCIDLib::TVoid StreamTo(TBinOutStream& strmToWriteTo) const final
        {
            strmToWriteTo << m_pntFrom << m_pntTo;
        }


    private :
        // --------------------------------------------------------------------
        //  Private data members
        // --------------------------------------------------------------------
        TPoint  m_pntFrom;
        TPoint  m_pntTo;


        // --------------------------------------------------------------------
        //  Magic macros
        // --------------------------------------------------------------------
        RTTIDefs(TLineWidget,TBaseWidget)
        DefPolyDup(TLineWidget)
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
        TCircleWidget() = default;
        TCircleWidget(const TPoint& pntCenter, const tCIDLib::TCard4 c4Radius) :

            m_c4Radius(c4Radius)
            , m_pntCenter(pntCenter)
        {}


        // --------------------------------------------------------------------
        //  Public, inherited methods
        // --------------------------------------------------------------------
        TTextOutStream& Draw(TTextOutStream& strmToWriteTo) const override
        {
            TParent::Draw(strmToWriteTo) << L"Center/Radius: {" << m_pntCenter
                                         << L"," << m_c4Radius << L"}";
            return strmToWriteTo;
        }


    protected   :
        // --------------------------------------------------------------------
        //  Protected, implemented methods
        // --------------------------------------------------------------------
        tCIDLib::TVoid StreamFrom(TBinInStream& strmToReadFrom) override
        {
            strmToReadFrom >> m_pntCenter >> m_c4Radius;
        }

        tCIDLib::TVoid StreamTo(TBinOutStream& strmToWriteTo) const override
        {
            strmToWriteTo << m_pntCenter << m_c4Radius;
        }


    private :
        // --------------------------------------------------------------------
        //  Private data members
        // --------------------------------------------------------------------
        tCIDLib::TCard4 m_c4Radius = 1;
        TPoint          m_pntCenter;

        // --------------------------------------------------------------------
        //  Magic macros
        // --------------------------------------------------------------------
        RTTIDefs(TCircleWidget,TBaseWidget)
        DefPolyDup(TCircleWidget)
};



// ----------------------------------------------------------------------------
//   CLASS: TFilledCircleWidget
//  PREFIX: wid
// ----------------------------------------------------------------------------
class TFilledCircleWidget : public TCircleWidget
{
    public  :
        // --------------------------------------------------------------------
        //  Public constructors and destructors
        // --------------------------------------------------------------------
        TFilledCircleWidget() = default;
        TFilledCircleWidget(const   TPoint&         pntCenter
                            , const tCIDLib::TCard4 c4Radius
                            , const TRGBClr&        rgbFill) :

            TParent(pntCenter, c4Radius)
            , m_rgbFill(rgbFill)
        {}


        // --------------------------------------------------------------------
        //  Public, inherited methods
        // --------------------------------------------------------------------
        TTextOutStream& Draw(TTextOutStream& strmToWriteTo) const final
        {
            TParent::Draw(strmToWriteTo) << L", Fill Color: " << m_rgbFill;
            return strmToWriteTo;
        }


    protected   :
        // --------------------------------------------------------------------
        //  Protected, implemented methods
        // --------------------------------------------------------------------
        tCIDLib::TVoid StreamFrom(TBinInStream& strmToReadFrom) final
        {
            TParent::StreamFrom(strmToReadFrom);
            strmToReadFrom >> m_rgbFill;
        }

        tCIDLib::TVoid StreamTo(TBinOutStream& strmToWriteTo) const final
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
        RTTIDefs(TFilledCircleWidget,TCircleWidget)
        DefPolyDup(TFilledCircleWidget)
};


