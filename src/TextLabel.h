#ifndef TEXTLABEL_H
#define TEXTLABEL_H


class FXAPI TextLabel : public FXFrame
{
    FXDECLARE(TextLabel)
protected:
    FXString      contents;       // Edited text
    const FXchar *delimiters;     // Set of delimiters
    FXFont       *font;           // Text font
    FXColor       textColor;      // Text color
    FXColor       selbackColor;   // Selected background color
    FXColor       seltextColor;   // Selected text color
    FXColor       cursorColor;    // Color of the Cursor
    FXint         cursor;         // Cursor position
    FXint         anchor;         // Anchor position
    FXint         columns;        // Number of columns visible
    FXint         shift;          // Shift amount
    FXString      clipped;        // Clipped text
protected:
    TextLabel()
    {}
    FXint index(FXint x) const;
    FXint coord(FXint i) const;
    void drawTextRange(FXDCWindow& dc,FXint fm,FXint to);
    void drawTextFragment(FXDCWindow& dc,FXint x,FXint y,FXint fm,FXint to);
    FXint rightWord(FXint pos) const;
    FXint leftWord(FXint pos) const;
    FXint wordStart(FXint pos) const;
    FXint wordEnd(FXint pos) const;
private:
    TextLabel(const TextLabel&);
    TextLabel& operator=(const TextLabel&);
public:
    long onPaint(FXObject*,FXSelector,void*);
    long onUpdate(FXObject*,FXSelector,void*);
    long onKeyPress(FXObject*,FXSelector,void*);
    long onKeyRelease(FXObject*,FXSelector,void*);
    long onLeftBtnPress(FXObject*,FXSelector,void*);
    long onLeftBtnRelease(FXObject*,FXSelector,void*);
    long onMotion(FXObject*,FXSelector,void*);
    long onSelectionLost(FXObject*,FXSelector,void*);
    long onSelectionGained(FXObject*,FXSelector,void*);
    long onSelectionRequest(FXObject*,FXSelector,void* ptr);
    long onClipboardLost(FXObject*,FXSelector,void*);
    long onClipboardGained(FXObject*,FXSelector,void*);
    long onClipboardRequest(FXObject*,FXSelector,void*);
    long onFocusSelf(FXObject*,FXSelector,void*);
    long onFocusIn(FXObject*,FXSelector,void*);
    long onFocusOut(FXObject*,FXSelector,void*);
    long onAutoScroll(FXObject*,FXSelector,void*);
    long onCmdCursorHome(FXObject*,FXSelector,void*);
    long onCmdCursorEnd(FXObject*,FXSelector,void*);
    long onCmdCursorRight(FXObject*,FXSelector,void*);
    long onCmdCursorLeft(FXObject*,FXSelector,void*);
    long onCmdCursorWordLeft(FXObject*,FXSelector,void*);
    long onCmdCursorWordRight(FXObject*,FXSelector,void*);
    long onCmdCursorWordStart(FXObject*,FXSelector,void*);
    long onCmdCursorWordEnd(FXObject*,FXSelector,void*);
    long onCmdMark(FXObject*,FXSelector,void*);
    long onCmdExtend(FXObject*,FXSelector,void*);
    long onCmdSelectAll(FXObject*,FXSelector,void*);
    long onCmdDeselectAll(FXObject*,FXSelector,void*);
    long onCmdCopySel(FXObject*,FXSelector,void*);
    long onUpdHaveSelection(FXObject*,FXSelector,void*);
    long onUpdSelectAll(FXObject*,FXSelector,void*);
public:

    // Default text delimiters
    static const FXchar textDelimiters[];

public:

    enum
    {
        ID_CURSOR_HOME=FXFrame::ID_LAST,
        ID_CURSOR_END,
        ID_CURSOR_RIGHT,
        ID_CURSOR_LEFT,
        ID_CURSOR_WORD_LEFT,
        ID_CURSOR_WORD_RIGHT,
        ID_CURSOR_WORD_START,
        ID_CURSOR_WORD_END,
        ID_MARK,
        ID_EXTEND,
        ID_SELECT_ALL,
        ID_DESELECT_ALL,
        ID_COPY_SEL,
        ID_LAST
    };

public:

    // Construct text field wide enough to display ncols columns
    TextLabel(FXComposite* p,FXint ncols,FXObject* tgt=NULL,FXSelector sel=0,FXuint opts=TEXTFIELD_NORMAL,FXint x=0,FXint y=0,FXint w=0,FXint h=0,FXint pl=DEFAULT_PAD,FXint pr=DEFAULT_PAD,FXint pt=DEFAULT_PAD,FXint pb=DEFAULT_PAD);

    // Create server-side resources
    virtual void create();

    // Perform layout
    virtual void layout();

    // Enable text field
    virtual void enable();

    // Disable text field
    virtual void disable();

    // Return default width
    virtual FXint getDefaultWidth();

    // Return default height
    virtual FXint getDefaultHeight();

    // Yes, text field may receive focus
    virtual bool canFocus() const;

    // Move the focus to this window
    virtual void setFocus();

    // Remove the focus from this window
    virtual void killFocus();

    // Set cursor position
    void setCursorPos(FXint pos);

    // Return cursor position
    FXint getCursorPos() const
    {
        return cursor;
    }

    // Change anchor position
    void setAnchorPos(FXint pos);

    // Return anchor position
    FXint getAnchorPos() const
    {
        return anchor;
    }

    // Change the text and move cursor to end
    void setText(const FXString& text,FXbool notify=FALSE);

    // Get the text for this label
    FXString getText() const
    {
        return contents;
    }

    // Set the text font
    void setFont(FXFont* fnt);

    // Get the text font
    FXFont* getFont() const
    {
        return font;
    }

    // Change text color
    void setTextColor(FXColor clr);

    // Return text color
    FXColor getTextColor() const
    {
        return textColor;
    }

    // Change selected background color
    void setSelBackColor(FXColor clr);

    // Return selected background color
    FXColor getSelBackColor() const
    {
        return selbackColor;
    }

    // Change selected text color
    void setSelTextColor(FXColor clr);

    // Return selected text color
    FXColor getSelTextColor() const
    {
        return seltextColor;
    }

    // Changes the cursor color
    void setCursorColor(FXColor clr);

    // Return the cursor color
    FXColor getCursorColor() const
    {
        return cursorColor;
    }

    /*
    * Change the default width of the text field in terms of a number
    * of columns times the width of the numeral '8'.
    */
    void setNumColumns(FXint cols);

    // Return number of columns
    FXint getNumColumns() const
    {
        return columns;
    }

    /*
    * Change text justification mode. The justify mode is a combination of
    * horizontal justification (JUSTIFY_LEFT, JUSTIFY_RIGHT, or JUSTIFY_CENTER_X),
    * and vertical justification (JUSTIFY_TOP, JUSTIFY_BOTTOM, JUSTIFY_CENTER_Y).
    * Note that JUSTIFY_CENTER_X can not be set from the constructor since by
    * default text fields are left-justified.
    */
    void setJustify(FXuint mode);

    // Return text justification mode
    FXuint getJustify() const;

    // Change word delimiters
    void setDelimiters(const FXchar* delims=textDelimiters)
    {
        delimiters=delims;
    }

    // Return word delimiters
    const FXchar* getDelimiters() const
    {
        return delimiters;
    }

    // Select all text
    FXbool selectAll();

    // Select len characters starting at given position pos
    FXbool setSelection(FXint pos,FXint len);

    // Extend the selection from the anchor to the given position
    FXbool extendSelection(FXint pos);

    // Unselect the text
    FXbool killSelection();

    // Return TRUE if position pos is selected
    FXbool isPosSelected(FXint pos) const;

    // Return TRUE if position is fully visible
    FXbool isPosVisible(FXint pos) const;

    // Scroll text to make the given position visible
    void makePositionVisible(FXint pos);

    // Destructor
    virtual ~TextLabel();
};

#endif
