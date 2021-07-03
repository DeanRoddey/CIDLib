//
// FILE NAME: CIDXML_Type.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/20/1999
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
//  This file is the main types file of the facility. It contains all of
//  the non-class types of the facility.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


class TXMLEntitySrc;

namespace tCIDXML
{
    // -----------------------------------------------------------------------
    //  Entity sources are passed around as counted pointers to constant
    //  entity sources. It uses the prefix 'esr';
    // -----------------------------------------------------------------------
    using TEntitySrcRef = TCntPtr<const TXMLEntitySrc>;


    // -----------------------------------------------------------------------
    //  This type represents the attribute types defined by XML 1.0
    // -----------------------------------------------------------------------
    enum class EAttrTypes
    {
        CData
        , Enumeration
        , Entity
        , Entities
        , ID
        , IDRef
        , IDRefs
        , NmToken
        , NmTokens
        , Notation

        , Count
        , Min           = CData
        , Max           = Notation
        , Unknown       = -1
    };


    // -----------------------------------------------------------------------
    //  These represente the base encodings that we can auto-sense. They are
    //  ordered in ascending order of the byte sequences of the <?xml string
    //  in that particular encoding, and they are used as indexes into a set
    //  of static arrays SO DON'T change the order!
    // -----------------------------------------------------------------------
    enum class EBaseEncodings
    {
        UCS4_B
        , UTF16_B
        , UCS4_L
        , UTF16_L
        , UTF8
        , EBCDIC_US

        , Count
    };


    // -----------------------------------------------------------------------
    //  These are used to represent content specification nodes
    // -----------------------------------------------------------------------
    enum class ECMNodeTypes
    {
        ZeroOrOne
        , ZeroOrMore
        , OneOrMore
        , Alternation
        , Sequence
        , Leaf

        , Count
        , Unknown
    };


    // -----------------------------------------------------------------------
    //  These are used by the methods that parse XML/Text decls. It tells it
    //  which type to expect.
    // -----------------------------------------------------------------------
    enum class EDeclTypes
    {
        XML
        , Text

        , Count
    };


    // -----------------------------------------------------------------------
    //  These represent the defaulting values for attributes under XML 1.0
    // -----------------------------------------------------------------------
    enum class EDefAttrTypes
    {
        Default
        , Required
        , Implied
        , Fixed

        , Count
        , Min           = Default
        , Max           = Fixed
        , Unknown       = -1
    };


    // -----------------------------------------------------------------------
    //  These are the core element models that are supported by XML 1.0
    // -----------------------------------------------------------------------
    enum class EElemModels
    {
        Empty
        , Any
        , Mixed
        , Children

        , Count
    };


    // -----------------------------------------------------------------------
    //  This enum is used by the TXMLElemDecl class, and it is used to
    //  indicate why the element decl was created and put into the pool.
    // -----------------------------------------------------------------------
    enum class EElemReasons
    {
        AsRootElem
        , InContentModel
        , InAttList
        , Declared

        , Count
    };


    // -----------------------------------------------------------------------
    //  This enum indicates the various ways that an element's content model
    //  can accept text content. For instance, a CHILDREN element can have
    //  whitespace, and an EMPTY element cannot take even whitespace. The
    //  element stack maintains a value of this type that represents the top
    //  of stack, for quick testing during parsing of char data.
    // -----------------------------------------------------------------------
    enum class EElemTextTypes
    {
        None
        , SpaceIgnorable
        , Any

        , Count
    };


    // -----------------------------------------------------------------------
    //  These values represent the types of entities we can have
    // -----------------------------------------------------------------------
    enum class EEntityTypes
    {
        General
        , Parameter
        , Neither

        , Count
    };


    // -----------------------------------------------------------------------
    //  All posted XML errors are of this type
    // -----------------------------------------------------------------------
    enum class EErrTypes
    {
        Warning
        , Error
        , Validation
        , Unknown


        , Count
        , Min           = Warning
        , Max           = Unknown
    };


    // -----------------------------------------------------------------------
    //  These are used to indicate what happened during the scanning of an
    //  entity reference. The expanded value is either returned directly or
    //  pushed as a reader.
    // -----------------------------------------------------------------------
    enum class EEntityRes
    {
        Returned
        , Pushed
        , Empty

        , Count
    };


    // -----------------------------------------------------------------------
    //  Used in the facility class' EscapeFor() to indicate what rules should
    //  apply in the escapement processing.
    // -----------------------------------------------------------------------
    enum class EEscTypes
    {
        Attribute
        , ElemText
    };


    // -----------------------------------------------------------------------
    //  These are used to indicate what type of id markup to expect
    // -----------------------------------------------------------------------
    enum class EIDTypes
    {
        External
        , Public
        , Either

        , Count
        , Min           = External
        , Max           = Either
    };


    // -----------------------------------------------------------------------
    //  These represent the types of markup that can exist in main content,
    //  plus a special case end of input type.
    // -----------------------------------------------------------------------
    enum class EMarkupTypes
    {
        Characters
        , Comment
        , CDATA
        , StartTag
        , EndTag
        , PI

        , EOI
        , Unknown

        , Count
    };


    // -----------------------------------------------------------------------
    //  These indicate the types of nodes that can be represented in a
    //  tree representation.
    // -----------------------------------------------------------------------
    enum class ENodeTypes
    {
        Text
        , Element
        , PI
        , Comment
        , DTD
        , Decl

        , Count
        , Min           = Text
        , Max           = Decl
    };


    // -----------------------------------------------------------------------
    //  These are flags for the core parser that indicate what stuff it
    //  should bother to inform the caller of. The BC and AC values mean
    //  'before content' and 'after content', i.e. in the prolog or misc
    //  sections. ISS means 'internal subset' of the DTD.
    //
    //  Some of this stuff is not even supported by common public XML APIs,
    //  so these flags allow the parser to avoid callouts for stuff that isn't
    //  going to get used.
    //
    //  Some special combos are provided, that will spit out the info required
    //  to meet certain APIs or situations.
    // -----------------------------------------------------------------------
    enum class EParseFlags : tCIDLib::TCard4
    {
        None                = 0x0
        , SpaceBC           = 0x0000001
        , CommentsBC        = 0x0000002
        , PIsBC             = 0x0000004

        , SpaceAC           = 0x0000010
        , CommentsAC        = 0x0000020
        , PIsAC             = 0x0000040

        , SpaceISS          = 0x0000100
        , CommentsISS       = 0x0000200
        , PIsISS            = 0x0000400
        , MarkupISS         = 0x0000800

        , SpaceESS          = 0x0001000
        , CommentsESS       = 0x0002000
        , PIsESS            = 0x0004000
        , MarkupESS         = 0x0008000

        , IgnorableSpace    = 0x0010000
        , Comments          = 0x0020000
        , PI                = 0x0040000
        , Chars             = 0x0080000
        , Tags              = 0x0100000

        , XMLDecl           = 0x1000000
        , TextDecl          = 0x2000000
        , DOCType           = 0x4000000
        , Topology          = 0x8000000

        // Common combinations
        , All               = 0xFFFFFFF
        , Standard          = 0xDFF0FFF
        , SAX1              = 0x1FF0000
        , JustContent       = 0x0FF0000
        , TagsNText         = 0x0180000


        // These are for internal use
        , BCMask            = 0x900000F
        , ACMask            = 0x80000F0
        , ISSMask           = 0xE000F00
        , ESSMask           = 0xA00F000
        , ContentMask       = 0xAFF0000

        , SpaceMask         = 0x0011111
        , CommentMask       = 0x0022222
        , PIMask            = 0x0044444
        , MarkupMask        = 0x0008800
    };

    // -----------------------------------------------------------------------
    //  This value is used to indicate where in the XML source something is.
    //  Note that these are set to the mask values of EParseFlags, to make
    //  it fast to figure out if a parser option is valid for the current
    //  position.
    // -----------------------------------------------------------------------
    enum class ELocations : tCIDLib::TCard4
    {
        BeforeContent       = tCIDLib::TCard4(EParseFlags::BCMask)
        , IntSubset         = tCIDLib::TCard4(EParseFlags::ISSMask)
        , ExtSubset         = tCIDLib::TCard4(EParseFlags::ESSMask)
        , InContent         = tCIDLib::TCard4(EParseFlags::ContentMask)
        , AfterContent      = tCIDLib::TCard4(EParseFlags::ACMask)
    };


    // -----------------------------------------------------------------------
    //  These are options that tell the parse about optional modes that it
    //  should or should not implement.
    // -----------------------------------------------------------------------
    enum class EParseOpts : tCIDLib::TCard4
    {
        None                = 0x0
        , Validate          = 0x0000001
        , Namespaces        = 0x0000002
        , IgnoreDTD         = 0x0000004
        , IgnoreBadChars    = 0x0000008

        , All               = 0x000001F
    };


    // -----------------------------------------------------------------------
    //  Used by the PrintTo() method of the tree classes.
    // -----------------------------------------------------------------------
    enum class EPrintFmts  : tCIDLib::TCard4
    {
        Unknown
        , Pretty
        , Flat

        , Count
    };


    // -----------------------------------------------------------------------
    //  Used by the PrintTo() method of the tree classes, when doing pretty printing,
    //  to indicate options for printing.
    // -----------------------------------------------------------------------
    enum class EPrintOpts : tCIDLib::TCard4
    {
        None                = 0x0000
        , NoWrap            = 0x0001
        , StripLTSpace      = 0x0002
        , EscapeNL          = 0x0004

        , AllBits           = 0x0007
    };


    // -----------------------------------------------------------------------
    //  These are the options for a PE ref. When a PE ref is seen, it can be
    //  in a place where space is required, and there are different rules
    //  according to where it happens.
    // -----------------------------------------------------------------------
    enum class EPERefOpts : tCIDLib::TCard2
    {
        None                = 0x0000
        , SpaceRequired     = 0x0001
        , InLiteral         = 0x0002
        , InMarkup          = 0x0004

        , SpaceInLiteral    = 0x0003
        , SpaceInMarkup     = 0x0005
        , LiteralMarkup     = 0x0006
    };


    // -----------------------------------------------------------------------
    //  This value is used in some pool operations to tell the pool what
    //  you want to happen.
    // -----------------------------------------------------------------------
    enum class EPoolOpts
    {
        Find
        , FindOrAdd

        , Count
    };


    // -----------------------------------------------------------------------
    //  These are used to indicate to an entity expansion call whether its
    //  from a literal or not.
    // -----------------------------------------------------------------------
    enum class ERefFrom
    {
        Literal
        , OutsideLiteral

        , Count
    };


    // -----------------------------------------------------------------------
    //  These are the type values passed to the entity resolver. It tells the
    //  handler of these events what type of entity this is, which is very
    //  important since its not possible to tell in many cases by the name of
    //  the entity. In particular they will want to be able to know when its
    //  the external subset, since that's the most often redirected entity.
    //
    //  We also define an 'any' magic value. This will be used any time there
    //  is a means to choose a type and the caller really just wants any of
    //  them.
    // -----------------------------------------------------------------------
    enum class EResolveTypes
    {
        ParamEntity
        , GeneralEntity
        , ExternalSubset

        , Count
        , Any
    };


    // -----------------------------------------------------------------------
    //  This is a status result returned from the validation method of the
    //  content models.
    // -----------------------------------------------------------------------
    enum class EValidRes
    {
        Success
        , TooFew
        , TooMany
        , Mismatch

        , Count
    };
}



// ---------------------------------------------------------------------------
//  Provide streaming support and standard enum tricks for some of our enums
// ---------------------------------------------------------------------------
#if !defined(CIDXML_NOCLASSES)
EnumStreamMacros(tCIDXML::EAttrTypes)
EnumStreamMacros(tCIDXML::EDefAttrTypes)
EnumStreamMacros(tCIDXML::ENodeTypes)

BmpEnumTricks(tCIDXML::EParseFlags)
BmpEnumTricks(tCIDXML::EParseOpts)
BmpEnumTricks(tCIDXML::EPrintOpts)
BmpEnumTricks(tCIDXML::EPERefOpts)

#endif

