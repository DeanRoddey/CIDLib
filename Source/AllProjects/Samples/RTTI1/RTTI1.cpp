//
// FILE NAME: RTTI1.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/08/1997
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the main module for the first of the RTTI oriented demo
//  programs. This one does not do anything realistic but just shows all of
//  of the things you can do with the RTTI system.
//
//  In order to make it obvious what is happening, a small class is defined
//  here that implements all of the standard RTTI stuff. The class is trivial
//  so that it's implementation does not cloud the picture. The class is
//  very trivial so its just done all inline.
//
//  RTTI enabled classes must:
//
//  1)  Derive from TObject.
//  2)  Use the RTTIDefs() macro in the class definition.
//  3)  Use the RTTIDecls() or AdvRTTIDecls() macros in their cpp file.
//  4)  Support a default constructor if AdvRTTIDecls is used. The
//      constructor can be protected, but then the class then has to use the
//          BefriendFactory() macro so that the factory method can access the
//          default constructor.
//  5)  Although its not necessary, supporting polymorphic duplication is
//          a plus by supporting the MDuplicable interface. This requires a
//          copy constructor.
//
//      * Template classes should use the TemplateRTTIDefs() macro in their
//          class definition. Templates with multiple instantiation parms
//          cannot use the macro and must implement the methods directly,
//          but there are not many such template classes and most of them
//          are already provided by CIDLib (i.e. some collections.)
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ----------------------------------------------------------------------------
//  Includes. This program is so simple that we don't even have a header of
//  our own. So just include CIDLib, which is all we need.
// ----------------------------------------------------------------------------
#include    "CIDLib.hpp"


// ----------------------------------------------------------------------------
//  Forward references
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc
(
        TThread&            thrThis
        , tCIDLib::TVoid*   pData
);


// ----------------------------------------------------------------------------
//  Local data
//
//  strmOut
//      This is our output strema class. You can use a console object as well,
//      but since we need no interactivity, this is simpler. Its just a text
//      file output stream with the standard out as the file.
// ----------------------------------------------------------------------------
static TTextFileOutStream   strmOut(tCIDLib::EStdFiles::StdOut);


// ----------------------------------------------------------------------------
//  Do the magic main module code
// ----------------------------------------------------------------------------
CIDLib_MainModule(TThread(L"RTTI1MainThread", eMainThreadFunc))



// ----------------------------------------------------------------------------
//   CLASS: TTestClass
//  PREFIX: test
// ----------------------------------------------------------------------------
class TTestClass : public TObject, public MDuplicable
{
    public  :
        // --------------------------------------------------------------------
        //  Constructors and Destructors
        // --------------------------------------------------------------------
        TTestClass(const tCIDLib::TInt4 i4Value) :

            m_i4Value(i4Value)
        {
        }

        TTestClass(const TTestClass& testToCopy) :

            m_i4Value(testToCopy.m_i4Value)
        {
        }

        ~TTestClass() {}


        // --------------------------------------------------------------------
        //  Public, non-virtual methods
        // --------------------------------------------------------------------
        tCIDLib::TInt4 i4Value() const
        {
            return m_i4Value;
        }


    protected  :
        // --------------------------------------------------------------------
        //  Hidden constructors
        // --------------------------------------------------------------------
        TTestClass() :

            m_i4Value(kCIDLib::i4MaxInt)
        {
        }

    private :
        // --------------------------------------------------------------------
        //  Private data
        //
        //  m_i4Value
        //      Our data member
        // --------------------------------------------------------------------
        tCIDLib::TInt4  m_i4Value;


        // --------------------------------------------------------------------
        //  Do needed macros. Always just put them at the end of the class
        //  definition because they modify the public/protected/private
        //  scope at will.
        //
        //  BefriendFactory
        //      This method lets us support factory creation (which requires
        //      a default constructor) without exposing our default
        //      constructor.
        //
        //  DefPolyDup
        //      Does the default polymorphic duplication, which is just a
        //      call to our own copy constructor to new up a copy and return
        //      it. This implements the pobjDuplicate() method which we get
        //      from MDuplicable.
        //
        //  RTTIDefs()
        //      Indicates our class and our parent class. It also implements
        //      the standard RTTI methods: clsIsA(), clsParent(), and
        //      bIsDescendantOf().
        // --------------------------------------------------------------------
        BefriendFactory(TTestClass)
        DefPolyDup(TTestClass)
        RTTIDefs(TTestClass,TObject)
};

// ----------------------------------------------------------------------------
//  Do our out of line macros. We want to support factory creation (or dynamic
//  creation as some would call it) so we use AdvRTTIDecls().
// ----------------------------------------------------------------------------
AdvRTTIDecls(TTestClass,TObject)


// ----------------------------------------------------------------------------
//  Local functions
// ----------------------------------------------------------------------------
//
//  This is the the thread function for the main thread. This guy just does
//  a lot of RTTI stuff.
//
tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    //
    //  Ok, lets get started. First all all, any RTTI enabled class has
    //  a const static member of TClass that is the 'meta class' for that
    //  class. So lets dump it out to the output console.
    //
    strmOut << L"Our type is: " << TTestClass::clsThis() << kCIDLib::EndLn;

    //
    //  Another way to get to the same place, and to do it polymorphically,
    //  is the clsIsA() method. So lets create a TTestClass object and then
    //  get another pointer to it via the base TObject class.
    //
    TTestClass  testObj(1);
    TObject*    pobjTest = &testObj;

    //
    //  So we can do a dump of the class via the object or via the pointer
    //  and get the same result. clsIsA() always returns the REAL type of
    //  class, not of the type its accessed through.
    //
    strmOut << L"Through testObj: " << testObj.clsIsA() << kCIDLib::NewLn
            << L"Through pobjTest: " << pobjTest->clsIsA() << kCIDLib::EndLn;

    //
    //  We can also get the class type of our parent class, which returns
    //  its class object.
    //
    strmOut << L"testObj's parent class is: " << testObj.clsParent()
            << kCIDLib::NewLn;

    //
    //  And of course we can access the static object via the object but
    //  accessing it via a base class will get the static object of the
    //  base class.
    //
    strmOut << L"Through testObj.clsThis(): " << testObj.clsThis() << kCIDLib::NewLn
            << L"Through pobjTest->clsThis(): " << pobjTest->clsThis()
            << kCIDLib::EndLn;

    //
    //  We can compare the types of an object to see if its of a particular
    //  type.
    //
    if (testObj.clsIsA() == TTestClass::clsThis())
    {
        strmOut << L"Yes, testObject is a "
                << TTestClass::clsThis() << kCIDLib::NewLn;
    }

    //
    //  We can test for descendence from a particular class. So lets do one
    //  that's true and one that is not.
    //
    if (testObj.bIsDescendantOf(TObject::clsThis()))
    {
        strmOut << L"Yes, testObj is descended from "
                << TObject::clsThis() << kCIDLib::EndLn;
    }

    if (!testObj.bIsDescendantOf(TArea::clsThis()))
    {
        strmOut << L"No, testObj is not descended from "
                << TArea::clsThis() << kCIDLib::EndLn;
    }

    //
    //  We can also do somethings using just the type name at runtime.
    //  For instance we can get the class object for a class. This stuff
    //  is done via static methods of the TClass class.
    //
    TClass clsTmp = TClass::clsForClass(L"TTestClass");
    strmOut << L"The dynamically gotten class is: " << clsTmp << kCIDLib::EndLn;

    //
    //  We can create a new object of a type that supports dynamic creation.
    //  First we will use the rawest mechanism which returns it as a TObject
    //  object.
    //
    TObject* pobjDynamic = TClass::pobjMakeNewOfClass(L"TTestClass");
    strmOut << L"Dynamically created object is of type: "
            << pobjDynamic->clsIsA() << kCIDLib::EndLn;
    delete pobjDynamic;

    //
    //  There is a template method that does a lot of work for you, taking
    //  a pointer, making sure that the pointer can legally point to an
    //  object of the type to be created, and creating it, and casting the
    //  returned object to the desired type.
    //
    TTestClass* ptestDynamic= pobjMakeNewOfClass<TTestClass>(L"TTestClass");
    strmOut << L"Dynamically created object is of type: "
            << ptestDynamic->clsIsA() << kCIDLib::EndLn;

    //
    //  You can do a dynamic cast test that will return 0 if the cast is
    //  not valid, or the object cast as the new type if the cast is legal.
    //  This is in the style used by the standard C++ RTTI.
    //
    pobjDynamic = dynamic_cast<TObject*>(ptestDynamic);
    if (!pobjDynamic)
        strmOut << L"Could not cast ptestDynamic to pobjDynamic" << kCIDLib::EndLn;

    // Clean up the temp object
    delete ptestDynamic;

    return tCIDLib::EExitCodes::Normal;
}

