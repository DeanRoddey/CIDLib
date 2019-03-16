//
// FILE NAME: CIDMacroEng_ClassInfo.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/12/2003
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
//  This is the header file for the CIDMacroEng_ClassInfo.cpp file, which
//  implements the TMEngClassInfo class. This class represents a defined
//  class in the macro system. Classes must be registered with the engine
//  instance in order to make them availble for use macros. One of the things
//  provided by the class info is a factory function to create a value object,
//  derived from a base value class, that can hold values of that type.
//
//  A class, like most other entities, must exist within a namespace, which is
//  a hierarchical space that is used to provide disambiguation both in usage
//  within a macro and on disk to make it easy to link an imported class to
//  some storage on disk or in some other storage. All paths start with
//  "MEng" and can create a hierarchy underneath that.
//
//  This is an abstract base class, so it must be derived from to create the
//  actual class info. Note that you must also derive a value class to
//  go along with your class info class, in order to provide storage for
//  values of that type.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)


// ---------------------------------------------------------------------------
//  CLASS: TMEngClassInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngClassInfo : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Public class types
        // -------------------------------------------------------------------
        using TLiteralList = TRefKeyedHashSet<TMEngLiteralVal,TString,TStringKeyOps>;


        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strKey(const TMEngClassInfo& meciSrc);


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngClassInfo() = delete;

        TMEngClassInfo(const TMEngClassInfo&) = delete;

        ~TMEngClassInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngClassInfo& operator=(const TMEngClassInfo&) = delete;


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bCanCastFrom
        (
            const   tCIDLib::TCard2         c2SrcId
        );

        virtual tCIDMacroEng::ECastRes eCastFrom
        (
                    TCIDMacroEngine&        meOwner
            , const TMEngClassVal&          mecvSrc
            ,       TMEngClassVal&          mecvTarget
        );

        virtual tCIDLib::TVoid Init
        (
                    TCIDMacroEngine&        meOwner
        );

        virtual TMEngClassVal* pmecvMakeStorage
        (
            const   TString&                strName
            ,       TCIDMacroEngine&        meOwner
            , const tCIDMacroEng::EConstTypes  eConst
        )   const = 0;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddDirective
        (
            const   TString&                strName
            , const TString&                strValue
        );

        tCIDLib::TVoid AddLiteral
        (
                    TMEngLiteralVal* const  pmelvToAdopt
        );

        tCIDLib::TVoid AddMethodImpl
        (
                    TMEngMethodImpl* const  pmethiToAdopt
        );

        tCIDLib::TBoolean bAddClassImport
        (
            const   TString&                strClassPath
        );

        tCIDLib::TBoolean bCheckDupName
        (
                    TCIDMacroEngine&        meOwner
            , const TString&                strToCheck
        )   const;

        tCIDLib::TBoolean bFindDirective
        (
            const   TString&                strToFind
            ,       TString&                strToFill
        );

        tCIDLib::TBoolean bHasDefCtor() const
        {
            return (m_c2DefCtorId != kMacroEng::c2BadId);
        }

        tCIDLib::TBoolean bHasMember
        (
            const   TString&                strNameToCheck
            , const tCIDLib::TBoolean       bThisClassOnly
        )   const;

        tCIDLib::TBoolean bHasMethod
        (
            const   TString&                strNameToCheck
        )   const;

        tCIDLib::TBoolean bImportsClass(const TString& strClassPath) const
        {
            return m_colImports.bKeyExists(strClassPath);
        }

        tCIDLib::TBoolean bIsCopyable() const
        {
            return m_bCopyable;
        }

        tCIDLib::TBoolean bAddNestedType
        (
            const   TString&                strClassPath
        );

        tCIDLib::TVoid BaseClassInit
        (
                    TCIDMacroEngine&        meOwner
        );

        tCIDLib::TCard2 c2AddMember
        (
            const   TMEngMemberInfo&        memiToAdd
            , const TMEngClassInfo&         meciNew
        );

        tCIDLib::TCard2 c2AddMethodInfo
        (
            const   TMEngMethodInfo&        memethiToAdd
        );

        tCIDLib::TCard2 c2DefCtorId() const
        {
            return m_c2DefCtorId;
        }

        tCIDLib::TCard2 c2FindMember
        (
            const   TString&                strName
            , const tCIDLib::TBoolean       bThisClassOnly
        )   const;

        tCIDLib::TCard2 c2FindMethod
        (
            const   TString&                strName
        )   const;

        tCIDLib::TCard2 c2FirstMemberId() const;

        tCIDLib::TCard2 c2FirstMethodId() const;

        tCIDLib::TCard2 c2Id() const;

        tCIDLib::TCard2 c2MemberId
        (
            const   TString&                strName
        )   const;

        tCIDLib::TCard2 c2MethodId
        (
            const   TString&                strName
        )   const;

        tCIDLib::TCard2 c2ParentClassId() const;

        tCIDLib::TCard4 c4CtorCount() const;

        tCIDLib::TCard4 c4GetCtorParmList
        (
                    tCIDMacroEng::TParmIdTable& pitToFill
        )   const;

        tCIDLib::TCard4 c4MemberCount() const
        {
            return m_colMembers.c4ElemCount();
        }

        tCIDLib::TCard4 c4MethodCount() const
        {
            return m_colMethods.c4ElemCount();
        }

        const TLiteralList::TCursor cursLiterals() const;

        tCIDMacroEng::EClassExt eExtend() const;

        tCIDMacroEng::EClassExt eExtend
        (
            const   tCIDMacroEng::EClassExt    eToSet
        );

        tCIDLib::TVoid ExportTo
        (
                    TCIDMacroEngine&        meOwner
            ,       TMEngClassInfo&         meciTarget
        )   const;

        tCIDLib::TVoid Invoke
        (
                    TCIDMacroEngine&        meOwner
            ,       TMEngClassVal&          mecvInstance
            , const tCIDLib::TCard2         c2MethodId
            , const tCIDMacroEng::EDispatch    eDispatch
        )   const;

        const TMEngMemberInfo& memiFind(const tCIDLib::TCard2 c2Id) const
        {
            #if CID_DEBUG_ON
            CheckMemberId(c2Id, CID_LINE);
            #endif
            return *m_colMembersById[c2Id];
        }

        TMEngMemberInfo& memiFind(const tCIDLib::TCard2 c2Id)
        {
            #if CID_DEBUG_ON
            CheckMemberId(c2Id, CID_LINE);
            #endif
            return *m_colMembersById[c2Id];
        }

        const TMEngMethodInfo& methiFind(const tCIDLib::TCard2 c2Id) const
        {
            #if CID_DEBUG_ON
            CheckMethodId(c2Id, CID_LINE);
            #endif
            return *m_colMethodsById[c2Id];
        }

        TMEngMethodInfo& methiFind(const tCIDLib::TCard2 c2Id)
        {
            #if CID_DEBUG_ON
            CheckMethodId(c2Id, CID_LINE);
            #endif
            return *m_colMethodsById[c2Id];
        }

        const TMEngLiteralVal* pmelvFind
        (
            const   TString&                strName
            ,       TCIDMacroEngine&        meOwner
            , const tCIDLib::TBoolean       bRecurse
        )   const;

        const TMEngMemberInfo* pmemiFind
        (
            const   TString&                strName
            , const tCIDLib::TBoolean       bThisClassOnly
        )   const;

        TMEngMemberInfo* pmemiFind
        (
            const   TString&                strName
            , const tCIDLib::TBoolean       bThisClassOnly
        );

        const TMEngMethodImpl* pmethDefCtor() const;

        const TMEngMethodInfo* pmethiDefCtor() const;

        const TMEngMethodImpl* pmethFind
        (
            const   tCIDLib::TCard2         c2Id
        )   const;

        TMEngMethodImpl* pmethFind
        (
            const   tCIDLib::TCard2         c2Id
        );

        const TMEngMethodImpl* pmethFind
        (
            const   TString&                strName
        )   const;

        TMEngMethodImpl* pmethFind
        (
            const   TString&                strName
        );

        TMEngMethodInfo* pmethiFind(const TString& strName)
        {
            return m_colMethods.pobjFindByKey(strName);
        }

        const TMEngMethodInfo* pmethiFind(const TString& strName) const
        {
            return m_colMethods.pobjFindByKey(strName);
        }

        const TString& strBasePath() const
        {
            return m_strBasePath;
        }

        const TString& strClassPath() const
        {
            return m_strClassPath;
        }

        const TString& strName() const
        {
            return m_strName;
        }

        const TString& strParentClassPath() const
        {
            return m_strParentClassPath;
        }

        tCIDLib::TVoid SetToNested
        (
            const   TString&                strClassPath
        );


    protected :
        // -------------------------------------------------------------------
        //  The facility class is our friend so that he can call SetTypeId
        //  when we are registered.
        // -------------------------------------------------------------------
        friend class TCIDMacroEngine;
        friend class TMacroEngParser;


        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TMEngClassInfo
        (
            const   TString&                strName
            , const TString&                strBasePath
            ,       TCIDMacroEngine&        meOwner
            , const tCIDLib::TBoolean       bCopyable
            , const tCIDMacroEng::EClassExt    eExtend
            , const TString&                strParentClass
        );

        TMEngClassInfo
        (
            const   TString&                strName
            , const TString&                strBasePath
            ,       TCIDMacroEngine&        meOwner
            , const tCIDLib::TBoolean       bCopyable
            , const tCIDMacroEng::EClassExt    eExtend
            , const tCIDLib::TCh* const     pszParentClass
        );


        // -------------------------------------------------------------------
        //  Protected, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bInvokeMethod
        (
                    TCIDMacroEngine&        meOwner
            , const TMEngMethodInfo&        methiTarget
            ,       TMEngClassVal&          mecvInstance
        ) = 0;


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid LoadMemberVals
        (
                    TMEngClassVal&          mecvToLoad
            ,       TCIDMacroEngine&        meOwner
        )   const;

        tCIDLib::TVoid SetClassId
        (
            const   tCIDLib::TCard2         c2ToSet
        );


    private :
        // -------------------------------------------------------------------
        //  Private data types and constants
        // -------------------------------------------------------------------
        class TImportItem
        {
            public :
                // -----------------------------------------------------------
                //  Public, static methods
                // -----------------------------------------------------------
                static const TString& strKey(const TImportItem& itemSrc);


                // -----------------------------------------------------------
                //  Constructors and Destructor
                // -----------------------------------------------------------
                TImportItem
                (
                    const   TString&            strImport
                    , const tCIDLib::TBoolean   bNested
                );

                TImportItem
                (
                    const   TImportItem&        itemToCopy
                );

                ~TImportItem();

                tCIDLib::TVoid operator=
                (
                    const   TImportItem&        itemToAssign
                );


                tCIDLib::TBoolean   m_bNested;
                TString             m_strImport;
        };

        using TDirectiveList    = tCIDLib::TKVHashSet;
        using TMemberList       = TKeyedHashSet<TMEngMemberInfo,TString,TStringKeyOps>;
        using TMethodList       = TKeyedHashSet<TMEngMethodInfo,TString,TStringKeyOps>;
        using TMethodImplList   = TRefVector<TMEngMethodImpl>;
        using TMethodIdList     = TRefVector<TMEngMethodInfo>;
        using TMemberIdList     = TRefVector<TMEngMemberInfo>;
        using TImportList       = TKeyedHashSet<TImportItem,TString,TStringKeyOps>;


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CheckMethodId
        (
            const   tCIDLib::TCard2         c2Id
            , const tCIDLib::TCard4         c4Line
        )   const;

        tCIDLib::TVoid CheckMemberId
        (
            const   tCIDLib::TCard2         c2Id
            , const tCIDLib::TCard4         c4Line
        )   const;

        tCIDLib::TVoid ValidateCallFrame
        (
            const   tCIDLib::TCard2         c2MethodId
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bCopyable
        //      All classes are either copyable or not. A class is copyable if it and
        //      it's ancestor classes only have members that are themselves of copyable
        //      classes. All of the intrinsic classes are copyable, so from there on
        //      out, any classes that are composed only of those types or other types
        //      made up of those types, are copyable.
        //
        //      This flag is initially set when the parent class is set in the ctor,
        //      and updated each time a member is added. We don't want to have to do
        //      that whole recursion at runtime each time someone wants to know whether
        //      this class is copyable or not.
        //
        //  m_c2DefCtorId
        //      We watch the methods added and if we see a default ctor, we'll store its
        //      id here. Default ctors are looked up for a number of reasons, so knowing
        //      if a class has one and finding it quickly is a good efficiency tweak. If
        //      it doesn't have one, then this is kMacroEng::c2BadId;
        //
        //  m_c2FirstMemberId
        //  m_c2FirstMethodId
        //      The ids of the first member and method of our own, as apposed to those
        //      we inherited from our parent.
        //
        //  m_c2Id
        //      Each registered type is given a unique id for the invocation, so that
        //      opcodes can store type ids.
        //
        //  m_c2NextMemberId
        //  m_c2NextMethodId
        //      These are used to assign unique (within this unit) ids to methods and
        //      members.
        //
        //  m_c2ParentClassId
        //      The class id of the parent class of this class.
        //
        //  m_colDirectives
        //      A list of key/value pairs that holds any directives that were
        //      set on this class.
        //
        //  m_colImports
        //      The list of classes that this class imports, and any nested types it
        //      defines. This limits the visibility of external things that we have
        //      access to. We can only access our own stuff and that of any classes
        //      that we import. We store the full class paht  of the class being
        //      imported. We also store our nested types, which are marked as such.
        //      This list is also used, when a class imports us, to implicitly all
        //      our nested types.
        //
        //  m_colLiterals
        //      These are just like members except that they are inherently constant
        //      and can only be of fundamental type (char, numeric, string), and are
        //      visible to the outside world directly. Since they are literals, they
        //      are just compiled directly into code as constants, so there are no
        //      linkage issues. They are initialized right in place, instead of in
        //      any constructor.
        //
        //  m_colMembers
        //  m_colMembersById
        //      The members that were defined for this class. They are represented by
        //      member info objects. We also have a non-adopting ref vector that we
        //      use to provide 'by id' fast access. Since the ids are assigned as
        //      ascending indices, they become an index into this list (after
        //      subtracting 1, since the ids are 1 based.)
        //
        //  m_colMethods
        //  m_colMethodsById
        //      The list of methods that make up this class. They are keyed on their
        //      relative name. We also have a non-adopting ref vector that we use to
        //      provide 'by id' fast access. Since the ids are assigned as ascending
        //      indices, they become an index into this list (after subtracting 1,
        //      since the ids are 1 based.)
        //
        //  m_colMethodImpls
        //      The m_colMethods collection contains the descriptions of the methods.
        //      This collection contains the actual implementation objects for the
        //      methods. These contain the id of their related method info object, so
        //      we find these by just brute force iteration looking for a desired id.
        //
        //  m_eExtend
        //      The type of extension allowed for this class, at this class level.
        //      Defaults to non-final to until set otherwise.
        //
        //  m_strBasePath
        //      The base part of the path of this class, i.e. without the last component
        //      that represents this actual class. There is often a need to do lookups
        //      based on the based part of the class' path so we want a pre-fab string
        //      set up for it.
        //
        //  m_strClassPath
        //      The class path of this class, which would be something like
        //      "MEng.System.Runtime.Audio", i.e. it's a hierarchical path with the
        //      highest level of containment at the left, and the final part being the
        //      same as the m_strName, i.e. the actual name of this class. MEng must be
        //      the initial component, since it is the root of all classes.
        //
        //  m_strName
        //      The name of this class, which would be something like TextOutStream or
        //      BinaryOutFile. This will be the last component of the class path.
        //
        //  m_strParentClassPath
        //      The path of the class that this class derives from. This is what is
        //      initially set during ctor. It will subsequently be looked up and our
        //      InitClass() method called to complete the setup, and m_c2ParentClassId
        //      set.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bCopyable;
        tCIDLib::TCard2         m_c2DefCtorId;
        tCIDLib::TCard2         m_c2FirstMemberId;
        tCIDLib::TCard2         m_c2FirstMethodId;
        tCIDLib::TCard2         m_c2Id;
        tCIDLib::TCard2         m_c2NextMemberId;
        tCIDLib::TCard2         m_c2NextMethodId;
        tCIDLib::TCard2         m_c2ParentClassId;
        TDirectiveList          m_colDirectives;
        TImportList             m_colImports;
        TLiteralList            m_colLiterals;
        TMemberList             m_colMembers;
        TMemberIdList           m_colMembersById;
        TMethodList             m_colMethods;
        TMethodIdList           m_colMethodsById;
        TMethodImplList         m_colMethodImpls;
        tCIDMacroEng::EClassExt m_eExtend;
        TString                 m_strBasePath;
        TString                 m_strClassPath;
        TString                 m_strName;
        TString                 m_strParentClassPath;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngClassInfo,TObject)
};

#pragma CIDLIB_POPPACK


