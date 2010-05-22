#include "config.h"
#include "i18n.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>

#include <fx.h>
#include <fxkeys.h>
#include <FXPNGIcon.h>

#include "xfedefs.h"
#include "icons.h"
#include "xfeutils.h"
#include "MessageBox.h"
#include "FilePanel.h"
#include "XFileExplorer.h"


// Add FOX hacks
#include "foxhacks.cpp"

// Main window
FXMainWindow *mainWindow;

// Global variables
char **args;

// Used to force panel view mode from command line
FXint panel_mode=-1;


// Hand cursor replacement
#define hand_width 32
#define hand_height 32
#define hand_x_hot 6
#define hand_y_hot 1
 static const FXuchar hand_bits[] = {
	0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x90, 0x00, 0x00, 0x00,
	0x90, 0x00, 0x00, 0x00, 0x90, 0x00, 0x00, 0x00, 0x90, 0x07, 0x00, 0x00,
	0x97, 0x1a, 0x00, 0x00, 0x99, 0x2a, 0x00, 0x00, 0x11, 0x28, 0x00, 0x00,
	0x12, 0x20, 0x00, 0x00, 0x02, 0x20, 0x00, 0x00, 0x02, 0x20, 0x00, 0x00,
	0x04, 0x20, 0x00, 0x00, 0x04, 0x10, 0x00, 0x00, 0xf8, 0x0f, 0x00, 0x00,
	0xf0, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
 
 static const FXuchar hand_mask_bits[] = {
	0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0xf0, 0x00, 0x00, 0x00,
	0xf0, 0x00, 0x00, 0x00, 0xf0, 0x00, 0x00, 0x00, 0xf0, 0x07, 0x00, 0x00,
	0xf7, 0x1f, 0x00, 0x00, 0xff, 0x3f, 0x00, 0x00, 0xff, 0x3f, 0x00, 0x00,
	0xfe, 0x3f, 0x00, 0x00, 0xfe, 0x3f, 0x00, 0x00, 0xfe, 0x3f, 0x00, 0x00,
	0xfc, 0x3f, 0x00, 0x00, 0xfc, 0x1f, 0x00, 0x00, 0xf8, 0x0f, 0x00, 0x00,
	0xf0, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
  
// Usage message
#define USAGE_MSG	_("\
\nUsage: xfe [options] [startdir] \n\
\n\
    [options] can be any of the following:\n\
\n\
        -h,   --help         Print (this) help screen and exit.\n\
        -v,   --version      Print version information and exit.\n\
        -i,   --iconic       Start iconified.\n\
        -m,   --maximized    Start maximized.\n\
        -p n, --panels n     Force panel view mode to n (n=0 => Tree and one panel,\n\
                             n=1 => One panel, n=2 => Two panels, n=3 => Tree and two panels).\n\
\n\
    [startdir] is the path to the initial directory you want to\n\
    open on start up.\n\
\n")



int main(int argc,char *argv[])
{
    const char *title   = "Xfe";
	const char *appname = "xfe";
	const char *vdrname = "Xfe";
	int i;
	FXbool loadicons;
	FXString startdir="";
	FXbool iconic=FALSE;
	FXbool maximized=FALSE;

#ifdef HAVE_SETLOCALE
  	// Set locale via LC_ALL.
  	setlocale (LC_ALL, "");
#endif

#if ENABLE_NLS
  	// Set the text message domain.
  	bindtextdomain(PACKAGE, LOCALEDIR);
	bind_textdomain_codeset(PACKAGE,"utf-8");
  	textdomain(PACKAGE);
#endif

	// Parse basic arguments
	for(i = 1; i < argc; ++i)
	{
    	if(compare(argv[i],"-v")==0 || compare(argv[i],"--version")==0)
		{
			fprintf(stdout,"%s version %s\n",PACKAGE,VERSION);
			exit(0);
		}
    	else if(compare(argv[i],"-h")==0 || compare(argv[i],"--help")==0)
		{
			fprintf(stdout,USAGE_MSG);
			exit(0);
		}
    	else if(compare(argv[i],"-i")==0 || compare(argv[i],"--iconic")==0)
		{
			iconic=TRUE;
		}
		else if(compare(argv[i],"-m")==0 || compare(argv[i],"--maximized")==0)
		{
			maximized=TRUE;
		}
		else if(compare(argv[i],"-p")==0 || compare(argv[i],"--panels")==0)
		{
			if ( ++i < argc ) panel_mode=atoi(argv[i]); 
			if ( (panel_mode<0) || (panel_mode>3) )
			{
				 fprintf(stderr,_("Warning: Unknown panel mode, revert to last saved panel mode\n"));
				 panel_mode = -1;
			}
		}
		else
	    {
			// Start directory, if any
			startdir=argv[i];
	    }
	}

    // Global variable (used to properly restart Xfe)
	args = argv;

	// Application creation
    FXApp* application=new FXApp(appname,vdrname);
    application->init(argc,argv);

    // Redefine the default hand cursor
    FXCursor* hand=new FXCursor(application,hand_bits,hand_mask_bits,hand_width,hand_height,hand_x_hot,hand_y_hot);
    application->setDefaultCursor(DEF_HAND_CURSOR,hand);

	// Read registry thru foxhacks
	application->reg().read();

	// Set base color (to change the default base color at first run)
	FXColor basecolor=application->reg().readColorEntry("SETTINGS","basecolor",FXRGB(237,233,227));
	application->setBaseColor(basecolor);

	// Load all application icons
	loadicons=loadAppIcons(application);
	
	// Set normal font
	FXString fontspec;
	fontspec=application->reg().readStringEntry("SETTINGS","font","Helvetica,100,normal,regular");
	if(!fontspec.empty())
	{
    	FXFont* normalFont=new FXFont(application,fontspec);
        application->setNormalFont(normalFont);
    }
 
    // If root
	if (getuid()==0)
        title="Xfe (root)";

	// In case start directory is given in URI form, convert it
	startdir=::fileFromURI(startdir);

	// Make sure start directory path is clean 
	if(startdir!="")
		startdir=::filePath(startdir);

   	// Create and run application
	mainWindow=new XFileExplorer(application,iconic, maximized,startdir,title,xfeicon,minixfeicon);

	// Catch SIGCHLD to harvest zombie child processes
	application->addSignal(SIGCHLD,mainWindow,XFileExplorer::ID_HARVEST,TRUE);

	// Also catch interrupt so we can gracefully terminate
	application->addSignal(SIGINT,mainWindow,XFileExplorer::ID_QUIT);

	application->create();
	
	// Tooltips setup time and duration
	application->setTooltipPause(TOOLTIP_PAUSE);
	application->setTooltipTime(TOOLTIP_TIME);

	if (!loadicons)
		 MessageBox::error(application,BOX_OK,_("Error loading icons"),_("Unable to load some icons. Please check your icons path!"));
   	application->run();

}
