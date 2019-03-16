//
// FILE NAME: CIDLib_PathString.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/12/1993
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
//  This is the header for the CIDLib_PathStr.Cpp file. This file implements
//  the TPathStr class, which is a derivative of TString that knows how to
//  interpret its contents as a file system path. It is fully interopable with
//  strings but is far more convenient when dealing with path names.
//
// CAVEATS/GOTCHAS:
//
//  1)  This class, and the underlying kernel level methods it calls must assume
//      the backslash as the path separator. This is our portable path format.
//      Platforms that don't use it must convert as required when calling down into
//      the virtual kernel level and passing paths on to OS calls.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//  CLASS: TPathStr
// PREFIX: path
// ---------------------------------------------------------------------------
class CIDLIBEXP TPathStr : public TString
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor.
        // -------------------------------------------------------------------
        TPathStr();

        TPathStr
        (
            const   tCIDLib::TCh*           pszInitValue
        );

        explicit TPathStr
        (
            const   tCIDLib::TCard4         c4InitSize
        );

        TPathStr
        (
            const   TString&                strInitComp
        );

        TPathStr
        (
            const   TString&                strFirstPart
            , const TString&                strSecondPart
        );

        TPathStr(const TPathStr&) = default;

        ~TPathStr();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TPathStr& operator=(const TPathStr& pathSrc) = default;

        TPathStr& operator=(const TString& strSrc)
        {
            TString::operator=(strSrc);
            return *this;
        }

        TPathStr& operator=(const tCIDLib::TCh* const pszSrc)
        {
            TString::operator=(pszSrc);
            return *this;
        }

        tCIDLib::TBoolean operator==(const TString& strSrc) const
        {
            return TString::operator==(strSrc);
        }

        tCIDLib::TBoolean operator==(const tCIDLib::TCh* const pszSrc) const
        {
            return TString::operator==(pszSrc);
        }

        tCIDLib::TBoolean operator==(const TPathStr& pathSrc) const
        {
            return TString::operator==(pathSrc);
        }

        tCIDLib::TBoolean operator!=(const TString& strSrc) const
        {
            return TString::operator!=(strSrc);
        }

        tCIDLib::TBoolean operator!=(const tCIDLib::TCh* const pszSrc) const
        {
            return TString::operator!=(pszSrc);
        }

        tCIDLib::TBoolean operator!=(const TPathStr& pathSrc) const
        {
            return TString::operator!=(pathSrc);
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddLevel
        (
            const   TString&                strNewLevel
        );

        tCIDLib::TVoid AddLevels
        (
            const   TString&                strNewLevel1
            , const TString&                strNewLevel2
        );

        tCIDLib::TVoid AddLevels
        (
            const   TString&                strNewLevel1
            , const TString&                strNewLevel2
            , const TString&                strNewLevel3
        );

        tCIDLib::TVoid AddToBasePath
        (
            const   TString&                strBasePath
        );

        tCIDLib::TVoid AddTrailingSeparator();

        tCIDLib::TVoid AppendExt
        (
            const   TString&                strExt
        );

        tCIDLib::TBoolean bExtractExt
        (
                    TString&                strToFill
        );

        tCIDLib::TBoolean bExtractNameExt
        (
                    TString&                strToFill
        );

        tCIDLib::TBoolean bExtractPath
        (
                    TString&                strToFill
        );

        tCIDLib::TBoolean bIsFullyQualified() const;

        tCIDLib::TBoolean bIsNormalDir() const;

        tCIDLib::TBoolean bHasExt() const;

        tCIDLib::TBoolean bHasName() const;

        tCIDLib::TBoolean bHasPath() const;

        tCIDLib::TBoolean bHasVolume() const;

        tCIDLib::TBoolean bHasWildCards() const;

        tCIDLib::TBoolean bQueryExt
        (
                    TString&                strToFill
        )   const;

        tCIDLib::TBoolean bQueryName
        (
                    TString&                strToFill
        )   const;

        tCIDLib::TBoolean bQueryNameExt
        (
                    TString&                strToFill
        )   const;

        tCIDLib::TBoolean bQueryPath
        (
                    TString&                strToFill
        )   const;

        tCIDLib::TBoolean bQueryVolume
        (
                    TString&                strToFill
        )   const;

        tCIDLib::TBoolean bRemoveExt();

        tCIDLib::TBoolean bRemoveLevel();

        tCIDLib::TBoolean bRemoveName();

        tCIDLib::TBoolean bRemoveNameExt();

        tCIDLib::TBoolean bRemovePath();

        tCIDLib::TBoolean bRemoveTrailingSeparator();

        tCIDLib::TVoid Normalize();


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TPathStr,TString)
        DefPolyDup(TPathStr)
};

#pragma CIDLIB_POPPACK

