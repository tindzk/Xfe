#ifndef STARTUPNOTIFICATION_H
#define STARTUPNOTIFICATION_H

#include <fx.h>

#ifdef STARTUP_NOTIFICATION

#include "libsn/sn.h"

void startup_completed (void);
Time gettimestamp (void);
FXint runcmd(FXString, FXString, FXbool, FXString);

#else

FXint runcmd(FXString);

#endif

#endif

