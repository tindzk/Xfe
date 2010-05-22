#include "config.h"
#include "i18n.h"


#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>

#include <fx.h>

#include "xfedefs.h"
#include "xfeutils.h"
#include "startupnotification.h"

// Run a command and simulate a startup time
FXint runcmd(FXString cmd)
{
	// Run the command in background
	cmd += " &";
	system(cmd.text());

	return 0;
}
