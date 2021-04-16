//
// FILE NAME: CIDNet_JSONParser.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/22/2011
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
//  This is the header for the CIDNet_JSONParser.cpp module. We have a main parser class,
//   and a tree of classes that represent the various types of values that we have to
//  be able to store.
//
//  We have a base class that just provides a means to store heterogenous lists of
//  objects or values.
//
//  The hierarchy is:
//
//      Value
//        SimpleValue   - Holds values for non-container types
//        Cont          - Base value container type
//          Array       - An indexed list of Values
//          Object      - A named list of Values
//
//
//  So it's a recursive hierarchy. Ultimately everything can be dealt with via the base
//  value class. You can look at the type and cast it to its actual type to access the
//  info in it.
//
//  For SimpleValue types, you can look at the type and know how to interpret the value
//  it contains, which is always just made available as text.
//
//  The base container class has helpers to find nodes of particular types and names
//  and to validate that they are of the expected type, and to add new children of
//  specific types.
//
//  The value class has a name field, but it's not always valid of course. For the
//  children of an array, they have no names. So the name will just be empty in those
//  cases.
//
//  The array and object classes are basically vestigial and just there to reflect the
//  JSON actual heirarchy. All the access is done via the base container class.
//
//  Parsing JSON
//
//  You can call pjprsnParse() which will give you back a pointer to the top
//  level value (should be an object in almost all  cases.) You can iterate that tree
//  as desired. You are responsible for deleting the top level value, which will
//  recursively clean up the contained stuff.
//
//  You can also call bParse which takes a callback, usually a lambda, and calls
//  back for each directory and file. It is files first, then sub-directories, so
//  breadth-first, though the starting directory will be called back first before
//  anything else.
//
//  The callback gets a 'path' to the current node being called back for, the
//  text formatted value of that node, and the type. So you can cast it to its
//  actual type based on the type enum.
//
//      []( const   TString&                strPath
//          , const TString&                strVal
//          , const tCIDNet::EJSONVTypes    eType) -> tCIDLib::TBoolean
//      {
//          // Do something the info
//          return kCIDLib::True;
//      }
//
//  The callback returns true to continue the search or false to stop it at
//  any point.
//
//  Callback Paths
//
//  The callback paths are in the from:
//
//      /container/name
//
//  So objects and arrays are treated like directories, and the paths are to
//  value objects in those directories.
//
//  If the value is named, then name is the actual name of the value. If it is
//  not, then it is in the form [x], meaning the xth child of the container.
//
//  So, for this JSON:
//
//  {
//      "Val1" : true,
//      "AnObj" : { "Child1" : 2, "Child2" : 3 },
//      "AnArray" : [ 1, 2, 3 ]
//  }
//
//  The paths and their values would be:
//
//      /Val1               true
//      /AnObj/Child1       2
//      /AnObj/Child2       3
//      /AnArray/[1]        1
//      /AnArray/[2]        2
//      /AnArray/[3]        3
//
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TJSONArray;
class TJSONObject;


// ---------------------------------------------------------------------------
//   CLASS: TJSONValue
//  PREFIX: jprsn
// ---------------------------------------------------------------------------
class CIDNETEXP TJSONValue
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TJSONValue() = delete;

        TJSONValue(const TJSONValue&) = delete;
        TJSONValue(TJSONValue&&) = delete;

        virtual ~TJSONValue();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TJSONValue& operator=(const TJSONValue&) = delete;
        TJSONValue& operator=(TJSONValue&&) = delete;


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TVoid FormatToText
        (
                    TTextOutStream&         strmTar
            , const tCIDLib::TCard4         c4Level
            , const tCIDLib::TCard4         c4IndentSz
        )   const;

        virtual tCIDLib::TVoid Reset();

        virtual const TString& strValue() const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsContType() const;

        tCIDLib::TBoolean bIsSimpleType() const;

        tCIDNet::EJSONVTypes eType() const;

        const TString& strName() const;


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TJSONValue
        (
            const   tCIDNet::EJSONVTypes    eType
            , const TString&                strName
        );


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid SetType
        (
            const   tCIDNet::EJSONVTypes    eToSet
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_eType
        //      The value type of this value
        //
        //  m_strName
        //      The name of this value if applicable.
        // -------------------------------------------------------------------
        tCIDNet::EJSONVTypes    m_eType;
        TString                 m_strName;
};



// ---------------------------------------------------------------------------
//   CLASS: TJSONSimpleVal
//  PREFIX: jprsn
// ---------------------------------------------------------------------------
class CIDNETEXP TJSONSimpleVal : public TJSONValue
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TJSONSimpleVal() = delete;

        TJSONSimpleVal
        (
            const   TString&                strValue
            , const tCIDNet::EJSONVTypes    eValType
            , const TString&                strName
            , const tCIDLib::TBoolean       bEscape = kCIDLib::False
        );

        TJSONSimpleVal
        (
            const   TString&                strName
            , const tCIDLib::TBoolean       bValue
        );

        TJSONSimpleVal
        (
            const   TString&                strName
            , const tCIDLib::TCard1         c1Value
        );

        TJSONSimpleVal
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2Value
        );

        TJSONSimpleVal
        (
            const   TString&                strName
            , const tCIDLib::TCard4         c4Value
        );

        TJSONSimpleVal
        (
            const   TString&                strName
            , const tCIDLib::TCard8         c8Value
        );

        TJSONSimpleVal
        (
            const   TString&                strName
            , const tCIDLib::TFloat8        f8Value
        );

        TJSONSimpleVal
        (
            const   TString&                strName
            , const tCIDLib::TInt1          i1Value
        );

        TJSONSimpleVal
        (
            const   TString&                strName
            , const tCIDLib::TInt2          i2Value
        );

        TJSONSimpleVal
        (
            const   TString&                strName
            , const tCIDLib::TInt4          i4Value
        );

        TJSONSimpleVal
        (
            const   TString&                strName
            , const tCIDLib::TInt8          i8Value
        );

        TJSONSimpleVal
        (
            const   TString&                strName
            , const TString&                strValue
            , const tCIDLib::TBoolean       bEscape = kCIDLib::False
        );

        TJSONSimpleVal(const TJSONSimpleVal&) = delete;
        TJSONSimpleVal(TJSONSimpleVal&&) = delete;

        ~TJSONSimpleVal();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TJSONSimpleVal& operator=(const TJSONSimpleVal&) = delete;
        TJSONSimpleVal& operator=(TJSONSimpleVal&&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatToText
        (
                    TTextOutStream&         strmTar
            , const tCIDLib::TCard4         c4Level
            , const tCIDLib::TCard4         c4IndentSz
        )   const final;

        tCIDLib::TVoid Reset() final;

        const TString& strValue() const final;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid SetBoolValue
        (
            const   tCIDLib::TBoolean       bToSet
        );

        tCIDLib::TVoid SetNumValue
        (
            const   tCIDLib::TCard4         c4ToSet
        );

        tCIDLib::TVoid SetNumValue
        (
            const   tCIDLib::TFloat8        f8ToSet
        );

        tCIDLib::TVoid SetNumValue
        (
            const   tCIDLib::TInt4          i4ToSet
        );

        tCIDLib::TVoid SetStrValue
        (
            const   TString&                strToSet
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_strValue
        //      The value of this simple value node, all of which are
        //      representable as a string.
        // -------------------------------------------------------------------
        TString     m_strValue;
};




// ---------------------------------------------------------------------------
//   CLASS: TJSONCont
//  PREFIX: jprsn
// ---------------------------------------------------------------------------
class CIDNETEXP TJSONCont : public TJSONValue
{
    public :
        // -------------------------------------------------------------------
        //  Public types and constants
        // -------------------------------------------------------------------
        static const tCIDLib::TCard4        c4MaxChildren = kCIDLib::c4Sz_64K;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TJSONCont(const TJSONCont&) = delete;
        TJSONCont(TJSONCont&&) = delete;

        ~TJSONCont();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TJSONCont& operator=(const TJSONCont&) = delete;
        TJSONCont& operator=(TJSONCont&&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatToText
        (
                    TTextOutStream&         strmTar
            , const tCIDLib::TCard4         c4Level
            , const tCIDLib::TCard4         c4IndentSz
        )   const override;

        tCIDLib::TVoid Reset() override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddBoolean
        (
            const   TString&                strName
            , const tCIDLib::TBoolean       bValue
        );

        tCIDLib::TVoid AddCard
        (
            const   TString&                strName
            , const tCIDLib::TCard1         c1Value
        );

        tCIDLib::TVoid AddCard
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2Value
        );

        tCIDLib::TVoid AddCard
        (
            const   TString&                strName
            , const tCIDLib::TCard4         c4Value
        );

        tCIDLib::TVoid AddCard
        (
            const   TString&                strName
            , const tCIDLib::TCard8         c8Value
        );

        tCIDLib::TVoid AddFloat
        (
            const   TString&                strName
            , const tCIDLib::TFloat8        f8Value
        );

        tCIDLib::TVoid AddInt
        (
            const   TString&                strName
            , const tCIDLib::TInt1          i1Value
        );

        tCIDLib::TVoid AddInt
        (
            const   TString&                strName
            , const tCIDLib::TInt2          i2Value
        );

        tCIDLib::TVoid AddInt
        (
            const   TString&                strName
            , const tCIDLib::TInt4          i4Value
        );

        tCIDLib::TVoid AddInt
        (
            const   TString&                strName
            , const tCIDLib::TInt8          i8Value
        );

        tCIDLib::TVoid AddString
        (
            const   TString&                strName
            , const TString&                strValue
        );

        tCIDLib::TVoid AdoptValue
        (
                    TJSONValue* const       pjprsnToAdopt
        );

        tCIDLib::TBoolean bFindVal
        (
            const   TString&                strToFind
        )   const;

        tCIDLib::TBoolean bQueryVal
        (
            const   TString&                strToFind
            ,       TString&                strToFill
        )   const;

        tCIDLib::TCard4 c4FindVal
        (
            const   TString&                strToFind
        )   const;

        tCIDLib::TCard4 c4ValCount() const;

        tCIDNet::EJSONVTypes eTypeAt
        (
            const   tCIDLib::TCard4         c4At
        )   const;

        tCIDLib::TFloat8 f8FindVal
        (
            const   TString&                strToFind
        )   const;

        tCIDLib::TInt4 i4FindVal
        (
            const   TString&                strToFind
        )   const;

        const TJSONValue& jprsnValueAt
        (
            const   tCIDLib::TCard4         c4At
        )   const;

        const TJSONArray* pjprsnArrayAt
        (
            const   tCIDLib::TCard4         c4At
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::True
        )   const;

        const TJSONArray* pjprsnFindArray
        (
            const   TString&                strToFind
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::True
        )   const;

        const TJSONObject* pjprsnFindObj
        (
            const   TString&                strToFind
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::True
        )   const;

        const TJSONSimpleVal* pjprsnFindSVal
        (
            const   TString&                strToFind
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::True
        )   const;

        const TJSONValue* pjprsnFindVal
        (
            const   TString&                strToFind
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::True
        )   const;

        const TJSONValue* pjprsnFindVal
        (
            const   TString&                strToFind
            , const tCIDLib::TCard4         c4StartAt
            ,       tCIDLib::TCard4&        c4FoundAt
        )   const;

        const TJSONObject* pjprsnObjAt
        (
            const   tCIDLib::TCard4         c4At
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::True
        )   const;

        const TJSONSimpleVal* pjprsnSValAt
        (
            const   tCIDLib::TCard4         c4At
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::True
        )   const;

        const TString& strNameAt
        (
            const   tCIDLib::TCard4         c4At
        )   const;

        const TString& strValueAt
        (
            const   tCIDLib::TCard4         c4At
        )   const;

        const TString& strValByName
        (
            const   TString&                strToFind
        )   const;

        tCIDLib::TVoid SetBoolValue
        (
            const   TString&                strToFind
            , const tCIDLib::TBoolean       bToSet
            , const tCIDLib::TBoolean       bAddIfNot = kCIDLib::False
        );

        tCIDLib::TVoid SetNumValue
        (
            const   TString&                strToFind
            , const tCIDLib::TCard4         c4ToSet
            , const tCIDLib::TBoolean       bAddIfNot = kCIDLib::False
        );

        tCIDLib::TVoid SetNumValue
        (
            const   TString&                strToFind
            , const tCIDLib::TFloat8        f8ToSet
            , const tCIDLib::TBoolean       bAddIfNot = kCIDLib::False
        );

        tCIDLib::TVoid SetNumValue
        (
            const   TString&                strToFind
            , const tCIDLib::TInt4          i4ToSet
            , const tCIDLib::TBoolean       bAddIfNot = kCIDLib::False
        );

        tCIDLib::TVoid SetStrValue
        (
            const   TString&                strToFind
            , const TString&                strToSet
            , const tCIDLib::TBoolean       bAddIfNot = kCIDLib::False
        );


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TJSONCont
        (
            const   tCIDNet::EJSONVTypes    eValType
            , const TString&                strName
        );


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        TJSONSimpleVal* pjprsnSValByName
        (
            const   TString&                strToFind
            , const tCIDNet::EJSONVTypes    eType
            , const tCIDLib::TBoolean       bThrowIfNot
        );

        TJSONValue* pjprsnValByName
        (
            const   TString&                strToFind
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::True
        );


    private :
        // -------------------------------------------------------------------
        //  Private class types
        // -------------------------------------------------------------------
        using TValList = TRefVector<TJSONValue>;


        // -------------------------------------------------------------------
        //  Private, non-virtua lmethods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bTestIsSimpleType
        (
            const   TJSONValue&             jprsnTest
            , const tCIDLib::TCard4         c4Line
            , const tCIDLib::TBoolean       bThrowIfNot
        )   const;

        tCIDLib::TBoolean bTestNodeType
        (
            const   TJSONValue&             jprsnTest
            , const tCIDNet::EJSONVTypes    eTest
            , const tCIDLib::TCard4         c4Line
            , const tCIDLib::TBoolean       bThrowIfNot
        )   const;

        const TJSONValue* pjprsnTestIndex
        (
            const   tCIDLib::TCard4         c4At
            , const tCIDLib::TCard4         c4Line
        )   const;



        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_colValues
        //      The values owned by this container.
        // -------------------------------------------------------------------
        TValList    m_colValues;
};



// ---------------------------------------------------------------------------
//   CLASS: TJSONArray
//  PREFIX: jprsn
// ---------------------------------------------------------------------------
class CIDNETEXP TJSONArray : public TJSONCont
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TJSONArray() = delete;

        TJSONArray
        (
            const   TString&                strName
        );

        TJSONArray(const TJSONArray&) = delete;
        TJSONArray(TJSONArray&&) = delete;

        ~TJSONArray();


        // -------------------------------------------------------------------
        //  Public constructors
        // -------------------------------------------------------------------
        TJSONArray& operator=(const TJSONArray&) = delete;
        TJSONArray& operator=(TJSONArray&&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Reset() override;
};



// ---------------------------------------------------------------------------
//   CLASS: TJSONObject
//  PREFIX: jprsn
// ---------------------------------------------------------------------------
class CIDNETEXP TJSONObject : public TJSONCont
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TJSONObject() = delete;

        TJSONObject
        (
            const   TString&            strName
        );

        TJSONObject(const TJSONObject&) = delete;
        TJSONObject(TJSONObject&&) = delete;

        ~TJSONObject();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TJSONObject& operator=(const TJSONObject&) = delete;
        TJSONObject& operator=(TJSONObject&&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Reset() final;
};





// ---------------------------------------------------------------------------
//   CLASS: TJSONParser
//  PREFIX: jprs
// ---------------------------------------------------------------------------
class CIDNETEXP TJSONParser : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Public types
        //
        //  We use a simple parser that returns tokens of these types. This
        //  is only needed internally but is returned by methods so C++ rules
        //  require it to be public.
        // -------------------------------------------------------------------
        enum class ETokens
        {
            CloseArray
            , CloseObj
            , Colon
            , Comma
            , False
            , Null
            , Number
            , OpenArray
            , OpenObj
            , String
            , True
        };



        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TVoid FormatJSON
        (
            const   TJSONValue&             jprsnStart
            ,       TTextOutStream&         strmTar
            , const tCIDLib::TCard4         c4IndentSize = 4
            , const tCIDLib::TBoolean       bResetFirst = kCIDLib::True
        );


        //
        //  Handles callback based iteration. The path is modified, with it being
        //  added to and re-truncated as we go up and down the hierarchy. We need a
        //  little class to store recursion info since we use a stack.
        //
        struct TForEachLevel
        {
            public :
                const TJSONCont*    pjprsnCur;
                tCIDLib::TCard4     c4PathLen;
                tCIDLib::TCard4     c4ChildCount;
                tCIDLib::TCard4     c4CurIndex;
        };
        template <typename IterCB> static
        tCIDLib::TBoolean bForEach( const   TJSONValue&     jprsnStart
                                    ,       IterCB          iterCB
                                    ,       TString&        strCurPath)
        {
            //
            //  There's a chance it's a single simple value. If so do one callback
            //  with the root path and return the return. This lets us avoid
            //  complications in the loop below.
            //
            if (!jprsnStart.bIsContType())
            {
                strCurPath.Append(kCIDLib::chForwardSlash);
                strCurPath.Append(jprsnStart.strName());
                return iterCB(strCurPath, jprsnStart.strValue(), jprsnStart.eType());
            }

            // If the current path is empty, set it to /
            strCurPath.StripWhitespace();
            if (strCurPath.bIsEmpty())
                strCurPath.Append(kCIDLib::chForwardSlash);

            //
            //  Else we need to iterate. We use a stack based iteration, so create
            //  the stack.
            //
            TStack<TForEachLevel> colStack;

            //
            //  Start with the start node, casting to container since we now know it is
            //  one. The stack is empty. If this is the only container, then we fall out
            //  after one pass.
            //
            const TJSONCont* pjprsnCont = static_cast<const TJSONCont*>(&jprsnStart);
            tCIDLib::TCard4 c4Count = pjprsnCont->c4ValCount();
            tCIDLib::TCard4 c4Index = 0;

            //
            //  Now loop until we pop the start back off or the return flag goes false
            //  which means the caller wants to stop.
            //
            tCIDLib::TBoolean bRet = kCIDLib::True;
            TForEachLevel felTmp;
            while (kCIDLib::True)
            {
                // Let's iterate the current container
                while (c4Index < c4Count)
                {
                    const TJSONValue* pjprsnChild = &pjprsnCont->jprsnValueAt(c4Index++);

                    if (pjprsnChild->bIsContType())
                    {
                        // It's a container, so push and start processing this guy
                        felTmp.pjprsnCur = pjprsnCont;
                        felTmp.c4PathLen = strCurPath.c4Length();
                        felTmp.c4CurIndex = c4Index;
                        felTmp.c4ChildCount = c4Count;
                        colStack.objPush(felTmp);

                        //
                        //  Build up the new path. If no name, we do a [x] type
                        //  representation. Else add the name
                        //
                        if (pjprsnChild->strName().bIsEmpty())
                        {
                            strCurPath.Append(kCIDLib::chOpenBracket);
                            strCurPath.AppendFormatted(c4Index);
                            strCurPath.Append(kCIDLib::chCloseBracket);
                        }
                         else
                        {
                            strCurPath.Append(pjprsnChild->strName());
                        }
                        strCurPath.Append(kCIDLib::chForwardSlash);

                        // Make this guy the new container
                        pjprsnCont = static_cast<const TJSONCont*>(pjprsnChild);
                        c4Index = 0;
                        c4Count = pjprsnCont->c4ValCount();
                    }
                     else
                    {
                        // Just a simple type, so do the callback and set the return
                        const tCIDLib::TCard4 c4SaveLen = strCurPath.c4Length();
                        if (pjprsnChild->strName().bIsEmpty())
                        {
                            strCurPath.Append(kCIDLib::chOpenBracket);
                            strCurPath.AppendFormatted(c4Index);
                            strCurPath.Append(kCIDLib::chCloseBracket);
                        }
                         else
                        {
                            strCurPath.Append(pjprsnChild->strName());
                        }
                        bRet = iterCB
                        (
                            strCurPath, pjprsnChild->strValue(), pjprsnChild->eType()
                        );
                        strCurPath.CapAt(c4SaveLen);
                    }
                }

                // If the stack is empty or return is false, break out
                if (colStack.bIsEmpty() || !bRet)
                    break;

                // Else pop and continue. We truncate path back to previous length
                TForEachLevel& felCur = colStack.objPeek();
                pjprsnCont = felCur.pjprsnCur;
                strCurPath.CapAt(felCur.c4PathLen);
                c4Index = felCur.c4CurIndex;
                c4Count = felCur.c4ChildCount;
                colStack.TrashTop();
            }

            return bRet;
        }

        static tCIDLib::TVoid OutputArrayHdr
        (
            const   TString&                strName
            ,       TTextOutStream&         strmTar
        );

        static tCIDLib::TVoid OutputBool
        (
            const   TString&                strName
            , const tCIDLib::TBoolean       bValue
            ,       TTextOutStream&         strmTar
            , const tCIDLib::TBoolean       bLastVal = kCIDLib::False
        );

        static tCIDLib::TVoid OutputCard
        (
            const   TString&                strName
            , const tCIDLib::TCard4         c4Value
            ,       TTextOutStream&         strmTar
            , const tCIDLib::TBoolean       bLastVal = kCIDLib::False
        );

        static tCIDLib::TVoid OutputFloat
        (
            const   TString&                strName
            , const tCIDLib::TFloat8        f8Value
            ,       TTextOutStream&         strmTar
            , const tCIDLib::TBoolean       bLastVal = kCIDLib::False
        );

        static tCIDLib::TVoid OutputInt
        (
            const   TString&                strName
            , const tCIDLib::TInt4          i4Value
            ,       TTextOutStream&         strmTar
            , const tCIDLib::TBoolean       bLastVal = kCIDLib::False
        );

        static tCIDLib::TVoid OutputKVPs
        (
            const   TString&                strName
            , const tCIDLib::TKVPCollect&   colList
            ,       TTextOutStream&         strmTar
            , const tCIDLib::TBoolean       bLastVal = kCIDLib::False
        );

        static tCIDLib::TVoid OutputObjHdr
        (
            const   TString&                strName
            ,       TTextOutStream&         strmTar
        );

        static tCIDLib::TVoid OutputStr
        (
            const   TString&                strName
            , const TString&                strValue
            ,       TTextOutStream&         strmTar
            , const tCIDLib::TBoolean       bLastVal = kCIDLib::False
        );

        static tCIDLib::TVoid OutputStrArray
        (
            const   TString&                strName
            , const tCIDLib::TStrCollect&   colList
            ,       TTextOutStream&         strmTar
            , const tCIDLib::TBoolean       bLastVal = kCIDLib::False
        );

        static tCIDLib::TVoid OutputText
        (
            const   TString&                strName
            , const tCIDLib::TCh* const     pszValue
            ,       TTextOutStream&         strmTar
            , const tCIDLib::TBoolean       bLastVal = kCIDLib::False
        );



        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TJSONParser();

        TJSONParser(const TJSONParser&) = delete;

        ~TJSONParser();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TJSONParser& operator=(const TJSONParser&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        TJSONValue* pjprsnParse
        (
                    TTextInStream&          strmSrc
        );

        template <typename IterCB>
        tCIDLib::TBoolean bParse(TTextInStream& strmSrc, IterCB iterCB)
        {
            TJSONValue* pjprsnCur = pjprsnParse(strmSrc);
            TJanitor<TJSONValue> janTree(pjprsnCur);

            //
            //  Call a static helper, so that this iteration callback can be done
            //  separate from the parse as well. Pass in a root start path in this
            //  case since we know that's what we have.
            //
            TString strPath(L"/");
            return bForEach(*pjprsnCur, iterCB, strPath);
        }


    private :
        // -------------------------------------------------------------------
        //  Private, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TVoid EscapeStr
        (
                    TTextOutStream&         strmTar
            , const tCIDLib::TCh* const     pszText
        );


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bTestClose
        (
                    TTextInStream&          strmSrc
            , const tCIDLib::TCard4         c4Line
            , const ETokens                 eExpType
        );

        tCIDLib::TBoolean bTestOpen
        (
                    TTextInStream&          strmSrc
            , const tCIDLib::TCard4         c4Line
            ,       ETokens&                eTokFound
        );

        tCIDLib::TCh chGetNext
        (
                    TTextInStream&          strmSrc
        );

        ETokens eGetNextToken
        (
                    TString&                strToFill
            ,       TTextInStream&          strmSrc
        );

        TJSONValue* pjprsnParseValue
        (
                    TTextInStream&          strmSrc
            , const tCIDLib::TCard4         c4NestingLevel
            , const TString&                strName
        );

        tCIDLib::TVoid ParseArray
        (
                    TJSONArray&             jprsnCont
            ,       TTextInStream&          strmSrc
            , const tCIDLib::TCard4         c4NestingLevel
        );

        tCIDLib::TVoid ParseObject
        (
                    TJSONObject&            jprsnParent
            ,       TTextInStream&          strmSrc
            , const tCIDLib::TCard4         c4NestingLevel
        );

        tCIDLib::TVoid TestColon
        (
                    TTextInStream&          strmSrc
            , const tCIDLib::TCard4         c4Line
        );

        tCIDLib::TVoid TestOpenElem
        (
                    TTextInStream&          strmSrc
            , const tCIDLib::TCard4         c4Line
        );

        tCIDLib::TVoid ThrowExpected
        (
            const   tCIDLib::TCh* const     pszExpected
            , const TString&                strGot
            , const tCIDLib::TCard4         c4Line
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4Col
        //  m_c4Line
        //      For error reporting purposes we track the current position
        //      within the input.
        //
        //  m_chPushback
        //      We need to be able to push back a single character during
        //      parsing. If non-null, then this is taken as the next char
        //      and then nulled out.
        //
        //  m_strTmpTest
        //      This is used by the TextXXX methods to hold the value, to
        //      avoid creating and destroying lots of strings.
        // -------------------------------------------------------------------
        tCIDLib::TCard4 m_c4Col;
        tCIDLib::TCard4 m_c4Line;
        tCIDLib::TCh    m_chPushback;
        TString         m_strTmpTest;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TJSONParser,TObject)
};

#pragma CIDLIB_POPPACK


