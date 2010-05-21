#ifndef FILELIST_H
#define FILELIST_H

#include "StringList.h"
#include "IconList.h"


struct FileAssoc;
class  FileDict;
class  FileList;

// File List options (prefixed by underscore to avoid conflict with the FOX library)
enum
{
	_FILELIST_SHOWHIDDEN   = 0x04000000, // Show hidden files or directories
	_FILELIST_SHOWDIRS     = 0x08000000, // Show only directories
	_FILELIST_SEARCH       = 0x10000000, // File list is a search list (must be the same value as in IconList)
};

// File item
class FXAPI FileItem : public IconItem
{
    FXDECLARE(FileItem)
    friend class FileList;
protected:
    FileAssoc  *assoc;                      // File association record
	FileItem   *link;                   	// Link to next item
    unsigned long long size;                // File size
    FXTime        date;                     // File date (mtime)
	FXTime		  cdate;					// Changed date (ctime)
	FXTime		  deldate;					// Deletion date
protected:
    FileItem():assoc(NULL),link(NULL),size(0),date(0)
    {}
protected:
    enum{
        FOLDER     = 64,                        // Directory item
        EXECUTABLE = 128,                       // Executable item
        SYMLINK    = 256,                       // Symbolic linked item
        CHARDEV    = 512,                       // Character special item
        BLOCKDEV   = 1024,                      // Block special item
        FIFO       = 2048,                      // FIFO item
        SOCK       = 4096                       // Socket item
    };
public:
    // Constructor
    FileItem(const FXString& text,FXIcon* bi=NULL,FXIcon* mi=NULL,void* ptr=NULL):IconItem(text,bi,mi,ptr),assoc(NULL),link(NULL),size(0),date(0)
    {}

    // Return true if this is a file item
    FXbool isFile() const
    {
        return (state&(FOLDER|BLOCKDEV|CHARDEV|FIFO|SOCK))==0;
    }

    // Return true if this is a directory item
    FXbool isDirectory() const
    {
        return (state&FOLDER)!=0;
    }

    // Return true if this is an executable item
    FXbool isExecutable() const
    {
        return (state&EXECUTABLE)!=0;
    }

    // Return true if this is a symbolic link item
    FXbool isSymlink() const
    {
        return (state&SYMLINK)!=0;
    }

    // Return true if this is a character device item
    FXbool isChardev() const
    {
        return (state&CHARDEV)!=0;
    }

    // Return true if this is a block device item
    FXbool isBlockdev() const
    {
        return (state&BLOCKDEV)!=0;
    }

    // Return true if this is an FIFO item
    FXbool isFifo() const
    {
        return (state&FIFO)!=0;
    }

    // Return true if this is a socket
    FXbool isSocket() const
    {
        return (state&SOCK)!=0;
    }

    // Return the file-association object for this item
    FileAssoc* getAssoc() const
    {
        return assoc;
    }

    // Return the file size for this item
    unsigned long long getSize() const
    {
        return size;
    }

    // Return the date for this item
    FXTime getDate() const
    {
        return date;
    }
};


// File List object
class FXAPI FileList : public IconList
{
    FXDECLARE(FileList)
protected:
	FileItem	*list;                  // File item list
	FXint 		 prevIndex;
	FXString     directory;             // Current directory
    FXString     orgdirectory;          // Original directory
    FXString     dropdirectory;         // Drop directory
    FXDragAction dropaction;            // Drop action
    FXString     dragfiles;             // Dragged files
    FileDict	*associations;          // Association table
    FXString     pattern;               // Pattern of file names
    FXuint       matchmode;             // File wildcard match mode
    FXTime       timestamp;             // Time when last refreshed
	FXuint       counter;               // Refresh counter
	FXbool		allowrefresh;			// Allow or disallow periodic refresh
	FXbool		displaythumbnails;		// Display thumbnails
	FXString	trashfileslocation;     // Location of the trash files directory
	FXString	trashinfolocation;      // Location of the trash info directory
	FXbool      single;                 // Allow single click navigation for directories
	FXbool		dirsfirst;				// Sort directories first
	FXint		deldatesize;
	FXint		origpathsize;
	FXWindow	*focuswindow;			// Window used to test focus

public:
	StringList		*backhist;          // Back history
	StringList		*forwardhist;       // Forward history

protected:
    FileList()
	{}
    virtual IconItem *createItem(const FXString& text,FXIcon *big,FXIcon* mini,void* ptr);
    void updateItems(FXbool);
    void listItems(FXbool);
private:
    FileList(const FileList&);
    FileList &operator=(const FileList&);
public:
	long onCmdRefresh(FXObject*,FXSelector,void*);
    long onCmdRefreshTimer(FXObject*,FXSelector,void*);
    long onOpenTimer(FXObject*,FXSelector,void*);
    long onDNDEnter(FXObject*,FXSelector,void*);
    long onDNDLeave(FXObject*,FXSelector,void*);
    long onDNDMotion(FXObject*,FXSelector,void*);
    long onDNDDrop(FXObject*,FXSelector,void*);
    long onDNDRequest(FXObject*,FXSelector,void*);
    long onBeginDrag(FXObject*,FXSelector,void*);
    long onEndDrag(FXObject*,FXSelector,void*);
    long onDragged(FXObject*,FXSelector,void*);
    long onCmdDirectoryUp(FXObject*,FXSelector,void*);
    long onUpdDirectoryUp(FXObject*,FXSelector,void*);
	long onCmdSortByName(FXObject*,FXSelector,void*);
	long onCmdSortByDirName(FXObject*,FXSelector,void*);
	long onUpdSortByName(FXObject*,FXSelector,void*);
	long onUpdSortByDirName(FXObject*,FXSelector,void*);
	long onCmdSortByType(FXObject*,FXSelector,void*);
	long onUpdSortByType(FXObject*,FXSelector,void*);
	long onCmdSortBySize(FXObject*,FXSelector,void*);
	long onUpdSortBySize(FXObject*,FXSelector,void*);
	long onCmdSortByExt(FXObject*,FXSelector,void*);
	long onUpdSortByExt(FXObject*,FXSelector,void*);
	long onCmdSortByTime(FXObject*,FXSelector,void*);
	long onUpdSortByTime(FXObject*,FXSelector,void*);
	long onCmdSortByUser(FXObject*,FXSelector,void*);
	long onUpdSortByUser(FXObject*,FXSelector,void*);
	long onCmdSortByGroup(FXObject*,FXSelector,void*);
	long onUpdSortByGroup(FXObject*,FXSelector,void*);
	long onCmdSortByPerm(FXObject*,FXSelector,void*);
	long onUpdSortByPerm(FXObject*,FXSelector,void*);
	long onCmdSortByDeltime(FXObject*,FXSelector,void*);
	long onUpdSortByDeltime(FXObject*,FXSelector,void*);
	long onCmdSortByOrigpath(FXObject*,FXSelector,void*);
	long onUpdSortByOrigpath(FXObject*,FXSelector,void*);
	long onCmdSortReverse(FXObject*,FXSelector,void*);
	long onUpdSortReverse(FXObject*,FXSelector,void*);
	long onCmdSortCase(FXObject*,FXSelector,void*);
	long onUpdSortCase(FXObject*,FXSelector,void*);
    long onCmdSetPattern(FXObject*,FXSelector,void*);
    long onUpdSetPattern(FXObject*,FXSelector,void*);
    long onCmdToggleHidden(FXObject*,FXSelector,void*);
    long onUpdToggleHidden(FXObject*,FXSelector,void*);
    long onCmdShowHidden(FXObject*,FXSelector,void*);
    long onUpdShowHidden(FXObject*,FXSelector,void*);
    long onCmdHideHidden(FXObject*,FXSelector,void*);
    long onUpdHideHidden(FXObject*,FXSelector,void*);
	long onCmdHeader(FXObject*,FXSelector,void*);
	long onUpdHeader(FXObject*,FXSelector,void*);
	long onCmdToggleThumbnails(FXObject*,FXSelector,void*);
	long onUpdToggleThumbnails(FXObject* sender,FXSelector,void*);
	long onCmdDirsFirst(FXObject*,FXSelector,void*);
	long onUpdDirsFirst(FXObject*,FXSelector,void*);
	long onCmdDragCopy(FXObject* sender,FXSelector,void*);
	long onCmdDragMove(FXObject* sender,FXSelector,void*);
	long onCmdDragLink(FXObject* sender,FXSelector,void*);
	long onCmdDragReject(FXObject* sender,FXSelector,void*);
	
public:
	static FXint ascending(const IconItem* pa,const IconItem* pb);
	static FXint descending(const IconItem* pa,const IconItem* pb);
	static FXint ascendingCase(const IconItem* pa,const IconItem* pb);
	static FXint descendingCase(const IconItem* pa,const IconItem* pb);
	static FXint ascendingDir(const IconItem* pa,const IconItem* pb);
	static FXint descendingDir(const IconItem* pa,const IconItem* pb);
	static FXint ascendingDirCase(const IconItem* pa,const IconItem* pb);
	static FXint descendingDirCase(const IconItem* pa,const IconItem* pb);
	static FXint ascendingType(const IconItem* pa,const IconItem* pb);
	static FXint descendingType(const IconItem* pa,const IconItem* pb);
	static FXint ascendingSize(const IconItem* pa,const IconItem* pb);
	static FXint descendingSize(const IconItem* pa,const IconItem* pb);
	static FXint ascendingExt(const IconItem* pa,const IconItem* pb);
	static FXint descendingExt(const IconItem* pa,const IconItem* pb);
	static FXint ascendingTime(const IconItem* pa,const IconItem* pb);
	static FXint descendingTime(const IconItem* pa,const IconItem* pb);
	static FXint ascendingUser(const IconItem* pa,const IconItem* pb);
	static FXint descendingUser(const IconItem* pa,const IconItem* pb);
	static FXint ascendingGroup(const IconItem* pa,const IconItem* pb);
	static FXint descendingGroup(const IconItem* pa,const IconItem* pb);
	static FXint ascendingPerm(const IconItem* pa,const IconItem* pb);
	static FXint descendingPerm(const IconItem* pa,const IconItem* pb);
	static FXint ascendingDeltime(const IconItem* pa,const IconItem* pb);
	static FXint descendingDeltime(const IconItem* pa,const IconItem* pb);
	static FXint ascendingOrigpath(const IconItem* pa,const IconItem* pb);
	static FXint descendingOrigpath(const IconItem* pa,const IconItem* pb);
	static FXint ascendingMix(const IconItem* pa,const IconItem* pb);
	static FXint descendingMix(const IconItem* pa,const IconItem* pb);
	static FXint ascendingCaseMix(const IconItem* pa,const IconItem* pb);
	static FXint descendingCaseMix(const IconItem* pa,const IconItem* pb);
	static FXint ascendingDirMix(const IconItem* pa,const IconItem* pb);
	static FXint descendingDirMix(const IconItem* pa,const IconItem* pb);
	static FXint ascendingDirCaseMix(const IconItem* pa,const IconItem* pb);
	static FXint descendingDirCaseMix(const IconItem* pa,const IconItem* pb);
	static FXint ascendingTypeMix(const IconItem* pa,const IconItem* pb);
	static FXint descendingTypeMix(const IconItem* pa,const IconItem* pb);
	static FXint ascendingSizeMix(const IconItem* pa,const IconItem* pb);
	static FXint descendingSizeMix(const IconItem* pa,const IconItem* pb);
	static FXint ascendingExtMix(const IconItem* pa,const IconItem* pb);
	static FXint descendingExtMix(const IconItem* pa,const IconItem* pb);
	static FXint ascendingTimeMix(const IconItem* pa,const IconItem* pb);
	static FXint descendingTimeMix(const IconItem* pa,const IconItem* pb);
	static FXint ascendingUserMix(const IconItem* pa,const IconItem* pb);
	static FXint descendingUserMix(const IconItem* pa,const IconItem* pb);
	static FXint ascendingGroupMix(const IconItem* pa,const IconItem* pb);
	static FXint descendingGroupMix(const IconItem* pa,const IconItem* pb);
	static FXint ascendingPermMix(const IconItem* pa,const IconItem* pb);
	static FXint descendingPermMix(const IconItem* pa,const IconItem* pb);
	static FXint ascendingDeltimeMix(const IconItem* pa,const IconItem* pb);
	static FXint descendingDeltimeMix(const IconItem* pa,const IconItem* pb);
	static FXint ascendingOrigpathMix(const IconItem* pa,const IconItem* pb);
	static FXint descendingOrigpathMix(const IconItem* pa,const IconItem* pb);
public:
	enum {
		// Note : the order of the 10 following sort IDs must be kept
        ID_SORT_BY_NAME=IconList::ID_LAST,
        ID_SORT_BY_SIZE,
        ID_SORT_BY_TYPE,
        ID_SORT_BY_EXT,
        ID_SORT_BY_TIME,
        ID_SORT_BY_USER,
        ID_SORT_BY_GROUP,
        ID_SORT_BY_PERM,   
        ID_SORT_BY_ORIGPATH,
        ID_SORT_BY_DELTIME,
        ID_SORT_BY_DIRNAME,
        ID_SORT_REVERSE,
    	ID_SORT_CASE,
    	ID_DIRS_FIRST,
        ID_DIRECTORY_UP,
        ID_SET_PATTERN,
        ID_SET_DIRECTORY,
        ID_SHOW_HIDDEN,
        ID_HIDE_HIDDEN,
        ID_TOGGLE_HIDDEN,
		ID_TOGGLE_THUMBNAILS,
        ID_REFRESH_TIMER,
		ID_REFRESH,
        ID_OPEN_TIMER,
		ID_DRAG_COPY,
		ID_DRAG_MOVE,
		ID_DRAG_LINK,
		ID_DRAG_REJECT,
        ID_LAST
    };
public:

    // Construct a file list
    FileList(FXWindow* focuswin, FXComposite *p,FXObject* tgt=NULL,FXSelector sel=0,FXbool showthumbs=FALSE,FXuint opts=0,FXint x=0,FXint y=0,FXint w=0,FXint h=0);

    // Create server-side resources
    virtual void create();

	// Scan the current directory and update the items if needed, or if force is TRUE
	void scan(FXbool force=TRUE);

    // Set current file
    void setCurrentFile(const FXString& file);

    // Return current file
    FXString getCurrentFile() const;

    // Set current directory
	void setDirectory(const FXString& path,const FXbool histupdate=TRUE,FXString prevpath= "");

	// Return current directory
    FXString getDirectory() const
    {
        return directory;
    }

    // Change wildcard matching pattern
    void setPattern(const FXString& ptrn);

    // Return wildcard pattern
    FXString getPattern() const
    {
        return pattern;
    }

    // Return TRUE if item is a directory
    FXbool isItemDirectory(FXint index) const;

    // Return TRUE if item is a file
    FXbool isItemFile(FXint index) const;

    // Return TRUE if item is executable
    FXbool isItemExecutable(FXint index) const;

    // Return TRUE if item is a symbolic link
    FXbool isItemLink(FXint index) const;

	// Get number of selected items
	FXint getNumSelectedItems(void) const
	{
		FXint num=0;
		for (int u=0; u<getNumItems(); u++)
			if (isItemSelected(u))
				num++;
		return num;
	}


	// Get number of selected items and index of first selected item
	FXint getNumSelectedItems(FXint *index) const
	{
		FXint num=0, itm=-1;
		for (int u=0; u<getNumItems(); u++)
			if (isItemSelected(u))
			{
				if (itm== -1)
					itm= u;
				num++;
			}
		(*index)=itm;
		return num;
	}

	// Return number of selected items
	//FXint getNumSelectedItems(void) const;

	// Return number of selected items and index of first selected item  
	//FXint getNumSelectedItems(FXint *index) const;
	
	// Return name of item at index
    FXString getItemFilename(FXint index) const;

	// Get pathname from item at index, relatively to the current directory
    FXString getItemPathname(FXint index) const;

	// Get full pathname from item at index, as obtained from the label string 
    FXString getItemFullPathname(FXint index) const;

    // Return file association of item
    FileAssoc* getItemAssoc(FXint index) const;

	// Return file size of the item
	unsigned long long getItemFileSize(FXint index) const;

    // Return wildcard matching mode
    FXuint getMatchMode() const
    {
        return matchmode;
    }

	// Return directory first state for file name sorting
	FXbool getDirsFirst() const
	{
		return dirsfirst;
	}

	// Set directory first state for file name sorting
	void setDirsFirst(const FXbool dfirst)
	{
		dirsfirst=dfirst;
	}

	FXint getHeaderSize(FXint index) const;
	
	void setHeaderSize(FXint index,FXint size);

	// Allow or disallow periodic refresh
	void setAllowRefresh(const FXbool allow);

    // Change wildcard matching mode
    void setMatchMode(FXuint mode);

    // Return TRUE if showing hidden files
    FXbool shownHiddenFiles() const;

    // Show or hide hidden files
    void showHiddenFiles(FXbool showing);

    // Return TRUE if displaying thumbnails
    FXbool shownThumbnails() const;

    // Display or not thumbnails
    void showThumbnails(FXbool display);

    // Return TRUE if showing directories only
    FXbool showOnlyDirectories() const;

    // Show directories only
    void showOnlyDirectories(FXbool shown);

    // Change file associations
    void setAssociations(FileDict* assoc);

    // Return file associations
    FileDict* getAssociations() const
    {
        return associations;
    }

    // Destructor
    virtual ~FileList();
};


#endif
