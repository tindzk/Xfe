// Common Xfe constants

#ifndef COPYRIGHT
#define COPYRIGHT "Copyright (C) 2002-2009 Roland Baudin (roland65@free.fr)"
#endif

// Default initial main window width
#ifndef DEFAULT_WINDOW_WIDTH
#define DEFAULT_WINDOW_WIDTH 800
#endif

// Default initial main window heigth
#ifndef DEFAULT_WINDOW_HEIGHT
#define DEFAULT_WINDOW_HEIGHT 600
#endif

// Default initial main window X position
#ifndef DEFAULT_WINDOW_XPOS
#define DEFAULT_WINDOW_XPOS 50
#endif

// Default initial main window Y position
#ifndef DEFAULT_WINDOW_YPOS
#define DEFAULT_WINDOW_YPOS 50
#endif

// Maximum sizes for thumbnail image preview
#ifndef MAX_BIGTHUMB_SIZE
#define MAX_BIGTHUMB_SIZE 64
#endif
#ifndef MAX_MINITHUMB_SIZE
#define MAX_MINITHUMB_SIZE 20
#endif

// Minimum width of a file panel or directory panel
#ifndef MIN_PANEL_WIDTH
#define MIN_PANEL_WIDTH 100
#endif

// Maximum length of a file name
#ifndef MAXPATHLEN
#define MAXPATHLEN 1024
#endif

// Maximum length of a command line
#ifndef MAX_COMMAND_SIZE
#define MAX_COMMAND_SIZE 256
#endif

// Maximum length of a filter pattern
#ifndef MAX_PATTERN_SIZE
#define MAX_PATTERN_SIZE 64
#endif

// Root directory string
#ifndef ROOTDIR
#define ROOTDIR "/"
#endif

// Path separator
#ifndef PATHSEPSTRING
#define PATHSEPSTRING "/"
#endif

// Path separator
#ifndef PATHSEPCHAR
#define PATHSEPCHAR '/'
#endif

// Maximum number of path links 
#ifndef MAXLINKS
#define MAX_LINKS 40
#endif

// Run history size
#ifndef RUN_HIST_SIZE
#define RUN_HIST_SIZE 20
#endif

// Open with history size
#ifndef OPEN_HIST_SIZE
#define OPEN_HIST_SIZE 20
#endif

// Filter history size
#ifndef FILTER_HIST_SIZE
#define FILTER_HIST_SIZE 20
#endif

// Local Xfe directory path
#ifndef LOCALPATH
#define LOCALPATH ".config/xfe"
#endif

// Local trashcan directory path
#ifndef TRASHPATH
#define TRASHPATH ".local/share/Trash"
#endif

// Local trashcan directory path for files
#ifndef TRASHFILESPATH
#define TRASHFILESPATH ".local/share/Trash/files"
#endif

// Local trashcan directory path for infos
#ifndef TRASHINFOPATH
#define TRASHINFOPATH ".local/share/Trash/info"
#endif

// Xfe application name
#ifndef XFEAPPNAME
#define XFEAPPNAME "xfe"
#endif

// Xfe vendor name
#ifndef XFEVDRNAME
#define XFEVDRNAME "Xfe"
#endif 

// Xfe config file name
#ifndef XFECONFIGNAME
#define XFECONFIGNAME "xferc"
#endif

// Default icon path
#ifndef DEFAULTICONPATH
#define DEFAULTICONPATH   "~/.config/xfe/icons/xfe-theme:/usr/local/share/xfe/icons/xfe-theme:/usr/share/xfe/icons/xfe-theme"
#endif

// Command to launch Xfe as root with su, using Xvt as a terminal
#ifndef SUCMD
#define SUCMD "-fn 7x14 -geometry 60x4 -e su -c 'nohup xfe >& /dev/null & sleep 1'"
#endif

// Tooltips setup time and duration
#ifndef TOOLTIP_PAUSE 
#define TOOLTIP_PAUSE 500
#endif

#ifndef TOOLTIP_TIME
#define TOOLTIP_TIME 10000
#endif

// Coefficient used to darken the sorted column in detailed mode
#ifndef DARKEN_SORT
#define DARKEN_SORT 0.96
#endif

// Default terminal program
#ifndef DEFAULT_TERMINAL
#define DEFAULT_TERMINAL "xterm -sb"
#endif

// Common macros

// Tab character
#define TAB (FXString)"\t"
#define TAB2 (FXString)"\t\t"

// Macro to add tab characters before and after a given string
#define TABS(s) ((FXString)"\t"+(s)+(FXString)"\t")

// Macro to add parentheses before and after a given string
#define PARS(s) ((FXString)" ("+(s)+(FXString)")")
