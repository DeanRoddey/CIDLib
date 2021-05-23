//
// FILE NAME: CIDLib_PathString.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/12/1993
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

        explicit TPathStr
        (
            const   tCIDLib::TCard4         c4InitSize
        );

        TPathStr
        (
            const   TStringView&            strvInit
        );

        TPathStr
        (
            const   TString&                strInit
        );

        TPathStr
        (
            const   tCIDLib::TCh* const     pszInit
        );

        TPathStr
        (
            const   TStringView&            strvFirstPart
            , const TStringView&            strvSecondPart
        );

        TPathStr(const TPathStr&) = default;
        TPathStr(TPathStr&&) = default;

        ~TPathStr();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TPathStr& operator=(const TPathStr&) = default;
        TPathStr& operator=(TPathStr&&) = default;

        tCIDLib::TBoolean operator!=(const TPathStr& pathSrc) const
        {
            return TString::operator!=(pathSrc);
        }

        tCIDLib::TBoolean operator==(const TPathStr& pathSrc) const
        {
            return TString::operator==(pathSrc);
        }

        TPathStr& operator=(const TString& strSrc)
        {
            TParent::operator=(strSrc);
            return *this;
        }

        TPathStr& operator=(const TStringView& strvSrc)
        {
            TParent::operator=(strvSrc);
            return *this;
        }

        TPathStr& operator=(const tCIDLib::TCh* const pszSrc)
        {
            TParent::operator=(pszSrc);
            return *this;
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddLevel
        (
            const   TStringView&            strNewLevel
        );

        tCIDLib::TVoid AddLevels
        (
            const   TStringView&            strNewLevel1
            , const TStringView&            strNewLevel2
        );

        tCIDLib::TVoid AddLevels
        (
            const   TStringView&            strNewLevel1
            , const TStringView&            strNewLevel2
            , const TStringView&            strNewLevel3
        );

        tCIDLib::TVoid AddLevels
        (
            const   TStringView&            strNewLevel1
            , const TStringView&            strNewLevel2
            , const TStringView&            strNewLevel3
            , const TStringView&            strNewLevel4
        );

        tCIDLib::TVoid AddToBasePath
        (
            const   TStringView&            strBasePath
        );

        tCIDLib::TVoid AddTrailingSeparator();

        tCIDLib::TVoid AppendExt
        (
            const   TStringView&            strvExt
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

