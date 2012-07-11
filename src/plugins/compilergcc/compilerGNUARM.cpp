/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h>
#include <prep.h>
#ifndef CB_PRECOMP
    #include <wx/intl.h>
    #include <wx/regex.h>
    #include <wx/utils.h> // wxGetOSDirectory, wxGetEnv
#endif
#include <wx/filefn.h> // wxFileExists
#include <wx/fileconf.h> // wxFileConfig
#include "compilerGNUARM.h"

CompilerGNUARM::CompilerGNUARM()
    : Compiler(_("GNU ARM GCC Compiler"),_T("arm-elf-gcc"))
{
    Reset();
}

CompilerGNUARM::~CompilerGNUARM()
{
    //dtor
}

Compiler * CompilerGNUARM::CreateCopy()
{
    Compiler* c = new CompilerGNUARM(*this);
    c->SetExtraPaths(m_ExtraPaths); // wxArrayString doesn't seem to be copied with the default copy ctor...
    return c;
}

void CompilerGNUARM::Reset()
{
    m_Options.ClearOptions();
    LoadDefaultOptions(wxT("arm-elf-gcc"));

    LoadDefaultRegExArray();

    m_CompilerOptions.Clear();
    m_LinkerOptions.Clear();
    m_LinkLibs.Clear();
    m_CmdsBefore.Clear();
    m_CmdsAfter.Clear();
} // end of Reset

void CompilerGNUARM::LoadDefaultRegExArray()
{
    m_RegExes.Clear();
    LoadRegExArray(wxT("arm-elf-gcc"));
}

AutoDetectResult CompilerGNUARM::AutoDetectInstallationDir()
{
    wxString sep = wxFileName::GetPathSeparator();
    if (platform::windows)
    {
        // Search for GNUARM installation dir
        wxString windir = wxGetOSDirectory();
        wxFileConfig ini(_T(""), _T(""), windir + _T("/GnuARM.ini"), _T(""), wxCONFIG_USE_LOCAL_FILE | wxCONFIG_USE_NO_ESCAPE_CHARACTERS);
        // need it as const , so correct overloaded method will be selected
        wxString Programs = _T("C:\\Program Files");
        // what's the "Program Files" location
        // TO DO : support 64 bit ->    32 bit apps are in "ProgramFiles(x86)"
        //                              64 bit apps are in "ProgramFiles"
        wxGetEnv(_T("ProgramFiles"), &Programs);
        // need it as const , so correct overloaded method will be selected
        const wxString ProgramsConst = Programs + _T("\\GNUARM");
        m_MasterPath = ini.Read(_T("/InstallSettings/InstallPath"), ProgramsConst);

        if (wxFileExists(m_MasterPath + sep + _T("bin") + sep + m_Programs.C))
        {
            m_Programs.MAKE = _T("make.exe"); // we distribute "make" not "mingw32-make"
        }
    }
    else
        m_MasterPath = _T("/usr");

    AutoDetectResult ret = wxFileExists(m_MasterPath + sep + _T("bin") + sep + m_Programs.C) ? adrDetected : adrGuessed;
    if (ret == adrDetected)
    {
        AddIncludeDir(m_MasterPath + sep + _T("include"));
        AddLibDir(m_MasterPath + sep + _T("lib"));
    }
    return ret;
} // end of AutoDetectInstallationDir
