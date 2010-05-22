#include "config.h"
#include "i18n.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>

#include <fx.h>
#include <fxkeys.h>
#include <FXPNGIcon.h>

#include "xfedefs.h"
#include "icons.h"
#include "xfeutils.h"
#include "FileDialog.h"
#include "FileList.h"
#include "Properties.h"
#include "XFileExplorer.h"
#include "InputDialog.h"
#include "BrowseInputDialog.h"
#include "ArchInputDialog.h"
#include "HistInputDialog.h"
#include "File.h"
#include "MessageBox.h"
#include "OverwriteBox.h"
#include "PathLinker.h"
#include "FilePanel.h"

// Duration (in ms) before we can stop refreshing the file list
// Used for file operations on a large list of files
#define STOP_LIST_REFRESH_INTERVAL 5000

// Number of files before stopping the file list refresh
#define STOP_LIST_REFRESH_NBMAX 100


// Clipboard notes :
// The uri-list type used for Xfe is the same as the Gnome uri-list type
// The standard uri-list type is used for KDE and non Gnome / XFCE file managers 
// A special uri-list type that containd only "0" (for copy) or "1" (for cut) is used for KDE compatibility


// Global Variables
extern FXMainWindow *mainWindow;

// Clipboard
extern FXString clipboard;
FXuint clipboard_type=0;


extern FXchar OpenHistory[OPEN_HIST_SIZE][MAX_COMMAND_SIZE];
extern FXint OpenNum;
extern FXchar FilterHistory[FILTER_HIST_SIZE][MAX_PATTERN_SIZE];
extern FXint FilterNum;
#if defined(linux)
extern FXStringDict* fsdevices;
extern FXStringDict* mtdevices;
#endif

extern FXbool allowPopupScroll;
extern FXuint single_click;


// Map
FXDEFMAP(FilePanel) FilePanelMap[]=
{
	FXMAPFUNC(SEL_CLIPBOARD_LOST,0,FilePanel::onClipboardLost),
	FXMAPFUNC(SEL_CLIPBOARD_GAINED,0,FilePanel::onClipboardGained),
	FXMAPFUNC(SEL_CLIPBOARD_REQUEST,0,FilePanel::onClipboardRequest),
	FXMAPFUNC(SEL_TIMEOUT,FilePanel::ID_STOP_LIST_REFRESH_TIMER,FilePanel::onCmdStopListRefreshTimer),
	FXMAPFUNC(SEL_COMMAND,FilePanel::ID_DIRECTORY_UP,FilePanel::onCmdDirectoryUp),
	FXMAPFUNC(SEL_COMMAND,FilePanel::ID_FILTER,FilePanel::onCmdItemFilter),
	FXMAPFUNC(SEL_COMMAND,FilePanel::ID_FILTER_CURRENT,FilePanel::onCmdItemFilter),
	FXMAPFUNC(SEL_COMMAND,FilePanel::ID_GO_HOME,FilePanel::onCmdGoHome),
	FXMAPFUNC(SEL_COMMAND,FilePanel::ID_GO_TRASH,FilePanel::onCmdGoTrash),
	FXMAPFUNC(SEL_COMMAND,FilePanel::ID_VIEW,FilePanel::onCmdEdit),
	FXMAPFUNC(SEL_COMMAND,FilePanel::ID_EDIT,FilePanel::onCmdEdit),
	FXMAPFUNC(SEL_COMMAND,FilePanel::ID_PROPERTIES,FilePanel::onCmdProperties),
	FXMAPFUNC(SEL_COMMAND,FilePanel::ID_XTERM,FilePanel::onCmdXTerm),
	FXMAPFUNC(SEL_COMMAND,FilePanel::ID_NEW_DIR,FilePanel::onCmdNewDir),
	FXMAPFUNC(SEL_COMMAND,FilePanel::ID_NEW_FILE,FilePanel::onCmdNewFile),
	FXMAPFUNC(SEL_COMMAND,FilePanel::ID_NEW_SYMLINK,FilePanel::onCmdNewSymlink),
	FXMAPFUNC(SEL_COMMAND,FilePanel::ID_FILE_COPY,FilePanel::onCmdFileMan),
	FXMAPFUNC(SEL_COMMAND,FilePanel::ID_FILE_CUT,FilePanel::onCmdFileMan),
	FXMAPFUNC(SEL_COMMAND,FilePanel::ID_FILE_COPYTO,FilePanel::onCmdFileMan),
	FXMAPFUNC(SEL_COMMAND,FilePanel::ID_FILE_MOVETO,FilePanel::onCmdFileMan),
	FXMAPFUNC(SEL_COMMAND,FilePanel::ID_FILE_RENAME,FilePanel::onCmdFileMan),
	FXMAPFUNC(SEL_COMMAND,FilePanel::ID_FILE_SYMLINK,FilePanel::onCmdFileMan),
	FXMAPFUNC(SEL_COMMAND,FilePanel::ID_COPY_CLIPBOARD,FilePanel::onCmdCopyCut),
	FXMAPFUNC(SEL_COMMAND,FilePanel::ID_CUT_CLIPBOARD,FilePanel::onCmdCopyCut),
	FXMAPFUNC(SEL_COMMAND,FilePanel::ID_PASTE_CLIPBOARD,FilePanel::onCmdPaste),
	FXMAPFUNC(SEL_COMMAND,FilePanel::ID_FILE_TRASH,FilePanel::onCmdFileTrash),
	FXMAPFUNC(SEL_COMMAND,FilePanel::ID_FILE_RESTORE,FilePanel::onCmdFileRestore),
	FXMAPFUNC(SEL_COMMAND,FilePanel::ID_FILE_DELETE,FilePanel::onCmdFileDelete),
	FXMAPFUNC(SEL_COMMAND,FilePanel::ID_OPEN_WITH,FilePanel::onCmdOpenWith),
	FXMAPFUNC(SEL_COMMAND,FilePanel::ID_OPEN,FilePanel::onCmdOpen),
	FXMAPFUNC(SEL_COMMAND,FilePanel::ID_REFRESH,FilePanel::onCmdRefresh),
	FXMAPFUNC(SEL_COMMAND,FilePanel::ID_SHOW_BIG_ICONS,FilePanel::onCmdShow),
	FXMAPFUNC(SEL_COMMAND,FilePanel::ID_SHOW_MINI_ICONS,FilePanel::onCmdShow),
	FXMAPFUNC(SEL_COMMAND,FilePanel::ID_SHOW_DETAILS,FilePanel::onCmdShow),
	FXMAPFUNC(SEL_COMMAND,FilePanel::ID_TOGGLE_HIDDEN,FilePanel::onCmdToggleHidden),
	FXMAPFUNC(SEL_COMMAND,FilePanel::ID_TOGGLE_THUMBNAILS,FilePanel::onCmdToggleThumbnails),
	FXMAPFUNC(SEL_COMMAND,FilePanel::ID_SELECT_ALL,FilePanel::onCmdSelect),
	FXMAPFUNC(SEL_COMMAND,FilePanel::ID_DESELECT_ALL,FilePanel::onCmdSelect),
	FXMAPFUNC(SEL_COMMAND,FilePanel::ID_SELECT_INVERSE,FilePanel::onCmdSelect),
	FXMAPFUNC(SEL_COMMAND,FilePanel::ID_ADD_TO_ARCH,FilePanel::onCmdAddToArch),
	FXMAPFUNC(SEL_COMMAND,FilePanel::ID_EXTRACT,FilePanel::onCmdExtract),
	FXMAPFUNC(SEL_COMMAND,FilePanel::ID_EXTRACT_TO_FOLDER,FilePanel::onCmdExtractToFolder),
	FXMAPFUNC(SEL_COMMAND,FilePanel::ID_EXTRACT_HERE,FilePanel::onCmdExtractHere),
	FXMAPFUNC(SEL_RIGHTBUTTONRELEASE,FilePanel::ID_FILELIST,FilePanel::onCmdPopupMenu),
	FXMAPFUNC(SEL_COMMAND,FilePanel::ID_POPUP_MENU,FilePanel::onCmdPopupMenu),
	FXMAPFUNC(SEL_DOUBLECLICKED,FilePanel::ID_FILELIST,FilePanel::onCmdItemDoubleClicked),
	FXMAPFUNC(SEL_CLICKED,FilePanel::ID_FILELIST,FilePanel::onCmdItemClicked),
	FXMAPFUNC(SEL_FOCUSIN,FilePanel::ID_FILELIST,FilePanel::onCmdFocus),
	FXMAPFUNC(SEL_UPDATE,FilePanel::ID_STATUS,FilePanel::onUpdStatus),
	FXMAPFUNC(SEL_UPDATE,FilePanel::ID_DIRECTORY_UP,FilePanel::onUpdUp),
	FXMAPFUNC(SEL_UPDATE,FilePanel::ID_COPY_CLIPBOARD,FilePanel::onUpdMenu),
	FXMAPFUNC(SEL_UPDATE,FilePanel::ID_CUT_CLIPBOARD,FilePanel::onUpdMenu),
	FXMAPFUNC(SEL_UPDATE,FilePanel::ID_PASTE_CLIPBOARD,FilePanel::onUpdPaste),
	FXMAPFUNC(SEL_UPDATE,FilePanel::ID_PROPERTIES,FilePanel::onUpdMenu),
	FXMAPFUNC(SEL_UPDATE,FilePanel::ID_FILE_TRASH,FilePanel::onUpdFileTrash),
	FXMAPFUNC(SEL_UPDATE,FilePanel::ID_FILE_RESTORE,FilePanel::onUpdFileRestore),
	FXMAPFUNC(SEL_UPDATE,FilePanel::ID_GO_TRASH,FilePanel::onUpdGoTrash),
	FXMAPFUNC(SEL_UPDATE,FilePanel::ID_FILE_DELETE,FilePanel::onUpdFileDelete),
	FXMAPFUNC(SEL_UPDATE,FilePanel::ID_FILE_MOVETO,FilePanel::onUpdMenu),
	FXMAPFUNC(SEL_UPDATE,FilePanel::ID_FILE_COPYTO,FilePanel::onUpdMenu),
	FXMAPFUNC(SEL_UPDATE,FilePanel::ID_FILE_RENAME,FilePanel::onUpdSelMult),
	FXMAPFUNC(SEL_UPDATE,FilePanel::ID_EDIT,FilePanel::onUpdOpen),
	FXMAPFUNC(SEL_UPDATE,FilePanel::ID_VIEW,FilePanel::onUpdOpen),
	FXMAPFUNC(SEL_UPDATE,FilePanel::ID_OPEN,FilePanel::onUpdOpen),
	FXMAPFUNC(SEL_UPDATE,FilePanel::ID_ADD_TO_ARCH,FilePanel::onUpdAddToArch),
	FXMAPFUNC(SEL_UPDATE,FilePanel::ID_SHOW_BIG_ICONS,FilePanel::onUpdShow),
	FXMAPFUNC(SEL_UPDATE,FilePanel::ID_SHOW_MINI_ICONS,FilePanel::onUpdShow),
	FXMAPFUNC(SEL_UPDATE,FilePanel::ID_SHOW_DETAILS,FilePanel::onUpdShow),
	FXMAPFUNC(SEL_UPDATE,FilePanel::ID_TOGGLE_HIDDEN,FilePanel::onUpdToggleHidden),
	FXMAPFUNC(SEL_UPDATE,FilePanel::ID_TOGGLE_THUMBNAILS,FilePanel::onUpdToggleThumbnails),
#if defined(linux)
	FXMAPFUNC(SEL_COMMAND,FilePanel::ID_MOUNT,FilePanel::onCmdMount),
	FXMAPFUNC(SEL_COMMAND,FilePanel::ID_UMOUNT,FilePanel::onCmdMount),
	FXMAPFUNC(SEL_UPDATE,FilePanel::ID_MOUNT,FilePanel::onUpdMount),
	FXMAPFUNC(SEL_UPDATE,FilePanel::ID_UMOUNT,FilePanel::onUpdUnmount),
#endif
};

FXint runcmd(FXString cmd)
{
	// Run the command in background
	cmd += " &";
	system(cmd.text());

	return 0;
}

// Object implementation 
FXIMPLEMENT(FilePanel,FXVerticalFrame,FilePanelMap,ARRAYNUMBER(FilePanelMap))

// Construct File Panel
FilePanel::FilePanel(FXWindow *owner, const FXchar* nm,FXComposite *p, DirPanel* dp, FXuint name_size, FXuint size_size, FXuint type_size, FXuint ext_size,
             FXuint modd_size, FXuint user_size, FXuint grou_size, FXuint attr_size, FXuint deldate_size, FXuint origpath_size, FXbool showthumbs, FXColor listbackcolor, FXColor listforecolor,
			 FXColor attentioncolor, FXbool smoothscroll,FXuint opts,FXint x,FXint y,FXint w,FXint h):
             FXVerticalFrame(p,opts,x,y,w,h,0,0,0,0)
{
    name=nm;
	dirpanel=dp;
	attenclr=attentioncolor;

    // Global container
    FXVerticalFrame* cont=new FXVerticalFrame(this,LAYOUT_FILL_Y|LAYOUT_FILL_X|FRAME_RAISED,0,0,0,0, 0,0,0,0, 0,0);
	
	// Container for the path linker
	FXHorizontalFrame* pathframe=new FXHorizontalFrame(cont,LAYOUT_LEFT|JUSTIFY_LEFT|LAYOUT_FILL_X|FRAME_NONE,0,0,0,0, 0,0,0,0);
			
	// File list
    
    // Smooth scrolling
	FXuint options;
	if (smoothscroll)
		options=LAYOUT_FILL_X|LAYOUT_FILL_Y|_ICONLIST_MINI_ICONS;
	else
		options=LAYOUT_FILL_X|LAYOUT_FILL_Y|_ICONLIST_MINI_ICONS|SCROLLERS_DONT_TRACK;

    FXVerticalFrame* cont2=new FXVerticalFrame(cont,LAYOUT_FILL_Y|LAYOUT_FILL_X|FRAME_SUNKEN,0,0,0,0, 0,0,0,0, 0,0);
	list=new FileList(owner,cont2,this,ID_FILELIST,showthumbs,options);
	list->setHeaderSize(0,name_size);
    list->setHeaderSize(1,size_size);
    list->setHeaderSize(2,type_size);
    list->setHeaderSize(3,ext_size);
    list->setHeaderSize(4,modd_size);
    list->setHeaderSize(5,user_size);
    list->setHeaderSize(6,grou_size);
    list->setHeaderSize(7,attr_size);  	
    list->setHeaderSize(8,deldate_size);  	
    list->setHeaderSize(9,origpath_size);  	
	list->setTextColor(listforecolor);
	list->setBackColor(listbackcolor);
	
	// Visually indicate if the panel is active
	activeicon=new FXLabel(pathframe,"",graybuttonicon,JUSTIFY_LEFT|LAYOUT_LEFT);

	// Path text
	pathtext=new TextLabel(pathframe,0,this,ID_FILELIST,LAYOUT_FILL_X|LAYOUT_FILL_Y);
	pathtext->setBackColor(getApp()->getBaseColor());

	// Path linker	
	pathlink = new PathLinker(pathframe,list,dirpanel->getList(),JUSTIFY_LEFT|LAYOUT_LEFT|LAYOUT_FILL_X);
		
	// Status bar
	statusbar=new FXHorizontalFrame(cont,LAYOUT_LEFT|JUSTIFY_LEFT|LAYOUT_FILL_X|FRAME_NONE,0,0,0,0, 3,3,3,3);

    statusbar->setTarget(this);
    statusbar->setSelector(FXSEL(SEL_UPDATE,FilePanel::ID_STATUS));

	FXString key=getApp()->reg().readStringEntry("KEYBINDINGS","hidden_files","Ctrl-F6");
    new FXToggleButton(statusbar,TAB+_("Show hidden files")+PARS(key),TAB+_("Hide hidden files")+PARS(key),showhiddenicon,hidehiddenicon,this->list,
	                   FileList::ID_TOGGLE_HIDDEN,TOGGLEBUTTON_TOOLBAR|LAYOUT_LEFT|ICON_BEFORE_TEXT|FRAME_RAISED);
    
	key=getApp()->reg().readStringEntry("KEYBINDINGS","thumbnails","Ctrl-F7");
	new FXToggleButton(statusbar,TAB+_("Show thumbnails")+PARS(key),TAB+_("Hide thumbnails")+PARS(key),showthumbicon,hidethumbicon,this->list,
	                   FileList::ID_TOGGLE_THUMBNAILS,TOGGLEBUTTON_TOOLBAR|LAYOUT_LEFT|ICON_BEFORE_TEXT|FRAME_RAISED);

	key=getApp()->reg().readStringEntry("KEYBINDINGS","filter","Ctrl-D");
    new FXButton(statusbar,TAB+_("Filter")+PARS(key),filtericon,this,
	                   FilePanel::ID_FILTER,BUTTON_TOOLBAR|LAYOUT_LEFT|ICON_BEFORE_TEXT|FRAME_RAISED);
	
    FXHorizontalFrame* hframe=new FXHorizontalFrame(statusbar,LAYOUT_LEFT|JUSTIFY_LEFT|LAYOUT_FILL_X|FRAME_SUNKEN,0,0,0,0, 0,0,0,0);
	statuslabel=new FXLabel(hframe,_("Status"),NULL,JUSTIFY_LEFT|LAYOUT_LEFT);
	filterlabel=new FXLabel(hframe,"",NULL,JUSTIFY_LEFT|LAYOUT_LEFT|LAYOUT_FILL_X);

	corner=new FXDragCorner(statusbar);

	// Initializations
	selmult=FALSE;
	current=NULL;
    
	// Single click navigation
    single_click=getApp()->reg().readUnsignedEntry("SETTINGS","single_click",SINGLE_CLICK_NONE);
	if (single_click==SINGLE_CLICK_DIR_FILE)
		list->setDefaultCursor(getApp()->getDefaultCursor(DEF_HAND_CURSOR));

	// Dialogs
	operationdialogsingle=NULL;
	operationdialogmultiple=NULL;
	newdirdialog=NULL;
	newfiledialog=NULL;
	newlinkdialog=NULL;
	opendialog=NULL;
	archdialog=NULL;
	filterdialog=NULL;
	
	// Home and trahscan locations
	homelocation=getenv("HOME");
    if(homelocation=="")
        homelocation=ROOTDIR;
	trashlocation=homelocation+PATHSEPSTRING+TRASHPATH;
	trashfileslocation=homelocation+PATHSEPSTRING+TRASHFILESPATH;
	trashinfolocation=homelocation+PATHSEPSTRING+TRASHINFOPATH;
	
	// Start location (we return to the start location after each chdir)
	startlocation=FXSystem::getCurrentDirectory();

	// Initialize clipboard flags
	clipboard_locked=FALSE;
	paste=FALSE;
	
	// Initialize control flag for right click popup menu
	ctrlflag=FALSE;
	
	// Initialize the Shift-F10 flag
	shiftf10=FALSE;

	// Initialize the active panel flag
	isactive=FALSE;
}


// Create X window
void FilePanel::create()
{
    // Register standard uri-list type
	urilistType=getApp()->registerDragType("text/uri-list");
    
	// Register special uri-list type used for Gnome, XFCE and Xfe 
	xfelistType=getApp()->registerDragType("x-special/gnome-copied-files");
	
	// Register special uri-list type used for KDE 
	kdelistType=getApp()->registerDragType("application/x-kde-cutselection");

	// Register standard UTF-8 text type used for file dialogs 
	utf8Type=getApp()->registerDragType("UTF8_STRING");
	
	// Display or hide path linker
	FXbool show_pathlink=getApp()->reg().readUnsignedEntry("SETTINGS","show_pathlinker",TRUE);
	if (show_pathlink)
	{
		pathtext->hide();
		pathlink->show();
	}
	else
	{	
		pathtext->show();
		pathlink->hide();
	}
	
	FXVerticalFrame::create();
}


// Destructor
FilePanel::~FilePanel()
{
    delete list;
    delete current;
    delete next;
    delete statuslabel;
	delete filterlabel;
	delete statusbar;
	delete pathlink;
	delete newfiledialog;
	delete newlinkdialog;
	delete newdirdialog;
	delete opendialog;
	delete archdialog;
	delete filterdialog;
	delete operationdialogsingle;
	delete operationdialogmultiple;
	delete pathtext;
}


// Make panel active
void FilePanel::setActive()
{
	// Set active icon
	activeicon->setIcon(greenbuttonicon);
	activeicon->setTipText(_("Panel has focus"));
		
    pathlink->focus();
    current=this;
	
	// Make dirpanel point on the current directory
	dirpanel->setDirectory(current->list->getDirectory(),TRUE);

	// Make dirpanel inactive
	dirpanel->setInactive();
	
    next->setInactive();
    list->setFocus();
	
	isactive=TRUE;
}


// Make panel inactive
void FilePanel::setInactive(FXbool force)
{
	// Set active icon
	activeicon->setIcon(graybuttonicon);
	activeicon->setTipText(_("Panel does not have focus"));
	
    // By default we set the panel inactive
	if (force)
	{
		current=next;
		list->handle(this,FXSEL(SEL_COMMAND,FileList::ID_DESELECT_ALL),NULL);
		
		isactive=FALSE;
	}
}


// Make panel focus (i.e. active) when clicked
long FilePanel::onCmdFocus(FXObject* sender,FXSelector sel, void* ptr)
{
	setActive();
    return 1;
}



// Set Pointer to Another FilePanel
void FilePanel::Next(FilePanel* nxt)
{
    next=nxt;
}


// Show or hide drag corner
void FilePanel::showCorner(FXbool show)
{
	if (show)
		corner->show();
	else
		corner->hide();
}
		

// Update location history when changing directory (home, up or double click)
void FilePanel::updateLocation()
{
    FXString item;
	FXint i=0;
	FXComboBox* address=((XFileExplorer*) mainWindow)->getAddressBox();
  	address->setNumVisible(5);
	FXint count=address->getNumItems();
    FXString p=list->getDirectory();
	
	// Remember latest directory in the location address
	if(!count)
	{
		count++;
		address->insertItem(0,address->getText());
	}
    while(i < count)
	{
    	item=address->getItem(i++);
    	if(streq((const char*)&p[0],(const char*)&item[0]))
		{
			i--;
			break;
		}
	}
    if(i==count)
		address->insertItem(0,list->getDirectory());
}


// We now really do have the clipboard, keep clipboard content
long FilePanel::onClipboardGained(FXObject* sender,FXSelector sel,void* ptr)
{
    FXVerticalFrame::onClipboardGained(sender,sel,ptr);
    return 1;
}


// We lost the clipboard
long FilePanel::onClipboardLost(FXObject* sender,FXSelector sel,void* ptr)
{
    FXVerticalFrame::onClipboardLost(sender,sel,ptr);
	return 1;
}


// Somebody wants our clipboard content
long FilePanel::onClipboardRequest(FXObject* sender,FXSelector sel,void* ptr)
{
    FXEvent *event=(FXEvent*)ptr;
    FXuchar *data;
    FXuint len;

	// Perhaps the target wants to supply its own data for the clipboard
    if(FXVerticalFrame::onClipboardRequest(sender,sel,ptr))
		return 1;

    // Clipboard target is xfelistType (Xfe, Gnome or XFCE)
	if(event->target==xfelistType)
    {			
		// Don't modify the clipboard if we are called from updPaste()
		if (!clipboard_locked)
		{
			// Prepend "copy" or "cut" as in the Gnome way and avoid duplicating these strings
			if (clipboard.find("copy\n")<0 && clipboard.find("cut\n")<0)
			{
				if (clipboard_type==CUT_CLIPBOARD)
					clipboard = "cut\n" + clipboard;
				else
					clipboard = "copy\n" + clipboard;
			}		
		}
		
		// Return clipboard content
		if(event->target==xfelistType)
		{
			if(!clipboard.empty())
			{
				len=clipboard.length();
				FXMEMDUP(&data,clipboard.text(),FXuchar,len);
				setDNDData(FROM_CLIPBOARD,event->target,data,len);
				
				// Return because xfelistType is not compatible with other types
				return 1;
			}
		}
    }

    // Clipboard target is kdelisType (KDE)
    if(event->target==kdelistType)
    {
		// The only data to be passed in this case is "0" for copy and "1" for cut
		// The uri data are passed using the standard uri-list type
		FXString flag;
		if (clipboard_type==CUT_CLIPBOARD)
			flag = "1";
		else
			flag = "0";

        // Return clipboard content
        if(event->target==kdelistType)
        {
			FXMEMDUP(&data,flag.text(),FXuchar,1);
			setDNDData(FROM_CLIPBOARD,event->target,data,1);
		}
    }

    // Clipboard target is urilistType (KDE apps ; non Gnome, non XFCE and non Xfe apps)  
    if(event->target==urilistType)
    {
		if(!clipboard.empty())
		{
			len=clipboard.length();
			FXMEMDUP(&data,clipboard.text(),FXuchar,len);
			setDNDData(FROM_CLIPBOARD,event->target,data,len);
		
			return 1;
		}
    }
	
    // Clipboard target is utf8Type (to paste file pathes as text to other applications)  
    if(event->target==utf8Type)
    {
		if(!clipboard.empty())
		{			
			FXint beg=0, end=0;
			FXString str="";
			FXString filepath, url;

			// Clipboard don't contain 'copy\n' or 'cut\n' as first line
			if (clipboard.find("copy\n")<0 && clipboard.find("cut\n")<0)
			{
				// Remove the 'file:' prefix for each file path
				while (1)
				{
					end=clipboard.find('\n',end);
					if (end<0)
						break;
					url=clipboard.mid(beg,end-beg+1);
					filepath=FXURL::decode(FXURL::fileFromURL(url));
					str += filepath;
					end++;
					beg=end;
				}
				end=str.length();
				str=str.mid(0,end-1);
			}
			
			// Clipboard contains 'copy\n' or 'cut\n' as first line, thus skip it
			else
			{				
				// Start after the 'copy\n' or 'cut\n' prefix
				end=clipboard.find('\n',0);
				end++;
				beg=end;
				
				// Remove the 'file:' prefix for each file path
				while (1)
				{
					end=clipboard.find('\n',end);
					if (end<0)
						break;
					url=clipboard.mid(beg,end-beg+1);
					filepath=FXURL::decode(FXURL::fileFromURL(url));
					str += filepath;
					end++;
					beg=end;
				}
				end=str.length();
				str=str.mid(0,end-1);
			}
			
			if(!str.empty())
			{
				len=str.length();
				FXMEMDUP(&data,str.text(),FXuchar,len);
				setDNDData(FROM_CLIPBOARD,event->target,data,len);
			
				return 1;
			}
		}
    }
    return 0;
}


// Copy or cut to clipboard
long FilePanel::onCmdCopyCut(FXObject*,FXSelector sel,void*)
{		
    FXString name,curdir;
	
	// Clear clipboard
	clipboard.clear();

    // Clipboard type
	if (FXSELID(sel)==ID_CUT_CLIPBOARD)
		clipboard_type=CUT_CLIPBOARD;
	else
		clipboard_type=COPY_CLIPBOARD;

	// Items number in the file list
    FXint num=current->list->getNumSelectedItems();
			
    if (num==0)
		return 0;

	// If exist selected files, use them
	else if (num>=1)
	{
		// Eventually deselect the '..' directory
		if (current->list->isItemSelected(0))
			current->list->deselectItem(0);

    	// Construct the uri list of files and fill the clipboard with it
    	curdir=current->list->getDirectory();
		for (int u= 0; u< current->list->getNumItems (); u++)
		{
        	if (current->list->isItemSelected(u))
        	{
            	name=current->list->getItemText(u).text();
            	name=name.section('\t',0);
            	if (curdir==ROOTDIR)
					clipboard += FXURL::encode(::fileToURI(curdir+name))+"\n";
				else
					clipboard += FXURL::encode(::fileToURI(curdir+PATHSEPSTRING+name))+"\n";
        	}
		}
	}
		
	// Acquire the clipboard
	FXDragType types[4];
	types[0]=xfelistType;
	types[1]=kdelistType;
	types[2]=urilistType;
	types[3]=utf8Type;
	if (acquireClipboard(types,4))
		return 0;
    
	return 1;
}


// Paste file(s) from clipboard
long FilePanel::onCmdPaste(FXObject*,FXSelector sel,void*)
{
    FXuchar *data;
    FXuint len;
	FXint beg, end, pos;
	FXString chaine, url, param;		  
	FXint num=0;
	FXbool from_kde=FALSE;

	// If source is xfelistType (Gnome, XFCE, or Xfe app)
    if(getDNDData(FROM_CLIPBOARD,xfelistType,data,len))
    {
        FXRESIZE(&data,FXuchar,len+1);
        data[len]='\0';
		
		clipboard=(FXchar*)data;
		
		// Loop over clipboard items
		for(beg=0; beg<clipboard.length(); beg=end+1)
		{
			if((end=clipboard.find("\n",beg))<0)
				end=clipboard.length();

			// Obtain item url
			url=clipboard.mid(beg,end-beg);
				
			// Eventually remove the trailing '\r' if any
			if ((pos=url.rfind('\r'))>0)
				url.erase(pos);

			// Process first item
			if (num==0)
			{
				// First item should be "copy" or "cut"
				if (url=="copy")
				{
					clipboard_type=COPY_CLIPBOARD;
					num++;
				}
				else if (url=="cut")
				{
					clipboard_type=CUT_CLIPBOARD;
					num++;
				}
				
				// If first item is not "copy" nor "cut", process it as a normal url
				// and use default clipboard type
				else
				{											
					// Update the param string
					param += FXURL::decode(FXURL::fileFromURL(url)) + "\n";
						
					// Add one more because the first line "copy" or "cut" was not present
					num += 2;
				}
			}
			
			// Process other items
			else
			{
				// Update the param string
				param += FXURL::decode(FXURL::fileFromURL(url)) + "\n";
				num++;
			}
		}

		// Construct the final param string passed to the file management routine
		param = current->list->getDirectory()+"\n" + FXStringVal(num-1) + "\n" + param;
		
		// Copy or cut operation depending on the clipboard type
		switch(clipboard_type)
		{
		case COPY_CLIPBOARD:
			sel=FXSEL(SEL_COMMAND,FilePanel::ID_FILE_COPY);
			break;
		case CUT_CLIPBOARD:
			clipboard.clear();
			sel=FXSEL(SEL_COMMAND,FilePanel::ID_FILE_CUT);
			break;
		}
		paste=TRUE;
		handle(this,sel,(void*)param.text());
    		
        // Free data pointer
		FXFREE(&data);

		// Return here because xfelistType is not compatible with other types
		return 1;
	}

	// If source type is kdelistType (KDE)
    if(getDNDData(FROM_CLIPBOARD,kdelistType,data,len))
    {		
        from_kde=TRUE;
		
		FXRESIZE(&data,FXuchar,len+1);
        data[len]='\0';
		clipboard=(FXchar*)data;
				
		// Obtain clipboard type (copy or cut)
		if (clipboard=="1")
			clipboard_type=CUT_CLIPBOARD;
		else
			clipboard_type=COPY_CLIPBOARD;
		
		FXFREE(&data);
	}
	
	
	// If source type is urilistType (KDE apps ; non Gnome, non XFCE and non Xfe apps)
    if(getDNDData(FROM_CLIPBOARD,urilistType,data,len))
    {		
		// For non KDE apps, set action to copy
		if (!from_kde)
			clipboard_type=COPY_CLIPBOARD;
		
        FXRESIZE(&data,FXuchar,len+1);
        data[len]='\0';
		clipboard=(FXchar*)data;
				
		// Loop over clipboard items
		for(beg=0; beg<clipboard.length(); beg=end+1)
		{
			if((end=clipboard.find("\n",beg))<0)
				end=clipboard.length();

			// Obtain item url
			url=clipboard.mid(beg,end-beg);

			// Eventually remove the trailing '\r' if any
			if ((pos=url.rfind('\r'))>0)
				url.erase(pos);

			// Update the param string
			param += FXURL::decode(FXURL::fileFromURL(url)) + "\n";
			num++;
		}

		// Construct the final param string passed to the file management routine
		param = current->list->getDirectory()+"\n" + FXStringVal(num) + "\n" + param;
				
		// Copy or cut operation depending on the clipboard type
		switch(clipboard_type)
		{
		case COPY_CLIPBOARD:
			sel=FXSEL(SEL_COMMAND,FilePanel::ID_FILE_COPY);
			break;
		case CUT_CLIPBOARD:
			clipboard.clear();
			sel=FXSEL(SEL_COMMAND,FilePanel::ID_FILE_CUT);
			break;
		}
		paste=TRUE;
		handle(this,sel,(void*)param.text());
					
		FXFREE(&data);
		return 1;
	}
	return 0;
}


// Double Click on File Item
long FilePanel::onCmdItemDoubleClicked(FXObject* sender,FXSelector sel, void* ptr)
{
	FXString cmd, cmdname, filename, pathname;

 	// Wait cursor
    getApp()->beginWaitCursor();

	long item= (long) ptr;
    if(item > -1)
    {
		// File name and path
		filename=list->getItemFilename(item);
		pathname=list->getItemPathname(item);

        // If directory, open the directory
        if(list->isItemDirectory(item))
        {
            // Does not have access
			if(!::isReadExecutable(pathname))
			{
            	MessageBox::error(this,BOX_OK_SU,_("Error"),_(" Permission to: %s denied."), pathname.text());
				getApp()->endWaitCursor();
                return 0;
			}
            if(filename=="..")
                list->handle(this,FXSEL(SEL_COMMAND,FileList::ID_DIRECTORY_UP),NULL);
            else
                list->setDirectory(pathname);
            
			// Change directory in tree list  
			dirpanel->setDirectory(pathname,TRUE);
			current->updatePath();
			
			// Update location history			
			updateLocation();
        }
        else if(list->isItemFile(item))
        {
			// Update associations dictionary
			FileDict *assocdict=new FileDict(getApp());			
			FileAssoc *association=assocdict->findFileBinding(pathname.text());

			// Shell script
			//if (mimetype(pathname) == "text/x-shellscript")
               //current->handle(this,FXSEL(SEL_COMMAND,ID_EDIT),NULL);
			
			// If there is an association
            //else if(association)
            if(association)
			{
				// Use it to open the file
				if(association->command.section(',',0) != "")
                {
					cmdname=association->command.section(',',0);
					
					// If command exists, run it
					if (::existCommand(cmdname))
					{
						cmd=cmdname+" "+::quote(pathname);
						runcmd(cmd);
					}

					// If command does not exist, call the "Open with..." dialog
					else
					{
						getApp()->endWaitCursor();
                    	current->handle(this,FXSEL(SEL_COMMAND,ID_OPEN_WITH),NULL);
					}
                }
                
				// Or execute it
				else if(list->isItemExecutable(item))
                {
                    cmdname=FXPath::name(pathname);
                    cmd=::quote(pathname);
					runcmd(cmd);
               	}
				
				// Or call the "Open with..." dialog
                else
				{
					getApp()->endWaitCursor();
					current->handle(this,FXSEL(SEL_COMMAND,ID_OPEN_WITH),NULL);
				}
			}
            
			// If no association but executable
			else if(list->isItemExecutable(item))
            {
				cmdname=FXPath::name(pathname);
				cmd=::quote(pathname);
				runcmd(cmd);
            }
			
			// Other cases
            else
			{
				getApp()->endWaitCursor();
				current->handle(this,FXSEL(SEL_COMMAND,ID_OPEN_WITH),NULL);
			}
        }
    }
	
	getApp()->endWaitCursor();

    return 1;
}


// Single click on File Item
long FilePanel::onCmdItemClicked(FXObject* sender,FXSelector sel, void* ptr)
{
	// Make panel active
	setActive();
		
	if (single_click != SINGLE_CLICK_NONE)
	{		
		// In detailed mode, avoid single click when cursor is not over the first column
		FXint x, y;
		FXuint state;
		getCursorPosition(x,y,state);
		FXbool allow=TRUE;
		if (!(list->getListStyle()&(_ICONLIST_BIG_ICONS|_ICONLIST_MINI_ICONS)) && (x-list->getXPosition())>list->getHeaderSize(0))
			allow=FALSE;
		
		// Single click with control or shift
		if (state&(CONTROLMASK|SHIFTMASK))
			return 1;
		
		// Single click without control or shift
		else
		{
			FXString cmd, cmdname, filename, pathname;

			// Wait cursor
			getApp()->beginWaitCursor();

			long item= (long) ptr;
			if(item > -1)
			{
				// File name and path
				filename=list->getItemFilename(item);
				pathname=list->getItemPathname(item);

				// If directory, open the directory
				if((single_click != SINGLE_CLICK_NONE) && list->isItemDirectory(item) && allow)
				{
					// Does not have access
					if(!::isReadExecutable(pathname))
					{
						MessageBox::error(this,BOX_OK_SU,_("Error"),_(" Permission to: %s denied."), pathname.text());
						getApp()->endWaitCursor();
						return 0;
					}
					if(filename=="..")
						list->handle(this,FXSEL(SEL_COMMAND,FileList::ID_DIRECTORY_UP),NULL);
					else
						list->setDirectory(pathname);
					
					// Change directory in tree list  
					dirpanel->setDirectory(pathname,TRUE);
					current->updatePath();
					
					// Update location history			
					updateLocation();
				}
				
				// If file, use the association if any
				else if((single_click==SINGLE_CLICK_DIR_FILE) && list->isItemFile(item) && allow)
				{
					// Update associations dictionary
					FileDict *assocdict=new FileDict(getApp());			
					FileAssoc *association=assocdict->findFileBinding(pathname.text());

					// Shell script
					//if (mimetype(pathname) == "text/x-shellscript")
					   //current->handle(this,FXSEL(SEL_COMMAND,ID_EDIT),NULL);
					
					// If there is an association
					//else if(association)
					if(association)
					{
						// Use it to open the file
						if(association->command.section(',',0) != "")
						{
							cmdname=association->command.section(',',0);

							// If command exists, run it
							if (::existCommand(cmdname))
							{
								cmd=cmdname+" "+::quote(pathname);
								runcmd(cmd);
							}

							// If command does not exist, call the "Open with..." dialog
							else
							{
								getApp()->endWaitCursor();
								current->handle(this,FXSEL(SEL_COMMAND,ID_OPEN_WITH),NULL);
							}
						}
						
						// Or execute it
						else if(list->isItemExecutable(item))
						{
							cmdname=FXPath::name(pathname);
							cmd=::quote(pathname);
							runcmd(cmd);
						}
						
						// Or call the "Open with..." dialog
						else
						{
							getApp()->endWaitCursor();
							current->handle(this,FXSEL(SEL_COMMAND,ID_OPEN_WITH),NULL);
						}
					}
					
					// If no association but executable
					else if(list->isItemExecutable(item))
					{
						cmdname=FXPath::name(pathname);
						cmd=::quote(pathname);
						runcmd(cmd);
					}
					
					// Other cases
					else
					{
						getApp()->endWaitCursor();
						current->handle(this,FXSEL(SEL_COMMAND,ID_OPEN_WITH),NULL);
					}
				}
			}
			getApp()->endWaitCursor();
		}
	}
    return 1;
}



// Go to parent directory
long FilePanel::onCmdDirectoryUp(FXObject* sender,FXSelector sel, void* ptr)
{
    current->list->handle(this,FXSEL(SEL_COMMAND,FileList::ID_DIRECTORY_UP),NULL);
    current->list->setFocus();
    dirpanel->setDirectory(current->list->getDirectory(),TRUE);
    current->updatePath();
	updateLocation();
    return 1;
}


// Go to home directory
long FilePanel::onCmdGoHome(FXObject* sender,FXSelector sel, void* ptr)
{
    current->list->setDirectory(homelocation);
    current->list->setFocus();
    dirpanel->setDirectory(homelocation,TRUE);
    current->updatePath();
	updateLocation();
    return 1;
}


// Go to trash directory
long FilePanel::onCmdGoTrash(FXObject* sender,FXSelector sel, void* ptr)
{
    current->list->setDirectory(trashfileslocation);
    current->list->setFocus();
    dirpanel->setDirectory(trashfileslocation,TRUE);
    current->updatePath();
	updateLocation();
    return 1;
}


// Set the flag that allows to stop the file list refresh
long FilePanel::onCmdStopListRefreshTimer(FXObject*,FXSelector,void*)
{
	stopListRefresh=TRUE;

    return 0;
}



// Copy/Move/Rename/Symlink file(s)
long FilePanel::onCmdFileMan(FXObject* sender,FXSelector sel,void* ptr)
{
	FXint num;
	FXString src, targetdir, target, name, source;
	FXint firstitem=0, lastitem=0;
	
	// Focus on this panel list
	current->list->setFocus();
	
	// Confirmation dialog?
	FXbool ask_before_copy=getApp()->reg().readUnsignedEntry("OPTIONS","ask_before_copy",TRUE);

	// If we are we called from the paste command, get the parameters from the pointer
	if (paste)
	{
		// Reset the flag
		paste=FALSE;

		// Get the parameters
		FXString str=(char*)ptr;
		targetdir=str.section('\n',0);
		num=FXUIntVal(str.section('\n',1));
		src=str.after('\n',2);
		
		// If no item, return
		if(num<=0)
			return 0;
	}
	
	// Obtain the parameters from the file panel
	else
	{
		// Current directory
		FXString curdir=current->list->getDirectory();
		
		// Number of selected items
		num=current->list->getNumSelectedItems();
		
		// If no item, return
		if (num<=0)
			return 0;
		
		// Eventually deselect the '..' directory
		if (current->list->isItemSelected(0))
			current->list->deselectItem(0);

		// Obtain the list of source files and the target directory
		for (int u=0; u<current->list->getNumItems(); u++)
		{
			if (current->list->isItemSelected(u))
			{
				if (firstitem==0)
					firstitem=u;
				lastitem=u;
				name=current->list->getItemText(u).text();
				name=name.section('\t',0);
				src += curdir+PATHSEPSTRING+name+"\n";
			}
		}
		targetdir=current->next->list->getDirectory();
				
		if(!current->next->shown() || FXSELID(sel)==ID_FILE_RENAME)
			targetdir=current->list->getDirectory();
	}
				
	// Number of items in the FileList
	FXint numitems=current->list->getNumItems();

	// Go to target directory
	//chdir(targetdir.text());
	
	// Name and directory of the first source file
	source=src.section('\n',0);
	name=FXPath::name(source);
	FXString dir=FXPath::directory(source);
	
	// Initialise target name	
	if (targetdir!=ROOTDIR)
		target=targetdir+PATHSEPSTRING;
	else
		target=targetdir;
	
    // Configure the command, title, message, etc.
    FXIcon *icon=NULL;
	FXString command, title, message;
	if (FXSELID(sel)==ID_FILE_COPY)
    {
        command="copy";
        title=_("Copy");
        icon=copy_bigicon;
        if(num==1)
        {
            message=_("Copy ");
            message+=source;
		}
        else
			message.format(_("Copy %s items from: %s"),FXStringVal(num).text(),dir.text());
    }
    if (FXSELID(sel)==ID_FILE_RENAME)
    {
        command="rename";
        title=_("Rename");
        icon=move_bigicon;
        if(num==1)
        {
            message=_("Rename ");
			message+=name;
            target=name;
			title=_("Rename");
        }
        else
			return 0;
    }
	if (FXSELID(sel)==ID_FILE_COPYTO)
    {
        command="copy";
        title=_("Copy");
        icon=copy_bigicon;
        if(num==1)
        {
            message=_("Copy ");
            message+=source;
		}
        else
			message.format(_("Copy %s items from: %s"),FXStringVal(num).text(),dir.text());
    }
    if (FXSELID(sel)==ID_FILE_MOVETO)
    {
        command="move";
        title=_("Move");
        icon=move_bigicon;
        if(num==1)
        {
            message=_("Move ");
            message+=source;
            title=_("Move");
        }
        else
			message.format(_("Move %s items from: %s"),FXStringVal(num).text(),dir.text());
    }
    if (FXSELID(sel)==ID_FILE_CUT)
    {
        command="move";
        title=_("Move");
        icon=move_bigicon;
        if(num==1)
        {
            message=_("Move ");
            message+=source;
            title=_("Move");
        }
        else
			message.format(_("Move %s items from: %s"),FXStringVal(num).text(),dir.text());
    }
    if (FXSELID(sel)==ID_FILE_SYMLINK)
    {
        command="symlink";
		title=_("Symlink");
        icon=link_bigicon;
        if(num==1)
        {
            message=_("Symlink ");
            message+=source;
        }
        else
			message.format(_("Symlink %s items from: %s"),FXStringVal(num).text(),dir.text());
    }
	
    // File operation dialog, if needed
	if (ask_before_copy || source==target ||  FXSELID(sel)==ID_FILE_COPYTO || FXSELID(sel)==ID_FILE_MOVETO ||  FXSELID(sel)==ID_FILE_RENAME || FXSELID(sel)==ID_FILE_SYMLINK)
	{   
		if (num==1)
		{
			if (operationdialogsingle==NULL)
				operationdialogsingle=new BrowseInputDialog(this,"","",title,_("To:"),icon,BROWSE_INPUT_MIXED);
			operationdialogsingle->setTitle(title);
			operationdialogsingle->setMessage(message);
			operationdialogsingle->setText(target);
			operationdialogsingle->CursorEnd();
			operationdialogsingle->setDirectory(targetdir);
			int rc=1;
			rc=operationdialogsingle->execute(PLACEMENT_CURSOR);
			target=operationdialogsingle->getText();
			if (!rc)
				return 0;
		}
		else
		{
			if (operationdialogmultiple==NULL)
				operationdialogmultiple=new BrowseInputDialog(this,"","",title,_("To folder:"),icon,BROWSE_INPUT_FOLDER);
			operationdialogmultiple->setTitle(title);
			operationdialogmultiple->setMessage(message);
			operationdialogmultiple->setText(target);
			operationdialogmultiple->CursorEnd();
			operationdialogmultiple->setDirectory(targetdir);
			int rc=1;
			rc=operationdialogmultiple->execute(PLACEMENT_CURSOR);
			target=operationdialogmultiple->getText();
			if (!rc)
				return 0;
		}
	}

	// Nothing entered
	if (target=="")
	{
		MessageBox::warning(this,BOX_OK,_("Warning"),_("File name is empty, operation cancelled"));
		return 0;
	}
	
	// Update target and target parent directory
	target=::filePath(target,targetdir);
		
    // Target directory not writable
    if(!::isWritable(targetdir))
    {
        MessageBox::error(this,BOX_OK_SU,_("Error"), _("Can't write to %s: Permission denied"), targetdir.text());
        return 0;
    }

	// Multiple sources and non existent destination
	if (num>1 && !::exists(target))
	{
        MessageBox::error(this,BOX_OK,_("Error"),_("Folder %s doesn't exist"),target.text());
        return 0;
	}
		
	// Multiple sources and target is a file
	if (num>1 && ::isFile(target))
	{ 			
        MessageBox::error(this,BOX_OK,_("Error"),_("%s is not a folder"),target.text());
        return 0;
	}

	// Target is a directory and is not writable
    if (::isDirectory(target) && !::isWritable(target))
    {
        MessageBox::error(this,BOX_OK_SU,_("Error"),_("Can't write to %s: Permission denied"), target.text());
        return 0;
    }
	
	// Target is a file and its parent directory is not writable
	if (::isFile(target) && !::isWritable(targetdir))
	{
        MessageBox::error(this,BOX_OK_SU,_("Error"),_("Can't write to %s: Permission denied"),targetdir.text());
        return 0;
	}

	// Target parent directory doesn't exist
	if (!::exists(targetdir))
	{
		MessageBox::error(this,BOX_OK,_("Error"),_("Folder %s doesn't exist"),targetdir.text());
        return 0;
	}
		
	// Target parent directory is not a directory
	if (!::isDirectory(targetdir))
	{
		MessageBox::error(this,BOX_OK,_("Error"),_("%s is not a folder"),targetdir.text());
        return 0;
	}
		
	// One source
	File *f=NULL;
	FXbool ret;
    if(num==1)
	{
		// An empty source file name corresponds to the ".." file
		// Don't perform any file operation on it!
		if (source=="")
			return 0;
		
		// Wait cursor
		getApp()->beginWaitCursor();

		// File object
        if(command=="copy")
        {
			f=new File(this,_("File copy"),COPY);
        	f->create();
			
			// If target file is located at trash location, also create the corresponding trashinfo file
			// Do it silently and don't report any error if it fails
			FXbool use_trash_can=getApp()->reg().readUnsignedEntry("OPTIONS","use_trash_can",TRUE);
			if (use_trash_can && target==trashfileslocation )
			{						
				// Trash files path name
				FXString trashpathname=createTrashpathname(source,trashfileslocation);
				
				// Adjust target name to get the _N suffix if any
				FXString trashtarget=target+PATHSEPSTRING+FXPath::name(trashpathname);
		
				// Create trashinfo file
				createTrashinfo(source,trashpathname,trashfileslocation,trashinfolocation);

				// Copy source to trash target
				ret=f->copy(source,trashtarget);

			}

			// Copy source to target
			else
				ret=f->copy(source,target);

			// An error has occurred
			if (ret==0 && !f->isCancelled())
			{
				f->hideProgressDialog();
				MessageBox::error(this,BOX_OK,_("Error"),_("An error has occurred during the copy file operation!"));
			}
					
			// If action is cancelled in progress dialog
			if (f->isCancelled())
			{
				f->hideProgressDialog();
				MessageBox::error(this,BOX_OK,_("Error"),_("Copy file operation cancelled!"));
			}
        }
        else if(command=="rename")
        {
			f=new File(this,_("File rename"),RENAME);
        	f->create();
			ret=f->rename(source,target);

			// If source file is located at trash location, try to also remove the corresponding trashinfo file if it exists
			// Do it silently and don't report any error if it fails
			FXbool use_trash_can=getApp()->reg().readUnsignedEntry("OPTIONS","use_trash_can",TRUE);
			if (use_trash_can && ret && (source.left(trashfileslocation.length())==trashfileslocation) )
			{
				FXString trashinfopathname=trashinfolocation+PATHSEPSTRING+FXPath::name(source)+".trashinfo";
				::unlink(trashinfopathname.text());
			}
        }
        else if(command=="move")
        {
			f=new File(this,_("File move"),MOVE);
        	f->create();

			// If target file is located at trash location, also create the corresponding trashinfo file
			// Do it silently and don't report any error if it fails
			FXbool use_trash_can=getApp()->reg().readUnsignedEntry("OPTIONS","use_trash_can",TRUE);
			if (use_trash_can && target==trashfileslocation )
			{						
				// Trash files path name
				FXString trashpathname=createTrashpathname(source,trashfileslocation);
				
				// Adjust target name to get the _N suffix if any
				FXString trashtarget=target+PATHSEPSTRING+FXPath::name(trashpathname);
		
				// Create trashinfo file
				createTrashinfo(source,trashpathname,trashfileslocation,trashinfolocation);

				// Move source to trash target
				ret=f->move(source,trashtarget);
			}

			// Move source to target
			else
				ret=f->move(source,target);
			
			// If source file is located at trash location, try to also remove the corresponding trashinfo file if it exists
			// Do it silently and don't report any error if it fails
			if (use_trash_can && ret && (source.left(trashfileslocation.length())==trashfileslocation) )
			{
				FXString trashinfopathname=trashinfolocation+PATHSEPSTRING+FXPath::name(source)+".trashinfo";
				::unlink(trashinfopathname.text());
			}
			
			// An error has occurred
			if (ret==0 && !f->isCancelled())
			{
				f->hideProgressDialog();
				MessageBox::error(this,BOX_OK,_("Error"),_("An error has occurred during the move file operation!"));
			}
					
			// If action is cancelled in progress dialog
			if (f->isCancelled())
			{
				f->hideProgressDialog();
				MessageBox::error(this,BOX_OK,_("Error"),_("Move file operation cancelled!"));
			}
        }
        else if(command=="symlink")
        {
			f=new File(this,_("Symlink"),SYMLINK);
        	f->create();
			f->symlink(source,target);
		}
       	else
           	exit(-1);
									
		getApp()->endWaitCursor();
		delete f;
	}
	
	// Multiple sources
	// Note : rename cannot be used in this case!
    else if(num>1)
    {
		// Wait cursor
		getApp()->beginWaitCursor();

		// File object
        if (command=="copy")
			f=new File(this,_("File copy"),COPY);
		else if (command=="move")
			f=new File(this,_("File move"),MOVE);
		else if  (command=="symlink")
			f=new File(this,_("Symlink"),SYMLINK);
			
		f->create();
		
		// Initialize file list stop refresh timer and flag
		stopListRefresh=FALSE;
		getApp()->addTimeout(this,ID_STOP_LIST_REFRESH_TIMER,STOP_LIST_REFRESH_INTERVAL);
		
		// Loop on the multiple files
		for(FXint i=0;i<num;i++)
        {
			// Stop refreshing the file list if file operation is long and has many files
			// This avoids flickering and speeds up things a bit
			if (stopListRefresh && i>STOP_LIST_REFRESH_NBMAX)
			{
				// Force a last refresh if current panel is destination
				if ( current->getDirectory()==targetdir)
					current->list->onCmdRefresh(0,0,0);
				
				// Force a last refresh if next panel is destination
				if ( next->getDirectory()==targetdir)
					next->list->onCmdRefresh(0,0,0);

				// Tell the dir and file list to not refresh anymore
				setAllowRefresh(FALSE);
				next->setAllowRefresh(FALSE);
				dirpanel->setAllowDirsizeRefresh(FALSE);

				// Don't need to stop again
				stopListRefresh=FALSE;
			}
			
			// Individual source file
			source=src.section('\n',i);

			// An empty file name corresponds to the ".." file (why?)
			// Don't perform any file operation on it! 
			if (source!="")
			{
            	if(command=="copy")
				{
					// If target file is located at trash location, also create the corresponding trashinfo file
					// Do it silently and don't report any error if it fails
					FXbool use_trash_can=getApp()->reg().readUnsignedEntry("OPTIONS","use_trash_can",TRUE);
					if (use_trash_can && target==trashfileslocation )
					{						
						// Trash files path name
						FXString trashpathname=createTrashpathname(source,trashfileslocation);
						
						// Adjust target name to get the _N suffix if any
						FXString trashtarget=target+PATHSEPSTRING+FXPath::name(trashpathname);
				
						// Create trashinfo file
						createTrashinfo(source,trashpathname,trashfileslocation,trashinfolocation);

						// Copy source to trash target
						ret=f->copy(source,trashtarget);
					}

					// Copy source to target
					else
						ret=f->copy(source,target);

					// An error has occurred
					if (ret==0 && !f->isCancelled())
					{
						f->hideProgressDialog();
						MessageBox::error(this,BOX_OK,_("Error"),_("An error has occurred during the copy file operation!"));
						break;
					}
					
					// If action is cancelled in progress dialog
					if (f->isCancelled())
					{
						f->hideProgressDialog();
						MessageBox::error(this,BOX_OK,_("Error"),_("Copy file operation cancelled!"));
						break;
					}
				}
            	else if(command=="move")
            	{
					// If target file is located at trash location, also create the corresponding trashinfo file
					// Do it silently and don't report any error if it fails
					FXbool use_trash_can=getApp()->reg().readUnsignedEntry("OPTIONS","use_trash_can",TRUE);
					if (use_trash_can && target==trashfileslocation )
					{						
						// Trash files path name
						FXString trashpathname=createTrashpathname(source,trashfileslocation);
						
						// Adjust target name to get the _N suffix if any
						FXString trashtarget=target+PATHSEPSTRING+FXPath::name(trashpathname);
				
						// Create trashinfo file
						createTrashinfo(source,trashpathname,trashfileslocation,trashinfolocation);

						// Move source to trash target
						ret=f->move(source,trashtarget);
					}

					// Move source to target
					else
						ret=f->move(source,target);
					
					// If source file is located at trash location, try to also remove the corresponding trashinfo file if it exists
					// Do it silently and don't report any error if it fails
					if (use_trash_can && ret && (source.left(trashfileslocation.length())==trashfileslocation) )
					{
						FXString trashinfopathname=trashinfolocation+PATHSEPSTRING+FXPath::name(source)+".trashinfo";
						::unlink(trashinfopathname.text());
					}
				
					// An error has occurred
					if (ret==0 && !f->isCancelled())
					{
						f->hideProgressDialog();
						MessageBox::error(this,BOX_OK,_("Error"),_("An error has occurred during the move file operation!"));
						break;
					}
					
					// If action is cancelled in progress dialog
					if (f->isCancelled())
					{
						f->hideProgressDialog();
						MessageBox::error(this,BOX_OK,_("Error"),_("Move file operation cancelled!"));
						break;
					}
           	 	}
				else if (command=="symlink")
				{
					ret=f->symlink(source,target);
					
					// An error has occurred
					if (ret==0 && !f->isCancelled())
					{
						f->hideProgressDialog();
						MessageBox::error(this,BOX_OK,_("Error"),_("An error has occurred during the symlink operation!"));
						break;
					}
					
					// If action is cancelled in progress dialog
					if (f->isCancelled())
					{
						f->hideProgressDialog();
						MessageBox::error(this,BOX_OK,_("Error"),_("Symlink operation cancelled!"));
						break;
					}
				}
            	else
                	exit(-1);
			}
		}		
		
		// Reinit timer and refresh flags
		getApp()->removeTimeout(this,ID_STOP_LIST_REFRESH_TIMER);
		current->setAllowRefresh(TRUE);
		next->setAllowRefresh(TRUE);
		dirpanel->setAllowDirsizeRefresh(TRUE);
		
		getApp()->endWaitCursor();		
		delete f;
	}	
	
	// Force panel refresh
	onCmdRefresh(0,0,0);
	
	// Enable previous or last selected item for keyboard navigation
	if ((FXSELID(sel)==ID_FILE_MOVETO || FXSELID(sel)==ID_FILE_RENAME) && current->list->getNumItems()<numitems)
	{
		firstitem=(firstitem<1) ? 0 : firstitem-1;
		current->list->enableItem(firstitem);
		current->list->setCurrentItem(firstitem);
	}
	else
	{
		current->list->enableItem(lastitem);
		current->list->setCurrentItem(lastitem);
	}

    return 1;
}



// Trash files from the file list or the tree list
long FilePanel::onCmdFileTrash(FXObject*,FXSelector,void*)
{
	FXint firstitem=0;
	File *f=NULL;
    current->list->setFocus();
	FXString dir=current->list->getDirectory();

    FXbool confirm_trash=getApp()->reg().readUnsignedEntry("OPTIONS","confirm_trash",TRUE);
	
	// If we don't have permission to write to the parent directory
    if(!::isWritable(dir))
    {
        MessageBox::error(this,BOX_OK_SU,_("Error"),_("Can't write to %s: Permission denied"),dir.text());
        return 0;
    }

	// If we don't have permission to write to the trash directory
    if(!::isWritable(trashfileslocation))
    {
        MessageBox::error(this,BOX_OK_SU,_("Error"),_("Can't write to trash location %s: Permission denied"),trashfileslocation.text());
        return 0;
    }

	// Items number in the file list
    FXint num=current->list->getNumSelectedItems();
	
	// If nothing selected, return
	if(num==0)
		return 0;
	
	// If exist selected files, use them
	else if (num>=1)
	{
		// Eventually deselect the '..' directory
		if (current->list->isItemSelected(0))
			current->list->deselectItem(0);

		if(confirm_trash)
    	{
        	FXString message;
			if (num==1)
				message=_("Move the selected item to trash can?");
			else
				message.format(_("Move %s selected items to trash can?"),FXStringVal(num).text());
				
			MessageBox box(this,_("Confirm Trash"),message,delete_bigicon,BOX_OK_CANCEL|DECOR_TITLE|DECOR_BORDER);
        	if(box.execute(PLACEMENT_CURSOR) != BOX_CLICKED_OK)
            	return 0;
   		}

		// Wait cursor
		getApp()->beginWaitCursor();

		// File object
   		f=new File(this,_("Move to trash"),DELETE);
   		f->create();
		list->setAllowRefresh(FALSE);	

		// Overwrite initialisations
		FXbool overwrite=FALSE;
		FXbool overwrite_all=FALSE;
		FXbool skip_all=FALSE;
		
		// Delete selected files
		FXString filename, pathname;    			
		FXint i=0;			
		stopListRefresh=FALSE;
		for(int u=0;u< current->list->getNumItems (); u++)
    	{
        	if (current->list->isItemSelected (u))
        	{
            	// Get index of first selected item
				if (firstitem==0)
					firstitem=u;
				
				// Stop refreshing the dirsize in dirpanel
				// when there are many files to delete
				i++;
				if (!stopListRefresh && i>STOP_LIST_REFRESH_NBMAX)
				{
					dirpanel->setAllowDirsizeRefresh(FALSE);
					stopListRefresh=TRUE;
				}

				// Get file name and path
				filename=current->list->getItemFilename(u);
				pathname=current->list->getItemPathname(u);
	
				// If we don't have permission to write to the file
 				if (!::isWritable(pathname))
    			{
					// Overwrite dialog if necessary
					if (!(overwrite_all | skip_all))
					{
						f->hideProgressDialog();
						FXString msg;
						msg.format(_("File %s is write-protected, move it anyway to trash can?"),pathname.text());
						OverwriteBox* dlg=new OverwriteBox(this,_("Confirm Trash"),msg);
						FXuint answer=dlg->execute(PLACEMENT_SCREEN);
						delete dlg;
						switch(answer)
						{
							// Cancel
							case 0:
								goto end;
								break;
							
							// Overwrite
							case 1:
								overwrite=TRUE;
								break;
							
							// Overwrite all
							case 2:
								overwrite_all=TRUE;
								break;
							
							// Skip
							case 3:
								overwrite=FALSE;
								break;
							
							// Skip all
							case 4:
								skip_all=TRUE;
								break;
						}
					}
					if((overwrite | overwrite_all) & !skip_all)
					{
						// Caution!! Don't delete parent directory!!
						if (filename!="..")
						{							
							// Trash files path name
							FXString trashpathname=createTrashpathname(pathname,trashfileslocation);
					
							// Create trashinfo file
							createTrashinfo(pathname,trashpathname,trashfileslocation,trashinfolocation);

							// Move file to trash files location
							FXbool ret=f->move(pathname,trashpathname);

							// An error has occurred
							if (ret==0 && !f->isCancelled())
							{
								f->hideProgressDialog();
								MessageBox::error(this,BOX_OK,_("Error"),_("An error has occurred during the move to trash operation!"));
								break;
							}
						}
					}
					f->showProgressDialog();
    			}

				// If we have permission to write
				else
				{
					// Caution!! Don't delete parent directory!!
					if (filename!="..")
					{
						// Trash files path name
						FXString trashpathname=createTrashpathname(pathname,trashfileslocation);
				
						// Create trashinfo file
						createTrashinfo(pathname,trashpathname,trashfileslocation,trashinfolocation);

						// Move file to trash files location
						FXbool ret=f->move(pathname,trashpathname);
													
						// An error has occurred
						if (ret==0 && !f->isCancelled())
						{
							f->hideProgressDialog();
							MessageBox::error(this,BOX_OK,_("Error"),_("An error has occurred during the move to trash operation!"));
							break;
						}
					}
					// If action is cancelled in progress dialog
					if (f->isCancelled())
					{
						f->hideProgressDialog();
						MessageBox::error(this,BOX_OK,_("Error"),_("Move to trash file operation cancelled!"));
						break;
					}
				}
        	}
    	}
end:
		getApp()->endWaitCursor();
		delete f;
	}
	// Force FilePanel and DirPanel refresh
	list->setAllowRefresh(TRUE);	
	stopListRefresh=FALSE;
	dirpanel->setAllowDirsizeRefresh(TRUE);
	onCmdRefresh(0,0,0);

	// Enable last item before the first selected item (for keyboard navigation)
	firstitem=(firstitem<1) ? 0 : firstitem-1;
	current->list->enableItem(firstitem);
	current->list->setCurrentItem(firstitem);
	
	return 1;
}


// Restore files from trash can
long FilePanel::onCmdFileRestore(FXObject*,FXSelector,void*)
{
	FXint firstitem=0;
	File *f=NULL;
    current->list->setFocus();
	FXString dir=current->list->getDirectory();
    FXbool confirm_trash=getApp()->reg().readUnsignedEntry("OPTIONS","confirm_trash",TRUE);
	
	// Items number in the file list
    FXint num=current->list->getNumSelectedItems();
	
	// If nothing selected, return
	if(num==0)
		return 0;
	
	// If exist selected files, use them
	else if (num>=1)
	{
		// Eventually deselect the '..' directory
		if (current->list->isItemSelected(0))
			current->list->deselectItem(0);

		// Wait cursor
		getApp()->beginWaitCursor();

		// File object
   		f=new File(this,_("Restore from trash"),DELETE);
   		f->create();
		list->setAllowRefresh(FALSE);	
		
		// Restore (i.e. move to their original location) selected files
		FXString filename, pathname;    			
		FXint i=0;			
		stopListRefresh=FALSE;
		for(int u=0;u< current->list->getNumItems (); u++)
    	{
        	if (current->list->isItemSelected (u))
        	{
            	// Get index of first selected item
				if (firstitem==0)
					firstitem=u;
				
				// Stop refreshing the dirsize in dirpanel
				// when there are many files to delete
				i++;
				if (!stopListRefresh && i>STOP_LIST_REFRESH_NBMAX)
				{
					dirpanel->setAllowDirsizeRefresh(FALSE);
					stopListRefresh=TRUE;
				}

				// Get file name and path
				filename=current->list->getItemFilename(u);
				pathname=current->list->getItemPathname(u);
	
				// Don't restore '..' directory
				if (filename!="..")
				{
					// Obtain trash base name and sub path
					FXString subpath=pathname;
					subpath.erase(0,trashfileslocation.length()+1);
					FXString trashbasename=subpath.before('/');
					if (trashbasename=="")
						trashbasename=name;						
					subpath.erase(0,trashbasename.length());
					
					// Read the .trashinfo file
					FILE *fp;
					FXchar line[1024];
					FXbool success=TRUE;
					FXString trashinfopathname=trashinfolocation+PATHSEPSTRING+trashbasename+".trashinfo";
					FXString origpathname="";
					
					if ((fp=fopen(trashinfopathname.text(),"r"))!=NULL)
					{
						// Read the first two lines and get the strings
						if (fgets(line,sizeof(line),fp)==NULL)
							success=FALSE;
						if (fgets(line,sizeof(line),fp)==NULL)
							success=FALSE;
						if (success)
						{
							origpathname=line;
							origpathname=origpathname.after('=');
							origpathname=origpathname.before('\n');
						}						
						fclose(fp);
						origpathname=origpathname+subpath;
					}

					// Confirm restore dialog
					if(confirm_trash && u==firstitem)
					{
						FXString message;
						if (num==1)
						{
							if (::isDirectory(pathname))
								message.format(_("Restore folder %s to its original location %s ?"),filename.text(),origpathname.text());
							else
								message.format(_("Restore file %s to its original location %s ?"),filename.text(),origpathname.text());
						}	
						else
							message.format(_("Restore %s selected items to their original locations?"),FXStringVal(num).text());
						f->hideProgressDialog();
						MessageBox box(this,_("Confirm Restore"),message,restore_bigicon,BOX_OK_CANCEL|DECOR_TITLE|DECOR_BORDER);
						if(box.execute(PLACEMENT_CURSOR) != BOX_CLICKED_OK)
						{
							getApp()->endWaitCursor();
							delete f;
							return 0;
						}
						f->showProgressDialog();
					}

					if (origpathname=="")
					{
						f->hideProgressDialog();
						MessageBox::error(this,BOX_OK,_("Error"),_("Restore information not available for %s"),pathname.text());
						goto end;
					}

					// If parent dir of the original location does not exist
					FXString origparentdir=FXPath::directory(origpathname);
					if (!::exists(origparentdir))
					{
						// Ask the user if he wants to create it
						f->hideProgressDialog();
						FXString message;
						message.format(_("Parent directory %s does not exist, do you want to create it?"),origparentdir.text());						
						MessageBox box(this,_("Confirm Restore"),message,restore_bigicon,BOX_OK_CANCEL|DECOR_TITLE|DECOR_BORDER);
						if(box.execute(PLACEMENT_CURSOR) != BOX_CLICKED_OK)
							goto end;
						else
						{
							errno=0;
							FXint ret=mkpath(origparentdir.text(),0755);
							FXint errcode=errno;
							if (ret==-1)
							{
								f->hideProgressDialog();
								if (errcode)
									MessageBox::error(this,BOX_OK,_("Error"),_("Can't create folder %s: %s"),origparentdir.text(),strerror(errcode));
								else
									MessageBox::error(this,BOX_OK,_("Error"),_("Can't create folder %s"),origparentdir.text());
								goto end;					
							}
							f->showProgressDialog();

						}
					}

					// Move file to original location (with restore option)
					FXbool ret=f->move(pathname,origpathname,TRUE);
					
					// An error has occurred
					if (ret==0 && !f->isCancelled())
					{
						f->hideProgressDialog();
						MessageBox::error(this,BOX_OK,_("Error"),_("An error has occurred during the restore from trash operation!"));
						goto end;					
					}
					
					// Silently remove trashinfo file
					FXString trashfilespathname=trashfileslocation+PATHSEPSTRING+trashbasename;
					if (pathname==trashfilespathname && !::exists(trashfilespathname))
						::unlink(trashinfopathname.text());
				}
				// If action is cancelled in progress dialog
				if (f->isCancelled())
				{
					f->hideProgressDialog();
					MessageBox::error(this,BOX_OK,_("Error"),_("Restore from trash file operation cancelled!"));
					goto end;					
				}
        	}
    	}
end:
		getApp()->endWaitCursor();
		delete f;
	}
	// Force FilePanel and DirPanel refresh
	list->setAllowRefresh(TRUE);	
	stopListRefresh=FALSE;
	dirpanel->setAllowDirsizeRefresh(TRUE);
	onCmdRefresh(0,0,0);

	// Enable last item before the first selected item (for keyboard navigation)
	firstitem=(firstitem<1) ? 0 : firstitem-1;
	current->list->enableItem(firstitem);
	current->list->setCurrentItem(firstitem);

	return 1;
}


// Definitively delete files from the file list or the tree list (no trash can)
long FilePanel::onCmdFileDelete(FXObject*,FXSelector,void*)
{
	FXint firstitem=0;
	File *f=NULL;
    current->list->setFocus();
	FXString dir=current->list->getDirectory();

    FXbool confirm_del=getApp()->reg().readUnsignedEntry("OPTIONS","confirm_delete",TRUE);
    FXbool confirm_del_emptydir=getApp()->reg().readUnsignedEntry("OPTIONS","confirm_delete_emptydir",TRUE);
	
	// If we don't have permission to write to the parent directory
    if(!::isWritable(dir))
    {
        MessageBox::error(this,BOX_OK_SU,_("Error"),_("Can't write to %s: Permission denied"),dir.text());
        return 0;
    }

	// Items number in the file list
    FXint num=current->list->getNumSelectedItems();

	// If nothing selected, return
	if(num==0)
		return 0;
	
	// If exist selected files, use them
	else if (num>=1)
	{
		// Eventually deselect the '..' directory
		if (current->list->isItemSelected(0))
			current->list->deselectItem(0);

		if(confirm_del)
    	{
        	FXString message;
			if (num==1)
				message=_("Definitively delete the selected item?");
			else
				message.format(_("Definitely delete %s selected items?"),FXStringVal(num).text());
			MessageBox box(this,_("Confirm Delete"),message,delete_big_permicon,BOX_OK_CANCEL|DECOR_TITLE|DECOR_BORDER);
        	if(box.execute(PLACEMENT_CURSOR) != BOX_CLICKED_OK)
            	return 0;
		}
		// Wait cursor
		getApp()->beginWaitCursor();

		// File object
   		f=new File(this,_("File delete"),DELETE);
   		f->create();
		list->setAllowRefresh(FALSE);	

		// Overwrite initialisations
		FXbool overwrite=FALSE;
		FXbool overwrite_all=FALSE;
		FXbool skip_all=FALSE;
		FXbool ask_del_empty=TRUE;
		FXbool skip_all_del_emptydir=FALSE;
		
		// Delete selected files
		FXString filename, pathname;    
		FXint i=0;			
		stopListRefresh=FALSE;
		for(int u=0;u< current->list->getNumItems (); u++)
    	{
        	if (current->list->isItemSelected (u))
        	{
				// Get index of first selected item
				if (firstitem==0)
					firstitem=u;
					
				// Stop refreshing the dirsize in dirpanel
				// when there are many files to delete
				i++;
				if (!stopListRefresh && i>STOP_LIST_REFRESH_NBMAX)
				{
					dirpanel->setAllowDirsizeRefresh(FALSE);
					stopListRefresh=TRUE;
				}

            	// Get file name and path
				filename=current->list->getItemFilename(u);
				pathname=current->list->getItemPathname(u);

				// Confirm empty directory deletion
				if (confirm_del & confirm_del_emptydir & ask_del_empty)
				{
					if (::isEmptyDir(pathname)==0)
					{
						if (skip_all_del_emptydir)
							continue;
							
						f->hideProgressDialog();
						FXString msg;
						msg.format(_("Folder %s is not empty, delete it anyway?"),pathname.text());						
						OverwriteBox* dlg=new OverwriteBox(this,_("Confirm Delete"),msg);
						FXuint answer=dlg->execute(PLACEMENT_SCREEN);
						delete dlg;
						switch(answer)
						{
							// Cancel
							case 0:
								goto end;
								break;
							
							// Yes
							case 1:
								break;
							
							// Yes for all
							case 2:
								ask_del_empty=FALSE;
								break;
							
							// Skip
							case 3:
								continue;
								break;
							
							// Skip all
							case 4:
								skip_all_del_emptydir=TRUE;
								continue;
								break;
						}
						f->showProgressDialog();
					}
				}

				// If we don't have permission to write to the file
 				if (!::isWritable(pathname))
    			{
					// Overwrite dialog if necessary
					if (!(overwrite_all | skip_all))
					{
						f->hideProgressDialog();
						FXString msg;
						msg.format(_("File %s is write-protected, delete it anyway?"),pathname.text());
						OverwriteBox* dlg=new OverwriteBox(this,_("Confirm Delete"),msg);
						FXuint answer=dlg->execute(PLACEMENT_SCREEN);
						delete dlg;
						switch(answer)
						{
							case 1:
								overwrite=TRUE;
								break;
							case 2:
								overwrite_all=TRUE;
								break;
							case 3:
								overwrite=FALSE;
								break;
							case 4:
								skip_all=TRUE;
								break;
						}
					}
					if((overwrite | overwrite_all) & !skip_all)
					{
						// Caution!! Don't delete parent directory!!
						if (filename!="..")
						{
							// Definitively remove file or folder
							f->remove(pathname);
						}
					}
					f->showProgressDialog();
    			}

				// If we have permission to write
				else
				{
					// Caution!! Don't delete parent directory!!
					if (filename!="..")
					{
						// Definitively remove file or folder
						f->remove(pathname);
						
						// If is located at trash location, try to also remove the corresponding trashinfo file if it exists
						// Do it silently and don't report any error if it fails
						FXbool use_trash_can=getApp()->reg().readUnsignedEntry("OPTIONS","use_trash_can",TRUE);
						if (use_trash_can && (pathname.left(trashfileslocation.length())==trashfileslocation) )
						{
							FXString trashinfopathname=trashinfolocation+PATHSEPSTRING+filename+".trashinfo";
							::unlink(trashinfopathname.text());
						}
					}
					// If action is cancelled in progress dialog
					if (f->isCancelled())
					{
						f->hideProgressDialog();
						MessageBox::error(this,BOX_OK,_("Error"),_("Delete file operation cancelled!"));
						break;
					}
				}
        	}
    	}
end:
		getApp()->endWaitCursor();
		delete f;
	}
	// Force FilePanel and DirPanel refresh
	list->setAllowRefresh(TRUE);	
	stopListRefresh=FALSE;
	dirpanel->setAllowDirsizeRefresh(TRUE);
	onCmdRefresh(0,0,0);

	// Enable last item before the first selected item (for keyboard navigation)
	firstitem=(firstitem<1) ? 0 : firstitem-1;
	current->list->enableItem(firstitem);
	current->list->setCurrentItem(firstitem);

	return 1;
}


// Edit file
long FilePanel::onCmdEdit(FXObject*,FXSelector s,void*)
{
    // Wait cursor
    getApp()->beginWaitCursor();

	FXString txteditor=getApp()->reg().readStringEntry("PROGS","txteditor","xfw");
    FXString txtviewer=getApp()->reg().readStringEntry("PROGS","txtviewer","xfv");

    FXString pathname, samecmd, cmd, cmdname, itemslist=" "; 
	FileAssoc* association;
	FXbool same=TRUE;
	FXbool first=TRUE;
	
	current->list->setFocus();

	if (current->list->getNumSelectedItems()==0)
	{
		getApp()->endWaitCursor();
		return 0;
	}
    
	// Update associations dictionary
	FileDict *assocdict=new FileDict(getApp());			
	
	// Check if all files have the same association
	for (FXint u=0; u< current->list->getNumItems (); u++)
	{
		if (current->list->isItemSelected(u))
		{
			// Increment number of selected items
			pathname=current->list->getItemPathname(u);
			association=assocdict->findFileBinding(pathname.text());
		
			// If there is an association
			if (association)
			{
				// Use it to edit/view the files
				if(FXSELID(s)==ID_EDIT)
				{
					cmd=association->command.section(',',2);
					if(cmd.length()==0)
						cmd=txteditor;
				}
				else
				{
					cmd=association->command.section(',',1);
					if(cmd.length()==0)
						cmd=txtviewer;
				}
				if (cmd.text() != NULL)
				{
					
					// First selected item
					if (first)
					{
						samecmd = cmd;
						first=FALSE;
					}
					
					if (samecmd != cmd)
					{
						same=FALSE;
						break;
					}
					
					// List of selected items
					itemslist += ::quote(pathname) + " ";
				}
				else
				{
					same=FALSE;
					break;
				}
			}
			
			// No association
			else
			{
				same=FALSE;
				break;
			}
		}
	}

	// Same association for all files : execute the associated or default editor or viewer
	if (same)
	{
		cmdname=samecmd;
		
		// If command exists, run it
		if (::existCommand(cmdname))
		{
			cmd=cmdname+itemslist;
			runcmd(cmd);
		}

		// If command does not exist, call the "Open with..." dialog
		else
		{
			getApp()->endWaitCursor();
			current->handle(this,FXSEL(SEL_COMMAND,ID_OPEN_WITH),NULL);
		}
	}
	
	// Files have different associations : handle them separately
	else
	{
		for (int u=0; u< current->list->getNumItems (); u++)
		{
			if (current->list->isItemSelected(u))
			{
				pathname=current->list->getItemPathname(u);
				association=assocdict->findFileBinding(pathname.text());
				
				// If there is an association
				if(association)
				{
					// Use it to edit/view the file
					if(FXSELID(s)==ID_EDIT)
					{
						cmd=association->command.section(',',2);
						if(cmd.length()==0)
							cmd=txteditor;
					}
					else
					{
						cmd=association->command.section(',',1);
						if(cmd.length()==0)
							cmd=txtviewer;
					}
					if(cmd.text() != NULL)
					{
						cmdname=cmd;
						
						// If command exists, run it
						if (::existCommand(cmdname))
						{
							cmd=cmdname+" "+::quote(pathname);
							runcmd(cmd);
						}

						// If command does not exist, call the "Open with..." dialog
						else
						{
							getApp()->endWaitCursor();
							current->handle(this,FXSEL(SEL_COMMAND,ID_OPEN_WITH),NULL);
						}
					}
				}
				
				// No association
				else
				{
					if(FXSELID(s)==ID_EDIT)
						cmd=txteditor;
					else
						cmd=txtviewer;
					
					cmdname=cmd;
					
					// If command exists, run it
					if (::existCommand(cmdname))
					{
						cmd=cmdname+" "+::quote(pathname);
						runcmd(cmd);
					}

					// If command does not exist, call the "Open with..." dialog
					else
					{
						getApp()->endWaitCursor();
						current->handle(this,FXSEL(SEL_COMMAND,ID_OPEN_WITH),NULL);
					}
				}
			}
		}
	}
	
	getApp()->endWaitCursor();

    return 1;
}


// File or directory properties 
long FilePanel::onCmdProperties(FXObject* sender,FXSelector,void*)
{
    current->list->setFocus();

    FXint num, itm;	
	num=current->list->getNumSelectedItems(&itm);

	// If no selected files in the file list, use the selected folder from the tree list (if any)
	if (num==0)
	{
    	DirItem* item=(DirItem*)dirpanel->getCurrentItem();
		if (!item)
			return 0;
			
		FXString pathname=dirpanel->getItemPathname(item);
    	FXString path=FXPath::directory(pathname);
    	FXString filename=FXPath::name(pathname);
        PropertiesBox* attrdlg=new PropertiesBox(this,filename,path);
        if(attrdlg->execute(PLACEMENT_SCREEN))
        {
            current->list->setDirectory(ROOTDIR);
            current->list->setDirectory(path);
            current->updatePath();
        }
        delete attrdlg;
	}

	// There is one selected file in the file list
	else if (num==1)
    {
		// Eventually deselect the '..' directory
		if (current->list->isItemSelected(0))
			current->list->deselectItem(0);

    	FXString path=current->list->getDirectory();
        FXString filename=current->list->getItemText(itm);
        filename=filename.section('\t',0);
		PropertiesBox* attrdlg=new PropertiesBox(this,filename,path);
        if(attrdlg->execute(PLACEMENT_SCREEN))
        {
            current->list->setDirectory(ROOTDIR);
            current->list->setDirectory(path);
            current->updatePath();
        }
        delete attrdlg;
    }

	// There are multiple selected files in the file list
	else if (num>1)
    {
		chdir(current->list->getDirectory().text());
    	FXString path=current->list->getDirectory();
        FXString *files=new FXString[num];

		// Eventually deselect the '..' directory
		if (current->list->isItemSelected(0))
			current->list->deselectItem(0);

        int i= 0;
        for (int u= 0; u< current->list->getNumItems (); u++)
		{
            if (current->list->isItemSelected(u))
            {
                files[i]=current->list->getItemText(u).section('\t',0);
                i++;
            }
		}
			
        PropertiesBox* attrdlg=new PropertiesBox(this,files,num,path);
        if(attrdlg->execute(PLACEMENT_SCREEN))
        {
            current->list->setDirectory(ROOTDIR);
            current->list->setDirectory(path);
            current->updatePath();
        }
        delete attrdlg;
        delete[]files;
		chdir(startlocation.text());
    }
	return 1;
}


// Create new directory
long FilePanel::onCmdNewDir(FXObject*,FXSelector,void*)
{
	FXString dirname="";
	
	// Focus on current panel list
	current->list->setFocus();

    FXString dirpath=current->list->getDirectory();
    if(dirpath!=ROOTDIR)
        dirpath+=PATHSEPSTRING;

    if (newdirdialog==NULL)
		newdirdialog=new InputDialog(this,"",_("Create new folder:"),_("New Folder"));		
	newdirdialog->setText("");

	// Accept was pressed
    if(newdirdialog->execute(PLACEMENT_CURSOR))
    {
		dirname=dirpath+newdirdialog->getText();
        if(dirname!=dirpath)
        {
			// Create the new dir according to the current umask
			int mask;
			mask=umask(0);
			umask(mask);
			
			// Note that the umask value is in decimal (511 means octal 0777)
			errno=0;
			FXint ret=::mkdir(dirname.text(),511 & ~mask);
			FXint errcode=errno;
			if (ret==-1)
			{
				if (errcode)
					MessageBox::error(this,BOX_OK_SU,_("Error"),"Can't create folder %s: %s",dirname.text(),strerror(errcode));
				else
					MessageBox::error(this,BOX_OK_SU,_("Error"),"Can't create folder %s",dirname.text());
    			return 0;
			}
		}							
    }
	
	// Cancel was pressed
	else
		return 0;

	// Force panel refresh
	onCmdRefresh(0,0,0);

	// Enable created item, if any (for keyboard navigation)
	FXString name;
	for (FXint u=0; u< current->list->getNumItems (); u++)
	{
		name=current->list->getItemPathname(u);
		if (name==dirname)
		{
			current->list->enableItem(u);
			current->list->setCurrentItem(u);
			break;
		}
	}

    return 1;
}


// Create new file
long FilePanel::onCmdNewFile(FXObject*,FXSelector,void*)
{
	FXString filename="";
	
	// Focus on current panel list
    current->list->setFocus();

    FXString pathname=current->list->getDirectory();
    if(pathname!=ROOTDIR)
        pathname+=PATHSEPSTRING;

    if (newfiledialog==NULL)
		newfiledialog=new InputDialog(this,"",_("Create new file:"),_("New File"),"",NULL,FALSE);
	newfiledialog->setText("");
    
	// Accept was pressed
	if(newfiledialog->execute(PLACEMENT_CURSOR))
    {	
		filename=pathname+newfiledialog->getText();
		FILE *file;
		if(filename!=pathname)
        {
			// Test some error conditions
			if (::exists(filename))
        	{
				MessageBox::error(this,BOX_OK,_("Error"),_("File or folder %s already exists"), filename.text());
        		return 0;
			}
			// Create the new file			
			errno=0;
			if (!(file=fopen(filename.text(),"w+"))  ||  fclose(file) )
			{
				if (errno)
					MessageBox::error(this,BOX_OK_SU,_("Error"),"Can't create file %s: %s",filename.text(),strerror(errno));
				else
					MessageBox::error(this,BOX_OK_SU,_("Error"),"Can't create file %s",filename.text());
    			return 0;
			}
			// Change the file permissions according to the current umask
			FXint mask;
			mask=umask(0);
			umask(mask);
			errno=0;
			FXint rc=chmod(filename.text(), 438 & ~mask);
			FXint errcode=errno;
			if (rc)
			{
				if (errcode)
					MessageBox::error(this,BOX_OK_SU,_("Error"),"Can't set permissions in %s: %s",filename.text(),strerror(errcode));
				else
					MessageBox::error(this,BOX_OK_SU,_("Error"),"Can't set permissions in %s",filename.text());
			}
		}							
    }
	
	// Cancel was pressed
	else
		return 0;
	
	// Force panel refresh
	onCmdRefresh(0,0,0);

	// Enable created item, if any (for keyboard navigation)
	FXString name;
	for (FXint u=0; u< current->list->getNumItems (); u++)
	{
		name=current->list->getItemPathname(u);
		if (name==filename)
		{
			current->list->enableItem(u);
			current->list->setCurrentItem(u);
			break;
		}
	}
	
	return 1;
}


// Create new symbolic link
long FilePanel::onCmdNewSymlink(FXObject*,FXSelector,void*)
{
	FXString linkname="";
	
	// Focus on current panel list
    current->list->setFocus();

    FXString linkpath=current->list->getDirectory();
    if(linkpath!=ROOTDIR)
        linkpath+=PATHSEPSTRING;

    if (newlinkdialog==NULL)
		newlinkdialog=new InputDialog(this,"",_("Create new symbolic link:"),_("New Symlink"),"",NULL,FALSE);
	newlinkdialog->setText("");

	// Accept was pressed
    if(newlinkdialog->execute(PLACEMENT_CURSOR))
    {
		if (newlinkdialog->getText()=="")
		{
			MessageBox::warning(this,BOX_OK,_("Warning"),_("File name is empty, operation cancelled"));
			return 0;
		}
		linkname=linkpath+newlinkdialog->getText();
		File *f;
		if(linkname!=linkpath)
        {
			// Test some error conditions
			if (::exists(linkname))
        	{
				MessageBox::error(this,BOX_OK,_("Error"),_("File or folder %s already exists"), linkname.text());
        		return 0;
			}
			
			// Select target
			FileDialog browse(this,_("Select the symlink refered file or folder"));
			browse.setDirectory(linkpath);
			browse.setSelectMode(SELECT_FILE_MIXED);
			if(browse.execute())
			{
				FXString linksource=browse.getFilename();
				
				// Source does not exist
				if (!::exists(linksource))
				{
					MessageBox::error(this,BOX_OK,_("Error"),_("Symlink source %s does not exist"), linksource.text());
					return 0;
				}
				
				f=new File(this,_("Symlink"),SYMLINK);
				f->create();
				f->symlink(linksource,linkname);
				delete f;
			}
			//else
				//return 0;
		}							
    }

	// Cancel was pressed
	else
		return 0;
	
	// Force panel refresh
	onCmdRefresh(0,0,0);

	// Enable created item, if any (for keyboard navigation)
	FXString name;
	for (FXint u=0; u< current->list->getNumItems (); u++)
	{
		name=current->list->getItemPathname(u);
		if (name==linkname)
		{
			current->list->enableItem(u);
			current->list->setCurrentItem(u);
			break;
		}
	}
	
	return 1;
}


// Open single or multiple files
long FilePanel::onCmdOpen(FXObject*,FXSelector,void*)
{	
	// Wait cursor
	getApp()->beginWaitCursor();

    FXString pathname, samecmd, cmd, cmdname, itemslist=" "; 
	FileAssoc* association;
	FXbool same=TRUE;
	FXbool first=TRUE;
	
	current->list->setFocus();
	if (current->list->getNumSelectedItems()==0)
	{
		getApp()->endWaitCursor();
		return 0;
	}
    
	// Update associations dictionary
	FileDict *assocdict=new FileDict(getApp());			
	
	// Check if all files have the same association
	for (FXint u=0; u< current->list->getNumItems (); u++)
	{
		if (current->list->isItemSelected(u))
		{
			// Increment number of selected items
			pathname=current->list->getItemPathname(u);
			association=assocdict->findFileBinding(pathname.text());
		
			if (association)
			{
				cmd = association->command.section(',',0);
				if (cmd != "")
				{
					// First selected item
					if (first)
					{
						samecmd = cmd;
						first=FALSE;
					}
					
					if (samecmd != cmd)
					{
						same=FALSE;
						break;
					}
					
					// List of selected items
					itemslist += ::quote(pathname) + " ";
				}
				else
				{
					same=FALSE;
					break;
				}
			}
			else
			{
				same=FALSE;
				break;
			}
		}
	}

	// Same command for all files : execute it
	if (same)
	{
		cmdname=samecmd;

		// If command exists, run it
		if (::existCommand(cmdname))
		{
			cmd=samecmd+itemslist;
			runcmd(cmd);
		}

		// If command does not exist, call the "Open with..." dialog
		else
		{
			getApp()->endWaitCursor();
			current->handle(this,FXSEL(SEL_COMMAND,ID_OPEN_WITH),NULL);
		}
	}
	
	// Files have different commands : handle them separately
	else
	{
		for (int u=0; u< current->list->getNumItems (); u++)
		{
			if (current->list->isItemSelected(u))
			{
				pathname=current->list->getItemPathname(u);
				association=assocdict->findFileBinding(pathname.text());
								
				// Shell script
				//if (mimetype(pathname) == "text/x-shellscript")
				   //current->handle(this,FXSEL(SEL_COMMAND,ID_EDIT),NULL);
				
				// If there is an association
				//else if(association)
				if(association)
				{
					// Use it to open the file
					cmd = association->command.section(',',0);
					if(cmd != "")
					{
						cmdname=cmd;

						// If command exists, run it
						if (::existCommand(cmdname))
						{
							cmd=cmdname+" "+::quote(pathname);
							runcmd(cmd);
						}

						// If command does not exist, call the "Open with..." dialog
						else
						{
							getApp()->endWaitCursor();
							current->handle(this,FXSEL(SEL_COMMAND,ID_OPEN_WITH),NULL);
						}
					}
					
					// or execute it
					else if(current->list->isItemExecutable(u))
					{
						cmdname=FXPath::name(pathname);
						cmd=::quote(pathname);
						runcmd(cmd);
					}
					
					// or call the "Open with..." dialog
					else
					{
						getApp()->endWaitCursor();
						current->handle(this,FXSEL(SEL_COMMAND,ID_OPEN_WITH),NULL);
					}
				}
				
				// If no association but executable
				else if(current->list->isItemExecutable(u))
				{
					cmdname=FXPath::name(pathname);
					cmd=::quote(pathname);
					runcmd(cmd);
				}
				
				// Other cases
				else
				{
					getApp()->endWaitCursor();
					current->handle(this,FXSEL(SEL_COMMAND,ID_OPEN_WITH),NULL);
				}
			}
		}
	}
	
	getApp()->endWaitCursor();

    return 1;
}

// Open with
long FilePanel::onCmdOpenWith(FXObject*,FXSelector,void*)
{	
	char **str=NULL;
    current->list->setFocus();

	if (current->list->getNumSelectedItems()==0)
		return 0;

    FXString cmd="", cmdname;
	if (opendialog==NULL)
		opendialog=new HistInputDialog(this,"",_("Open selected file(s) with:"),_("Open With"), "", NULL, HIST_INPUT_EXECUTABLE_FILE, TRUE, _("A&ssociate"));
	opendialog->setText(cmd);
    
	// Dialog with history list and associate checkbox
    opendialog->CursorEnd();
    opendialog->SelectAll();
    opendialog->clearItems();
	for(FXint i=0;i<OpenNum;i++)
        opendialog->appendItem(OpenHistory[i]);
	opendialog->setDirectory(ROOTDIR);
    if(opendialog->execute())
    {
        cmd=opendialog->getText();
		if (cmd=="")
		{			
			MessageBox::warning(this,BOX_OK,_("Warning"),_("File name is empty, operation cancelled"));
			return 0;
		}
		
        for (FXint u= 0; u< current->list->getNumItems (); u++)
		{
            if (current->list->isItemSelected(u))
            {
                // Handles "associate" checkbox for "open with..." dialog
                if (opendialog->getOption())
                {
                    FXString filename=current->list->getItemFilename(u);
					FXString ext=FXPath::extension(filename).text();
                    if (ext == "")
						ext=FXPath::name(filename);

					// Convert extension to lower case
					ext=ext.lower();

                    FileAssoc* association=current->list->getItemAssoc(u);

                    if (association)
                    {
                        // Update existing association
                        FXString oldfileassoc=getApp()->reg().readStringEntry("FILETYPES", ext.text(),"");
						oldfileassoc.erase(0, oldfileassoc.section(';',0).section(',',0).length());
                        oldfileassoc.prepend(opendialog->getText());
                        getApp()->reg().writeStringEntry("FILETYPES", ext.text(), oldfileassoc.text());

						// Handle file association
						str=new char*[2];
						str[0]=new char[strlen(ext.text())+1];
						str[1]=new char[strlen(oldfileassoc.text())+1];
						strlcpy(str[0],ext.text(),ext.length()+1);
						strlcpy(str[1],oldfileassoc.text(),oldfileassoc.length()+1);
						mainWindow->handle(this, FXSEL(SEL_COMMAND,XFileExplorer::ID_FILE_ASSOC), str);
                    }
                    else
                    {
                        // New association
						FXString newcmd=opendialog->getText().append(";Document;;;;");
						getApp()->reg().writeStringEntry("FILETYPES", ext.text(), newcmd.text());
                  
						// Handle file association
						str=new char*[2];
						str[0]=new char[strlen(ext.text())+1];
						str[1]=new char[strlen(newcmd.text())+1];
						strlcpy(str[0],ext.text(),ext.length()+1);
						strlcpy(str[1],newcmd.text(),newcmd.length()+1);
						mainWindow->handle(this, FXSEL(SEL_COMMAND,XFileExplorer::ID_FILE_ASSOC),str);
                    }
                }
                // End
				
				FXString pathname=current->list->getItemPathname(u);
				cmdname=cmd;
				cmd+=" ";
                cmd=cmd+::quote(pathname);
            }
		}
		
		// Run command if it exists
		getApp()->beginWaitCursor();

		// If command exists, run it
		if (::existCommand(cmdname))
			runcmd(cmd);

		// If command does not exist, call the "Open with..." dialog
		else
		{
			getApp()->endWaitCursor();
			current->handle(this,FXSEL(SEL_COMMAND,ID_OPEN_WITH),NULL);
			return 1;
		}

		// Update history list
		OpenNum=opendialog->getHistorySize();
		cmd=opendialog->getText();
		
		// Check if cmd is a new string, i.e. is not already in history
		FXbool newstr=TRUE;
		for (FXint i=0;i<OpenNum-1;i++)
		{
			if (streq(OpenHistory[i],cmd.text()))
			{
				newstr=FALSE;
				break;
			}
		}
		
		// No new string or history limit reached
		if (!newstr || OpenNum>OPEN_HIST_SIZE)
			OpenNum--;
		
		// New string
		if (newstr)
		{
			// FIFO
			strlcpy(OpenHistory[0],cmd.text(),cmd.length()+1);
			for(FXint i=1;i<OpenNum;i++)
				strlcpy(OpenHistory[i],opendialog->getHistoryItem(i-1).text(),opendialog->getHistoryItem(i-1).length()+1);
		}

		getApp()->endWaitCursor();
    }
    	
    return 1;
}


long FilePanel::onCmdItemFilter(FXObject* o, FXSelector sel, void*)
{
	if (FilterNum==0)
	{
		strlcpy(FilterHistory[FilterNum],"*",2);
		FilterNum++;
	}
	
    FXint i;
    FXString pat=list->getPattern();
	if (filterdialog==NULL)
		filterdialog=new HistInputDialog(this,pat,_("Show files:"),_("Filter"),"",NULL,HIST_INPUT_FILE);
    filterdialog->CursorEnd();
    filterdialog->SelectAll();
	filterdialog->clearItems();
	
	for(FXint i=0; i<FilterNum; i++)
        filterdialog->appendItem(FilterHistory[i]);

    if(filterdialog->execute() && (pat=filterdialog->getText()) != "")
    {
		// Change file list patten
		if (FXSELID(sel)==ID_FILTER_CURRENT)
			current->list->setPattern(pat);
		else
			list->setPattern(pat);
		
		FXbool newstr=TRUE;
        for(i=0; i<FilterNum; i++)
		{
            if(streq(FilterHistory[i],pat.text()))
			{
                newstr=FALSE;
				break;
			}
		}
		// Append new string to the list bottom 		
		if(newstr && FilterNum<FILTER_HIST_SIZE)
        {
			strlcpy(FilterHistory[FilterNum],pat.text(),pat.length()+1);
			FilterNum++;
        }
    }
    return 1;
}


// Panel context menu
long FilePanel::onCmdPopupMenu(FXObject* o,FXSelector s,void* p)
{
    // Make panel active
	setActive();

	// Check if control key or Shift-F10 was pressed
	if (p!=NULL)
	{
		FXEvent* event=(FXEvent*)p;
		if (event->state&CONTROLMASK)
			ctrlflag=TRUE;
		if (event->state&SHIFTMASK && event->code==KEY_F10)
			shiftf10=TRUE;
	}
	
	// Use to select the item under cursor when right clicking
	// Only when Shift-F10 was not pressed
    if(!shiftf10 && list->getNumSelectedItems()<=1)
    {
        FXint x,y;
        FXuint state;
        list->getCursorPosition(x,y,state);
 
        int item=list->getItemAt(x,y);

        if (list->getCurrentItem ()>= 0)
            list->deselectItem(list->getCurrentItem());
        if (item>= 0)
        {
            list->setCurrentItem(item);
            if(!list->selectItem(item));            
        }
    }
    
	// If first item (i.e. the '..' item)
	if (list->getNumSelectedItems()==1 && list->isItemSelected(0))
		ctrlflag=TRUE;
	
	// If control flag is set, deselect all items
	if(ctrlflag)
		list->handle(o,FXSEL(SEL_COMMAND,FileList::ID_DESELECT_ALL),p);
 
    // Popup menu pane
	FXMenuPane* menu = new FXMenuPane(this);
    FXint x,y;
    FXuint state;
    getRoot()->getCursorPosition(x,y,state);

    FXint num, itm;	
	num=current->list->getNumSelectedItems(&itm);

    // No selection or control flag set
	if(num == 0 || current->ctrlflag)
    {
		// Reset the control flag
		ctrlflag=FALSE;
		
		// Menu items
    	new FXMenuCommand(menu,_("New& file..."),newfileicon,current,FilePanel::ID_NEW_FILE);
    	new FXMenuCommand(menu,_("New f&older..."),newfoldericon,current,FilePanel::ID_NEW_DIR);
    	new FXMenuCommand(menu,_("New s&ymlink..."),newlinkicon,current,FilePanel::ID_NEW_SYMLINK);
        new FXMenuCommand(menu,_("Fi&lter..."),filtericon,current,FilePanel::ID_FILTER);
        new FXMenuSeparator(menu);
		new FXMenuCommand(menu,_("&Paste"),paste_clpicon,current,FilePanel::ID_PASTE_CLIPBOARD);
        new FXMenuSeparator(menu);
        new FXMenuCheck(menu,_("&Hidden files"),current->list,FileList::ID_TOGGLE_HIDDEN);
        new FXMenuCheck(menu,_("Thum&bnails"),current->list,FileList::ID_TOGGLE_THUMBNAILS);
        new FXMenuSeparator(menu);
        new FXMenuRadio(menu,_("B&ig icons"),current->list,IconList::ID_SHOW_BIG_ICONS);
        new FXMenuRadio(menu,_("&Small icons"),current->list,IconList::ID_SHOW_MINI_ICONS);
        new FXMenuRadio(menu,_("&Full file list"),current->list,IconList::ID_SHOW_DETAILS);
        new FXMenuSeparator(menu);
        new FXMenuRadio(menu,_("&Rows"),current->list,FileList::ID_ARRANGE_BY_ROWS);
        new FXMenuRadio(menu,_("&Columns"),current->list,FileList::ID_ARRANGE_BY_COLUMNS);
        new FXMenuCheck(menu,_("Autosize"),current->list,FileList::ID_AUTOSIZE);
        new FXMenuSeparator(menu);
        new FXMenuRadio(menu,_("&Name"),current->list,FileList::ID_SORT_BY_NAME);
        new FXMenuRadio(menu,_("Si&ze"),current->list,FileList::ID_SORT_BY_SIZE);
        new FXMenuRadio(menu,_("&Type"),current->list,FileList::ID_SORT_BY_TYPE);
        new FXMenuRadio(menu,_("E&xtension"),current->list,FileList::ID_SORT_BY_EXT);
        new FXMenuRadio(menu,_("&Date"),current->list,FileList::ID_SORT_BY_TIME);
        new FXMenuRadio(menu,_("&User"),current->list,FileList::ID_SORT_BY_USER);
        new FXMenuRadio(menu,_("&Group"),current->list,FileList::ID_SORT_BY_GROUP);
        new FXMenuRadio(menu,_("Per&missions"),current->list,FileList::ID_SORT_BY_PERM);
        new FXMenuRadio(menu,_("Deletion date"),current->list,FileList::ID_SORT_BY_DELTIME);
        new FXMenuSeparator(menu);
  		new FXMenuCheck(menu,_("Ignore c&ase"),current->list,FileList::ID_SORT_CASE);
   		new FXMenuCheck(menu,_("Dir&ectories first"),current->list,FileList::ID_DIRS_FIRST);
   		new FXMenuCheck(menu,_("Re&verse order"),current->list,FileList::ID_SORT_REVERSE);
   }
    // Non empty selection
	else
    {
		// Deselect the '..' item
		if (current->list->isItemSelected(0))
			current->list->deselectItem(0);

		// Panel submenu items
        FXMenuPane* submenu=new FXMenuPane(this);
    	new FXMenuCommand(submenu,_("Ne&w file..."),newfileicon,current,FilePanel::ID_NEW_FILE);
    	new FXMenuCommand(submenu,_("New f&older..."),newfoldericon,current,FilePanel::ID_NEW_DIR);
    	new FXMenuCommand(submenu,_("New s&ymlink..."),newlinkicon,current,FilePanel::ID_NEW_SYMLINK);
        new FXMenuCommand(submenu,_("Fi&lter..."),filtericon,current,FilePanel::ID_FILTER);
        new FXMenuSeparator(submenu);
		new FXMenuCommand(submenu,_("&Paste"),paste_clpicon,current,FilePanel::ID_PASTE_CLIPBOARD);
        new FXMenuSeparator(submenu);
        new FXMenuCheck(submenu,_("&Hidden files"),current->list,FileList::ID_TOGGLE_HIDDEN);
        new FXMenuCheck(submenu,_("Thum&bnails"),current->list,FileList::ID_TOGGLE_THUMBNAILS);
        new FXMenuSeparator(submenu);
        new FXMenuRadio(submenu,_("B&ig icons"),current->list,IconList::ID_SHOW_BIG_ICONS);
        new FXMenuRadio(submenu,_("&Small icons"),current->list,IconList::ID_SHOW_MINI_ICONS);
        new FXMenuRadio(submenu,_("&Full file list"),current->list,IconList::ID_SHOW_DETAILS);
        new FXMenuSeparator(submenu);
        new FXMenuRadio(submenu,_("&Rows"),current->list,FileList::ID_ARRANGE_BY_ROWS);
        new FXMenuRadio(submenu,_("&Columns"),current->list,FileList::ID_ARRANGE_BY_COLUMNS);
        new FXMenuCheck(submenu,_("Autosize"),current->list,FileList::ID_AUTOSIZE);
        new FXMenuSeparator(submenu);
        new FXMenuRadio(submenu,_("&Name"),current->list,FileList::ID_SORT_BY_NAME);
        new FXMenuRadio(submenu,_("Si&ze"),current->list,FileList::ID_SORT_BY_SIZE);
        new FXMenuRadio(submenu,_("&Type"),current->list,FileList::ID_SORT_BY_TYPE);
        new FXMenuRadio(submenu,_("E&xtension"),current->list,FileList::ID_SORT_BY_EXT);
        new FXMenuRadio(submenu,_("&Date"),current->list,FileList::ID_SORT_BY_TIME);
        new FXMenuRadio(submenu,_("&User"),current->list,FileList::ID_SORT_BY_USER);
        new FXMenuRadio(submenu,_("&Group"),current->list,FileList::ID_SORT_BY_GROUP);
        new FXMenuRadio(submenu,_("Per&missions"),current->list,FileList::ID_SORT_BY_PERM);
        new FXMenuRadio(submenu,_("Deletion date"),current->list,FileList::ID_SORT_BY_DELTIME);
        new FXMenuSeparator(submenu);
  		new FXMenuCheck(submenu,_("Ignore c&ase"),current->list,FileList::ID_SORT_CASE);
   		new FXMenuCheck(submenu,_("Dir&ectories first"),current->list,FileList::ID_DIRS_FIRST);
   		new FXMenuCheck(submenu,_("Re&verse order"),current->list,FileList::ID_SORT_REVERSE);
		new FXMenuCascade(menu,_("Pane&l"),NULL,submenu);
        new FXMenuSeparator(menu);
		
#if defined(linux)
		FXString name=current->list->getItemPathname(itm);
    	if(num==1 && (fsdevices->find(name.text()) || mtdevices->find(name.text()) ) )
		{
			new FXMenuCommand(menu,_("&Mount"),maphosticon,current,FilePanel::ID_MOUNT);
			new FXMenuCommand(menu,_("Unmount"),unmaphosticon,current,FilePanel::ID_UMOUNT);
    		new FXMenuSeparator(menu);
		}
#endif

        FXbool ar=FALSE;
        if(current->list->getItem (itm) && current->list->isItemFile(itm))
        {
			new FXMenuCommand(menu,_("Open &with..."),fileopenicon,current,FilePanel::ID_OPEN_WITH);
            new FXMenuCommand(menu,_("&Open"),fileopenicon,current,FilePanel::ID_OPEN);
			FXString name=current->list->getItemText(itm).section('\t',0);
			
			// Last and before last file extensions
			FXString ext1=name.rafter('.',1);
			FXString ext2=name.rafter('.',2);
			
			// Convert these extensions to lower case
			ext1.lower();			
			ext2.lower();

			// Destination folder name
			FXString extract_to_folder;
			if (ext2=="tar.gz" || ext2=="tar.bz2" || ext2=="tar.z")
				extract_to_folder=_("Extr&act to folder ")+name.section('\t',0).rbefore('.',2);
			else
				extract_to_folder=_("Extr&act to folder ")+name.section('\t',0).rbefore('.',1);

			// Display the extract and package menus according to the archive extensions
			if((num==1) && (ext2=="tar.gz" || ext2=="tar.bz2"))
			{
				ar=TRUE;
				new FXMenuCommand(menu,_("&Extract here"),archexticon,current,FilePanel::ID_EXTRACT_HERE);
				new FXMenuCommand(menu,extract_to_folder,archexticon,current,FilePanel::ID_EXTRACT_TO_FOLDER);
				new FXMenuCommand(menu,_("E&xtract to..."),archexticon,current,FilePanel::ID_EXTRACT);
			}
			else if ((num==1) && (ext1=="gz" || ext1=="bz2" || ext1=="z"))
			{
				ar=TRUE;
				new FXMenuCommand(menu,_("&Extract here"),archexticon,current,FilePanel::ID_EXTRACT_HERE);
			}
			else if((num==1) && (ext1=="tar" || ext1=="tgz" || ext1=="tbz2" || ext1=="zip" || ext1=="7z" || ext1=="lzh" || ext1=="rar" || ext1=="ace" || ext1=="arj"))
			{
				ar=TRUE;
				new FXMenuCommand(menu,_("&Extract here"),archexticon,current,FilePanel::ID_EXTRACT_HERE);
				new FXMenuCommand(menu,extract_to_folder,archexticon,current,FilePanel::ID_EXTRACT_TO_FOLDER);
				new FXMenuCommand(menu,_("E&xtract to..."),archexticon,current,FilePanel::ID_EXTRACT);
			}
			// Not archive nor package
			if(!ar)
			{
				new FXMenuCommand(menu,_("&View"),viewicon,current,FilePanel::ID_VIEW);
				new FXMenuCommand(menu,_("&Edit"),editicon,current,FilePanel::ID_EDIT);
			}
        }
        if(!ar)
            new FXMenuCommand(menu,_("&Add to archive..."),archaddicon,current,FilePanel::ID_ADD_TO_ARCH);
        new FXMenuSeparator(menu);
        new FXMenuCommand(menu,_("&Copy"),copy_clpicon,current,FilePanel::ID_COPY_CLIPBOARD);
        new FXMenuCommand(menu,_("C&ut"),cut_clpicon,current,FilePanel::ID_CUT_CLIPBOARD);
		new FXMenuCommand(menu,_("&Paste"),paste_clpicon,current,FilePanel::ID_PASTE_CLIPBOARD);
        new FXMenuSeparator(menu);
        new FXMenuCommand(menu,_("Re&name..."),renameiticon,current,FilePanel::ID_FILE_RENAME);
        new FXMenuCommand(menu,_("Copy &to..."),copy_clpicon,current,FilePanel::ID_FILE_COPYTO);
        new FXMenuCommand(menu,_("&Move to..."),moveiticon,current,FilePanel::ID_FILE_MOVETO);
        new FXMenuCommand(menu,_("Symlin&k to..."),minilinkicon,current,FilePanel::ID_FILE_SYMLINK);
        new FXMenuCommand(menu,_("Mo&ve to trash"),filedeleteicon,current,FilePanel::ID_FILE_TRASH);
        new FXMenuCommand(menu,_("Re&store from trash"),filerestoreicon,current,FilePanel::ID_FILE_RESTORE);
        new FXMenuCommand(menu,_("&Delete"),filedelete_permicon,current,FilePanel::ID_FILE_DELETE);
        new FXMenuSeparator(menu);
        new FXMenuCommand(menu,_("P&roperties..."),attribicon,current,FilePanel::ID_PROPERTIES);
	}
    menu->create();
    allowPopupScroll=TRUE;  // Allow keyboard scrolling
	
	menu->popup(NULL,x,y);
	getApp()->runModalWhileShown(menu);
    allowPopupScroll=FALSE;
	
    return 1;
}


// Run Terminal in the selected directory
long  FilePanel::onCmdXTerm(FXObject*,FXSelector,void*)
{
	getApp()->beginWaitCursor();
    chdir(current->list->getDirectory().text());
    FXString cmd=getApp()->reg().readStringEntry("PROGS","xterm","xterm -sb");
    cmd += " &";
    system(cmd.text());
    current->list->setFocus();
	chdir(startlocation.text());
	getApp()->endWaitCursor();
    return 1;
}


// Add files or directory to an archive
long FilePanel::onCmdAddToArch(FXObject* o,FXSelector,void*)
{
    FXString name, ext1, ext2, cmd, archive="";
	File *f;
	
	chdir(current->list->getDirectory().text());

  	// Eventually deselect the '..' directory
	if (current->list->isItemSelected(0))
		current->list->deselectItem(0);

	// Return if nothing is selected
	if(current->list->getNumSelectedItems()==0)
		return 0;
	
	// If only one item is selected, use its name as a starting guess for the archive name
	if (current->list->getNumSelectedItems()==1)
	{
		for (int u= 0; u< current->list->getNumItems(); u++)
			if (current->list->isItemSelected(u))
			{
				name=list->getItemFilename(u);
				break;
			}
		archive=name;
	}
	
    // Initial archive name with full path and default extension
	FXString archpath=current->list->getDirectory();	
	archive = archpath+PATHSEPSTRING+archive+".tar.gz";

	// Archive dialog
	if (archdialog==NULL)
		archdialog=new ArchInputDialog(this,"");	
	archdialog->setText(archive);
	archdialog->CursorEnd();

    if(archdialog->execute())
    {
        if (archdialog->getText()=="")
		{
			MessageBox::warning(this,BOX_OK,_("Warning"),_("File name is empty, operation cancelled"));
			return 0;
		}
		
		// Get string and preserve escape characters
		archive=::quote(archdialog->getText());
		
        // Get extensions of the archive name
        ext1=archdialog->getText().rafter('.',1);
		ext1.lower();
        ext2=archdialog->getText().rafter('.',2);
		ext2.lower();
		
		// Handle different archive formats
		if (ext2=="tar.gz")
            cmd="tar -zcvf "+archive+" ";
		else if (ext2=="tar.bz2")
            cmd="tar -jcvf "+archive+" ";
		else if (ext2=="tar.z")
           	cmd="tar -Zcvf "+archive+" ";
		else if (ext1=="tar")
           	cmd="tar -cvf "+archive+" ";
		else if (ext1=="gz")
			cmd="gzip -v ";				
		else if (ext1=="tgz")
            cmd="tar -zcvf "+archive+" ";
		else if (ext1=="bz2")
			cmd="bzip2 -v ";				
		else if (ext1=="tbz2")
            cmd="tar -jcvf "+archive+" ";
		else if (ext1=="z")
            cmd="compress -v ";
		else if (ext1=="zip")
            cmd="zip -r "+archive+" ";
		else if (ext1=="7z")
            cmd="7z a "+archive+" ";
		else if (ext1=="rar")
            cmd="rar a "+archive+" ";
		else if (ext1=="lzh")
            cmd="lha a "+archive+" ";
		else if (ext1=="arj")
            cmd="arj a -r "+archive+" ";
		
		// Default archive format
		else
		{
			archive += ".tar.gz";
			cmd="tar -zcvf "+archive+" ";
		}

        for (int u= 0; u< current->list->getNumItems(); u++)
		{
            if (current->list->isItemSelected(u))
            {
				// Don't include '..' in the list
				name=list->getItemFilename(u);				
				if (name!="..")
				{
                	cmd+=" ";
                	cmd=cmd+::quote(name);
                	cmd+=" ";
				}
            }
		}
		
		// Wait cursor
		getApp()->beginWaitCursor();

		// File object
		f=new File(this,_("Create archive"),ARCHIVE);
		f->create();

		// Create archive
		f->archive(archive,cmd);
		chdir(startlocation.text());
 
		getApp()->endWaitCursor();
		delete f; 

		// Force panel refresh
    	onCmdRefresh(0,0,0);
    }
    return 1;
}


// Extract archive
long FilePanel::onCmdExtract(FXObject*,FXSelector,void*)
{
    FXString name, ext1, ext2, cmd, dir, cdir;
	File *f;

    // Current directory
	cdir=current->list->getDirectory();
    
    // File selection dialog
    FileDialog browse(this,_("Select a destination folder"));
	const FXchar *patterns[]=
	{
		_("All Files"),          "*",	NULL
	};
	browse.setDirectory(homelocation);
	browse.setPatternList(patterns);
	browse.setSelectMode(SELECT_FILE_DIRECTORY);

	FXint num, item;	
	num=current->list->getNumSelectedItems(&item);
    if (current->list->getItem(item))
	{
		// Archive name and extensions
		name=current->list->getItemText(item).text();

		ext1=name.section('\t',0).rafter('.',1);
		ext2=name.section('\t',0).rafter('.',2);
		ext1.lower();
		ext2.lower();
		name=::quote(cdir + PATHSEPSTRING + name.section('\t',0));

		// Handle different archive formats
		if (ext2=="tar.gz")
			cmd="tar -zxvf ";
		else if (ext2=="tar.bz2")
			cmd="tar -jxvf ";
		else if (ext2=="tar.z")
			cmd="tar -Zxvf ";
		else if (ext1=="tar")
			cmd="tar -xvf ";
		else if (ext1=="gz")
			cmd="gunzip -v ";				
		else if (ext1=="tgz")
			cmd="tar -zxvf ";
		else if (ext1=="bz2")
			cmd="bunzip2 -v ";				
		else if (ext1=="tbz2")
			cmd="tar -jxvf ";
		else if (ext1=="z")
			cmd="uncompress -v ";
		else if (ext1=="zip")
			cmd="unzip -o ";
		else if (ext1=="7z")
			cmd="7z x -y ";
		else if (ext1=="rar")
			cmd="unrar x -o+ ";
		else if (ext1=="lzh")
			cmd="lha -xf ";
		else if (ext1=="ace")
            cmd="unace x ";
		else if (ext1=="arj")
            cmd="arj x -y ";
		else
			cmd="tar -zxvf ";

		// Final extract command
		cmd+=name+" ";
				

        // Extract archive
		if(browse.execute())
        {
			dir=browse.getFilename();
			
            if(isWritable(dir))
            {                
				// Wait cursor
				getApp()->beginWaitCursor();
				
				// File object
   				f=new File(this,_("Extract archive"),EXTRACT);
   				f->create();
				   
				// Extract archive
				f->extract(name,dir,cmd);
 
				getApp()->endWaitCursor();
				delete f; 
            }
            else
                MessageBox::error(this,BOX_OK_SU,_("Error"),_("Can't write to %s: Permission denied"),dir.text());
        }
	}
	
	// Force panel refresh
	onCmdRefresh(0,0,0);
	
    return 1;
}


// Extract archive to a folder name based on the archive name
long FilePanel::onCmdExtractToFolder(FXObject*,FXSelector,void*)
{
    FXString name, pathname, ext1, ext2, cmd, dirname, dirpath, cdir;
	File *f;

    // Current directory
	cdir=current->list->getDirectory();

	FXint num, item;	
	num=current->list->getNumSelectedItems(&item);
    if (current->list->getItem(item))
	{
		// Archive name and extensions
		name=current->list->getItemText(item).text();
		ext1=name.section('\t',0).rafter('.',1);
		ext2=name.section('\t',0).rafter('.',2);
		ext1.lower();
		ext2.lower();

		// Destination folder name
		if (ext2=="tar.gz" || ext2=="tar.bz2" || ext2=="tar.z")
			dirname=name.section('\t',0).rbefore('.',2);
		else
			dirname=name.section('\t',0).rbefore('.',1);
		
		// Create the new dir according to the current umask
		// Don't complain if directory already exists 
		FXint mask=umask(0);
		umask(mask);
		dirpath=cdir + PATHSEPSTRING + dirname;
		errno=0;
		FXint ret=::mkdir(dirpath.text(),511 & ~mask);
		FXint errcode=errno;
		if (ret==-1 && errcode!=EEXIST)
		{
			if (errcode)
				MessageBox::error(this,BOX_OK_SU,_("Error"),"Can't create folder %s: %s",dirpath.text(),strerror(errcode));
			else
				MessageBox::error(this,BOX_OK_SU,_("Error"),"Can't create folder %s",dirpath.text());
			return 0;
		}
			
		// Archive pathname
		pathname=::quote(cdir + PATHSEPSTRING + name.section('\t',0));
		
		// Handle different archive formats
		if (ext2=="tar.gz")
			cmd="tar -zxvf ";
		else if (ext2=="tar.bz2")
			cmd="tar -jxvf ";
		else if (ext2=="tar.z")
			cmd="tar -Zxvf ";
		else if (ext1=="tar")
			cmd="tar -xvf ";
		else if (ext1=="gz")
			cmd="gunzip -v ";				
		else if (ext1=="tgz")
			cmd="tar -zxvf ";
		else if (ext1=="bz2")
			cmd="bunzip2 -v ";				
		else if (ext1=="tbz2")
			cmd="tar -jxvf ";
		else if (ext1=="z")
			cmd="uncompress -v ";
		else if (ext1=="zip")
			cmd="unzip -o ";
		else if (ext1=="7z")
			cmd="7z x -y ";
		else if (ext1=="rar")
			cmd="unrar x -o+ ";
		else if (ext1=="lzh")
			cmd="lha -xf ";
		else if (ext1=="ace")
            cmd="unace x ";
		else if (ext1=="arj")
            cmd="arj x -y ";
		else
			cmd="tar -zxvf ";

		// Final extract command
		cmd+=pathname+" ";
									
		// Wait cursor
		getApp()->beginWaitCursor();
			
		// File object
		f=new File(this,_("Extract archive"),EXTRACT);
		f->create();
		   
		// Extract archive
		f->extract(pathname,dirpath,cmd);

		getApp()->endWaitCursor();
		delete f; 
	}
	
	// Force panel refresh
	onCmdRefresh(0,0,0);
	
    return 1;
}


// Extract archive in the current directory
long FilePanel::onCmdExtractHere(FXObject*,FXSelector,void*)
{
    FXString name, ext1, ext2, cmd, cdir;
	File *f;

    // Current directory
	cdir=current->list->getDirectory();

    FXint num, item;	
	num=current->list->getNumSelectedItems(&item);
    if (current->list->getItem (item))
	{
		if(isWritable(cdir))
		{
			// Archive name and extensions
			name=current->list->getItemText(item).text();
        	ext1=name.section('\t',0).rafter('.',1);
         	ext2=name.section('\t',0).rafter('.',2);
			ext1.lower();
			ext2.lower();
			name=::quote(cdir + PATHSEPSTRING + name.section('\t',0));

			// Handle different archive formats
			if (ext2=="tar.gz")
            	cmd="tar -zxvf ";
			else if (ext2=="tar.bz2")
            	cmd="tar -jxvf ";
			else if (ext2=="tar.z")
           		cmd="tar -Zxvf ";
			else if (ext1=="tar")
           		cmd="tar -xvf ";
			else if (ext1=="gz")
				cmd="gunzip -v ";				
			else if (ext1=="tgz")
            	cmd="tar -zxvf ";
			else if (ext1=="bz2")
				cmd="bunzip2 -v ";				
			else if (ext1=="tbz2")
            	cmd="tar -jxvf ";
			else if (ext1=="z")
            	cmd="uncompress -v ";
			else if (ext1=="zip")
            	cmd="unzip -o ";
			else if (ext1=="7z")
            	cmd="7z x -y ";
			else if (ext1=="rar")
            	cmd="unrar x -o+ ";
			else if (ext1=="lzh")
            	cmd="lha -xf ";
			else if (ext1=="ace")
				cmd="unace x ";
			else if (ext1=="arj")
				cmd="arj x -y ";
			else
				cmd="tar -zxvf ";

        	// Final extract command
			cmd+=name+" ";

			// Wait cursor
			getApp()->beginWaitCursor();
					
			// File object
   			f=new File(this,_("Extract archive"),EXTRACT);
   			f->create();
				   
			// Extract archive
			f->extract(name,cdir,cmd);
 
			getApp()->endWaitCursor();
			delete f; 
		}
		else
			MessageBox::error(this,BOX_OK_SU,_("Error"),_("Can't write to %s: Permission denied"),cdir.text());
	}
	
	// Force panel refresh
	onCmdRefresh(0,0,0);
	
    return 1;
}

// Force FilePanel and DirPanel refresh
long FilePanel::onCmdRefresh(FXObject*,FXSelector,void*)
{	
	current->list->setFocus();
    FXString dir=current->list->getDirectory();
    current->list->setDirectory(ROOTDIR);
    current->list->setDirectory(dir);    
	current->updatePath();
    return 1;
}


// Handle item selection
long FilePanel::onCmdSelect(FXObject* sender,FXSelector sel,void* ptr)
{
	current->list->setFocus();
    switch(FXSELID(sel))
    {
    case ID_SELECT_ALL:
        current->list->handle(sender,FXSEL(SEL_COMMAND,FileList::ID_SELECT_ALL),ptr);
        return 1;
    case ID_DESELECT_ALL:
        current->list->handle(sender,FXSEL(SEL_COMMAND,FileList::ID_DESELECT_ALL),ptr);
        return 1;
    case ID_SELECT_INVERSE:
        current->list->handle(sender,FXSEL(SEL_COMMAND,FileList::ID_SELECT_INVERSE),ptr);
        return 1;
    }
    return 1;
}



// Handle show commands
long FilePanel::onCmdShow(FXObject* sender,FXSelector sel,void* ptr)
{
    switch(FXSELID(sel))
    {
    case ID_SHOW_BIG_ICONS:
        current->list->handle(sender,FXSEL(SEL_COMMAND,FileList::ID_SHOW_BIG_ICONS),ptr);
        break;
    case ID_SHOW_MINI_ICONS:
        current->list->handle(sender,FXSEL(SEL_COMMAND,FileList::ID_SHOW_MINI_ICONS),ptr);
        break;
    case ID_SHOW_DETAILS:
        current->list->handle(sender,FXSEL(SEL_COMMAND,FileList::ID_SHOW_DETAILS),ptr);
        break;
    }
	
	// Set focus on current panel list
	current->list->setFocus();

    return 1;
}

// Update show commands
long FilePanel::onUpdShow(FXObject* sender,FXSelector sel,void* ptr)
{	
    FXuint msg=FXWindow::ID_UNCHECK;
    FXuint style=current->list->getListStyle();
		
    switch(FXSELID(sel))
    {
    case ID_SHOW_BIG_ICONS:
        if(style & _ICONLIST_BIG_ICONS)
            msg=FXWindow::ID_CHECK;
        break;
    case ID_SHOW_MINI_ICONS:
        if(style & _ICONLIST_MINI_ICONS)
            msg=FXWindow::ID_CHECK;
        break;
    case ID_SHOW_DETAILS:
       if(!(style & (_ICONLIST_MINI_ICONS | _ICONLIST_BIG_ICONS)))
            msg=FXWindow::ID_CHECK;
        break;
    }
    sender->handle(this,FXSEL(SEL_COMMAND,msg),ptr);

    return 1;
}

// Handle toggle hidden command
long FilePanel::onCmdToggleHidden(FXObject* sender,FXSelector sel,void* ptr)
{
	current->list->handle(sender,FXSEL(SEL_COMMAND,FileList::ID_TOGGLE_HIDDEN),ptr);
    return 1;
}

// Update toggle hidden command
long FilePanel::onUpdToggleHidden(FXObject* sender,FXSelector sel,void* ptr)
{	
    FXuint msg=FXWindow::ID_UNCHECK;
	FXbool hidden=current->list->shownHiddenFiles();
		
	if(hidden == FALSE)
		msg=FXWindow::ID_CHECK;
    sender->handle(this,FXSEL(SEL_COMMAND,msg),ptr);
    return 1;
}

// Handle toggle thumbnails command
long FilePanel::onCmdToggleThumbnails(FXObject* sender,FXSelector sel,void* ptr)
{
	current->list->handle(sender,FXSEL(SEL_COMMAND,FileList::ID_TOGGLE_THUMBNAILS),ptr);
    return 1;
}

// Update toggle hidden command
long FilePanel::onUpdToggleThumbnails(FXObject* sender,FXSelector sel,void* ptr)
{	
    FXuint msg=FXWindow::ID_UNCHECK;
	FXbool showthumb=current->list->shownThumbnails();
		
	if(showthumb == FALSE)
		msg=FXWindow::ID_CHECK;
    sender->handle(this,FXSEL(SEL_COMMAND,msg),ptr);
    return 1;
}


#if defined(linux)
// Mount/Unmount file systems
long FilePanel::onCmdMount(FXObject*,FXSelector sel,void*)
{
    FXString cmd, msg, text;
	unsigned int op;
	File *f;
	FXString dir;
	
	current->list->setFocus();

	// Use the selected directory in FilePanel if any
	// or use the selected directory in DirPanel
	if (current->list->getNumSelectedItems()==0)
		dir=current->list->getDirectory();
	else
	{
		for (int u= 0; u< current->list->getNumItems (); u++)
		{
			if (current->list->isItemSelected(u))
				dir=current->list->getItemPathname(u);
		}
	}		

	// If symbolic link, read the linked directory
	if (::isLink(dir))
		dir=readlink(dir);

    if(FXSELID(sel)==ID_MOUNT)
    {
        op=MOUNT;
		msg=_("Mount");
        cmd="mount ";
    }
    else
    {
        op=UNMOUNT;
		msg=_("Unmount");
        cmd="umount ";
    }
    cmd+=::quote(dir);
    cmd+=" 2>&1";
    chdir(ROOTDIR);

	// Wait cursor
	getApp()->beginWaitCursor();

	// File object
	text=msg + _(" file system...");
	f=new File(this,text.text(),op);
	f->create();
				   
	// Mount/unmount file system
	text=msg + _(" the folder:");
	f->mount(dir,text,cmd,op);
	chdir(startlocation.text());
 
	// If action is cancelled in progress dialog
	if (f->isCancelled())
	{
		f->hide();
		text=msg + _(" operation cancelled!");
		MessageBox::error(this,BOX_OK,_("Error"),text.text());
		delete f;
		return 0;
	}
					
	getApp()->endWaitCursor();
	delete f; 

	// Force panel refresh
    onCmdRefresh(0,0,0);

    return 1;
}


// Update the Mount button
long FilePanel::onUpdMount(FXObject* o,FXSelector sel,void*)
{
    FXString dir;

   	FXint num=current->list->getNumSelectedItems();
	
	// Use the selected directory in FilePanel if any
	// or use the selected directory in DirPanel
	if (num==0)
		dir=current->list->getDirectory();
	else
	{
		for (int u= 0; u< current->list->getNumItems (); u++)
		{
			if (current->list->isItemSelected(u))
				dir=current->list->getItemPathname(u);
		}
	}		

	if(fsdevices->find(dir.text()) && !mtdevices->find(dir.text()) && current->list->getNumItems() && !current->list->isItemSelected(0))
        o->handle(this,FXSEL(SEL_COMMAND,FXWindow::ID_ENABLE),NULL);
    else
        o->handle(this,FXSEL(SEL_COMMAND,FXWindow::ID_DISABLE),NULL);

    return 1;
}


// Update the Unmount button
long FilePanel::onUpdUnmount(FXObject* o,FXSelector sel,void*)
{
    FXString dir;
	
   	FXint num=current->list->getNumSelectedItems();
	
	// Use the selected directory in FilePanel if any
	// or use the selected directory in DirPanel
	if (num==0)
		dir=current->list->getDirectory();
	else
	{
		for (int u= 0; u< current->list->getNumItems (); u++)
		{
			if (current->list->isItemSelected (u))
				dir=current->list->getItemPathname(u);
		}
	}		

	if((fsdevices->find(dir.text()) || mtdevices->find(dir.text())) && current->list->getNumItems() && !current->list->isItemSelected(0))
        o->handle(this,FXSEL(SEL_COMMAND,FXWindow::ID_ENABLE),NULL);
    else
       o->handle(this,FXSEL(SEL_COMMAND,FXWindow::ID_DISABLE),NULL);

    return 1;
}

#endif // End #if defined(linux)


// Update the status bar and the path linker
long FilePanel::onUpdStatus(FXObject* sender,FXSelector,void*)
{
	// Update the status bar
    int item=-1;
	FXString str, linkto;
    FXchar size[64];
	unsigned long long sz=0;
	FXString hsize=_("0 bytes");
	
    FXString path=list->getDirectory();

   	FXint num=list->getNumSelectedItems();

	// To handle the update rename (ugly, I know)
	if (current==this)
	{
		if (num<=1)
			selmult=FALSE;
		else if (num>1)
			selmult=TRUE;
	}

    item=list->getCurrentItem();
		
    if(num>1)
    {
        for (int u= 0; u< list->getNumItems(); u++)
		{
            if (list->isItemSelected(u) && !list->isItemDirectory(u))
            {
				sz+=list->getItemFileSize(u);
				snprintf(size,sizeof(size)-1,"%llu",sz);
				hsize=::hSize(size);
            }
		}
		str.format(_("%s in %s selected items"),hsize.text(),FXStringVal(num).text());
    }
	else
	{
		if(num==0 || item<0)
		{
			num=list->getNumItems();
			if (num==1)
				str=_("1 item");
			else
				str=FXStringVal(num)+_(" items");
		}
		else
		{
			FXString string=list->getItemText(item);
			FXString name=string.section('\t',0);
			FXString type=string.section('\t',2);

			FXString date=string.section('\t',4);
			FXString usr=string.section('\t',5);
			FXString grp=string.section('\t',6);
			FXString perm=string.section('\t',7);

			if (type.contains(_("Broken link")))
			{
				linkto=readlink(path+PATHSEPSTRING+name);
				str=hsize+_(" in one selected item")+ " [" + name+"->" + linkto.text() + " | " + type + " | " + date 
				    + " | " + usr + " | " + grp + " | " + perm + "]";
			}
			else if (type.contains(_("Link")))
			{
				linkto=readlink(path+PATHSEPSTRING+name);
				str=hsize+_(" in one selected item")+ " [" + name+"->" + linkto.text() + " | " + type + " | " + date
                    + " | " + usr + " | " + grp + " | " + perm + "]";
			}
			else
			{
				for (int u= 0; u< list->getNumItems(); u++)
				{
					if (list->isItemSelected(u) && !list->isItemDirectory(u))
					{
						sz=list->getItemFileSize(u);
						snprintf(size,sizeof(size)-1,"%llu",sz);
						hsize=::hSize(size);
						break;
					}
				}
				str=hsize+_(" in one selected item")+ " [" + type + " | " + date
					+ " | " + usr + " | " + grp + " | " + perm + "]";
			}
		}
	}

	statuslabel->setText(str);

	// Add the filter pattern if any
	if (list->getPattern() != "*" && list->getPattern() != "*.*")
	{
		str.format(_(" - Filter: %s"),list->getPattern().text());		
		filterlabel->setText(str);
		filterlabel->setTextColor(attenclr);
	}
	else
		filterlabel->setText("");
	
    return 1;
}


// Update the path text and the path link
void FilePanel::updatePath()
{    
	pathlink->setPath(list->getDirectory());
	pathtext->setText(list->getDirectory());
}


// Update the go to parent directory command
long FilePanel::onUpdUp(FXObject* o,FXSelector,void*)
{
    FXButton *button=(FXButton*)o;
    int style=button->getButtonStyle();
    if(style & TOGGLEBUTTON_TOOLBAR)
    {
        if(current->list->getDirectory()!=ROOTDIR)
            o->handle(this,FXSEL(SEL_COMMAND,FXWindow::ID_ENABLE),NULL);
        else
            o->handle(this,FXSEL(SEL_COMMAND,FXWindow::ID_DISABLE),NULL);
    }
    else
        o->handle(this,FXSEL(SEL_COMMAND,FXWindow::ID_ENABLE),NULL);
    return 1;
}


// Update the paste button
long FilePanel::onUpdPaste(FXObject* o,FXSelector,void*)
{
    FXuchar *data;
    FXuint len;	
	FXString buf;
	FXbool clipboard_empty=TRUE;
	
	// Lock clipboard to prevent changes in method onCmdRequestClipboard()
	clipboard_locked=TRUE;
	
	// If source is xfelistType (Gnome, XFCE, or Xfe app)
    if (getDNDData(FROM_CLIPBOARD,xfelistType,data,len))
    {
        FXRESIZE(&data,FXuchar,len+1);
        data[len]='\0';
		buf=(FXchar*)data;
		
		// Check if valid clipboard
		if (buf.find("file:/")>=0)
			clipboard_empty=FALSE;
			
        // Free data pointer
		FXFREE(&data);
	}
		
	// If source type is urilistType (KDE apps ; non Gnome, non XFCE and non Xfe apps)
	else if (getDNDData(FROM_CLIPBOARD,urilistType,data,len))
	{		
		FXRESIZE(&data,FXuchar,len+1);
		data[len]='\0';
		buf=(FXchar*)data;
		
		// Check if valid clipboard
		if (buf.find("file:/")>=0)
			clipboard_empty=FALSE;
		
		// Free data pointer
		FXFREE(&data);
	}
		
	// Gray out the paste button, if necessary
	if (clipboard_empty)
		o->handle(this,FXSEL(SEL_COMMAND,FXWindow::ID_DISABLE),NULL);
	else
		o->handle(this,FXSEL(SEL_COMMAND,FXWindow::ID_ENABLE),NULL);

	// Unlock clipboard
	clipboard_locked=FALSE;

    return 1;
}


// Update menu items and toolbar buttons that are related to file operations
long FilePanel::onUpdMenu(FXObject* o,FXSelector sel,void*)
{
		
   	// Menu item is disabled when nothing or only ".." is selected
	FXint num;
	num=current->list->getNumSelectedItems();
	DirItem* item=(DirItem*)dirpanel->getCurrentItem();

	if ((dirpanel->shown() && item))
	{
		if (num==0)
			o->handle(this,FXSEL(SEL_COMMAND,FXWindow::ID_ENABLE),NULL);
		else if (num==1 && current->list->isItemSelected(0))
			o->handle(this,FXSEL(SEL_COMMAND,FXWindow::ID_DISABLE),NULL);
		else
			o->handle(this,FXSEL(SEL_COMMAND,FXWindow::ID_ENABLE),NULL);
	}
	else
	{
		if (num==0)
			o->handle(this,FXSEL(SEL_COMMAND,FXWindow::ID_DISABLE),NULL);
		else if (num==1 && current->list->isItemSelected(0))
			o->handle(this,FXSEL(SEL_COMMAND,FXWindow::ID_DISABLE),NULL);
		else
			o->handle(this,FXSEL(SEL_COMMAND,FXWindow::ID_ENABLE),NULL);
	}

    return 1;
}


// Update file delete menu item and toolbar button
long FilePanel::onUpdFileDelete(FXObject* o,FXSelector sel,void*)
{
	FXbool use_trash_can=getApp()->reg().readUnsignedEntry("OPTIONS","use_trash_can",TRUE);
	FXbool use_trash_bypass=getApp()->reg().readUnsignedEntry("OPTIONS","use_trash_bypass",FALSE);
	if (!use_trash_can | use_trash_bypass)
	{
		FXint num=current->list->getNumSelectedItems();
		if (num==0)
			o->handle(this,FXSEL(SEL_COMMAND,FXWindow::ID_DISABLE),NULL);
		else if (num==1 && current->list->isItemSelected(0))
			o->handle(this,FXSEL(SEL_COMMAND,FXWindow::ID_DISABLE),NULL);
		else
			o->handle(this,FXSEL(SEL_COMMAND,FXWindow::ID_ENABLE),NULL);
	}
	else
		o->handle(this,FXSEL(SEL_COMMAND,FXWindow::ID_DISABLE),NULL);

    return 1;
}


// Update move to trash menu item and toolbar button
long FilePanel::onUpdFileTrash(FXObject* o,FXSelector sel,void*)
{
	// Disable move to trash menu if we are in trash can
	// or if the trash can directory is selected

	FXbool trashenable=TRUE;
	FXString trashparentdir=trashlocation.rbefore('/');
	FXString curdir=current->list->getDirectory();

	if (curdir.left(trashlocation.length())==trashlocation)
		trashenable=FALSE;
		
	if (curdir==trashparentdir)
	{
		FXString pathname;
		for (int u= 0; u< current->list->getNumItems (); u++)
		{
			if (current->list->isItemSelected (u))
			{
				pathname=current->list->getItemPathname(u);
				if (pathname==trashlocation)
					trashenable=FALSE;
			}
		}
	}

	FXbool use_trash_can=getApp()->reg().readUnsignedEntry("OPTIONS","use_trash_can",TRUE);
	if (use_trash_can && trashenable)
	{
		FXint num=current->list->getNumSelectedItems();
		if (num==0)
			o->handle(this,FXSEL(SEL_COMMAND,FXWindow::ID_DISABLE),NULL);
		else if (num==1 && current->list->isItemSelected(0))
			o->handle(this,FXSEL(SEL_COMMAND,FXWindow::ID_DISABLE),NULL);
		else
			o->handle(this,FXSEL(SEL_COMMAND,FXWindow::ID_ENABLE),NULL);
	}
	else
		o->handle(this,FXSEL(SEL_COMMAND,FXWindow::ID_DISABLE),NULL);

    return 1;
}


// Update restore from trash menu item and toolbar button
long FilePanel::onUpdFileRestore(FXObject* o,FXSelector sel,void*)
{
	// Enable restore from trash menu if we are in trash can

	FXbool restoreenable=FALSE;
	FXString curdir=current->list->getDirectory();

	if (curdir.left(trashfileslocation.length())==trashfileslocation)
		restoreenable=TRUE;
		
	FXbool use_trash_can=getApp()->reg().readUnsignedEntry("OPTIONS","use_trash_can",TRUE);
	if (use_trash_can && restoreenable)
	{
		FXint num=current->list->getNumSelectedItems();
		if (num==0)
			o->handle(this,FXSEL(SEL_COMMAND,FXWindow::ID_DISABLE),NULL);
		else if (num==1 && current->list->isItemSelected(0))
			o->handle(this,FXSEL(SEL_COMMAND,FXWindow::ID_DISABLE),NULL);
		else
			o->handle(this,FXSEL(SEL_COMMAND,FXWindow::ID_ENABLE),NULL);
	}
	else
		o->handle(this,FXSEL(SEL_COMMAND,FXWindow::ID_DISABLE),NULL);

    return 1;
}


// Update go trash menu item and toolbar button
long FilePanel::onUpdGoTrash(FXObject* o,FXSelector sel,void*)
{

	FXbool use_trash_can=getApp()->reg().readUnsignedEntry("OPTIONS","use_trash_can",TRUE);
	if (use_trash_can)
		o->handle(this,FXSEL(SEL_COMMAND,FXWindow::ID_ENABLE),NULL);
	else
		o->handle(this,FXSEL(SEL_COMMAND,FXWindow::ID_DISABLE),NULL);

    return 1;
}

// Update file open menu
long FilePanel::onUpdOpen(FXObject* o,FXSelector,void*)
{
   	// Menu item is disabled when nothing or a directory (including "..") is selected
    FXint num, item;	
	num=current->list->getNumSelectedItems(&item);

	if (num==0)
		o->handle(this,FXSEL(SEL_COMMAND,FXWindow::ID_DISABLE),NULL);
	else
	{
		if (current->list->getItem (item) && current->list->isItemFile(item))
			o->handle(this,FXSEL(SEL_COMMAND,FXWindow::ID_ENABLE),NULL);
		else
			o->handle(this,FXSEL(SEL_COMMAND,FXWindow::ID_DISABLE),NULL);
	}
    return 1;
}


// Update the status of the menu items that should be disabled when selecting multiple files
long FilePanel::onUpdSelMult(FXObject* o, FXSelector sel, void*)
{
   	// Menu item is disabled when nothing is selected or multiple selection or ".." is only selected
	FXint num;
	num=current->list->getNumSelectedItems();
	DirItem* item=(DirItem*)dirpanel->getCurrentItem();

	if (num==0)
	{
		if (!item || !dirpanel->shown())
			o->handle(this,FXSEL(SEL_COMMAND,FXWindow::ID_DISABLE),NULL);
		else
    		o->handle(this,FXSEL(SEL_COMMAND,FXWindow::ID_ENABLE),NULL);
	}	
	else if (current->selmult || (num==1 && current->list->isItemSelected(0)))
		o->handle(this,FXSEL(SEL_COMMAND,FXWindow::ID_DISABLE),NULL);
	else
    	o->handle(this,FXSEL(SEL_COMMAND,FXWindow::ID_ENABLE),NULL);

	return 1;
}


// Update Add to archive menu
long FilePanel::onUpdAddToArch(FXObject* o,FXSelector,void*)
{
   	// Menu item is disabled when nothing or ".." is selected
    FXint num, item;	
	num=current->list->getNumSelectedItems(&item);
	if (num==0)
		o->handle(this,FXSEL(SEL_COMMAND,FXWindow::ID_DISABLE),NULL);	
	else if (num==1 && current->list->isItemSelected(0))
		o->handle(this,FXSEL(SEL_COMMAND,FXWindow::ID_DISABLE),NULL);
	else
		o->handle(this,FXSEL(SEL_COMMAND,FXWindow::ID_ENABLE),NULL);
    return 1;
}

