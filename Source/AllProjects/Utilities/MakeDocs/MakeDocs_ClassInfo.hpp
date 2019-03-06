//
// FILE NAME: MakeDocs_ClassInfo.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/10/1998
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
//  This is the header for the MakeDocs_ClassInfo.cpp file. This file
//  implements a simple class that maintains information about each class.
//  The facility class maintains a list of all classes and stores information
//  about them in these objects. It is designed to be easily used in a hashed
//  set collection, with the class name being the key.
//
//  Also implemented here is the TClassDocInfoKeyOps class, which is a very
//  simple TKeyOps derivative to allow TClassDocInfo objects to be used in a
//  hashed set.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//

// ----------------------------------------------------------------------------
//  CLASS: TClassDocInfo
// PREFIX: cldi
// ----------------------------------------------------------------------------
class TClassDocInfo
{
    public :
        // --------------------------------------------------------------------
        //  Public, static methods
        // --------------------------------------------------------------------
        static const TString& strGetKey(const TClassDocInfo& cldiSrc)
        {
            return cldiSrc.m_strClassName;
        }


        // --------------------------------------------------------------------
        //  Constructors and Destructors.
        // --------------------------------------------------------------------
        TClassDocInfo();

        TClassDocInfo
        (
            const   TString&                strClassName
        );

        TClassDocInfo
        (
            const   TClassDocInfo&          cldiToCopy
        );

        ~TClassDocInfo();


        // --------------------------------------------------------------------
        //  Public, non-virtual methods
        // --------------------------------------------------------------------
        tCIDLib::TVoid AddCategory
        (
            const   tMakeDocs::EClassCats   eToAdd
        );

        tCIDLib::TBoolean bIsInCategory
        (
            const   tMakeDocs::EClassCats   eToCheck
        )   const;

        const TString& strClassName() const;

        const TString& strClassName
        (
            const   TString&                strNewName
        );

        const TString& strParentClass() const;

        const TString& strParentClass
        (
            const   TString&                strNewName
        );


    private :
        typedef TFundArray<tCIDLib::TBoolean, tMakeDocs::EClassCats>    TCatFlagList;

        // --------------------------------------------------------------------
        //  Private, non-virtual methods
        // --------------------------------------------------------------------
        tCIDLib::TVoid CommonInit();


        // --------------------------------------------------------------------
        //  Private data members
        //
        //  m_fcolCategories
        //      A list of categories that this class belongs to. This is used
        //      to create categorized pages of classes. For each category this
        //      class belongs to, a True value is in that slot.
        //
        //  m_strClassName
        //      The name of the class that is represented by this object.
        //
        //  m_strParentClass
        //      The name of the class that is the parent of this class.
        // --------------------------------------------------------------------
        TCatFlagList    m_fcolCategories;
        TString         m_strClassName;
        TString         m_strParentClass;
};



// ----------------------------------------------------------------------------
//  TClassDocInfo: Public non-virtual methods
// ----------------------------------------------------------------------------
inline tCIDLib::TVoid
TClassDocInfo::AddCategory(const tMakeDocs::EClassCats eToAdd)
{
    m_fcolCategories[eToAdd] = kCIDLib::True;
}

inline tCIDLib::TBoolean
TClassDocInfo::bIsInCategory(const tMakeDocs::EClassCats eToCheck) const
{
    return m_fcolCategories[eToCheck];
}

inline const TString& TClassDocInfo::strClassName() const
{
    return m_strClassName;
}

inline const TString& TClassDocInfo::strClassName(const TString& strNewName)
{
    m_strClassName = strNewName;
    return m_strClassName;
}

inline const TString& TClassDocInfo::strParentClass() const
{
    return m_strParentClass;
}

inline const TString& TClassDocInfo::strParentClass(const TString& strNewName)
{
    m_strParentClass = strNewName;
    return m_strParentClass;
}


