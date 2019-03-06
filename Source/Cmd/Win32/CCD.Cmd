@ECHO OFF
REM ---------------------------------------------------------------------------
REM  This file is just a simple way of quickly getting to a particular
REM  project directory. Its pretty simplistic because NT's command language is
REM  pretty brain dead.
REM
REM  It just takes a parameter that indicates the desired directory. Some are
REM  special ones, and all others are assumed to be project directories.
REM ---------------------------------------------------------------------------

%CID_DRIVE%

IF "%1"=="all" GOTO CD_ALLPROJECTS
IF "%1"=="cidadvgraph" GOTO CD_CIDADVGRAPH
IF "%1"=="cidai" GOTO CD_CIDAI
IF "%1"=="cidcomm" GOTO CD_CIDCOMM
IF "%1"=="cidcrypto" GOTO CD_CIDCRYPTO
IF "%1"=="cidctrls" GOTO CD_CIDCTRLS
IF "%1"=="cidctrls2" GOTO CD_CIDCTRLS2
IF "%1"=="ciddae" GOTO CD_CIDDAE
IF "%1"=="ciddbase" GOTO CD_CIDDBASE
IF "%1"=="cidgraphdev" GOTO CD_CIDGRAPHDEV
IF "%1"=="cididl" GOTO CD_CIDIDL
IF "%1"=="cidimage" GOTO CD_CIDIMAGE
IF "%1"=="cidimgfact" GOTO CD_CIDIMGFACT
IF "%1"=="cidjpeg" GOTO CD_CIDJPEG
IF "%1"=="cidkinect" GOTO CD_CIDKINECT
IF "%1"=="cidlogsrv" GOTO CD_CIDLOGSRV
IF "%1"=="cidmacrodbg" GOTO CD_CIDMACRODBG
IF "%1"=="cidmacroeng" GOTO CD_CIDMACROENG
IF "%1"=="cidnamesrv" GOTO CD_CIDNAMESRV
IF "%1"=="cidnet" GOTO CD_CIDNET
IF "%1"=="cidorb" GOTO CD_CIDORB
IF "%1"=="cidorbwu" GOTO CD_CIDORBWU
IF "%1"=="cidorbuc" GOTO CD_CIDORBUC
IF "%1"=="cidpack" GOTO CD_CIDPACK
IF "%1"=="cidpubsub" GOTO CD_CIDPUBSUB
IF "%1"=="cidrtp" GOTO CD_CIDRTP
IF "%1"=="cidsock" GOTO CD_CIDSOCK
IF "%1"=="cidschan" GOTO CD_CIDSCHAN
IF "%1"=="cidspreco" GOTO CD_CIDSPRECO
IF "%1"=="cidpng" GOTO CD_CIDPNG
IF "%1"=="cidupnp" GOTO CD_CIDUPNP
IF "%1"=="cidtests" GOTO CD_TESTS
IF "%1"=="cidwutils" GOTO CD_CIDWUTILS
IF "%1"=="cidwebbrowser" GOTO CD_CIDWEBBROWSER
IF "%1"=="cidwebsock" GOTO CD_CIDWEBSOCK
IF "%1"=="cidxml" GOTO CD_CIDXML
IF "%1"=="cidzlib" GOTO CD_CIDZLIB
IF "%1"=="cmdocs" GOTO CD_CMDOCS
IF "%1"=="cmd" GOTO CD_CMD
IF "%1"=="commutils" GOTO CD_COMMUTILS
IF "%1"=="datautils" GOTO CD_DATAUTILS
IF "%1"=="demos" GOTO CD_DEMOS
IF "%1"=="develop" GOTO CD_DEVELOP
IF "%1"=="docs" GOTO CD_DOCS
IF "%1"=="langutils" GOTO CD_LANGUTILS
IF "%1"=="inst" GOTO CD_INSTALL
IF "%1"=="mediautils" GOTO CD_MEDIAUTILS
IF "%1"=="orbutils" GOTO CD_ORBUTILS
IF "%1"=="output" GOTO CD_OUTPUT
IF "%1"=="res" GOTO CD_RES
IF "%1"=="samples" GOTO CD_SAMPLES
IF "%1"=="src" GOTO CD_SOURCE
IF "%1"=="testmacroeng" GOTO CD_TESTMACROENG
IF "%1"=="testfw" GOTO CD_TESTFW
IF "%1"=="utils" GOTO CD_UTILS
IF "%1"=="wndutils" GOTO CD_WNDUTILS

IF "%1"=="root" GOTO CD_ROOT
IF "%1"=="top" GOTO CD_TOP


CD %CID_SRCTREE%\Source\AllProjects\%1
GOTO EXIT


:CD_ROOT
CD %CID_SRCTREE%\
GOTO EXIT

:CD_TOP
CD %CID_SRCTREE%\..
GOTO EXIT

:CD_ALLPROJECTS
CD %CID_SRCTREE%\Source\AllProjects
GOTO EXIT

:CD_CMDOCS
CD %CID_SRCTREE%\Source\ClassDocs
GOTO EXIT

:CD_CMD
CD %CID_SRCTREE%\Source\Cmd\Win32
GOTO EXIT


:CD_CIDADVGRAPH
CD %CID_SRCTREE%\Source\AllProjects\GraphicUtils\CIDAdvGraph
GOTO EXIT

:CD_CIDAI
CD %CID_SRCTREE%\Source\AllProjects\AIUtils\CIDAI
GOTO EXIT

:CD_CIDCOMM
CD %CID_SRCTREE%\Source\AllProjects\CommUtils\CIDComm
GOTO EXIT

:CD_CIDCRYPTO
CD %CID_SRCTREE%\Source\AllProjects\SecureUtils\CIDCrypto
GOTO EXIT

:CD_CIDCTRLS
CD %CID_SRCTREE%\Source\AllProjects\WndUtils\CIDCtrls
GOTO EXIT

:CD_CIDCTRLS2
CD %CID_SRCTREE%\Source\AllProjects\WndUtils\CIDCtrls2
GOTO EXIT

:CD_CIDDAE
CD %CID_SRCTREE%\Source\AllProjects\MediaUtils\CIDDAE
GOTO EXIT

:CD_CIDDBASE
CD %CID_SRCTREE%\Source\AllProjects\DataUtils\CIDDBase
GOTO EXIT

:CD_CIDGRAPHDEV
CD %CID_SRCTREE%\Source\AllProjects\GraphicUtils\CIDGraphDev
GOTO EXIT

:CD_CIDIDL
CD %CID_SRCTREE%\Source\AllProjects\OrbUtils\CIDIDL
GOTO EXIT

:CD_CIDIMAGE
CD %CID_SRCTREE%\Source\AllProjects\GraphicUtils\CIDImage
GOTO EXIT

:CD_CIDIMGFACT
CD %CID_SRCTREE%\Source\AllProjects\GraphicUtils\CIDImgFact
GOTO EXIT

:CD_CIDJPEG
CD %CID_SRCTREE%\Source\AllProjects\GraphicUtils\CIDJPEG
GOTO EXIT

:CD_CIDKINECT
CD %CID_SRCTREE%\Source\AllProjects\AIUtils\CIDKinect
GOTO EXIT

:CD_CIDLOGSRV
CD %CID_SRCTREE%\Source\AllProjects\OrbUtils\CIDLogSrv
GOTO EXIT

:CD_CIDMACRODBG
CD %CID_SRCTREE%\Source\AllProjects\LangUtils\CIDMacroDbg
GOTO EXIT

:CD_CIDMACROENG
CD %CID_SRCTREE%\Source\AllProjects\LangUtils\CIDMacroEng
GOTO EXIT

:CD_CIDNAMESRV
CD %CID_SRCTREE%\Source\AllProjects\OrbUtils\CIDNameSrv
GOTO EXIT

:CD_CIDORB
CD %CID_SRCTREE%\Source\AllProjects\OrbUtils\CIDOrb
GOTO EXIT

:CD_CIDORBWU
CD %CID_SRCTREE%\Source\AllProjects\OrbUtils\CIDOrbWU
GOTO EXIT

:CD_CIDORBUC
CD %CID_SRCTREE%\Source\AllProjects\OrbUtils\CIDOrbUC
GOTO EXIT

:CD_CIDNET
CD %CID_SRCTREE%\Source\AllProjects\CommUtils\CIDNet
GOTO EXIT

:CD_CIDPACK
CD %CID_SRCTREE%\Source\AllProjects\DataUtils\CIDPack
GOTO EXIT

:CD_CIDPNG
CD %CID_SRCTREE%\Source\AllProjects\GraphicUtils\CIDPNG
GOTO EXIT

:CD_CIDPUBSUB
CD %CID_SRCTREE%\Source\AllProjects\DataUtils\CIDPubSub
GOTO EXIT

:CD_CIDRTP
CD %CID_SRCTREE%\Source\AllProjects\MediaUtils\CIDRTP
GOTO EXIT

:CD_CIDSOCK
CD %CID_SRCTREE%\Source\AllProjects\CommUtils\CIDSock
GOTO EXIT

:CD_CIDSCHAN
CD %CID_SRCTREE%\Source\AllProjects\SecureUtils\CIDSChan
GOTO EXIT

:CD_CIDSPRECO
CD %CID_SRCTREE%\Source\AllProjects\AIUtils\CIDSpReco
GOTO EXIT

:CD_CIDUPNP
CD %CID_SRCTREE%\Source\AllProjects\CommUtils\CIDUPnP
GOTO EXIT

:CD_CIDWUTILS
CD %CID_SRCTREE%\Source\AllProjects\WndUtils\CIDWUtils
GOTO EXIT

:CD_CIDWEBBROWSER
CD %CID_SRCTREE%\Source\AllProjects\WndUtils\CIDWebBrowser
GOTO EXIT

:CD_CIDWEBSOCK
CD %CID_SRCTREE%\Source\AllProjects\CommUtils\CIDWebSock
GOTO EXIT

:CD_CIDXML
CD %CID_SRCTREE%\Source\AllProjects\DataUtils\CIDXML
GOTO EXIT

:CD_CIDZLIB
CD %CID_SRCTREE%\Source\AllProjects\DataUtils\CIDZLib
GOTO EXIT

:CD_COMMUTILS
CD %CID_SRCTREE%\Source\AllProjects\CommUtils
GOTO EXIT

:CD_DATAUTILS
CD %CID_SRCTREE%\Source\AllProjects\DataUtils
GOTO EXIT

:CD_DEVELOP
CD %CID_SRCTREE%\..
GOTO EXIT

:CD_DEMOS
CD %CID_SRCTREE%\Source\AllProjects\Demos
GOTO EXIT

:CD_DOCS
CD %CID_SRCTREE%\Source\PublicDocs
GOTO EXIT

:CD_LANGUTILS
CD %CID_SRCTREE%\Source\AllProjects\LangUtils
GOTO EXIT

:CD_MEDIAUTILS
CD %CID_SRCTREE%\Source\AllProjects\MediaUtils
GOTO EXIT

:CD_OUTPUT
CD %CIDLIB_SRCDIR%\Output
GOTO EXIT

:CD_RES
CD %CID_RESDIR%
GOTO EXIT

:CD_INSTALL
CD %CID_SRCTREE%\Source\Install
GOTO EXIT

:CD_ORBUTILS
CD %CID_SRCTREE%\Source\AllProjects\OrbUtils
GOTO EXIT

:CD_SAMPLES
CD %CID_SRCTREE%\Source\AllProjects\Samples
GOTO EXIT

:CD_SOURCE
CD %CID_SRCTREE%\Source
GOTO EXIT

:CD_TESTMACROENG
CD %CID_SRCTREE%\Source\AllProjects\Tests\TestMacroEng
GOTO EXIT

:CD_TESTFW
CD %CID_SRCTREE%\Source\AllProjects\Utilities\TestFW
GOTO EXIT

:CD_TESTS
CD %CID_SRCTREE%\Source\AllProjects\Tests
GOTO EXIT

:CD_UTILS
CD %CID_SRCTREE%\Source\AllProjects\Utilities
GOTO EXIT

:CD_WNDUTILS
CD %CID_SRCTREE%\Source\AllProjects\WndUtils
GOTO EXIT


:EXIT

REM If there is a second one, its a sub-dir
if "%2"=="" GOTO NO_SECOND

cd %2

:NO_SECOND

