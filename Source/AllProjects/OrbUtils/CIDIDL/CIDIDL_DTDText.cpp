//
// FILE NAME: CIDIDL_DTDText_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/07/2000
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
//  This file defines the static facility class member, s_strEmbeddedDTDText,
//  which holds the text for the DTD that we apply to CIDIDL files.
//
// This file contains the DTD for describing the methods and parameters,
// and their types, for CIDIDL interfaces. The IDL compiler will force this
// DTD to be used against CIDIDL files.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDIDL.hpp"



TString TFacCIDIDL::s_strEmbeddedDTDText =
L"<?xml encoding='$NativeWideChar$'?>\n"

//
// Its a signed value. This type maps the tCIDLib::TInt4 type.
//
L"<!ELEMENT   CIDIDL:TInt4 EMPTY>\n"

//
// Its a 4 byte unsigned value. This type maps the tCIDLib::TCard4 type.
//
L"<!ELEMENT   CIDIDL:TCard4 EMPTY>\n"

//
// Its a Unicode char. Maps to hhe tCIDLib::TCh type. The streaming system
// flattens text to UTF-8 format.
//
L"<!ELEMENT   CIDIDL:TCh EMPTY>\n"

//
// Its a floating value. This type maps the tCIDLib::TFloat8 type.
//
L"<!ELEMENT   CIDIDL:TFloat8 EMPTY>\n"

//
// Its just an arbitrary string. It will map to the TString class. The
// streaming system flattens text to UTF-8 format.
//
L"<!ELEMENT   CIDIDL:TString EMPTY>\n"

//
// Its a 8 byte unsigned value. This type maps the tCIDLib::TCard8 type.
//
L"<!ELEMENT   CIDIDL:TCard8 EMPTY>\n"

//
// Its a 8 byte signed value. This type maps the tCIDLib::TInt8 type.
//
L"<!ELEMENT   CIDIDL:TInt8 EMPTY>\n"

//
//  The value represents an enumerated type. The Type attribute indicates the
//  actual type it maps to.
//
L"<!ELEMENT   CIDIDL:Enumerated EMPTY>\n"
L"<!ATTLIST   CIDIDL:Enumerated\n"
L"            CIDIDL:Type CDATA #REQUIRED>\n"

//
//  The value represents a boolean type. It will map to the tCIDLib::TBoolean
//  type.
//
L"<!ELEMENT   CIDIDL:TBoolean EMPTY>\n"

//
//  The value is a bag of objects of a particular type. The ElemType field
//  indicates the class type of the elements. It will map to the TBag<ElemType>
//  template collection type.
//
L"<!ELEMENT   CIDIDL:TBag EMPTY>\n"
L"<!ATTLIST   CIDIDL:TBag\n"
L"            CIDIDL:ElemType NMTOKEN #REQUIRED>\n"

//
//  The value is a hash set of objects of a particular type. The
//  ElemType field indicates the class type of the elements. The KeyOps
//  field indicates the class that handles key ops for the element type.
//
//  It will map to the THashsSet<Elem,KeyOps> template collection type.
//  In this case, the element type is the key type.
//
//  The Modulus field is the hash modulus to use in the hash table.
//
L"<!ELEMENT   CIDIDL:THashSet EMPTY>\n"
L"<!ATTLIST   CIDIDL:THashSet\n"
L"            CIDIDL:ElemType NMTOKEN #REQUIRED\n"
L"            CIDIDL:KeyOps NMTOKEN #REQUIRED\n"
L"            CIDIDL:Modulus NMTOKEN #REQUIRED>\n"

//
//  The value is a keyed hash set of objects of a particular type. The
//  ElemType field indicates the class type of the elements. The KeyType
//  field indicates the class of the element's key. The KeyOps field
//  indicates the class that handles key ops for the key type.
//
//  It will map to the TKeyedHashsSet<Elem,Key,KeyOps> template collection
//  type.
//
//  The KeyExtract field indicates a key extraction method that is required
//  to get to the key field. Obviously this must be available on both sides
//  of the connection to work. It must be a static method of the element
//  class. This isn't a general restriction, just one of using them via
//  the IDL.
//
//  The Modulus field indicates the hash modulus you want the set to have.
//
L"<!ELEMENT   CIDIDL:TKeyedHashSet EMPTY>\n"
L"<!ATTLIST   CIDIDL:TKeyedHashSet\n"
L"            CIDIDL:ElemType NMTOKEN #REQUIRED\n"
L"            CIDIDL:KeyType NMTOKEN #REQUIRED\n"
L"            CIDIDL:KeyOps NMTOKEN #REQUIRED\n"
L"            CIDIDL:KeyExtract NMTOKEN #REQUIRED\n"
L"            CIDIDL:Modulus NMTOKEN #REQUIRED>\n"

//
//  The value is a vector of objects of a particular type. The ElemType field
//  indicates the class type of the elements. It will map to the
//  TVector<ElemType> template collection type.
//
L"<!ELEMENT   CIDIDL:TVector EMPTY>\n"
L"<!ATTLIST   CIDIDL:TVector\n"
L"            CIDIDL:ElemType NMTOKEN #REQUIRED>\n"

//
//  The value is a vector of items of a particular fundamental type. The ElemType
//  field indicates the class type of the elements. It will map to the
//  TFundVector<ElemType> template collection type.
//
L"<!ELEMENT   CIDIDL:TFundVector EMPTY>\n"
L"<!ATTLIST   CIDIDL:TFundVector\n"
L"            CIDIDL:ElemType CDATA #REQUIRED>\n"

//
//  The value is an array of items of a particular fundamental type. The ElemType
//  field indicates the class type of the elements. It will map to the
//  TFundArray<ElemType> template collection type.
//
L"<!ELEMENT   CIDIDL:TFundArray EMPTY>\n"
L"<!ATTLIST   CIDIDL:TFundArray\n"
L"            CIDIDL:ElemType CDATA #REQUIRED>\n"

//
//  The value is an object of a particular type. The Type field indicates
//  the class type of the object. You can mark it as movable, in which case
//  the server side generated code will do a move into the server side call
//  and there won't be a size value since the buffer will be the size that
//  was sent.
//
L"<!ELEMENT   CIDIDL:Object EMPTY>\n"
L"<!ATTLIST   CIDIDL:Object\n"
L"            CIDIDL:Type NMTOKEN #REQUIRED\n>\n"

//
//  Some special cases of object, which handle memory buffers. It's common to
//  stream a buffer of data over, which has pre-flattened info or just raw
//  data of some sort. But, there is nothing in the memory buffer classes that
//  indicate how much of the current allocation size is in use. This can cause
//  huge inefficiency by streaming over unused bytes. So we provide a special
//  case for memory buffers that includes the buffer itself and the number of
//  bytes to stream. These will cause another parm, an In TCard4 to be placed
//  before them, which must indicate the bytes to be streamed.
//
//  You can mark heap buffers as movable. In that case, the server side call
//  will do a move from the temp to the target, and there won't be a separate
//  size parameter since the buffer will be the size that was sent.
//
//  TMemBuf is an abstract class, but is OK for Out parameters and convenient to
//  have for those.
//
L"<!ELEMENT   CIDIDL:THeapBuf EMPTY>\n"
L"<!ELEMENT   CIDIDL:TMemBuf EMPTY>\n"



//
//  Used for return values to indicate no return type.
//
L"<!ELEMENT   CIDIDL:TVoid EMPTY>\n"

//
//  The types we support for parameters. These map to the EParmTypes enum.
//
L"<!ENTITY %  IDLPTypes   'CIDIDL:TInt4"
L"                        | CIDIDL:TInt8"
L"                        | CIDIDL:TCard4"
L"                        | CIDIDL:TCard8"
L"                        | CIDIDL:TCh"
L"                        | CIDIDL:TFloat8"
L"                        | CIDIDL:TString"
L"                        | CIDIDL:Enumerated"
L"                        | CIDIDL:TBag"
L"                        | CIDIDL:THashSet"
L"                        | CIDIDL:TKeyedHashSet"
L"                        | CIDIDL:TVector"
L"                        | CIDIDL:TFundArray"
L"                        | CIDIDL:TFundVector"
L"                        | CIDIDL:THeapBuf"
L"                        | CIDIDL:TMemBuf"
L"                        | CIDIDL:Object"
L"                        | CIDIDL:TBoolean'>"

//
//  The types we support for return values
//
L"<!ENTITY %  IDLRTypes   'CIDIDL:TInt4"
L"                        | CIDIDL:TInt8"
L"                        | CIDIDL:TCard4"
L"                        | CIDIDL:TCard8"
L"                        | CIDIDL:TCh"
L"                        | CIDIDL:TFloat8"
L"                        | CIDIDL:TString"
L"                        | CIDIDL:Enumerated"
L"                        | CIDIDL:Object"
L"                        | CIDIDL:TBoolean"
L"                        | CIDIDL:TVoid'>\n"

//
//  The types we support for global constants. In this case, the values are
//  the text for a parameter, so it's not for the content model of an element
//  and has no QName prefix.
//
//  These are also the supported types for structure members (plus other already
//  defined structures.)
//
L"<!ENTITY %  IDLCTypes   ' TInt1"
L"                        | TInt2"
L"                        | TInt4"
L"                        | TInt8"
L"                        | TCard1"
L"                        | TCard2"
L"                        | TCard4"
L"                        | TCard8"
L"                        | TCh"
L"                        | TFloat8"
L"                        | TString"
L"                        | TBoolean'>"


// The types we support for enum base types
L"<!ENTITY % BaseTypes  ' Default"
L"                        | TInt1"
L"                        | TInt2"
L"                        | TInt4"
L"                        | TCard1"
L"                        | TCard2"
L"                        | TCard4'>"

//
//  Each method can have one or more parameters. This element describes a
//  parameter. The values for each
//
L"<!ELEMENT   CIDIDL:Param (%IDLPTypes;)>\n"
L"<!ATTLIST   CIDIDL:Param\n"
L"            CIDIDL:Name NMTOKEN #REQUIRED\n"
L"            CIDIDL:Dir (In|Out|InOut) #REQUIRED\n"
L"            CIDIDL:DefVal CDATA #IMPLIED\n"
L"            CIDIDL:Moveable (Yes|No) 'No'>\n"

//
//  Each method has a return type. It must be provided explicitly, which
//  means you should use CIDIDL:Void if it returns nothing.
//
L"<!ELEMENT   CIDIDL:RetType (%IDLRTypes;)>\n"

//
//  This is a special element to define the constructor. In actual fact,
//  there are some parameters that are always added, so this just defines
//  extra ones that the IDL compiler will generate following the standard
//  ones.
//
L"<!ELEMENT   CIDIDL:Ctor (CIDIDL:Param+)>\n"

//
//  The method section desribes a single method in an interface. It's
//  contents are the return type and parameters.
//
L"<!ELEMENT   CIDIDL:Method (CIDIDL:RetType, CIDIDL:Param*)>\n"
L"<!ATTLIST   CIDIDL:Method\n"
L"            CIDIDL:TimeOut CDATA '30000'\n"
L"            CIDIDL:Name NMTOKEN #REQUIRED\n"
L"            CIDIDL:InBaseClass (Yes|No) 'No'>\n"

//
//  This is a special type of method. It has the same definition as a
//  regular one, but has special semantics to the compiler so it needs
//  a unique element name.
//
L"<!ELEMENT   CIDIDL:PollMethod (CIDIDL:RetType, CIDIDL:Param*)>\n"
L"<!ATTLIST   CIDIDL:PollMethod\n"
L"            CIDIDL:TimeOut CDATA '10000'\n"
L"            CIDIDL:Name NMTOKEN #REQUIRED\n"
L"            CIDIDL:InBaseClass (Yes|No) 'No'>\n"


// Some stuff below can have documentation nodes
L"<!ELEMENT   CIDIDL:DocText (#PCDATA)>\n"

//
//  Defines a constant, with optional documentation, and a list of such constants.
//
L"<!ELEMENT   CIDIDL:Constant (CIDIDL:DocText?)>\n"
L"<!ATTLIST   CIDIDL:Constant\n"
L"            CIDIDL:Name NMTOKEN #REQUIRED\n"
L"            CIDIDL:Type (%IDLCTypes;) #REQUIRED\n"
L"            CIDIDL:Value CDATA #REQUIRED>\n"
L"<!ELEMENT   CIDIDL:Constants (CIDIDL:Constant+)>\n"


//
//  Defines a structure. It can consist of members of the fundamental types, or of
//  other already defined structures.
//
L"<!ELEMENT   CIDIDL:Member EMPTY>\n"
L"<!ATTLIST   CIDIDL:Member\n"
L"            CIDIDL:Name NMTOKEN #REQUIRED\n"
L"            CIDIDL:Type (%IDLCTypes;) #REQUIRED>\n"
L"<!ELEMENT   CIDIDL:SubStruct EMPTY>\n"
L"<!ATTLIST   CIDIDL:SubStruct\n"
L"            CIDIDL:Name NMTOKEN #REQUIRED\n"
L"            CIDIDL:Type NMTOKEN #REQUIRED>\n"
L"<!ELEMENT   CIDIDL:Struct (CIDIDL:Member|CIDIDL:SubStruct)*>\n"
L"<!ATTLIST   CIDIDL:Struct\n"
L"            CIDIDL:Type NMTOKEN #REQUIRED>\n"

//
//  The Globals section defines lists of constants and Types. This section
//  is optional and is used to generate headers that contain the defined
//  consts and types.
//
L"<!ELEMENT   CIDIDL:EnumVal EMPTY>\n"
L"<!ATTLIST   CIDIDL:EnumVal\n"
L"            CIDIDL:Name NMTOKEN #REQUIRED\n"
L"            CIDIDL:AltText CDATA #IMPLIED\n"
L"            CIDIDL:AltText2 CDATA #IMPLIED\n"
L"            CIDIDL:Text CDATA #IMPLIED\n"
L"            CIDIDL:Value CDATA #IMPLIED\n"
L"            CIDIDL:AltNumVal CDATA #IMPLIED>\n"
L"<!ELEMENT   CIDIDL:EnumSyn EMPTY>\n"
L"<!ATTLIST   CIDIDL:EnumSyn\n"
L"            CIDIDL:Name NMTOKEN #REQUIRED\n"
L"            CIDIDL:Value CDATA #IMPLIED>\n"
L"<!ELEMENT   CIDIDL:Enum (CIDIDL:DocText?, CIDIDL:EnumVal+, CIDIDL:EnumSyn*)>\n"


L"<!ATTLIST   CIDIDL:Enum\n"
L"            CIDIDL:Name NMTOKEN #REQUIRED\n"

//
//  They can set a base type. For a bitmapped one, if not set explicitly, it will
//  be forced to TCard4. Otherwise, the compiler will choose.
//
L"            CIDIDL:BaseType (%BaseTypes;) #IMPLIED\n"

// Is this a contiguously numbered enum or does it have disjoint values
L"            CIDIDL:Type (Std|Bmp) 'Std'\n"

//
//  Indicate how alt text values are defined, and whether we support an alt numeric
//  value Note that we figure out the text source ourself, since it would be annoying to
//  have to always indicate it.
//
L"            CIDIDL:AltNumVal (Yes|No) 'No'\n"
L"            CIDIDL:AltTextSrc (No|Inline|Pattern) 'No'\n"
L"            CIDIDL:AltText2 (No|Yes) 'No'\n"


//
//  Cause eAltXlat/strAltXlat and eAltXlat2/strAltXlat2 methods to be generated to map
//  between the enum and one of the text values, presumably ones not used by the Xlat
//  Value above or each other. If not name or base name, then the type of text
//  referenced must be enabled above.
//
L"            CIDIDL:AltMap (No|AltText|AltText2|BaseName|Name|Text) 'No'\n"
L"            CIDIDL:AltMap2 (No|AltText|AltText2|BaseName|Name|Text) 'No'\n"
L"            CIDIDL:AltPattern CDATA ''\n"

//
//  Indictes whether we support the Format method, which can be used to format out
//  a list of the types of one of the to/from mapping table values. It can be used to
//  generate an enumeration list in some different formats. If not name or base name
//  then the type of text must have been enabled above.
//
L"            CIDIDL:FormatMap (No|AltText|AltText2|BaseName|Name|Text) 'No'\n"


//
//  Causes a strLoadXXX type method to be generated to load one of the types of text
//  values
L"            CIDIDL:LoadMap (No|AltText|AltText2|BaseName|Name|Text) 'No'\n"


//
//  Causes eXlatXXX and strXlatXXX methods to be generated to map between the enum
//  and one of the text values.
//
L"            CIDIDL:XlatMap (No|AltText|AltText2|BaseName|Name|Text) 'No'\n"


//
//  Causes an << operator to a text out stream to be generated for the indicated text
//  type.
//
L"            CIDIDL:TextStreamMap (No|AltText|AltText2|BaseName|Name|Text) 'No'\n"

// Controls whether binary streaming methods are generated
L"            CIDIDL:BinStream (No|Yes) 'No'\n"

// Controls whether inc/dec operators are generated
L"            CIDIDL:IncDec (No|Inc|Both) 'No'\n"


// Only used if CIDIDL:TextType is Load
L"            CIDIDL:MsgFac CDATA #IMPLIED\n"
L"            CIDIDL:MsgNS CDATA #IMPLIED>\n"


L"<!ELEMENT   CIDIDL:Types (CIDIDL:Enum|CIDIDL:Struct)*>\n"

L"<!ELEMENT   CIDIDL:Globals (CIDIDL:Constants?, CIDIDL:Types?)>\n"

//
//  The methods section contains zero or more methods or poll methods. This
//  is the section where the methods for the interface are defined.
//
L"<!ELEMENT   CIDIDL:Methods (CIDIDL:PollMethod | CIDIDL:Method)*>\n"

//
//  The IntfClass section contains optional constants, and contructor and
//  methods definitions. This is the part that is used to generate the actual
//  client proxy and server stub classes. It's attributes provide some overall
//  information about the classes generated.
//
L"<!ELEMENT   CIDIDL:ClassIntf (CIDIDL:Constants?, CIDIDL:Types?, CIDIDL:Ctor?, CIDIDL:Methods)>\n"
L"<!ATTLIST   CIDIDL:ClassIntf\n"
L"            CIDIDL:Name NMTOKEN #REQUIRED\n"
L"            CIDIDL:InterfaceId CDATA #REQUIRED\n"
L"            CIDIDL:ClientBase NMTOKEN 'TOrbClientBase'\n"
L"            CIDIDL:ServerBase NMTOKEN 'TOrbServerBase'>\n"


//
//  The top level interface element contains the optional globals and
//  interface classes sections. The globals section is used to generate a
//  header of constants and and enums. The class interface is used to generate
//  the actual client proxy and/or server stubb classes.
//
L"<!ELEMENT   CIDIDL:Interface (CIDIDL:Globals*, CIDIDL:ClassIntf?)>\n";

