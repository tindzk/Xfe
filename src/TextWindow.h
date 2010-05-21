#ifndef TEXTWINDOW_H
#define TEXTWINDOW_H

#include "DialogBox.h"

class TextWindow : public DialogBox
{
    FXDECLARE(TextWindow)
protected:
    FXText *text;
private:
    TextWindow()
    {}
    TextWindow(const TextWindow&);
public:
    enum{
        ID_CLOSE=DialogBox::ID_LAST,
        ID_LAST
    };
public:
    TextWindow(FXWindow* owner, const FXString& name, FXint nblines, FXint nbcols);
    TextWindow(FXApp* app, const FXString& name, FXint nblines, FXint nbcols);
    virtual ~TextWindow();
	void setText(const FXchar*);
	void appendText(const FXchar*);
	void scrollToLastLine(void);
	void setFont(FXFont*);
	FXint getLength(void);
};

#endif
