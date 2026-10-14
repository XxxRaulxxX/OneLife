#include "PNGFileDialog.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <wchar.h>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <commdlg.h>
#endif

#ifndef _WIN32
static char hasPNGExtension( const char *inPath ) {
    size_t n = strlen( inPath );
    return n >= 4 && inPath[n-4] == '.' &&
        tolower( (unsigned char)inPath[n-3] ) == 'p' &&
        tolower( (unsigned char)inPath[n-2] ) == 'n' &&
        tolower( (unsigned char)inPath[n-1] ) == 'g';
    }
#endif

char *choosePNGFile( char *outTemporary, const char **outError ) {
    *outTemporary = false;
    *outError = NULL;

#ifdef _WIN32
    // Runtime resolution avoids adding a Comdlg32 link flag to the old build.
    HMODULE dialogs = LoadLibraryW( L"comdlg32.dll" );
    if( dialogs == NULL ) {
        *outError = "Could not open the PNG file chooser";
        return NULL;
        }
    typedef BOOL (WINAPI *OpenDialog)( LPOPENFILENAMEW );
    typedef DWORD (WINAPI *DialogError)();
    OpenDialog openDialog = (OpenDialog)GetProcAddress(
        dialogs, "GetOpenFileNameW" );
    DialogError dialogError = (DialogError)GetProcAddress(
        dialogs, "CommDlgExtendedError" );
    if( openDialog == NULL || dialogError == NULL ) {
        FreeLibrary( dialogs );
        *outError = "Could not open the PNG file chooser";
        return NULL;
        }
    wchar_t selected[32768] = { 0 };
    OPENFILENAMEW dialog;
    memset( &dialog, 0, sizeof( dialog ) );
    dialog.lStructSize = sizeof( dialog );
    dialog.hwndOwner = GetActiveWindow();
    dialog.lpstrFilter = L"PNG images (*.png)\0*.png\0\0";
    dialog.nFilterIndex = 1;
    dialog.lpstrFile = selected;
    dialog.nMaxFile = sizeof( selected ) / sizeof( selected[0] );
    dialog.lpstrTitle = L"Load PNG File";
    dialog.lpstrDefExt = L"png";
    dialog.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST |
        OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
    BOOL accepted = openDialog( &dialog );
    DWORD error = accepted ? 0 : dialogError();
    FreeLibrary( dialogs );
    if( ! accepted ) {
        if( error != 0 ) {
            *outError = "Could not open the PNG file chooser";
            }
        return NULL;
        }
    size_t n = wcslen( selected );
    if( n < 4 || _wcsicmp( selected + n - 4, L".png" ) != 0 ) {
        *outError = "Please select a PNG file (.png)";
        return NULL;
        }

    // Unique ASCII relative names let FileInputStream read Unicode sources.
    // Neither the source file nor the process working directory is changed.
    wchar_t staging[MAX_PATH];
    if( GetTempFileNameW( L".", L"png", 0, staging ) == 0 ) {
        *outError = "Could not create a temporary PNG import file";
        return NULL;
        }
    if( ! CopyFileW( selected, staging, FALSE ) ) {
        DeleteFileW( staging );
        *outError = "Could not read the selected PNG file";
        return NULL;
        }
    char *result = new char[MAX_PATH];
    int length = WideCharToMultiByte( CP_ACP, 0, staging, -1,
                                    result, MAX_PATH, NULL, NULL );
    if( length == 0 ) {
        delete [] result;
        DeleteFileW( staging );
        *outError = "Could not read the temporary PNG import file";
        return NULL;
        }
    *outTemporary = true;
    return result;
#else
    const char *command;
#ifdef __APPLE__
    command = "osascript -e 'POSIX path of (choose file with prompt "
        "\"Load PNG File\" of type {\"public.png\"})' 2>/dev/null";
#else
    // Fixed commands: never insert selected filenames into shell commands.
    if( system( "command -v zenity >/dev/null 2>&1" ) == 0 ) {
        command = "zenity --file-selection --title='Load PNG File' "
            "--file-filter='PNG images | *.png *.PNG' 2>/dev/null";
        }
    else if( system( "command -v kdialog >/dev/null 2>&1" ) == 0 ) {
        command = "kdialog --getopenfilename . '*.png *.PNG|PNG images' "
            "--title 'Load PNG File' 2>/dev/null";
        }
    else {
        *outError = "PNG file chooser requires Zenity or KDialog";
        return NULL;
        }
#endif
    FILE *pipe = popen( command, "r" );
    if( pipe == NULL ) {
        *outError = "Could not open the PNG file chooser";
        return NULL;
        }
    char selected[32768];
    size_t length = fread( selected, 1, sizeof( selected ) - 1, pipe );
    int status = pclose( pipe );
    if( status != 0 || length == 0 ) {
        return NULL;
        }
    selected[length] = '\0';
    if( selected[length-1] == '\n' ) {
        selected[--length] = '\0';
        }
    if( ! hasPNGExtension( selected ) ) {
        *outError = "Please select a PNG file (.png)";
        return NULL;
        }
    char *result = new char[length + 1];
    memcpy( result, selected, length + 1 );
    return result;
#endif
    }
