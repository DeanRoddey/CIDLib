// ----------------------------------------------------------------------------
//  FILE: CIDDocComp_Info.hpp
//  DATE: Sun, Jun 16 22:02:47 2019 -0400
//
//  This file was generated by the Charmed Quark CIDIDL compiler. Do not make
//  changes by hand, because they will be lost if the file is regenerated.
// ----------------------------------------------------------------------------
#pragma once

#pragma CIDLIB_PACK(CIDLIBPACK)

// ----------------------------------------------------------------------------
// Constants namespace
// ----------------------------------------------------------------------------
namespace kCIDDocComp
{
    
    // ------------------------------------------------------------------------
    //  Some specific types we look for
    //                  
    // ------------------------------------------------------------------------
     const extern TString strType_Void;
    
    // ------------------------------------------------------------------------
    //  Values we look for in the XML cotnent.
    //                  
    // ------------------------------------------------------------------------
     const extern TString strXML_ColSpan;
     const extern TString strXML_DefValue;
     const extern TString strXML_Desc;
     const extern TString strXML_Extra;
     const extern TString strXML_Extra1;
     const extern TString strXML_Extra2;
     const extern TString strXML_FileName;
     const extern TString strXML_HelpText;
     const extern TString strXML_Id;
     const extern TString strXML_Keywords;
     const extern TString strXML_PageLink;
     const extern TString strXML_PageMap;
     const extern TString strXML_PageType;
     const extern TString strXML_Ref;
     const extern TString strXML_SubDir;
     const extern TString strXML_SubTopicMap;
     const extern TString strXML_Title;
     const extern TString strXML_TopicPage;
     const extern TString strXML_Type;
     const extern TString strXML_Virtual;
}

// ----------------------------------------------------------------------------
// Types namespace
// ----------------------------------------------------------------------------
namespace tCIDDocComp
{
    
    // ------------------------------------------------------------------------
    //  The overall class element has a set of bitmap flags that indicate various possible attributes of that class. We can't use None for
    //  no flags since it's used to report a failure to translate a value,
    //  so we define NoFlags. XML doesn't allow an empty NMTOKENS attr
    //  so we have to have something as a default in the DTD.
    //                  
    // ------------------------------------------------------------------------
    enum class EClsFlags : tCIDLib::TCard4
    {
        Dup = 0x0001
        , Fmt = 0x0002
        , BStrm = 0x0004
        , NoFlags = 0x1000
        , Count = 4
        , None = 0
        , AllBits = 0x1007
    };
     EClsFlags eXlatEClsFlags(const TString& strToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::False);
     const TString& strXlatEClsFlags(const EClsFlags eToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::True);
     tCIDLib::TBoolean bIsValidEnum(const EClsFlags eVal);

    
    // ------------------------------------------------------------------------
    //  The method element has a multi-token attribute that we use to
    //  indicate the various attributes of a method. The base name is
    //  the XML value and the text is the C++ value (if any)
    //  
    //  Ctor is to allow us to mark mark those since they have special
    //  display requirements when we format them out. We also mark operators
    //  just in case it's important.
    //  
    //  NoAttrs is a special one meaning none set, as apposed to the
    //  IDL generated None value which means no match when we do the
    //  translation from text to value.
    //                  
    // ------------------------------------------------------------------------
    enum class EMethAttrs : tCIDLib::TCard4
    {
        Const = 0x00000001
        , ConstExpr = 0x00000002
        , NoDiscard = 0x00000004
        , NoExcept = 0x00000008
        , Explicit = 0x00000010
        , Pure = 0x00000020
        , Static = 0x00000040
        , Virtual = 0x00000080
        , Override = 0x00000100
        , Friend = 0x00000200
        , Template = 0x00000400
        , Default = 0x00100000
        , Delete = 0x00200000
        , Ctor = 0x10000000
        , Operator = 0x20000000
        , NoAttrs = 0x80000000
        , Count = 16
        , None = 0
        , AllBits = 0xB03007FF
    };
     EMethAttrs eXlatEMethAttrs(const TString& strToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::False);
     const TString& strXlatEMethAttrs(const EMethAttrs eToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::True);
     const TString& strLoadEMethAttrs(const EMethAttrs eToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::False);
     tCIDLib::TBoolean bIsValidEnum(const EMethAttrs eVal);

    
    // ------------------------------------------------------------------------
    //  The kinds of markup we support in our general help text content.
    //  These match the XML elements that represent these markup types.
    //                  
    // ------------------------------------------------------------------------
    enum class EMUTypes
    {
        None
        , Bold
        , Break
        , ClassRef
        , Code
        , DIV
        , FacRef
        , Image
        , Indent
        , InlineNote
        , Italic
        , Link
        , List
        , ListItem
        , Note
        , Paragraph
        , SecTitle
        , SubSecTitle
        , Superscript
        , Table
        , TableCol
        , TableRow
        , Text
        , Count
        , Min = None
        , Max = Text
    };
     tCIDLib::TBoolean bIsValidEnum(const EMUTypes eVal);

    
    // ------------------------------------------------------------------------
    //  The types of pages we support. These values are the same as those
    //  in the DTD, so we can map directly from that to this.
    //                  
    // ------------------------------------------------------------------------
    enum class EPageTypes
    {
        None
        , CppClass
        , HelpPage
        , Count
        , Min = None
        , Max = HelpPage
    };
     EPageTypes eXlatEPageTypes(const TString& strToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::False);
     const TString& strXlatEPageTypes(const EPageTypes eToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::True);
     const TString& strLoadEPageTypes(const EPageTypes eToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::False);
     tCIDLib::TBoolean bIsValidEnum(const EPageTypes eVal);

    
    // ------------------------------------------------------------------------
    //  The possible directions of parameters.
    //                  
    // ------------------------------------------------------------------------
    enum class EParmDirs
    {
        In
        , Out
        , InOut
        , Count
        , Min = In
        , Max = InOut
    };
     EParmDirs eXlatEParmDirs(const TString& strToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::False);
     const TString& strXlatEParmDirs(const EParmDirs eToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::True);
     tCIDLib::TBoolean bIsValidEnum(const EParmDirs eVal);

    
    // ------------------------------------------------------------------------
    //  The possible ways the parameter can be passed by or a value
    //  returned.
    //      None    - For Constructors
    //      CPtr    - const pointer
    //      CPtrC   - const pointer to a const
    //      CRef    - const reference
    //      MRef    - A move ctor/op style reference
    //      Ref     - non-const reference
    //      Ptr     - non-const pointer
    //      PtrCRef - A reference to a pointer to a const
    //      PtrRef  - A reference to a pointer to non-const
    //      Value   - by value
    //  
    //      Void    - No return or parameter
    //  
    //  These name is the same as what is in the XML, so we can translate
    //  directly. None is used to indicate not set, so it's an error if we
    //  end up with a None.
    //                  
    // ------------------------------------------------------------------------
    enum class EParmPB
    {
        None
        , CPtr
        , CPtrC
        , CRef
        , MRef
        , Ref
        , Ptr
        , PtrC
        , PtrCRef
        , PtrRef
        , Val
        , Void
        , Count
        , Min = None
        , Max = Void
    };
     EParmPB eXlatEParmPB(const TString& strToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::False);
     const TString& strXlatEParmPB(const EParmPB eToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::True);
     tCIDLib::TBoolean bIsValidEnum(const EParmPB eVal);

    
    // ------------------------------------------------------------------------
    //  These are the special methods that can be defaulted or deleted. We
    //  can save a lot of work by just being able to indicate these in two
    //  small lists of deleted and defaulted members.
    //                  
    // ------------------------------------------------------------------------
    enum class ESpecMeths
    {
        None
        , AssignOp
        , CopyCtor
        , Dtor
        , DefCtor
        , MoveCtor
        , MoveOp
        , Count
        , Min = None
        , Max = MoveOp
    };
     ESpecMeths eXlatESpecMeths(const TString& strToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::False);
     const TString& strXlatESpecMeths(const ESpecMeths eToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::True);
     tCIDLib::TBoolean bIsValidEnum(const ESpecMeths eVal);

    
    // ------------------------------------------------------------------------
    //  The types of sources we push onto the source stack. This let's us
    //  know what the top of stack is, which is used to know the current
    //  parsing or output generation context.
    //                  
    // ------------------------------------------------------------------------
    enum class ESrcTypes
    {
        CppClass
        , HelpPage
        , Topic
        , Count
        , Min = CppClass
        , Max = Topic
    };
     tCIDLib::TBoolean bIsValidEnum(const ESrcTypes eVal);

    
    // ------------------------------------------------------------------------
    //  The standard C++ class/structure visibility types
    //                  
    // ------------------------------------------------------------------------
    enum class EVisTypes
    {
        Public
        , Protected
        , Private
        , Count
        , Min = Public
        , Max = Private
    };
     EVisTypes eXlatEVisTypes(const TString& strToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::False);
     const TString& strXlatEVisTypes(const EVisTypes eToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::True);
     const TString& strLoadEVisTypes(const EVisTypes eToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::False);
     tCIDLib::TBoolean bIsValidEnum(const EVisTypes eVal);

}

#pragma CIDLIB_POPPACK

constexpr tCIDDocComp::EClsFlags operator|(const tCIDDocComp::EClsFlags eLHS, const tCIDDocComp::EClsFlags eRHS)
{    
    return tCIDDocComp::EClsFlags(tCIDLib::TEnumMaskType(eLHS) | tCIDLib::TEnumMaskType(eRHS));
}
constexpr tCIDDocComp::EClsFlags& operator|=(tCIDDocComp::EClsFlags& eLHS, const tCIDDocComp::EClsFlags eRHS)
{    
    eLHS = tCIDDocComp::EClsFlags(tCIDLib::TEnumMaskType(eLHS) | tCIDLib::TEnumMaskType(eRHS));
    return eLHS;
}
constexpr tCIDDocComp::EClsFlags operator&(const tCIDDocComp::EClsFlags eLHS, const tCIDDocComp::EClsFlags eRHS)
{    
    return tCIDDocComp::EClsFlags(tCIDLib::TEnumMaskType(eLHS) & tCIDLib::TEnumMaskType(eRHS));
}
constexpr tCIDDocComp::EClsFlags& operator&=(tCIDDocComp::EClsFlags& eLHS, const tCIDDocComp::EClsFlags eRHS)
{    
    eLHS = tCIDDocComp::EClsFlags(tCIDLib::TEnumMaskType(eLHS) & tCIDLib::TEnumMaskType(eRHS));
    return eLHS;
}

constexpr tCIDDocComp::EMethAttrs operator|(const tCIDDocComp::EMethAttrs eLHS, const tCIDDocComp::EMethAttrs eRHS)
{    
    return tCIDDocComp::EMethAttrs(tCIDLib::TEnumMaskType(eLHS) | tCIDLib::TEnumMaskType(eRHS));
}
constexpr tCIDDocComp::EMethAttrs& operator|=(tCIDDocComp::EMethAttrs& eLHS, const tCIDDocComp::EMethAttrs eRHS)
{    
    eLHS = tCIDDocComp::EMethAttrs(tCIDLib::TEnumMaskType(eLHS) | tCIDLib::TEnumMaskType(eRHS));
    return eLHS;
}
constexpr tCIDDocComp::EMethAttrs operator&(const tCIDDocComp::EMethAttrs eLHS, const tCIDDocComp::EMethAttrs eRHS)
{    
    return tCIDDocComp::EMethAttrs(tCIDLib::TEnumMaskType(eLHS) & tCIDLib::TEnumMaskType(eRHS));
}
constexpr tCIDDocComp::EMethAttrs& operator&=(tCIDDocComp::EMethAttrs& eLHS, const tCIDDocComp::EMethAttrs eRHS)
{    
    eLHS = tCIDDocComp::EMethAttrs(tCIDLib::TEnumMaskType(eLHS) & tCIDLib::TEnumMaskType(eRHS));
    return eLHS;
}
 TTextOutStream& operator<<(TTextOutStream& strmTar, const tCIDDocComp::EMethAttrs eToStream);


 TTextOutStream& operator<<(TTextOutStream& strmTar, const tCIDDocComp::EPageTypes eToStream);

 TTextOutStream& operator<<(TTextOutStream& strmTar, const tCIDDocComp::EParmDirs eToStream);




 TTextOutStream& operator<<(TTextOutStream& strmTar, const tCIDDocComp::EVisTypes eToStream);

