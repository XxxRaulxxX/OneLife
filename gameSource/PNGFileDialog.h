#ifndef PNG_FILE_DIALOG_INCLUDED
#define PNG_FILE_DIALOG_INCLUDED

// Caller owns the new[] string. NULL is cancellation unless outError is set.
// On Windows a temporary copy bridges the editor's narrow-path file API.
// Remove the file after reading only when outTemporary is true.
char *choosePNGFile( char *outTemporary, const char **outError );

#endif
