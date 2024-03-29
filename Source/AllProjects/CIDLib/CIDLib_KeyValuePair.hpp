//
// FILE NAME: CIDLib_KeyValue.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/08/1993
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
//  TKeyValuePair is a simple class that is used in many places. There are lots
//  of places where key/value string pairs are used, so this class is provided
//  to support such functionality. Its just a way to store key/value string
//  pairs, which allows them to be manipulated easily.
//
//  We do a simple derivative of that which adds a boolean flag, which is often
//  a conenient combination.
//
//  TKeyObjPair is similar but is templatized and allows arbitrary classes
//  as the key and value. Both classes must support 'by value' semantics,
//  i.e. copy and assignment. They key class must also support equality and
//  less than operators.
//
//  We also implement a key/value(s) class that has one key and three values,
//  which is something not uncommonly needed, so having this avoids a number
//  of redundant ad hoc implementations. The client code csan use two or
//  all three of the values.
//
//  We also implement a simple string/float pair, which is something that various
//  bits of code could potentially use.
//
// CAVEATS/GOTCHAS:
//
//  1)  These classes are used very often within a hash map. So static key
//      extraction and comparison methods, which are required by some
//      collections, are provided as well.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TBoolean;

// ---------------------------------------------------------------------------
//   CLASS: TKeyValuePair
//  PREFIX: kval
// ---------------------------------------------------------------------------
class CIDLIBEXP TKeyValuePair

    : public TObject, public MDuplicable, public MStreamable, public MFormattable
{
    public  :
        // -------------------------------------------------------------------
        //  Public static methods
        // -------------------------------------------------------------------
        static tCIDLib::TBoolean
        bComp(const TKeyValuePair& kval1, const TKeyValuePair& kval2)
        {
            return (kval1 == kval2);
        }

        static tCIDLib::ESortComps
        eComp(const TKeyValuePair& kval1, const TKeyValuePair& kval2)
        {
            return kval1.m_strKey.eCompare(kval2.m_strKey);
        }

        static tCIDLib::ESortComps
        eCompI(const TKeyValuePair& kval1, const TKeyValuePair& kval2)
        {
            return kval1.m_strKey.eCompareI(kval2.m_strKey);
        }

        static tCIDLib::ESortComps
        eCompKey(const TString& strKey, const TKeyValuePair& kvalComp)
        {
            return strKey.eCompare(kvalComp.m_strKey);
        }

        static tCIDLib::ESortComps
        eCompKeyI(const TString& strKey, const TKeyValuePair& kvalComp)
        {
            return strKey.eCompareI(kvalComp.m_strKey);
        }

        static const TString& strExtractKey(const TKeyValuePair& kvalSrc)
        {
            return kvalSrc.m_strKey;
        }


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TKeyValuePair() = default;

        TKeyValuePair
        (
            const   TStringView&            strvKey
            , const TStringView&            strvValue
        );

        TKeyValuePair
        (
            const   TStringView&            strvKey
            , const tCIDLib::TCh            chValue
        );

        TKeyValuePair
        (
            const   TStringView&            strvKey
            , const TBoolean                bValue
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
        );

        TKeyValuePair
        (
            const   TStringView&            strvKey
            , const tCIDLib::TCard1         c1Value
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
        );

        TKeyValuePair
        (
            const   TStringView&            strvKey
            , const tCIDLib::TCard2         c2Value
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
        );

        TKeyValuePair
        (
            const   TStringView&            strvKey
            , const tCIDLib::TCard4         c4Value
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
        );

        TKeyValuePair
        (
            const   TStringView&            strvKey
            , const tCIDLib::TCard8         c8Value
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
        );

        TKeyValuePair
        (
            const   TStringView&            strvKey
            , const tCIDLib::TFloat4        f4Value
            , const tCIDLib::TCard4         c4DecDigs = 2
        );

        TKeyValuePair
        (
            const   TStringView&            strKey
            , const tCIDLib::TFloat8        f8Value
            , const tCIDLib::TCard4         c4DecDigs = 2
        );

        TKeyValuePair
        (
            const   TStringView&            strvKey
            , const tCIDLib::TInt1          i1Value
        );

        TKeyValuePair
        (
            const   TStringView&            strvKey
            , const tCIDLib::TInt2          i2Value
        );

        TKeyValuePair
        (
            const   TStringView&            strvKey
            , const tCIDLib::TInt4          i4Value
        );

        TKeyValuePair
        (
            const   TStringView&            strvKey
            , const tCIDLib::TInt8          i8Value
        );

        TKeyValuePair(const TKeyValuePair&) = default;
        TKeyValuePair(TKeyValuePair&&) = default;

        ~TKeyValuePair() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TKeyValuePair& operator=(const TKeyValuePair&) = default;
        TKeyValuePair& operator=(TKeyValuePair&&) = default;

        tCIDLib::TBoolean operator<
        (
            const   TKeyValuePair&          kvalSrc
        )   const;


        tCIDLib::TBoolean operator>
        (
            const   TKeyValuePair&          kvalSrc
        )   const;

        tCIDLib::TBoolean operator==
        (
            const   TKeyValuePair&          kvalSrc
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TKeyValuePair&          kvalSrc
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Clear();

        const TString& strKey() const;

        const TString& strKey
        (
            const   TStringView&            strvNewKey
        );

        TString& strValue();

        const TString& strValue() const;

        const TString& strValue
        (
            const   TStringView&            strvNewValue
        );

        tCIDLib::TVoid Set
        (
            const   TStringView&            strvNewKey
            , const TStringView&            strvNewValue
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatTo
        (
            CIOP    TTextOutStream&         strmDest
        )   const override;

        tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&           strmToReadFrom
        )   override;

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const override;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_strKey
        //      The string that represents the key of the pair
        //
        //  m_strValue
        //      The string that represents the value of the pair
        // -------------------------------------------------------------------
        TString m_strKey;
        TString m_strValue;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TKeyValuePair,TObject)
        DefPolyDup(TKeyValuePair)
};



// ---------------------------------------------------------------------------
//   CLASS: TKeyValFPair
//  PREFIX: kvalf
// ---------------------------------------------------------------------------
class CIDLIBEXP TKeyValFPair : public TKeyValuePair
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::ESortComps
        eCompKey(const TString& strKey, const TKeyValFPair& kvalfComp)
        {
            return strKey.eCompare(kvalfComp.strKey());
        }

        static tCIDLib::ESortComps
        eComp(const TKeyValFPair& kvalf1, const TKeyValFPair& kvalf2)
        {
            return kvalf1.strKey().eCompare(kvalf2.strKey());
        }

        static tCIDLib::ESortComps
        eCompKeyI(const TString& strKey, const TKeyValFPair& kvalfComp)
        {
            return strKey.eCompareI(kvalfComp.strKey());
        }

        static tCIDLib::ESortComps
        eCompI(const TKeyValFPair& kvalf1, const TKeyValFPair& kvalf2)
        {
            return kvalf1.strKey().eCompareI(kvalf2.strKey());
        }

        static const TString& strExtractKey(const TKeyValFPair& kvalfSrc)
        {
            return kvalfSrc.strKey();
        }


        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TKeyValFPair
        (
            const   tCIDLib::TBoolean       bFlag = kCIDLib::False
        );

        TKeyValFPair
        (
            const   TStringView&            strvKey
            , const TStringView&            strvValue
            , const tCIDLib::TBoolean       bFlag = kCIDLib::False
        );

        TKeyValFPair(const TKeyValFPair&) = default;
        TKeyValFPair(TKeyValFPair&&) = default;

        ~TKeyValFPair();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TKeyValFPair& operator=(const TKeyValFPair&) = default;
        TKeyValFPair& operator=(TKeyValFPair&&) = default;

        tCIDLib::TBoolean operator==
        (
            const   TKeyValFPair&           kvalfSrc
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TKeyValFPair&           kvalfSrc
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bFlag() const
        {
            return m_bFlag;
        }

        tCIDLib::TBoolean bFlag(const tCIDLib::TBoolean bToSet)
        {
            m_bFlag = bToSet;
            return m_bFlag;
        }

        tCIDLib::TVoid Set
        (
            const   TStringView&            strvNewKey
            , const TStringView&            strvNewValue
        );

        tCIDLib::TVoid Set
        (
            const   TStringView&            strvNewKey
            , const TStringView&            strvNewValue
            , const tCIDLib::TBoolean       bFlag
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&           strmToReadFrom
        )   final;

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const final;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bFlag
        //      A flag for use by client code for whatever
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bFlag;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TKeyValFPair, TKeyValuePair)
        DefPolyDup(TKeyValFPair)
};




// ---------------------------------------------------------------------------
//   CLASS: TKeyNumPair
//  PREFIX: knum
// ---------------------------------------------------------------------------
class CIDLIBEXP TKeyNumPair

    : public TObject, public MDuplicable, public MStreamable, public MFormattable
{
    public  :
        // -------------------------------------------------------------------
        //  Public static methods
        // -------------------------------------------------------------------
        static tCIDLib::ESortComps
        eComp(const TKeyNumPair& knum1, const TKeyNumPair& knum2)
        {
            return knum1.m_strKey.eCompare(knum2.m_strKey);
        }

        static tCIDLib::ESortComps
        eCompI(const TKeyNumPair& knum1, const TKeyNumPair& knum2)
        {
            return knum1.m_strKey.eCompareI(knum2.m_strKey);
        }

        static tCIDLib::ESortComps
        eCompKey(const TString& strKey, const TKeyNumPair& knumComp)
        {
            return strKey.eCompare(knumComp.m_strKey);
        }

        static tCIDLib::ESortComps
        eCompKeyI(const TString& strKey, const TKeyNumPair& knumComp)
        {
            return strKey.eCompareI(knumComp.m_strKey);
        }

        static const TString& strExtractKey(const TKeyNumPair& knumSrc)
        {
            return knumSrc.m_strKey;
        }


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TKeyNumPair() = default;

        TKeyNumPair
        (
            const   TStringView&            strvKey
            , const tCIDLib::TFloat4        f4Value
        );

        TKeyNumPair
        (
            const   TStringView&            strvKey
            , const tCIDLib::TFloat4        f4Value
            , const TStringView&            strValue
        );

        TKeyNumPair(const TKeyNumPair&) = default;
        TKeyNumPair(TKeyNumPair&&) = default;

        ~TKeyNumPair() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TKeyNumPair& operator=(const TKeyNumPair&) = default;
        TKeyNumPair& operator=(TKeyNumPair&&) = default;

        tCIDLib::TBoolean operator<
        (
            const   TKeyNumPair&            knumSrc
        )   const;

        tCIDLib::TBoolean operator>
        (
            const   TKeyNumPair&            knumSrc
        )   const;

        tCIDLib::TBoolean operator==
        (
            const   TKeyNumPair&            knumSrc
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TKeyNumPair&            knumSrc
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Clear();

        const TString& strKey() const;

        const TString& strKey
        (
            const   TStringView&            strvNewKey
        );

        tCIDLib::TFloat4 f4Value() const;

        tCIDLib::TFloat4 f4Value
        (
            const   tCIDLib::TFloat4        f4ToSet
        );

        const TString& strValue() const;

        const TString& strValue
        (
            const   TStringView&            strvToSet
        );

        tCIDLib::TVoid Set
        (
            const   TStringView&            strvNewKey
            , const tCIDLib::TFloat4        f4Value
        );

        tCIDLib::TVoid Set
        (
            const   TStringView&            strvNewKey
            , const tCIDLib::TFloat4        f4Value
            , const TStringView&            strvValue
        );

    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatTo
        (
            CIOP    TTextOutStream&         strmDest
        )   const final;

        tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&           strmToReadFrom
        )   final;

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const final;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_f4Value
        //      The numeric value
        //
        //  m_strKey
        //      The string that represents the key of the pair
        //
        //  m_strValue
        //      A string value that can be set as well.
        // -------------------------------------------------------------------
        tCIDLib::TFloat4    m_f4Value = 0;
        TString             m_strKey;
        TString             m_strValue;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TKeyNumPair,TObject)
        DefPolyDup(TKeyNumPair)
};





// ---------------------------------------------------------------------------
//   CLASS: TKeyValues
//  PREFIX: kvals
// ---------------------------------------------------------------------------
class CIDLIBEXP TKeyValues

    : public TObject, public MDuplicable, public MStreamable, public MFormattable
{
    public  :
        // -------------------------------------------------------------------
        //  Public static methods
        // -------------------------------------------------------------------
        static tCIDLib::ESortComps
        eComp(const TKeyValues& kvals1, const TKeyValues& kvals2)
        {
            return kvals1.m_strKey.eCompare(kvals2.m_strKey);
        }

        static tCIDLib::ESortComps
        eCompI(const TKeyValues& kvals1, const TKeyValues& kvals2)
        {
            return kvals1.m_strKey.eCompareI(kvals2.m_strKey);
        }

        static tCIDLib::ESortComps
        eCompKey(const TString& strKey, const TKeyValues& kvalsComp)
        {
            return strKey.eCompare(kvalsComp.m_strKey);
        }

        static tCIDLib::ESortComps
        eCompKeyI(const TString& strKey, const TKeyValues& kvalsComp)
        {
            return strKey.eCompareI(kvalsComp.m_strKey);
        }

        static const TString& strExtractKey(const TKeyValues& kvalsSrc)
        {
            return kvalsSrc.m_strKey;
        }


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TKeyValues() = default;

        TKeyValues
        (
            const   TStringView&                strvKey
            , const TStringView&                strvVal1
        );

        TKeyValues
        (
            const   TStringView&                strvKey
            , const TStringView&                strvVal1
            , const TStringView&                strvVal2
        );

        TKeyValues
        (
            const   TStringView&                strvKey
            , const TStringView&                strvVal1
            , const TStringView&                strvVal2
            , const TStringView&                strvVal3
        );

        TKeyValues(const TKeyValues&) = default;
        TKeyValues(TKeyValues&&) = default;

        ~TKeyValues() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TKeyValues& operator=(const TKeyValues&) = default;
        TKeyValues& operator=(TKeyValues&&) = default;

        tCIDLib::TBoolean operator<
        (
            const   TKeyValues&             kvalSrc
        )   const;

        tCIDLib::TBoolean operator>
        (
            const   TKeyValues&             kvalSrc
        )   const;

        tCIDLib::TBoolean operator==
        (
            const   TKeyValues&             kvalSrc
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TKeyValues&             kvalSrc
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Clear();

        const TString& strKey() const;

        const TString& strKey
        (
            const   TStringView&            strvNewKey
        );

        const TString& strVal1() const;

        const TString& strVal1
        (
            const   TStringView&            strvNewValue
        );

        const TString& strVal2() const;

        const TString& strVal2
        (
            const   TStringView&            strvNewValue
        );

        const TString& strVal3() const;

        const TString& strVal3
        (
            const   TStringView&            strvNewValue
        );

        tCIDLib::TVoid Set
        (
            const   TStringView&            strvNewKey
            , const TStringView&            strvVal1
        );

        tCIDLib::TVoid Set
        (
            const   TStringView&            strvNewKey
            , const TStringView&            strvVal1
            , const TStringView&            strvVal2
        );

        tCIDLib::TVoid Set
        (
            const   TStringView&            strvNewKey
            , const TStringView&            strvVal1
            , const TStringView&            strvVal2
            , const TStringView&            strvVal3
        );

    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatTo
        (
            CIOP    TTextOutStream&         strmDest
        )   const final;

        tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&           strmToReadFrom
        )   final;

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const final;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_strKey
        //      The string that represents the key of the pair
        //
        //  m_strValX
        //      These are the available values to associate with this key.
        // -------------------------------------------------------------------
        TString m_strKey;
        TString m_strVal1;
        TString m_strVal2;
        TString m_strVal3;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TKeyValues,TObject)
        DefPolyDup(TKeyValues)
};



// ---------------------------------------------------------------------------
//   CLASS: TKeyObjPair
//  PREFIX: kobj
// ---------------------------------------------------------------------------
template <typename TKey, class TValue> class TKeyObjPair : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Public static methods
        // -------------------------------------------------------------------
        static const TClass& clsThis()
        {
            static const TClass* pclsThis = nullptr;
            if (!pclsThis)
            {
                TBaseLock lockInit;
                if (!pclsThis)
                    pclsThis = new TClass(L"TKeyObjPair<TKey,TValue>");
            }
            return *pclsThis;
        }

        static const TKey& objExtractKey(const TKeyObjPair& kobjSrc)
        {
            return kobjSrc.objKey();
        }


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TKeyObjPair() = default;

        TKeyObjPair(const TKey& objKey, const TValue& objValue) :

            m_objKey(objKey)
            , m_objValue(objValue)
        {
        }

        TKeyObjPair(const TKeyObjPair&) = default;
        TKeyObjPair(TKeyObjPair&&) = default;

        ~TKeyObjPair() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        tCIDLib::TBoolean operator<(const TKeyObjPair& kobjSrc) const
        {
            return (m_objKey < kobjSrc.m_objKey);
        }

        tCIDLib::TBoolean operator>(const TKeyObjPair& kobjSrc) const
        {
            return (m_objKey > kobjSrc.m_objKey);
        }

        tCIDLib::TBoolean operator==(const TKeyObjPair& kobjSrc) const
        {
            return
            (
                (m_objKey == kobjSrc.m_objKey)
                && (m_objValue == kobjSrc.m_objValue)
            );
        }

        tCIDLib::TBoolean operator!=(const TKeyObjPair& kobjSrc) const
        {
            return !operator==(kobjSrc);
        }

        TKeyObjPair& operator=(const TKeyObjPair&) = default;
        TKeyObjPair& operator=(TKeyObjPair&&) = default;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsDescendantOf(const TClass& clsTarget) const final
        {
            if (clsTarget == clsThis())
                return kCIDLib::True;
            return TObject::bIsDescendantOf(clsTarget);
        }

        const TClass& clsIsA() const final
        {
            return clsThis();
        }

        const TClass& clsParent() const final
        {
            return TObject::clsThis();
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TKey& objKey() const
        {
            return m_objKey;
        }

        TKey& objKey()
        {
            return m_objKey;
        }

        const TKey& objKey(const TKey& objNewKey)
        {
            m_objKey = objNewKey;
            return m_objKey;
        }

        const TValue& objValue() const
        {
            return m_objValue;
        }

        TValue& objValue()
        {
            return m_objValue;
        }

        const TValue& objValue(const TValue& objNew)
        {
            m_objValue = objNew;
            return m_objValue;
        }


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_objKey
        //      The object that represents the key of the pair
        //
        //  m_objValue
        //      The object that represents the value of the pair
        // -------------------------------------------------------------------
        TKey    m_objKey;
        TValue  m_objValue;
};


//
//  We provide global streaming operators for the TKeyObjPair class since we
//  cannot assume streamability for it's instantiation types.
//
template <typename TKey, class TValue> TBinOutStream&
operator<<(TBinOutStream& strmToWriteTo, const TKeyObjPair<TKey,TValue>& objToWrite)
{
    strmToWriteTo << objToWrite.objKey() << objToWrite.objValue();
    return strmToWriteTo;
}

template <typename TKey, class TValue> TBinInStream&
operator>>(TBinInStream& strmToReadFrom, TKeyObjPair<TKey,TValue>& objToFill)
{
    strmToReadFrom >> objToFill.objKey() >> objToFill.objValue();
    return strmToReadFrom;
}

#pragma CIDLIB_POPPACK


