//
// FILE NAME: CIDBuild_ToolsDriver_Win32.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/10/1999
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
//  This file provides the Win32 specific parts of the TToolsDriver.
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
#include    "../CIDBuild.hpp"

#define     WIN32_LEAN_AND_MEAN
#include    <windows.h>



// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDBuild_ToolsDriver_Win32
{
    //
    //  These strings make up the leading and trailing parts of the manifest
    //  resource we have to apply to executables. In between will be a couple
    //  of lines generated on the fly based on the program name and version.
    //
    const tCIDLib::TCh* const pszLeadingManifest =
    (
        L"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
        L"<assembly xmlns=\"urn:schemas-microsoft-com:asm.v1\" manifestVersion=\"1.0\">\n"

        L"<assemblyIdentity type=\"win32\"\n"
    );

    const tCIDLib::TCh* const pszLeadingManifest2 =
    (
        L"                  processorArchitecture=\"x86\"\n"
        L"                  publicKeyToken=\"6595b64144ccf1df\"/>\n\n"
    );


    const tCIDLib::TCh* const pszGUIManifest =
    (
        // Force high DPI awareness
        L"  <asmv3:application xmlns:asmv3=\"urn:schemas-microsoft-com:asm.v3\">\n"
        L"     <asmv3:windowsSettings xmlns=\"http://schemas.microsoft.com/SMI/2005/WindowsSettings\">\n"
        L"       <dpiAware>true</dpiAware> \n"
        L"     </asmv3:windowsSettings>\n"
        L"  </asmv3:application>\n\n"
    );


    const tCIDLib::TCh* const pszTrailingManifest =
    (
        L"  <dependency>\n"
        L"    <dependentAssembly>\n"

        // Force V6 of the common controls to load, else we'd get 5 and it won't work
        L"       <assemblyIdentity type=\"win32\"\n"
        L"                         name=\"Microsoft.Windows.Common-Controls\"\n"
        L"                         version=\"6.0.0.0\"\n"
        L"                         processorArchitecture=\"*\"\n"
        L"                         publicKeyToken=\"6595b64144ccf1df\"\n"
        L"                         language=\"*\"/>\n"


        L"    </dependentAssembly>\n"
        L"  </dependency>\n"
        L"</assembly>\n"
    );
}



// ---------------------------------------------------------------------------
//  TToolsDriver: Protected, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TToolsDriver::PostLink( const   TProjectInfo&   projiTarget
                        , const TBldStr&        strTarget)
{
    //
    //  If it's an executable, then set a manifest on it
    //
    if ((projiTarget.eType() == tCIDBuild::EProjTypes::Executable)
    ||  (projiTarget.eType() == tCIDBuild::EProjTypes::Service))
    {
        if (facCIDBuild.bVerbose())
        {
            stdOut  << L"Adding manifest to: "
            << projiTarget.strProjectName()
            << kCIDBuild::EndLn;
        }

        TBldStr strManifest;
        strManifest.Append(CIDBuild_ToolsDriver_Win32::pszLeadingManifest);

        strManifest.Append(L"        name=\"CharmedQuarkSoftware.Software.");
        strManifest.Append(projiTarget.strProjectName());
        strManifest.Append(L"\"\n       version=\"");
        strManifest.Append(facCIDBuild.c4MajVer());
        strManifest.Append(L".");
        strManifest.Append(facCIDBuild.c4MinVer());
        strManifest.Append(L".");
        strManifest.Append(facCIDBuild.c4Revision());
        strManifest.Append(L".0");
        strManifest.Append(L"\"\n");
        strManifest.Append(CIDBuild_ToolsDriver_Win32::pszLeadingManifest2);

        // If a GUI application, then do DPI stuff
        if (projiTarget.eDisplayType() == tCIDBuild::EDisplayTypes::GUI)
            strManifest.Append(CIDBuild_ToolsDriver_Win32::pszGUIManifest);

        // Close off the manifest now
        strManifest.Append(CIDBuild_ToolsDriver_Win32::pszTrailingManifest);

        //
        //  We used to embed the resource ourself, but it got harder to deal with
        //  security issues so we just invoke mt.exe now. We build a dummy manifest
        //  file on the fly and embed it.
        //
        TBldStr strManFile;
        strManFile = facCIDBuild.strOutDir();
        strManFile.Append(L"/");
        strManFile.Append(L"Tmp.Manifest");

        {
            TTextFile tflManifest
            (
                strManFile
                , tCIDBuild::EEncodings::ASCII
                , tCIDLib::EAccessModes::Write
                , tCIDLib::ECreateActs::CreateAlways
            );
            tflManifest << strManifest;
        }


        const tCIDLib::TCh* apszParms[16];
        apszParms[0] = L"mt.exe";
        apszParms[1] = L"-nologo";
        apszParms[2] = L"-manifest";
        apszParms[3] = strManFile.pszBuffer();

        TBldStr strOutRes(L"-outputresource:");
        strOutRes.Append(strTarget);
        strOutRes.Append(L";#1");
        apszParms[4] = strOutRes.pszBuffer();

        tCIDLib::TCard4 c4Res;
        if (!TUtils::bExec(apszParms, 5, c4Res, kCIDLib::False))
        {
            stdOut  << L"Failed to embed manifest. Error="
                    << c4Res << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::Internal;
        }
    }
}



// ---------------------------------------------------------------------------
//  TToolsDriver: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TToolsDriver::BuildLibName( const   TBldStr&    strTargetProject
                            ,       TBldStr&    strToFill)
{
    //
    //  Get a copy of the project name and tack on a lib extension, which
    //  will give us the name of the lib for this project. We have to add
    //  in the version postfix though because lib files are versioned.
    //
    strToFill = strTargetProject;
    strToFill.Append(facCIDBuild.strVersionSuffix());
    strToFill.Append(kCIDBuild::pszLibExt);
}


