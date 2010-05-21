#ifndef KEYBINDINGS_H
#define KEYBINDINGS_H

#include "DialogBox.h"
#include "IconList.h"


class KeybindingsBox : public DialogBox
{
    FXDECLARE(KeybindingsBox)
protected:
	IconList* glbBindingsList;
	IconList* xfeBindingsList;
	IconList* xfiBindingsList;
	IconList* xfwBindingsList;
	FXStringDict* glbBindingsDict;
	FXStringDict* xfeBindingsDict;
	FXStringDict* xfiBindingsDict;
	FXStringDict* xfwBindingsDict;
	FXStringDict* glbBindingsDict_prev;
	FXStringDict* xfeBindingsDict_prev;
	FXStringDict* xfiBindingsDict_prev;
	FXStringDict* xfwBindingsDict_prev;
	FXbool changed;

private:
    KeybindingsBox()
    {
        ;
    }
public:
    enum{
        ID_ACCEPT=DialogBox::ID_LAST,
		ID_CANCEL,
        ID_GLB_BINDINGS_LIST,
        ID_XFE_BINDINGS_LIST,
		ID_XFI_BINDINGS_LIST,
		ID_XFW_BINDINGS_LIST,
		ID_GLB_SORT_BY_ACTIONNAME,
		ID_GLB_SORT_BY_REGISTRYKEY,
		ID_GLB_SORT_BY_KEYBINDING,
		ID_XFE_SORT_BY_ACTIONNAME,
		ID_XFE_SORT_BY_REGISTRYKEY,
		ID_XFE_SORT_BY_KEYBINDING,
		ID_XFI_SORT_BY_ACTIONNAME,
		ID_XFI_SORT_BY_REGISTRYKEY,
		ID_XFI_SORT_BY_KEYBINDING,
		ID_XFW_SORT_BY_ACTIONNAME,
		ID_XFW_SORT_BY_REGISTRYKEY,
		ID_XFW_SORT_BY_KEYBINDING,
        ID_LAST
    };
    KeybindingsBox(FXWindow*,FXStringDict*,FXStringDict*,FXStringDict*,FXStringDict*);
    virtual void create();
    virtual ~KeybindingsBox();
	FXuint execute(FXuint);
    long onCmdAccept(FXObject*,FXSelector,void*);
    long onCmdCancel(FXObject*,FXSelector,void*);
	long onCmdDefineGlbKeybindings(FXObject*,FXSelector,void*);
	long onCmdDefineXfeKeybindings(FXObject*,FXSelector,void*);
	long onCmdDefineXfiKeybindings(FXObject*,FXSelector,void*);
	long onCmdDefineXfwKeybindings(FXObject*,FXSelector,void*);
	long onCmdGlbSortByActionName(FXObject*,FXSelector,void*);
	long onCmdGlbSortByRegistryKey(FXObject*,FXSelector,void*);
	long onCmdGlbSortByKeyBinding(FXObject*,FXSelector,void*);
	long onCmdXfeSortByActionName(FXObject*,FXSelector,void*);
	long onCmdXfeSortByRegistryKey(FXObject*,FXSelector,void*);
	long onCmdXfeSortByKeyBinding(FXObject*,FXSelector,void*);
	long onCmdXfiSortByActionName(FXObject*,FXSelector,void*);
	long onCmdXfiSortByRegistryKey(FXObject*,FXSelector,void*);
	long onCmdXfiSortByKeyBinding(FXObject*,FXSelector,void*);
	long onCmdXfwSortByActionName(FXObject*,FXSelector,void*);
	long onCmdXfwSortByRegistryKey(FXObject*,FXSelector,void*);
	long onCmdXfwSortByKeyBinding(FXObject*,FXSelector,void*);
	long onCmdGlbHeaderClicked(FXObject*,FXSelector,void*);
	long onCmdXfeHeaderClicked(FXObject*,FXSelector,void*);
	long onCmdXfiHeaderClicked(FXObject*,FXSelector,void*);
	long onCmdXfwHeaderClicked(FXObject*,FXSelector,void*);
	long onUpdGlbHeader(FXObject*,FXSelector,void*);
	long onUpdXfeHeader(FXObject*,FXSelector,void*);
	long onUpdXfiHeader(FXObject*,FXSelector,void*);
	long onUpdXfwHeader(FXObject*,FXSelector,void*);
public:
	static FXint compareSection(const FXchar *p,const FXchar* q,FXint s);
	static FXint ascendingActionName(const IconItem* a,const IconItem* b);
	static FXint descendingActionName(const IconItem* a,const IconItem* b);
	static FXint ascendingRegistryKey(const IconItem* a,const IconItem* b);
	static FXint descendingRegistryKey(const IconItem* a,const IconItem* b);
	static FXint ascendingKeybinding(const IconItem* a,const IconItem* b);
	static FXint descendingKeybinding(const IconItem* a,const IconItem* b);
};
#endif
