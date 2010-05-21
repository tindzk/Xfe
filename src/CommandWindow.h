#ifndef COMMANDWINDOW_H
#define COMMANDWINDOW_H

#include "DialogBox.h"


class CommandWindow : public DialogBox
{
    FXDECLARE(CommandWindow)
protected:

    FXint         pid;                 // Proccess ID of child (valid if busy).
    FXint         pipes[2];            // Pipes to communicate with child process.
    FXChore      *wchore;		       // Chore to watch process.
    FXText       *text;

    FXString command;                  // Command string
	FXbool killed;                     // True if the cancel button was pressed
	FXbool closed;                     // True if the closed button was pressed

private:
    CommandWindow()
    {}
    CommandWindow(const CommandWindow&);
public:
    enum{
        ID_CLOSE=DialogBox::ID_LAST,
        ID_WATCHPROCESS,
        ID_KILLPROCESS,
        ID_LAST
    };
public:
    long onCmdKillProcess(FXObject*,FXSelector,void*);
    long onUpdKillProcess(FXObject*,FXSelector,void*);
    long onWatchProcess(FXObject*,FXSelector,void*);
	long onUpdClose(FXObject* sender,FXSelector,void*);
    virtual void create();
    virtual ~CommandWindow();
    FXint execCmd(FXString);
    CommandWindow(FXWindow *owner, const FXString& name,  FXString strcmd, FXint nblines, FXint nbcols);
    CommandWindow(FXApp *a, const FXString& name,  FXString strcmd, FXint nblines, FXint nbcols);
	long onCmdClose(FXObject*,FXSelector,void*);
	void setText(const FXchar*);
	void appendText(const FXchar* str);
	void scrollToLastLine(void);
	FXint getLength(void);
};

#endif
