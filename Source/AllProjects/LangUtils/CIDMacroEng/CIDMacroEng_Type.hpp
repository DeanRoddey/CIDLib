//
// FILE NAME: CIDMacroEng_Type.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/11/2003
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


#pragma CIDLIB_PACK(CIDLIBPACK)


namespace tCIDMacroEng
{
    // -----------------------------------------------------------------------
    //  When a cast is done, there are three possible results. It works, the
    //  two types are incompatible, or the source value is of a reasonable
    //  type, but it is out of range for the target.
    // -----------------------------------------------------------------------
    enum class ECastRes
    {
        Ok
        , Incompatible
        , Range
    };


    // -----------------------------------------------------------------------
    //  The attribute that controls how classes can be extended. Non-final
    //  means that it can be overridden, which is the default so it's not
    //  usually explicitly indicated. Final means that it can no longer be
    //  overridden. Abstract means that it must be overridden and cannot be
    //  instantiated itself.
    //
    //  Any derived class can declare itself final, thus stopping any further
    //  derivation. But since a final class cannot be derived from, there is
    //  no way to go back.
    //
    //  NOTE:   Final at this level overrides any per-method extension attrs.
    //          If the class cannot be derived from, the per-method attributes
    //          become irrelevant. If it can, then the per-method attributes
    //          indicate which methods can be overridden.
    // -----------------------------------------------------------------------
    enum class EClassExt
    {
        Abstract
        , NonFinal
        , Final
    };


    // -----------------------------------------------------------------------
    //  When the user uses class names, they generally just use the actual
    //  name, not the whole path. Only when clashes occur is a full name
    //  required. So the engine has a name resolution method that can resolve
    //  class names. It returns this enum to indicate what it found.
    // -----------------------------------------------------------------------
    enum class EClassMatches
    {
        NotFound
        , Unique
        , Ambiguous
    };


    // -----------------------------------------------------------------------
    //  An enum to use for const/non-const attributes, to make the code more
    //  self documenting. They are arranged so that it can be checked in a
    //  boolean way, i.e. if it's non-zero it's const.
    // -----------------------------------------------------------------------
    enum class EConstTypes
    {
        NonConst
        , Const
    };


    // -----------------------------------------------------------------------
    //  Method dispatch can be monomorphic or polymorphic, and this enum is
    //  used to indicate which. Monomorphic calls can still call further down
    //  their inheritance chain, it's just that the initial target is the
    //  actual class, not the most derived.
    // -----------------------------------------------------------------------
    enum class EDispatch
    {
        Mono
        , Poly
    };


    // -----------------------------------------------------------------------
    //  The class value classes support an internal formatting mechanism to
    //  support the debugger. This is used to ask them what format we'd like
    //  to get.
    // -----------------------------------------------------------------------
    enum class EDbgFmts
    {
        Short
        , Long
    };


    // -----------------------------------------------------------------------
    //  The two sides of an expression. This is passed to the expression
    //  parsing method to indicate which side we are looking for, which can
    //  restrict what is legal.
    // -----------------------------------------------------------------------
    enum class EExprSides
    {
        LHS
        , RHS
    };


    // -----------------------------------------------------------------------
    //  The types of expressions the language supports. This is returned from
    //  the expression parser method.
    // -----------------------------------------------------------------------
    enum class EExprTypes
    {
        BoolLiteral
        , CharLiteral
        , ConstObjectRef
        , CurLine
        , EnumValue
        , NumLiteral
        , ObjectRef
        , StringLiteral

        , Unknown
    };


    // -----------------------------------------------------------------------
    //  We insure that the intrinsic classes get added in a particular order,
    //  so that they have known ids. This allows us to do a lot of stuff
    //  internally a lot faster and simpler. The loading of these classes in
    //  the engine class MUST be kept in sync with this if you make changes,
    //  so that the come out with these ids!!
    // -----------------------------------------------------------------------
    enum class EIntrinsics : tCIDLib::TCard2
    {
        Object
        , Void
        , TextOutStream
        , Formattable
        , Enum
        , BaseInfo
        , Boolean
        , Char
        , String
        , Card1
        , Card2
        , Card4
        , Card8
        , Float4
        , Float8
        , Int1
        , Int2
        , Int4
        , Time
        , StringList
        , Exception
        , MemBuf
        , StringOutStream

        , Count

        , FirstNum = Card1
        , LastNum  = Int4
    };


    // -----------------------------------------------------------------------
    //  The engine can report macro level exceptions either at the point of
    //  throw, if they are not handled in the current method, or if they are
    //  not handled period.
    // -----------------------------------------------------------------------
    enum class EExceptReps
    {
        AtThrow
        , NotHandled

        , Count
    };


    // -----------------------------------------------------------------------
    //  The attribute that controls how methods can be extended. Required means
    //  it must be implemented in a derived class. Non-final means that it
    //  can be overridden. Final means that it can no longer be overridden.
    //
    //  Derived classes can override non-final methods and modify this
    //  attribute in the following legal ways:
    //
    //      Non-Final -> Final      No longer can be overridden
    //      Required  -> Final      No longer required and cannot be overridden
    //      Non-Final -> Required   Goes from being optional override to a
    //                              required override.
    //      Required  -> Non-Final  The derived class provided the required
    //                              override but it be further overridden.
    //
    //  So note that any forward movement is allowed, but the only backwards
    //  movement is required to non-final.
    // -----------------------------------------------------------------------
    enum class EMethExt
    {
        NonFinal
        , Required
        , Final
    };


    // -----------------------------------------------------------------------
    //  The numeric types we support.
    //
    //  NOTE:   Keep this in sync with the numeric class id's array in the
    //          engine class!
    //
    //          Also, these must be in the same order as the similar entries
    //          in the EIntrinsicIds above.
    // -----------------------------------------------------------------------
    enum class ENumTypes
    {
        None
        , Card1
        , Card2
        , Card4
        , Card8
        , Float4
        , Float8
        , Int1
        , Int2
        , Int4

        , Count
    };


    // -----------------------------------------------------------------------
    //  Returned from some operator lookup methods
    // -----------------------------------------------------------------------
    enum class EOpResults
    {
        NotOp
        , NotSupported
        , Assign
        , Binary
        , Unary
        , Logical
        , Bitwise
        , Mathematical
    };


    // -----------------------------------------------------------------------
    //  Since the macro language is used in some widely varied situations, we
    //  support an optimization level enum that lets the application indicate
    //  how agressive we should be in optimization. For a simple macro invoked
    //  on a key press that runs straight through, optimization is counter
    //  productive. For a CQC device driver, we want maximum optimization since
    //  they run forever and are very active.
    // -----------------------------------------------------------------------
    enum class EOptLevels
    {
        Minimal
        , Medium
        , Maximum
    };


    // -----------------------------------------------------------------------
    //  The types of events we get back from a parse. This is used when we
    //  store the error info, to know which entries are which type.
    // -----------------------------------------------------------------------
    enum class EPrsEvs
    {
        InfoMsg
        , WarningMsg
        , ErrorMsg
        , CIDLibExcept
        , UnknownExcept
    };


    // -----------------------------------------------------------------------
    //  When parsing, some of the calls can be tolerant of failure or not, or
    //  whether it should just check and issue no errors if it fails, and this
    //  enum is used to indicate which way it should act.
    // -----------------------------------------------------------------------
    enum class ERecovers
    {
        No
        , Yes
        , CheckOnly
    };


    // -----------------------------------------------------------------------
    //  When a name is referenced within a method, that name can only refer
    //  to a local, a parameter to the method, a member of a class, the current
    //  object, or a class name. This enum let's us keep up with which kind
    //  we find.
    // -----------------------------------------------------------------------
    enum class ERefTypes
    {
        Unknown
        , Local
        , Parm
        , Member
        , Parent
        , This
        , EnumConst
        , Exception
        , ThisCall
        , LitMember
    };


    // -----------------------------------------------------------------------
    //  The path resolver base class provides two modes for resolving a class
    //  path to it's text, so that derived implementations that are built on
    //  a source control system can be supported.
    // -----------------------------------------------------------------------
    enum class EResModes
    {
        ReadOnly
        , ReadWrite
    };


    // -----------------------------------------------------------------------
    //  The types of stack item. Used by the context stack to mark items that
    //  are pushed onto the call stack. This lets the engine know what to do
    //  when that item is popped. Note that Exception is a special case one
    //  that only is available within a Catch block, and just refers to a
    //  magic value object in the engine.
    // -----------------------------------------------------------------------
    enum class EStackItems
    {
        Unused
        , MethodCall
        , TempVal
        , Local
        , Member
        , Parm
        , StringPool
        , This
        , ColElem
        , Try
        , Exception

        , Count
    };


    // -----------------------------------------------------------------------
    //  The tokens that our parser returns.
    //
    //  NOTE:   Keep this in sync with the mapping array in the file
    //          CIDMacroEng_ParserTokenMap_.hpp.
    // -----------------------------------------------------------------------
    enum class ETokens
    {
        //
        //  This set of tokens have actual textual representations in the
        //  source text.
        //
        Abstract
        , ArrayOf
        , Begin
        , BeginDebug
        , Break
        , Case
        , Catch
        , Class
        , ClassPath
        , Const
        , Constructor
        , CurClassName
        , CurLine
        , Default
        , Destructor
        , Directives
        , DoLoop
        , DoWhile
        , DynTypeRef
        , Else
        , ElseIf
        , EndCase
        , EndCatch
        , EndClass
        , EndConstructor
        , EndDebug
        , EndDestructor
        , EndDirectives
        , EndEnum
        , EndForEach
        , EndIf
        , EndImports
        , EndLiterals
        , EndLocals
        , EndMembers
        , EndMethod
        , EndMethods
        , EndSwitch
        , EndTry
        , EndTypes
        , EndWhile
        , Enum
        , Exception
        , False
        , Final
        , ForEach
        , FTCase
        , If
        , Imports
        , In
        , InOut
        , Literals
        , Locals
        , Members
        , Method
        , Methods
        , NonConst
        , NonFinal
        , Out
        , Overrides
        , Parent
        , ParentClass
        , Private
        , Public
        , Required
        , Rethrow
        , Return
        , Returns
        , Switch
        , This
        , Throw
        , True
        , Try
        , TypeCast
        , Types
        , VectorOf
        , While


        //
        //  This set of tokens are special, single character tokens. These
        //  always are considered tokens unless they are in a quoted string.
        //  Be sure to update the special chars array in the file
        //  CIDMacroEng_ParserSrc.cpp, if you update this set of tokens.
        //
        , Colon
        , Comma
        , EqualSign
        , Exclaim
        , CloseBracket
        , CloseParen
        , OpenBracket
        , OpenParen
        , Period
        , SemiColon

        , Add
        , And
        , AndEq
        , Assign
        , Dec
        , Divide
        , DivideEq
        , GtThan
        , GtThanEq
        , Inc
        , LogAnd
        , LogOr
        , LogXor
        , LsThan
        , LsThanEq
        , ModDiv
        , ModDivEq
        , MinusEq
        , Multiply
        , MultiplyEq
        , NotEqual
        , Or
        , OrEq
        , PlusEq
        , Subtract
        , Xor
        , XorEq

        //
        //  These are purely symbolic and don't have any literal representation
        //  but are used to return indications back to the parser.
        //
        , EOF
        , QuotedString
        , NumericLiteral
        , CharLiteral


        , Count
        , NoMatch
        , FirstSymbolic = Colon
    };


    // -----------------------------------------------------------------------
    //  The visibility levels that classes can support for methods
    // -----------------------------------------------------------------------
    enum class EVisTypes
    {
        Private
        , Public
    };



    // -----------------------------------------------------------------------
    //  We use a simple two dimensional array to get a list of the parameters
    //  available for all of the ctors of a class. This is used for ctor
    //  resolution to match the provided parms.
    // -----------------------------------------------------------------------
    struct TParmIdItem
    {
        tCIDLib::TCard4     c4Count;
        tCIDLib::TBoolean   bDiscarded;
        tCIDLib::TCard2     c2MethodId;
        tCIDLib::TCard2     ac2Parms[kMacroEng::c4MaxMethodParms];
    };

    struct TParmIdTable
    {
        tCIDLib::TCard4     c4Count;
        TParmIdItem         apiiMethods[kMacroEng::c4MaxMethodParms];
    };
}

#pragma CIDLIB_POPPACK


