// @file LexXML.cxx
// Lexer for the CQC CML macro language
//

#include <cstdlib>
#include <cassert>
#include <cstring>
#include "ILexer.h"
#include "Scintilla.h"
#include "SciLexer.h"
#include "LexAccessor.h"
#include "StyleContext.h"
#include "LexerModule.h"
#include "CharacterSet.h"
#include "WordList.h"

#ifdef SCI_NAMESPACE
using namespace Scintilla ;
#endif

static const char* const CMLWordListDesc [] =
{
    "CML keywords",
    0
};

static const char* const CMLKeywords =
{
    "Methods EndMethods Method EndMethod Literals EndLiterals Imports EndImports "
    "ClassPath Locals EndLocals Case EndCase Switch If EndIf Else ElseIf IfElse Begin Constructor "
    "EndConstructor Types EndTypes Members EndMembers ParentClass Class EndClass "
    "In Out InOut Try EndTry Catch EndCatch Rethrow Return While EndWhile "
    "Public Final NonFinal Required Overrides Returns Const VectorOf FTCase TypeCast "
    "ArrayOf $CurClassName $CurLine $Exception $Parent DynTypeRef Directives False "
    "True DoLoop Break EndForEach ForEach"
};

static const char* const CMLTypes =
{
    "BaseInfo Boolean Card1 Card2 Card4 Char Enum Exception Formatabble "
    "Float4 Float8 Int1 Int2 Int4 String StringList Time Void "

    "ASCII AsyncHTTPCl Array Audio Base64 CommCfg CommPort ConsoleOutStream "
    "DataSrc DBConnect DBStatement DGramSocket DirIter IPEndPoint FileInStream "
    "FileOutStream FileSystem HTTPClient JSONAnchor JSONParser KVPair MD5 Digest "
    "MD5Hash MemBuf NamedValMap RandomNum Socket SockPinger Speech StreamSocket "
    "StringInStream StringOutStream StringTokenizer SysInfo TextInStream "
    "TextXCoder URL USBHID Vector XMLTreeAnchor XMLTreeParser "

    "AppCtrlClient CMBinBase CMLWSockBase CQCDriverBase CQCFldDef CRCHelpers "
    "CQCLogger CQCMediaBrwsInfo CQCMediaInfo CQCPLItem CQCPLMgr CQCUtils Event "
    "EventHandler EvMonBase MediaRenderer SimpleFldClient XGWClient V2Helpers"
};


class LexerCML : public ILexer
{
    public :
        LexerCML() :

            pointInNumberUsed(false),
            setKeywordChars(CharacterSet::setAlphaNum, "."),
            setBracketChars(CharacterSet::setNone, "[()]")
        {
            cmlKeywords.Set(CMLKeywords);
            cmlTypes.Set(CMLTypes);
        }

        virtual ~LexerCML () {}

        int SCI_METHOD Version () const
        {
            return 1;
        }

        void SCI_METHOD Release ()
        {
            delete this ;
        }

        const char* SCI_METHOD PropertyNames ()
        {
            return NULL ;
        }

        int SCI_METHOD PropertyType(const char*)
        {
            return -1;
        }

        const char* SCI_METHOD DescribeProperty(const char*)
        {
            return NULL;
        }

        int SCI_METHOD PropertySet(const char*, const char*)
        {
            return -1;
        }

        const char* SCI_METHOD DescribeWordListSets()
        {
            return CMLWordListDesc[0];
        }

        int SCI_METHOD WordListSet(int, const char*)
        {
            return -1;
        }

        void SCI_METHOD Lex(unsigned int startPos, int lengthDoc, int initStyle, IDocument* pAccess);

        void SCI_METHOD Fold(unsigned int startPos, int lengthDoc, int initStyle, IDocument* pAccess);

        void * SCI_METHOD PrivateCall(int, void*)
        {
            return NULL ;
        }

        static ILexer * LexerFactoryCML()
        {
            return new LexerCML();
        }

    private :
        void InitCharCheck(StyleContext& scToUse);

        bool pointInNumberUsed;
        WordList cmlKeywords;
        WordList cmlTypes;

        CharacterSet setKeywordChars;
        CharacterSet setBracketChars;
};

void
SCI_METHOD LexerCML::Lex(unsigned int startPos, int lengthDoc, int initStyle, IDocument* pAccess)
{
    const unsigned int MAX_STR_LEN = 200;
    LexAccessor styler(pAccess);
    StyleContext scCTX(startPos, lengthDoc, initStyle, styler);
    char* tmpStr = 0;
    tmpStr = new char [MAX_STR_LEN];

    for (; scCTX.More(); scCTX.Forward())
    {
        switch(scCTX.state)
        {
            case SCE_CML_BRACKET :
            case SCE_CML_DEFAULT :
            case SCE_CML_OPERATOR :
                InitCharCheck(scCTX);
                break ;

            case SCE_CML_COMMENT :
                if (scCTX.atLineEnd)
                    scCTX.SetState(SCE_CML_DEFAULT);
                break ;

            case SCE_CML_STRING :
                //
                //  Watch for an escaped string, and don't exit string mode. Basically
                //  we just skip any escaped char.
                //
                if (scCTX.Match('\\'))
                    scCTX.Forward();
                else if (scCTX.Match('\"'))
                    scCTX.ForwardSetState(SCE_CML_DEFAULT);
                break ;

            case SCE_CML_NUMBER :
                if (scCTX.Match('.'))
                {
                    if (pointInNumberUsed)
                        InitCharCheck(scCTX);
                    else
                        pointInNumberUsed = true;
                }
                 else
                {
                    if (!IsADigit(scCTX.ch))
                        InitCharCheck(scCTX);
                };
                break ;

            case SCE_CML_IDENTIFIER :
                if (!setKeywordChars.Contains(scCTX.ch))
                {
                    memset(tmpStr, 0, sizeof(char) * MAX_STR_LEN);
                    scCTX.GetCurrent(tmpStr, MAX_STR_LEN);

                    if ((scCTX.ch == '_') || (scCTX.ch == '-'))
                    {
                        //
                        //  It's a user defined variable, so we have to now skip
                        //  forward past any id characters before we start looking
                        //  for anything else, so we use the failed id state for
                        //  that.
                        //
                        scCTX.ChangeState(SCE_CML_DEFAULT);
                        scCTX.SetState(SCE_CML_FAILEDID);
                    }
                     else if (!strncmp(tmpStr, "MEng.", 5) || cmlTypes.InList(tmpStr))
                    {
                        // It's a full class path
                        scCTX.ChangeState(SCE_CML_TYPE);
                    }
                     else if (cmlKeywords.InList(tmpStr))
                    {
                        // It's a known keyord
                        scCTX.ChangeState(SCE_CML_KEYWORD);
                    }
                     else
                    {
                        // Nothing special set put it back to default
                        scCTX.ChangeState(SCE_CML_DEFAULT);
                    }

                    //
                    //  Do an initial character check on the failing char now,
                    //  if we didn't go into failed id state.
                    //
                    if (scCTX.state != SCE_CML_FAILEDID)
                        InitCharCheck(scCTX);
                }
                break;

            case SCE_CML_FAILEDID :
                //
                //  We want to just eat id chars. When we finally hit a non
                //  id char and non-hypen or underscore, check init state
                //  again.
                //
                if (!setKeywordChars.Contains(scCTX.ch)
                &&  (scCTX.ch != '_')
                &&  (scCTX.ch != '-'))
                {
                    scCTX.ChangeState(SCE_CML_DEFAULT);
                    InitCharCheck(scCTX);
                }
                break;
        };
    }
    scCTX.Complete();
    delete [] tmpStr;
}

void SCI_METHOD LexerCML::Fold(unsigned int, int, int, IDocument*)
{
}

//
//  Because of the fact that we have to retroactively figure out what we have in
//  some cases, and are already on the first char of the next thing, we have to
//  figure out what that next char is, and need to do this in multiple cases of
//  the lexer. So we break it out here.
//
void LexerCML::InitCharCheck(StyleContext& scToUse)
{
    if (scToUse.Match('/', '/'))
    {
        scToUse.SetState(SCE_CML_COMMENT);
        scToUse.Forward();
    }
     else if (scToUse.Match('!', '=')
          ||  scToUse.Match('+', '=')
          ||  scToUse.Match('-', '=')
          ||  scToUse.Match('/', '=')
          ||  scToUse.Match('*', '=')
          ||  scToUse.Match(':', '=')
          ||  scToUse.Match('+', '+')
          ||  scToUse.Match('-', '-'))
    {
        scToUse.SetState(SCE_CML_OPERATOR);
        scToUse.Forward();
    }
     else if (scToUse.Match('=')
          ||  scToUse.Match('-')
          ||  scToUse.Match('+')
          ||  scToUse.Match('/')
          ||  scToUse.Match('*')
          ||  scToUse.Match(';')
          ||  scToUse.Match(':')
          ||  scToUse.Match(','))
    {
        scToUse.SetState(SCE_CML_OPERATOR);
    }
     else if (scToUse.Match('\"'))
    {
        scToUse.SetState(SCE_CML_STRING);
    }
     else if (IsADigit(scToUse.ch) || (scToUse.Match('-') && IsADigit(scToUse.chNext)))
    {
        scToUse.SetState(SCE_CML_NUMBER);
        pointInNumberUsed = false;
    }
     else if (setBracketChars.Contains(scToUse.ch))
    {
        scToUse.SetState(SCE_CML_BRACKET);
    }
     else if (setKeywordChars.Contains(scToUse.ch))
    {
        scToUse.SetState(SCE_CML_IDENTIFIER);
    }
     else
    {
        scToUse.SetState(SCE_CML_DEFAULT);
    }
}


LexerModule lmCML(SCLEX_CML, LexerCML::LexerFactoryCML, "cml", CMLWordListDesc);

