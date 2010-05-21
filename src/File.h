#ifndef FILE_H
#define FILE_H


#include <fx.h>
#include "DialogBox.h"


// File operations
enum
{	
	COPY,
	RENAME,
	MOVE,
	SYMLINK,
	DELETE,
	CHMOD,
	CHOWN,
	EXTRACT,
	ARCHIVE,
#if defined(linux)
	PKG_INSTALL,
	PKG_UNINSTALL,
	MOUNT,
	UNMOUNT
#endif
};

// To search visited inodes
struct inodelist
{
    ino_t st_ino;
    inodelist *next;
};


class File : public DialogBox
{
    FXDECLARE(File)
protected:

	// Inline function
	// Force check of timeout for progress dialog (to avoid latency problems)
	FXint checkTimeout(void)
	{
		if (getApp()->hasTimeout(this,File::ID_TIMEOUT))
		{
			if (getApp()->remainingTimeout(this,File::ID_TIMEOUT)==0)
			{
				getApp()->removeTimeout(this,File::ID_TIMEOUT);
				show(PLACEMENT_OWNER);
					
				getApp()->forceRefresh();
				getApp()->flush();
				return 1;
			}
		}
		return 0;
	}

	void forceTimeout(void);
	void restartTimeout(void);
    long fullread(FXint fd, FXuchar* ptr, long len);
    long fullwrite(FXint fd, const FXuchar* ptr, long len);
	FXuint getOverwriteAnswer(FXString, FXString);
    FXbool copyfile(const FXString& source, const FXString& target, const FXbool preserve_date);
	FXbool copyrec(const FXString& source,const FXString& target,inodelist* inodes, const FXbool preserve_date);
    FXbool copydir(const FXString& source,const FXString& target,struct stat& parentstatus,inodelist* inodes, const FXbool preserve_date);
    FXint rchmod(FXchar* path, FXchar* file, mode_t mode, const FXbool dironly, const FXbool fileonly);
    FXint rchown(FXchar* path, FXchar* file, uid_t uid, gid_t gid, const FXbool dironly, const FXbool fileonly);
    FXLabel *uplabel;
    FXLabel *downlabel;
    FXProgressBar *progressbar;
	FXButton *cancelButton;
	FXbool overwrite;
	FXbool overwrite_all;
	FXbool skip_all;
	FXbool cancelled;
public:
    File()
    {}
	~File();
    void create();
    File(FXWindow *owner, FXString title, const FXuint operation);

	enum 
	{
		ID_CANCEL_BUTTON=DialogBox::ID_LAST,
		ID_TIMEOUT,
		ID_LAST
	};
	
	FXbool isCancelled()
	{
		return cancelled;		
	}
	
	void hideProgressDialog()
	{
		forceTimeout();
	}
	
	void showProgressDialog()
	{
		restartTimeout();
	}

	FXbool copy(const FXString& source, const FXString& target, const FXbool confirm_dialog=TRUE, const FXbool preserve_date=TRUE);
    FXbool rename(const FXString& source, const FXString& target);
    FXbool move(const FXString& source, const FXString& target, const FXbool restore=FALSE);
    FXbool symlink(const FXString& source, const FXString& target);
    FXbool remove(const FXString& file);

    FXint chmod(FXchar* path, FXchar* file, mode_t mode, const FXbool rec, const FXbool dironly=FALSE, const FXbool fileonly=FALSE);
	FXint chown(FXchar* path, FXchar *file, uid_t uid, gid_t gid, const FXbool rec, const FXbool dironly=FALSE, const FXbool fileonly=FALSE);
	FXint extract(const FXString name, const FXString dir,const FXString cmd);
	FXint archive(const FXString name, const FXString cmd);

#if defined(linux)
	FXint mount(const FXString dir, const FXString msg, const FXString cmd, const  FXuint op);
	FXint pkgInstall(const FXString name, const FXString cmd);
	FXint pkgUninstall(const FXString name, const FXString cmd);
#endif
	long onCmdCancel(FXObject*, FXSelector,void*);
    long onTimeout(FXObject*,FXSelector,void*);
};
#endif


