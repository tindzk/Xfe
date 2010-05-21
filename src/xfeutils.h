#ifndef XFEUTILS_H
#define XFEUTILS_H


#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>

#include <fx.h>

#include "xvt/xvt.h"


// Single click types
enum
{
	SINGLE_CLICK_NONE,
	SINGLE_CLICK_DIR,
	SINGLE_CLICK_DIR_FILE,
};


// Wait cursor states
enum
{
	BEGIN_CURSOR,
	END_CURSOR,
	QUERY_CURSOR
};


// Note : some inline functions must be declared in the header file or it won't compile!

// Convert a character to lower case
inline FXint toLower(FXint c)
{
	return ('A' <= c && c <= 'Z' ? c + 32 : c);
}


// To test if two strings are equal (strcmp replacement, thanks to Francesco Abbate)
inline FXint streq(const FXchar*a, const FXchar*b)
{
	if (a == NULL || b == NULL)
		return 0;
	return (strcmp(a, b) == 0);
}


// Convert a string to lower cases and returns the string size
inline void strlow (FXchar* str)
{
    while (*str)
    {
        *str = toLower( *str );
        ++str;
    }
} 

FXHotKey _parseAccel(const FXString&);
FXbool existCommand(FXString);
FXString getKeybinding(FXEvent*);
FXint mkpath(const FXchar*, mode_t);
FXString createTrashpathname(FXString, FXString);
FXint createTrashinfo(FXString, FXString, FXString, FXString);
FXString mimetype(FXString);
FXString quote(FXString);
FXbool isUtf8(const FXchar*, FXuint);
int statrep(const FXchar*, struct stat*);
int lstatrep(const FXchar*, struct stat*);
#if defined(linux)
int lstatmt(const FXchar*, struct stat*);
#endif
size_t strlcpy(FXchar*, const FXchar*, size_t);
size_t strlcat(FXchar*, const FXchar*, size_t);
unsigned long long dirsize(const FXchar*);
unsigned long long pathsize(FXchar*, FXuint*, FXuint*);
FXString hSize(FXchar*);
FXString cleanPath(const FXString);
FXString filePath(const FXString);
FXString filePath(const FXString, const FXString);
FXString fileFromURI(FXString);
FXString fileToURI(const FXString&);
long deltime(FXString);
FXint isEmptyDir(const FXString);
FXint hasSubDirs(const FXString);
FXbool exists(const FXString&);
FXbool isDirectory(const FXString&);
FXbool isFile(const FXString&);
FXbool isGroupMember(gid_t);
FXbool isWritable(const FXString&);
FXbool isReadable(const FXString&);
FXbool isReadExecutable(const FXString&);
FXbool isLink(const FXString&);
FXbool info(const FXString&, struct stat&);
FXString permissions(FXuint);
FXString readlink(const FXString&);
FXbool identical(const FXString& ,const FXString&);
FXint setWaitCursor(FXApp*, FXuint);
FXint runinxvt(FXString);

#endif


