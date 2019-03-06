//
// FILE NAME: MakeDocs_Type.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/14/1998
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
//  This is the types header for the program. It contains the tMakeDocs
//  namespace, which defines all of the program wide non-class types.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


namespace tMakeDocs
{
    // -------------------------------------------------------------------------
    //  Class category stuff
    //
    //  Each class can belong to various categories (up to all of them but no
    //  class is really part of all the categories.) The TClassDocInfo class,
    //  which represents classes internally, has a boolean array of
    //  EClassCats_Count elements. If that class belongs to a particular
    //  category that category's slot has kCIDLib::True in it.
    //
    //  This is used as an array of category names in MakeDocs_ThisFacility.cpp,
    //  so make sure to keep them in sync!
    // -------------------------------------------------------------------------
    enum class EClassCats
    {
        Audio
        , ClientServer
        , Collections
        , Communications
        , Coordinates
        , Cryptography
        , Debugging
        , EBiz
        , ErrorMgmt
        , Facilities
        , FileSystem
        , Fractals
        , Graphics
        , Handles
        , International
        , Janitors
        , Mathematics
        , MemoryMgmt
        , Miscellaneous
        , Mixins
        , NamingServices
        , ProcessControl
        , RayTracing
        , RTTI
        , Security
        , Socket
        , Streams
        , Strings
        , Synchronization
        , SystemInfo
        , TCPIP
        , TextProcessing
        , Threading
        , TimeServices
        , Windowing
        , XML

        , Count
        , Min           = Audio
        , Max           = XML
    };
}



// -----------------------------------------------------------------------------
//  Generate standard enum magic for some enums
// -----------------------------------------------------------------------------
StdEnumTricks(tMakeDocs::EClassCats)
