// ----------------------------------------------------------------------------
//  FILE: CIDMacroEng_Shared.hpp
//  DATE: Fri, Oct 01 19:41:29 2021 -0400
//
//  This file was generated by the Charmed Quark CIDIDL compiler. Do not make
//  changes by hand, because they will be lost if the file is regenerated.
// ----------------------------------------------------------------------------
#pragma once

#pragma CIDLIB_PACK(CIDLIBPACK)

// ----------------------------------------------------------------------------
// Types namespace
// ----------------------------------------------------------------------------
namespace tCIDMacroEng
{
    
    // ------------------------------------------------------------------------
    //  The type of flow control structures we support. This is mainly used
    //  in the TMEngFlowCtrlItem class, which is used by the parser to keep
    //  a pushdown stack of flow control structures as methods are parsed.
    //  
    //  With couple of exceptions, we only push starts of blocks, and the end
    //  blocks just confirm that the right start was on the top and pop the
    //  stack. But, in a couple places, such as try/catch, we have non-nested
    //  blocks that have continuity restrictions. So we push a try, which the
    //  end try sees but doesn't pop. The Catch must see the End Try which it
    //  pops and pushes a catch, and the end catch must see a Catch.
    //  
    //  We also basically push dummy EndCase items so that we can do correct
    //  book keeping, insuring that we only see a new case or the end of the
    //  switch start after the end of the last case.
    //                  
    // ------------------------------------------------------------------------
    enum class EFlowTypes
    {
        None
        , Case
        , Catch
        , DoLoop
        , DoWhile
        , Else
        , ElseIf
        , EndCase
        , EndTry
        , ForEach
        , FTCase
        , If
        , Switch
        , Try
        , While
        , Count
        , Min = None
        , Max = While
    };
    [[nodiscard]]  EFlowTypes eXlatEFlowTypes(const TString& strToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::False);
    [[nodiscard]]  const TString& strXlatEFlowTypes(const EFlowTypes eToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::True);
    [[nodiscard]]  tCIDLib::TBoolean bIsValidEnum(const EFlowTypes eVal);

     TTextOutStream& operator<<(TTextOutStream& strmTar, const tCIDMacroEng::EFlowTypes eToStream);
    
    // ------------------------------------------------------------------------
    //  The opcodes supported by our virtual machine. We define here the parms
    //  that each ones takes, and what it expects on the stack.
    //  
    //  NOTE:   If you modify this list, you must update the mapping array in
    //          the facility class and you must update the formatting method in
    //          TMEngOpCode, which needs to understand opcodes and their parms.
    //          And the TMEngOpMethodImpl class must be udpated to know how to
    //          implement the opcode.
    //  
    //  CallExcept      - A special case opcode, that should probably be
    //                    gotten rid of eventually, which calls a method
    //                    on the magic exception object.
    //  CallLocal
    //  CallMember
    //  CallParm        - These all are for method calls. They are the same
    //                    except what type of object called, i.e. how to
    //                    interpret the first index, which indicates the
    //                    id of the local, parm, or member to access. The
    //                    second index is the id of the method to call.
    //                    These all expect a call frame on the stack.
    //  CallStack       - A special call opcode used when a method is called
    //                    on the return of a previous method call. So the
    //                    new target is now on the stack. This one takes
    //                    two indices. The first is how many items down
    //                    from the stack top the target object is, and the
    //                    second is the method id to call.
    //  CallParent
    //  CallThis        - One index, which is the index of the method to
    //                    call. Target object is implicitly the current
    //                    one, but the method implementation is either from
    //                    this class or it's parent class, according to
    //                    the opcode. _CallParent let's us support calls
    //                    down the sequence of overridden methods.
    //  ColIndex        - Indexes a collection class. Expects the stack
    //                    top to hold the index in a Card4 value, and the
    //                    item under it to be the collection object. It
    //                    will pop them both off and leave the indexed
    //                    element on the top of the stack.
    //  Copy            - Expects two objects on the stack top, of the same
    //                    class, and both copyable. Copies the top object
    //                    to the object under it and pops both.
    //  CondEnumInc     - No parms. Expects some enumerated object on the
    //                    top of the stack. It conditionally increments the
    //                    value. If the value is already at the max, it does
    //                    nothing and leaves a false on the stack. Else, it
    //                    increments the value and leaves a true on the stack.
    //  CondJump
    //  CondJumpNP      - Expects a boolean value left on the stack top.
    //                    If that is true, it jumps by the offset given in
    //                    c4Immediate, else it doesn't. The NP doesn't pop
    //                    the stack after checking the top, the other does.
    //  CurLine         - c4Immediate holds the line number
    //  EndTry          - Expects a try stack item on the top of the stack
    //                    which it removes. It will be followed by an
    //                    unconditional jump around the catch.
    //  FlipTop         - Flips the top two items on the stack
    //  Jump            - c4Immediate holds the target IP
    //  LogicalAnd
    //  LogicalOr
    //  LogicalXor      - No parameters. Expects the top two stack values
    //                    to be boolean. It pops them off and replaces
    //                    them with a boolean result.
    //  MultiPop        - c4Immediate indicates number of pops
    //  Negate          - Expects the top of stack to be a non-const
    //                    boolean, whose value it flips and leaves on the
    //                    stack top.
    //  NoOp            - No parms, has no effect
    //  NotCondJump
    //  NotCondJumpNP   - Like CondJump, but with reverse boolean logic.
    //  PushCurLine     - No parms. Pushes the Card4 current line value
    //  PushEnum        - Two indices. The first is the enumerated type,
    //                    and the second is the ordinal value to push.
    //  PushExcept      - Pushes the magic exception object, which is only
    //                    available within a Catch block. It is a magic
    //                    value represented by the $Exception name.
    //  PushLocalConst  - c2Immediate indicates the id of the proc-local
    //                    temp const to push.
    //  PushLocalVar    - c2Immediate indicates the id of the proc-local
    //                    temp variable to push.
    //  PopTop          - No parms, just pops top of stack
    //  PopToReturn     - Copies the value on the top of stack to the return
    //                    object on the stack, then pops the stack. Doesn't
    //                    require any parms.
    //  PushImXX        - The xxImmediate field holds the value pushed
    //  PushLocal       - One index, the index of the local
    //  PushMember      - One index, the index of the member
    //  PushParm        - One index, the index of the parm
    //  PushStrPoolItem - One index, the index of the pool item within the
    //                    current method implementation. These are pushed
    //                    as const always.
    //  PushTempConst   - One index, the id of the type to push. Pushes
    //                    a value from the temp pool, creating one if needed.
    //  PushTempVar     - One index, the id of the type to push. Pushes
    //                    a value from the temp pool, creating one if needed.
    //  PushThis        - No parms, just pushes current object
    //  Repush          - c4Immediate indicates how far down from the top
    //                    of stack the item is that needs to be repushed.
    //  ResetEnum       - No parms. Expects an enum on the top of stack,
    //                    which it resets to it's first value.
    //  Return          - No parms, just exits procedure. Return value is
    //                    already on the stack.
    //  TableJump       - Supports switch statements. The first index
    //                    refers to the index into a table of jump tables
    //                    in the method impl. It expects the case value
    //                    to be on the top of stack, which is pops. The
    //                    second index is the class id of the type being
    //                    switched on.
    //  Throw           - Invokes an exception. It will kick off the
    //                    whole sequence of events that that implies. The
    //                    stack top must hold an enumerated value, which is
    //                    the error being thrown. The class name, line
    //                    number, and error text will be added automatically.
    //                    The boolean immediate field, if true, indicates a
    //                    rethrow, else just a throw.
    //  ThrowFmt        - Like Throw but allows up to 4 formattable objects
    //                    to be pushed onto the stack after the enum. These
    //                    will be used to replace tokens in the error text.
    //                    This can only be a throw, not a rethrow. c4Immediate
    //                    indicates how many formattable objects there are.
    //  Try             - Starts a try block. It holds the offset of the
    //                    catch block as the c4Immediate, and will push
    //                    a try item onto the runtime stack with this
    //                    offset.
    //  TypeCast        - The first index is the target type. It expects
    //                    the type to be cast to be on the top of stack.
    //                  
    // ------------------------------------------------------------------------
    enum class EOpCodes
    {
        None
        , CallExcept
        , CallLocal
        , CallMember
        , CallParent
        , CallParm
        , CallStack
        , CallThis
        , ColIndex
        , Copy
        , CondEnumInc
        , CondJump
        , CondJumpNP
        , CurLine
        , EndTry
        , FlipTop
        , Jump
        , LogicalAnd
        , LogicalOr
        , LogicalXor
        , MultiPop
        , Negate
        , NoOp
        , NotCondJumpNP
        , NotCondJump
        , PopTop
        , PopToReturn
        , PushCurLine
        , PushEnum
        , PushException
        , PushImBoolean
        , PushImCard1
        , PushImCard2
        , PushImCard4
        , PushImCard8
        , PushImChar
        , PushImFloat4
        , PushImFloat8
        , PushImInt1
        , PushImInt2
        , PushImInt4
        , PushLocal
        , PushMember
        , PushParm
        , PushStrPoolItem
        , PushTempConst
        , PushTempVar
        , PushThis
        , Repush
        , ResetEnum
        , Return
        , TableJump
        , Throw
        , ThrowFmt
        , Try
        , TypeCast
        , Count
        , Min = None
        , Max = TypeCast
        , FirstCall = CallExcept
        , LastCall = CallThis
    };
    [[nodiscard]]  EOpCodes eXlatEOpCodes(const TString& strToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::False);
    [[nodiscard]]  const TString& strXlatEOpCodes(const EOpCodes eToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::True);
    [[nodiscard]]  tCIDLib::TBoolean bIsValidEnum(const EOpCodes eVal);

     TTextOutStream& operator<<(TTextOutStream& strmTar, const tCIDMacroEng::EOpCodes eToStream);
    
    // ------------------------------------------------------------------------
    //  The types of parameters we support
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
    [[nodiscard]]  EParmDirs eXlatEParmDirs(const TString& strToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::False);
    [[nodiscard]]  const TString& strXlatEParmDirs(const EParmDirs eToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::True);
    [[nodiscard]]  tCIDLib::TBoolean bIsValidEnum(const EParmDirs eVal);

     TTextOutStream& operator<<(TTextOutStream& strmTar, const tCIDMacroEng::EParmDirs eToStream);
}

#pragma CIDLIB_POPPACK




