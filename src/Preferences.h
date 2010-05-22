#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <string.h>

#include "DialogBox.h"
#include "Keybindings.h"


// Number of modifiable colors
#define NUM_COLORS 11

// Number of themes
#define NUM_THEMES 13


struct Theme
{
    const char *name;
    FXColor color[NUM_COLORS];
    Theme()
    {
        name = "";
    };
    Theme(const char *n,FXColor base=0,FXColor bdr=0,FXColor bg=0,FXColor fg=0,
	                    FXColor selbg=0,FXColor selfg=0, FXColor listbg=0, FXColor listfg=0, FXColor listhl=0, FXColor pbarfg=0, FXColor attenfg=0)
    {
        name = strdup(n);
        color[0] = base;
        color[1] = bdr;
        color[2] = bg;
        color[3] = fg;
        color[4] = selbg;
        color[5] = selfg;
		color[6] = listbg;
		color[7] = listfg;
 		color[7] = listfg;
  		color[8] = listhl;
		color[9] = pbarfg;
		color[10]= attenfg;
	};
    FXbool operator != (const Theme&);
};

FXColor makeHiliteColorGradient(FXColor);
FXColor makeShadowColorGradient(FXColor);

class PreferencesBox : public DialogBox
{
    FXDECLARE(PreferencesBox)
private:
    PreferencesBox()
    {}
    FXComboBox *colorsBox;
    FXComboBox *themesBox;
    FXList     *themesList;
    FXTextField *iconpath;
    FXTextField *txteditor;
    FXTextField *txtviewer;
    FXTextField *imgviewer;
    FXTextField *xterm;
    FXTextField *imgeditor;
    FXTextField *archiver;
    FXTextField *pdfviewer;
    FXTextField *videoviewer;
    FXTextField *audioviewer;
    FXTextField *normalfont;
    FXTextField *textfont;
    FXString    oldiconpath;
    FXString    oldtxteditor;
    FXString    oldtxtviewer;
    FXString    oldimgviewer;
    FXString    oldxterm;
    FXString    oldnormalfont;
    FXString    oldtextfont;
    FXString    oldimgeditor;
    FXString    oldarchiver;
    FXString    oldpdfviewer;
    FXString    oldvideoviewer;
    FXString    oldaudioviewer;
    FXCheckButton  *autosave;
    FXCheckButton  *savewinpos;
    FXCheckButton  *diropen;
    FXCheckButton  *fileopen;
    FXCheckButton  *filetooltips;
    FXCheckButton  *relativeresize;
    FXCheckButton  *showpathlink;
    FXCheckButton  *quit;
    FXCheckButton  *rootmode;
    FXCheckButton  *trashcan;
    FXCheckButton  *trashbypass;
    FXCheckButton  *dnd;
    FXCheckButton  *trashmv;
    FXCheckButton  *del;
    FXCheckButton  *del_emptydir;
    FXCheckButton  *overwrite;
    FXCheckButton  *ask;
    FXCheckButton  *bg;
    FXCheckButton  *root_warning;
    FXCheckButton  *mount;
	FXCheckButton  *show_mount;
	FXCheckButton  *scroll;
	FXCheckButton  *controls;
	FXColorWell    *cwell;
    Theme          Themes[NUM_THEMES];
    Theme          currTheme;
    Theme          currTheme_prev;
	FXbool		   trashcan_prev;
	FXbool		   trashbypass_prev;
	FXbool		   autosave_prev;
	FXbool		   savewinpos_prev;
	FXbool         diropen_prev;
	FXbool         fileopen_prev;
	FXbool         filetooltips_prev;
	FXbool         relativeresize_prev;
	FXbool         show_pathlink;
	FXbool         show_pathlink_prev;
	FXuint	       value_prev;
	FXbool		   ask_prev;
	FXbool		   dnd_prev;
	FXbool		   trashmv_prev;
	FXbool		   del_prev;
	FXbool		   del_emptydir_prev;
	FXbool		   overwrite_prev;
	FXbool		   quit_prev;
	FXbool		   use_clearlooks;
	FXbool		   use_clearlooks_prev;
	FXbool         rootmode_prev;
#if defined(linux)
	FXbool		   mount_prev;
	FXbool		   show_mount_prev;
#endif
	FXbool		   root_warning_prev;
	FXuint		   themelist_prev;
	FXbool		   smoothscroll_prev;
	KeybindingsBox 		*bindingsbox;
	FXStringDict		*glbBindingsDict;
	FXStringDict		*xfeBindingsDict;
	FXStringDict		*xfiBindingsDict;
	FXStringDict		*xfwBindingsDict;
public:
    enum
	{
        ID_ACCEPT=DialogBox::ID_LAST,
		ID_CANCEL,
        ID_BROWSE_TXTEDIT,
        ID_BROWSE_TXTVIEW,
		ID_BROWSE_IMGVIEW,
		ID_BROWSE_ARCHIVER,
		ID_BROWSE_PDFVIEW,
		ID_BROWSE_VIDEOVIEW,
		ID_BROWSE_AUDIOVIEW,
        ID_BROWSE_XTERM,
        ID_COLOR,
		ID_NORMALFONT,
		ID_TEXTFONT,
        ID_THEME,
        ID_BROWSE_ICON_PATH,
		ID_TRASH_BYPASS,
		ID_CONFIRM_TRASH,
		ID_CONFIRM_DEL_EMPTYDIR,
		ID_STANDARD_CONTROLS,
		ID_CLEARLOOKS_CONTROLS,
		ID_WHEELADJUST,
		ID_SINGLE_CLICK_FILEOPEN,
		ID_FILE_TOOLTIPS,
		ID_RELATIVE_RESIZE,
		ID_SHOW_PATHLINK,
		ID_CHANGE_KEYBINDINGS,
		ID_RESTORE_KEYBINDINGS,
        ID_LAST
    };

public:
    PreferencesBox(FXWindow *win,FXColor listbackcolor=FXRGB(255,255,255), FXColor listforecolor=FXRGB(0,0,0), FXColor highlightcolor=FXRGB(238,238,238),FXColor pbarcolor=FXRGB(0,0,255),FXColor attentioncolor=FXRGB(255,0,0));
    long onCmdAccept(FXObject*,FXSelector,void*);
    long onCmdBrowse(FXObject*,FXSelector,void*);
    long onCmdColor (FXObject*,FXSelector,void*);
    long onUpdColor (FXObject*,FXSelector,void*);
    long onCmdTheme (FXObject*,FXSelector,void*);
    long onCmdBrowsePath(FXObject*,FXSelector,void*);
	long onCmdNormalFont(FXObject*,FXSelector,void*);
	long onCmdTextFont(FXObject*,FXSelector,void*);
	long onUpdTrash(FXObject*,FXSelector,void*);
	long onUpdConfirmDelEmptyDir(FXObject*,FXSelector,void*);
	long onCmdWheelAdjust(FXObject*,FXSelector,void*);
	long onUpdWheelAdjust(FXObject*,FXSelector,void*);
	long onUpdSingleClickFileopen(FXObject*,FXSelector,void*);
	FXuint execute(FXuint);
	long onCmdCancel(FXObject*,FXSelector,void*);
	long onCmdControls(FXObject*,FXSelector ,void*);
	long onUpdControls(FXObject*,FXSelector,void*);
	long onCmdChangeKeyBindings(FXObject*,FXSelector,void*);
	long onCmdRestoreKeyBindings(FXObject*,FXSelector,void*);
};
#endif
