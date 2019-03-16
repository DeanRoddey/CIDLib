//
// FILE NAME: CIDMacroEng_ParserTokenMap_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/87/2003
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
//  Due to the size of this map, we put it in this header and include it
//  into the CIDMacroEng_Parser.cpp file where it's needed.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//

TMapItem aitemMap[tCIDMacroEng::ETokens::Count] =
{
        { L"Abstract"           , 0 , tCIDMacroEng::ETokens::Abstract        }
    ,   { L"ArrayOf"            , 0 , tCIDMacroEng::ETokens::ArrayOf         }
    ,   { L"Begin"              , 0 , tCIDMacroEng::ETokens::Begin           }
    ,   { L"#BeginDebug"        , 0 , tCIDMacroEng::ETokens::BeginDebug      }
    ,   { L"Break"              , 0 , tCIDMacroEng::ETokens::Break           }
    ,   { L"Case"               , 0 , tCIDMacroEng::ETokens::Case            }
    ,   { L"Catch"              , 0 , tCIDMacroEng::ETokens::Catch           }
    ,   { L"Class"              , 0 , tCIDMacroEng::ETokens::Class           }
    ,   { L"ClassPath"          , 0 , tCIDMacroEng::ETokens::ClassPath       }
    ,   { L"Const"              , 0 , tCIDMacroEng::ETokens::Const           }
    ,   { L"Constructor"        , 0 , tCIDMacroEng::ETokens::Constructor     }
    ,   { L"$CurClassName"      , 0 , tCIDMacroEng::ETokens::CurClassName    }
    ,   { L"$CurLine"           , 0 , tCIDMacroEng::ETokens::CurLine         }
    ,   { L"Default"            , 0 , tCIDMacroEng::ETokens::Default         }
    ,   { L"Destructor"         , 0 , tCIDMacroEng::ETokens::Destructor      }
    ,   { L"Directives"         , 0 , tCIDMacroEng::ETokens::Directives      }
    ,   { L"DoLoop"             , 0 , tCIDMacroEng::ETokens::DoLoop          }
    ,   { L"DoWhile"            , 0 , tCIDMacroEng::ETokens::DoWhile         }
    ,   { L"DynTypeRef"         , 0 , tCIDMacroEng::ETokens::DynTypeRef      }
    ,   { L"Else"               , 0 , tCIDMacroEng::ETokens::Else            }
    ,   { L"ElseIf"             , 0 , tCIDMacroEng::ETokens::ElseIf          }
    ,   { L"EndCase"            , 0 , tCIDMacroEng::ETokens::EndCase         }
    ,   { L"EndCatch"           , 0 , tCIDMacroEng::ETokens::EndCatch        }
    ,   { L"EndClass"           , 0 , tCIDMacroEng::ETokens::EndClass        }
    ,   { L"EndConstructor"     , 0 , tCIDMacroEng::ETokens::EndConstructor  }
    ,   { L"#EndDebug"          , 0 , tCIDMacroEng::ETokens::EndDebug        }
    ,   { L"EndDestructor"      , 0 , tCIDMacroEng::ETokens::EndDestructor   }
    ,   { L"EndDirectives"      , 0 , tCIDMacroEng::ETokens::EndDirectives   }
    ,   { L"EndEnum"            , 0 , tCIDMacroEng::ETokens::EndEnum         }
    ,   { L"EndForEach"         , 0 , tCIDMacroEng::ETokens::EndForEach      }
    ,   { L"EndIf"              , 0 , tCIDMacroEng::ETokens::EndIf           }
    ,   { L"EndImports"         , 0 , tCIDMacroEng::ETokens::EndImports      }
    ,   { L"EndLiterals"        , 0 , tCIDMacroEng::ETokens::EndLiterals     }
    ,   { L"EndLocals"          , 0 , tCIDMacroEng::ETokens::EndLocals       }
    ,   { L"EndMembers"         , 0 , tCIDMacroEng::ETokens::EndMembers      }
    ,   { L"EndMethod"          , 0 , tCIDMacroEng::ETokens::EndMethod       }
    ,   { L"EndMethods"         , 0 , tCIDMacroEng::ETokens::EndMethods      }
    ,   { L"EndSwitch"          , 0 , tCIDMacroEng::ETokens::EndSwitch       }
    ,   { L"EndTry"             , 0 , tCIDMacroEng::ETokens::EndTry          }
    ,   { L"EndTypes"           , 0 , tCIDMacroEng::ETokens::EndTypes        }
    ,   { L"EndWhile"           , 0 , tCIDMacroEng::ETokens::EndWhile        }
    ,   { L"Enum"               , 0 , tCIDMacroEng::ETokens::Enum            }
    ,   { L"$Exception"         , 0 , tCIDMacroEng::ETokens::Exception       }
    ,   { L"False"              , 0 , tCIDMacroEng::ETokens::False           }
    ,   { L"Final"              , 0 , tCIDMacroEng::ETokens::Final           }
    ,   { L"ForEach"            , 0 , tCIDMacroEng::ETokens::ForEach         }
    ,   { L"FTCase"             , 0 , tCIDMacroEng::ETokens::FTCase          }
    ,   { L"If"                 , 0 , tCIDMacroEng::ETokens::If              }
    ,   { L"Imports"            , 0 , tCIDMacroEng::ETokens::Imports         }
    ,   { L"In"                 , 0 , tCIDMacroEng::ETokens::In              }
    ,   { L"InOut"              , 0 , tCIDMacroEng::ETokens::InOut           }
    ,   { L"Literals"           , 0 , tCIDMacroEng::ETokens::Literals        }
    ,   { L"Locals"             , 0 , tCIDMacroEng::ETokens::Locals          }
    ,   { L"Members"            , 0 , tCIDMacroEng::ETokens::Members         }
    ,   { L"Method"             , 0 , tCIDMacroEng::ETokens::Method          }
    ,   { L"Methods"            , 0 , tCIDMacroEng::ETokens::Methods         }
    ,   { L"NonConst"           , 0 , tCIDMacroEng::ETokens::NonConst        }
    ,   { L"NonFinal"           , 0 , tCIDMacroEng::ETokens::NonFinal        }
    ,   { L"Out"                , 0 , tCIDMacroEng::ETokens::Out             }
    ,   { L"Overrides"          , 0 , tCIDMacroEng::ETokens::Overrides       }
    ,   { L"$Parent"            , 0 , tCIDMacroEng::ETokens::Parent          }
    ,   { L"ParentClass"        , 0 , tCIDMacroEng::ETokens::ParentClass     }
    ,   { L"Private"            , 0 , tCIDMacroEng::ETokens::Private         }
    ,   { L"Public"             , 0 , tCIDMacroEng::ETokens::Public          }
    ,   { L"Required"           , 0 , tCIDMacroEng::ETokens::Required        }
    ,   { L"Rethrow"            , 0 , tCIDMacroEng::ETokens::Rethrow         }
    ,   { L"Return"             , 0 , tCIDMacroEng::ETokens::Return          }
    ,   { L"Returns"            , 0 , tCIDMacroEng::ETokens::Returns         }
    ,   { L"Switch"             , 0 , tCIDMacroEng::ETokens::Switch          }
    ,   { L"This"               , 0 , tCIDMacroEng::ETokens::This            }
    ,   { L"Throw"              , 0 , tCIDMacroEng::ETokens::Throw           }
    ,   { L"True"               , 0 , tCIDMacroEng::ETokens::True            }
    ,   { L"Try"                , 0 , tCIDMacroEng::ETokens::Try             }
    ,   { L"TypeCast"           , 0 , tCIDMacroEng::ETokens::TypeCast        }
    ,   { L"Types"              , 0 , tCIDMacroEng::ETokens::Types           }
    ,   { L"VectorOf"           , 0 , tCIDMacroEng::ETokens::VectorOf        }
    ,   { L"While"              , 0 , tCIDMacroEng::ETokens::While           }


    //
    //  These are not used for lookup, but we do them for translation from
    //  a token to its description.
    //
    , { L":"                    , 0 , tCIDMacroEng::ETokens::Colon           }
    , { L","                    , 0 , tCIDMacroEng::ETokens::Comma           }
    , { L"="                    , 0 , tCIDMacroEng::ETokens::EqualSign       }
    , { L"!"                    , 0 , tCIDMacroEng::ETokens::Exclaim         }
    , { L"]"                    , 0 , tCIDMacroEng::ETokens::CloseBracket    }
    , { L")"                    , 0 , tCIDMacroEng::ETokens::CloseParen      }
    , { L"["                    , 0 , tCIDMacroEng::ETokens::OpenBracket     }
    , { L"("                    , 0 , tCIDMacroEng::ETokens::OpenParen       }
    , { L"."                    , 0 , tCIDMacroEng::ETokens::Period          }
    , { L";"                    , 0 , tCIDMacroEng::ETokens::SemiColon       }

    , { L"+"                    , 0 , tCIDMacroEng::ETokens::Add             }
    , { L"&"                    , 0 , tCIDMacroEng::ETokens::And             }
    , { L"&="                   , 0 , tCIDMacroEng::ETokens::AndEq           }
    , { L":="                   , 0 , tCIDMacroEng::ETokens::Assign          }
    , { L"--"                   , 0 , tCIDMacroEng::ETokens::Dec             }
    , { L"/"                    , 0 , tCIDMacroEng::ETokens::Divide          }
    , { L"/="                   , 0 , tCIDMacroEng::ETokens::DivideEq        }
    , { L">"                    , 0 , tCIDMacroEng::ETokens::GtThan          }
    , { L">="                   , 0 , tCIDMacroEng::ETokens::GtThanEq        }
    , { L"++"                   , 0 , tCIDMacroEng::ETokens::Inc             }
    , { L"&&"                   , 0 , tCIDMacroEng::ETokens::LogAnd          }
    , { L"||"                   , 0 , tCIDMacroEng::ETokens::LogOr           }
    , { L"^^"                   , 0 , tCIDMacroEng::ETokens::LogXor          }
    , { L"<"                    , 0 , tCIDMacroEng::ETokens::LsThan          }
    , { L"<="                   , 0 , tCIDMacroEng::ETokens::LsThanEq        }
    , { L"-="                   , 0 , tCIDMacroEng::ETokens::MinusEq         }
    , { L"%"                    , 0 , tCIDMacroEng::ETokens::ModDiv          }
    , { L"%="                   , 0 , tCIDMacroEng::ETokens::ModDivEq        }
    , { L"*"                    , 0 , tCIDMacroEng::ETokens::Multiply        }
    , { L"*="                   , 0 , tCIDMacroEng::ETokens::MultiplyEq      }
    , { L"!="                   , 0 , tCIDMacroEng::ETokens::NotEqual        }
    , { L"|"                    , 0 , tCIDMacroEng::ETokens::Or              }
    , { L"|="                   , 0 , tCIDMacroEng::ETokens::OrEq            }
    , { L"+="                   , 0 , tCIDMacroEng::ETokens::PlusEq          }
    , { L"-"                    , 0 , tCIDMacroEng::ETokens::Subtract        }
    , { L"^"                    , 0 , tCIDMacroEng::ETokens::Xor             }
    , { L"^="                   , 0 , tCIDMacroEng::ETokens::XorEq           }

    // These are purely symbolic
    , { L"<end of file>"        , 0 , tCIDMacroEng::ETokens::EOF             }
    , { L"<quoted string>"      , 0 , tCIDMacroEng::ETokens::QuotedString    }
    , { L"<numeric literal>"    , 0 , tCIDMacroEng::ETokens::NumericLiteral  }
    , { L"<char literal>"       , 0 , tCIDMacroEng::ETokens::CharLiteral     }
};

