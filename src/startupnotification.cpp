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


#ifdef STARTUP_NOTIFICATION  // Use startup notification


// Indicate that launchee has completed startup
void startup_completed(void)
{
	SnLauncheeContext *launchee;	
	Display *xdisplay;
	SnDisplay *display;
  
	// Open display
	xdisplay = XOpenDisplay (NULL);
	if (xdisplay != NULL)
    {  
		// Create startup notification context
		display = sn_display_new (xdisplay,NULL,NULL);
		launchee = sn_launchee_context_new_from_environment (display,DefaultScreen (xdisplay));

		// Indicate startup has completed and free resources
		if(launchee)
		{
			sn_launchee_context_complete(launchee);
			sn_launchee_context_unref(launchee);
		}
		sn_display_unref(display);
		XCloseDisplay(xdisplay);
	}
}


// Obtain a timestamp for the startup notification
Time gettimestamp (void)
{
	static struct timeval tv;
	gettimeofday(&tv,NULL);
	return tv.tv_sec;
}


// Launch a command and initiate a startup notification 
FXint runcmd(FXString cmd, FXString cmdname, FXbool usesn=TRUE, FXString snexcepts="")
{
	// Get rid of possible command options
	cmdname=cmdname.before(' ');
	
	// Check if command is in the startup notification exception list
	FXbool startup_notify=TRUE;
	if(snexcepts != "")
	{
		FXString entry;
		for(FXint i=0; ; i++)
		{
			entry=snexcepts.section(':',i);
			if(streq(entry.text(),""))
				break;
			if(streq(entry.text(),cmdname.text()))
			{
				startup_notify=FALSE;
				break;
			}
		}
	}

	// Run command with startup notification
	if (usesn && startup_notify)
	{
		Display *xdisplay;
		SnDisplay *display;
		SnLauncherContext *context;
		Time timestamp;

		// Open display
		xdisplay = XOpenDisplay(NULL);
		if (xdisplay == NULL)
		{
			fprintf (stderr,_("Error! Can't open display\n"));
			return -1;
		}
	  
		// Message displayed in the task bar (if any)
		FXString message;
		message.format(_("Start of %s"),cmdname.text());

		// Initiate launcher context
		display = sn_display_new(xdisplay,NULL,NULL);
		context = sn_launcher_context_new (display, DefaultScreen (xdisplay));
		sn_launcher_context_set_name(context,message.text());
		sn_launcher_context_set_binary_name(context,cmdname.text());
		sn_launcher_context_set_description(context,message.text());
		sn_launcher_context_set_icon_name (context,cmdname.text());
		timestamp = gettimestamp();
		sn_launcher_context_initiate(context,"Xfe",cmd.text(),timestamp);

		// Run command in background
		cmd += " &";
		
		static pid_t child_pid = 0;
		switch ((child_pid = fork ()))
		{
			case -1:
				fprintf (stderr,_("Error! Fork failed: %s\n"),strerror (errno));
				break;
		
			case 0:
				sn_launcher_context_setup_child_process(context);
				execl("/bin/sh","sh","-c",cmd.text(),(char *)NULL);
				exit(1);
				break;
		}
		sn_launcher_context_unref (context);
	}
	
	// Run command without startup notification
	else
	{
		// Run command in background
		cmd += " &";
		system(cmd.text());
	
		// Just display the wait cursor during a second
		sleep(1);
	}
	
	return 0;
}


#else  // Don't use startup notification

// Run a command and simulate a startup time
FXint runcmd(FXString cmd)
{
	// Run the command in background
	cmd += " &";
	system(cmd.text());
	
	// Very ugly simulation of a startup time!!!
	sleep(SIMULATED_STARTUP_TIME);
	
	return 0;
}
#endif


