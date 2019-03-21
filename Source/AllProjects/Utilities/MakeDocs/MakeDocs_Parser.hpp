//
// FILE NAME: MakeDocs_Parser.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/10/1997
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
//  This is the header for the MakeDocs_Parser.cpp module, which implements
//  the TDocParser class. This class is a derivative of the standard text
//  stream lexer, from CIDLib.Dll. Our derivative just adds a little value
//  by building on the functionality of the base lexer class which understands
//  the particular lexical elements of our source files.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ----------------------------------------------------------------------------
//  CLASS: TDocParser
// PREFIX: prsr
// ----------------------------------------------------------------------------
class TDocParser : public TObject
{
    public  :
        // --------------------------------------------------------------------
        //  Constructors and Destructors.
        // --------------------------------------------------------------------
        TDocParser();

        ~TDocParser();


        // --------------------------------------------------------------------
        //  Public, non-virtual methods
        // --------------------------------------------------------------------


    private :
        // --------------------------------------------------------------------
        //  Magic macros
        // --------------------------------------------------------------------
        RTTIDefs(TDocParser, TObject)
};
