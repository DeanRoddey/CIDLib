//
// FILE NAME: CIDBuild_KeyValuePair.Hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/01/1998
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This is the header implements the very simple TKeyValuePair class which
//  is used internally in a number of places to store elements composed of
//  one key string and one value string.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//   CLASS: TKeyValuePair
//  PREFIX: kvp
// ---------------------------------------------------------------------------
class TKeyValuePair
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Operators
        // -------------------------------------------------------------------
        TKeyValuePair(  const   TBldStr&    strKey
                        , const TBldStr&    strValue) :

            m_strKey(strKey)
            , m_strValue(strValue)
        {
        }

        ~TKeyValuePair()
        {
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TBldStr& strKey() const
        {
            return m_strKey;
        }

        const TBldStr& strValue() const
        {
            return m_strValue;
        }

        tCIDLib::TVoid SetValue(const TBldStr&  strValue)
        {
            m_strValue = strValue;
        }


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_strKey
        //      The key string for this element.
        //
        //  m_strValue
        //      The value string for this element.
        // -------------------------------------------------------------------
        TBldStr m_strKey;
        TBldStr m_strValue;
};

