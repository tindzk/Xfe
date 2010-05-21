#ifndef XFILEWRITE_H
#define XFILEWRITE_H



class Preferences;
class EditWindow;


// Main Application class
class XFileWrite : public FXApp
{
    FXDECLARE(XFileWrite)
public:
    EditWindowList  windowlist;                   // Window list
private:
    XFileWrite()
    {}
    XFileWrite(const XFileWrite&);
    XFileWrite& operator=(const XFileWrite&);
public:
    enum{
        ID_CLOSEALL=FXApp::ID_LAST,
        ID_LAST
    };
public:
    long onCmdCloseAll(FXObject*,FXSelector,void*);
public:

    // Construct application object
    XFileWrite(const FXString&, const FXString&);

    // Initialize application
    virtual void init(int& argc, char** argv,bool connect=TRUE);

    // Exit application
    virtual void exit(FXint code=0);

    // Delete application object
    virtual ~XFileWrite();
};

#endif

