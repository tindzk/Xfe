#ifndef DIALOGBOX_H
#define DIALOGBOX_H


// Dialog Box window
class FXAPI DialogBox : public FXTopWindow
{
    FXDECLARE(DialogBox)
protected:
    DialogBox()
    {}
    DialogBox(const DialogBox&)
    {}
public:
    long onKeyPress(FXObject*,FXSelector,void*);
    long onKeyRelease(FXObject*,FXSelector,void*);
    long onClose(FXObject*,FXSelector,void*);
    long onCmdAccept(FXObject*,FXSelector,void*);
    long onCmdCancel(FXObject*,FXSelector,void*);
    long onCmdToggleOption(FXObject*,FXSelector,void*);
public:
    enum{
        ID_CANCEL=FXTopWindow::ID_LAST,
        ID_ACCEPT,
        ID_TOGGLE_OPTION,
        ID_LAST
    };
public:
    DialogBox(FXWindow* win,const FXString& name,FXuint opts=DECOR_TITLE|DECOR_BORDER,FXint x=0,FXint y=0,FXint w=0,FXint h=0,FXint pl=10,FXint pr=10,FXint pt=10,FXint pb=10,FXint hs=4,FXint vs=4);
    DialogBox(FXApp* a,const FXString& name,FXuint opts=DECOR_TITLE|DECOR_BORDER,FXint x=0,FXint y=0,FXint w=0,FXint h=0,FXint pl=10,FXint pr=10,FXint pt=10,FXint pb=10,FXint hs=4,FXint vs=4);
    virtual void show(FXuint placement=PLACEMENT_CURSOR);
    virtual void create();
    FXuint execute(FXuint placement=PLACEMENT_CURSOR);
    FXuint getOption();
protected:
    FXuint _option;
};


#endif
