// Input dialog for the add to archive command

#include "config.h"
#include "i18n.h"

#include <fx.h>
#include <fxkeys.h>

#include "icons.h"
#include "xfeutils.h"
#include "FileDialog.h"
#include "ArchInputDialog.h"



FXDEFMAP(ArchInputDialog) ArchInputDialogMap[]=
{
	FXMAPFUNC(SEL_KEYPRESS,0,ArchInputDialog::onCmdKeyPress),
	FXMAPFUNC(SEL_COMMAND,ArchInputDialog::ID_BROWSE_PATH,ArchInputDialog::onCmdBrowsePath),
	FXMAPFUNCS(SEL_COMMAND,ArchInputDialog::ID_FORMAT_TAR_GZ,ArchInputDialog::ID_FORMAT_ARJ,ArchInputDialog::onCmdOption),
	FXMAPFUNCS(SEL_UPDATE,ArchInputDialog::ID_FORMAT_TAR_GZ,ArchInputDialog::ID_FORMAT_ARJ,ArchInputDialog::onUpdOption),
};


// Object implementation
FXIMPLEMENT(ArchInputDialog,DialogBox,ArchInputDialogMap,ARRAYNUMBER(ArchInputDialogMap))

// Construct a dialog box
ArchInputDialog::ArchInputDialog(FXWindow *win,FXString inp):
        DialogBox(win,_("Add To Archive"),DECOR_TITLE|DECOR_BORDER|DECOR_STRETCHABLE)
{
    // Buttons
    FXHorizontalFrame *buttons=new FXHorizontalFrame(this,PACK_UNIFORM_WIDTH|LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X,0,0,0,0,10,10,5,5);
    
	// Accept
    new FXButton(buttons,_("&Accept"),NULL,this,ID_ACCEPT,FRAME_RAISED|FRAME_THICK|LAYOUT_RIGHT,0,0,0,0,20,20);
    
	// Cancel
    new FXButton(buttons,_("&Cancel"),NULL,this,ID_CANCEL,FRAME_RAISED|FRAME_THICK|LAYOUT_RIGHT,0,0,0,0,20,20);
    
	// Separator
    new FXHorizontalSeparator(this,LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X|SEPARATOR_GROOVE);

    // Vertical frame
	FXVerticalFrame *contents=new FXVerticalFrame(this,LAYOUT_SIDE_TOP|FRAME_NONE|LAYOUT_FILL_X|LAYOUT_FILL_Y);

	// Icon and message line
	FXMatrix *matrix = new FXMatrix(contents,2,MATRIX_BY_COLUMNS|LAYOUT_SIDE_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y);
	new FXLabel(matrix,"",bigarchaddicon,LAYOUT_LEFT);    
    new FXLabel(matrix,_("New archive name:"),NULL,JUSTIFY_LEFT|LAYOUT_CENTER_Y|LAYOUT_FILL_COLUMN|LAYOUT_FILL_ROW);

	// Label and input field
	FXMatrix *matrix3 = new FXMatrix(contents,2,MATRIX_BY_COLUMNS|LAYOUT_SIDE_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y);
	input = new FXTextField(matrix3,40,0,0,LAYOUT_CENTER_Y|LAYOUT_CENTER_X|FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_COLUMN|LAYOUT_FILL_ROW|LAYOUT_FILL_X);
	input->setText(inp);
	if (!isUtf8(inp.text(),inp.length()))
		new FXLabel(contents,_("=> Warning: file name is not UTF-8 encoded!"),NULL,LAYOUT_LEFT|LAYOUT_CENTER_Y|LAYOUT_FILL_ROW);
	new FXButton(matrix3,_("\tSelect destination..."),filedialogicon,this,ID_BROWSE_PATH,FRAME_RAISED|FRAME_THICK|LAYOUT_RIGHT|LAYOUT_CENTER_Y,0,0,0,0,20,20);
	
	// Select archive format
	FXMatrix *matrix4 = new FXMatrix(contents,2,MATRIX_BY_COLUMNS|LAYOUT_SIDE_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y);
	new FXLabel(matrix4,_("Format:"),NULL,LAYOUT_LEFT|LAYOUT_CENTER_Y|LAYOUT_FILL_ROW);
	popup=new FXPopup(this);
	option_tgz = new FXOption(popup,_("tar.gz\tArchive format is tar.gz"),NULL,this,ID_FORMAT_TAR_GZ,JUSTIFY_HZ_APART|ICON_AFTER_TEXT);
	option_zip = new FXOption(popup,_("zip\tArchive format is zip"),NULL,this,ID_FORMAT_ZIP,JUSTIFY_HZ_APART|ICON_AFTER_TEXT);
	option_7zip = new FXOption(popup,_("7z\tArchive format is 7z"),NULL,this,ID_FORMAT_7ZIP,JUSTIFY_HZ_APART|ICON_AFTER_TEXT);
	option_tbz2 = new FXOption(popup,_("tar.bz2\tArchive format is tar.bz2"),NULL,this,ID_FORMAT_TAR_BZ2,JUSTIFY_HZ_APART|ICON_AFTER_TEXT);
	option_tar = new FXOption(popup,_("tar\tArchive format is tar"),NULL,this,ID_FORMAT_TAR,JUSTIFY_HZ_APART|ICON_AFTER_TEXT);
	option_taz = new FXOption(popup,_("tar.Z\tArchive format is tar.Z"),NULL,this,ID_FORMAT_TAR_Z,JUSTIFY_HZ_APART|ICON_AFTER_TEXT);
	option_gz = new FXOption(popup,_("gz\tArchive format is gz"),NULL,this,ID_FORMAT_GZ,JUSTIFY_HZ_APART|ICON_AFTER_TEXT);
	option_bz2 = new FXOption(popup,_("bz2\tArchive format is bz2"),NULL,this,ID_FORMAT_BZ2,JUSTIFY_HZ_APART|ICON_AFTER_TEXT);
	option_z = new FXOption(popup,_("Z\tArchive format is Z"),NULL,this,ID_FORMAT_Z,JUSTIFY_HZ_APART|ICON_AFTER_TEXT);
	option_rar = new FXOption(popup,_("rar\tArchive format is rar"),NULL,this,ID_FORMAT_RAR,JUSTIFY_HZ_APART|ICON_AFTER_TEXT);
	option_lzh = new FXOption(popup,_("lzh\tArchive format is lzh"),NULL,this,ID_FORMAT_LZH,JUSTIFY_HZ_APART|ICON_AFTER_TEXT);
	option_arj = new FXOption(popup,_("arj\tArchive format is arj"),NULL,this,ID_FORMAT_ARJ,JUSTIFY_HZ_APART|ICON_AFTER_TEXT);
	optionmenu = new FXOptionMenu(matrix4,popup,LAYOUT_TOP|FRAME_RAISED|FRAME_THICK|JUSTIFY_HZ_APART|ICON_AFTER_TEXT);
}

void ArchInputDialog::create() {
	DialogBox::create();
	input->setFocus();
}

ArchInputDialog::~ArchInputDialog() {
	delete popup;
}

long ArchInputDialog::onCmdKeyPress(FXObject* sender,FXSelector sel,void* ptr) {
	FXEvent* event=(FXEvent*)ptr;

	switch(event->code) {
		case KEY_Escape:
			handle(this,FXSEL(SEL_COMMAND,ID_CANCEL),NULL);
			return 1;
		case KEY_KP_Enter:
		case KEY_Return:
			handle(this,FXSEL(SEL_COMMAND,ID_ACCEPT),NULL);
			return 1;
		default:
			FXTopWindow::onKeyPress(sender,sel,ptr);
			return 1;
	}

	return 0;
}

long ArchInputDialog::onCmdBrowsePath(FXObject* o,FXSelector s,void* p) {
	FileDialog browse(this, "Select a destination file");

	const FXchar *patterns[] = {
		"All Files", "*",
		NULL
	};

	browse.setPatternList(patterns);

	// Browse files in mixed mode
	browse.setSelectMode(SELECT_FILE_ANY);

	if (browse.execute()) {
		FXString path = browse.getFilename();
		FXString ext  = FXPath::name(input->getText());
		input->setText(path + ext);
	}


	return 1;
}

// Archive option
long ArchInputDialog::onCmdOption(FXObject*,FXSelector sel,void*) {
	// Get extensions of the archive name
	FXString str=input->getText();
	FXString ext1=str.rafter('.',1);
	ext1.lower();
	FXString ext2=str.rafter('.',2);
	ext2.lower();

	if (FXSELID(sel)==ID_FORMAT_TAR_GZ)
	{
		// Handle the different archive formats
		if (ext2=="tar.gz" || ext1=="tgz")
		{
			input->setText(str);
		}
		else if (ext2=="tar.bz2")
		{
			str=str.left(str.length()-8);
			str=str+".tar.gz";
			input->setText(str);
		}
		else if (ext2=="tar.z")
		{
			str=str.left(str.length()-6);
			str=str+".tar.gz";
			input->setText(str);
		}
		else if (ext1=="tbz2")
		{
			str=str.left(str.length()-5);
			str=str+".tar.gz";
			input->setText(str);
		}
		else if (ext1=="bz2" || ext1=="tar" || ext1=="zip" || ext1=="7z" || ext1=="rar" || ext1=="lzh" || ext1=="arj")
		{
			str=str.left(str.length()-4);
			str=str+".tar.gz";
			input->setText(str);
		}
		else if (ext1=="gz")
		{
			str=str.left(str.length()-3);
			str=str+".tar.gz";
			input->setText(str);
		}
		else if (ext1=="z")
		{
			str=str.left(str.length()-2);
			str=str+".tar.gz";
			input->setText(str);
		}
		else
		{			
			str=str+".tar.gz";
			input->setText(str);
		}
		
	}
	
	else if (FXSELID(sel)==ID_FORMAT_TAR_BZ2)
	{

		// Handle different archive formats
		if (ext2=="tar.bz2" || ext1=="tbz2")
		{
			input->setText(str);
		}
		else if (ext2=="tar.gz")
		{
			str=str.left(str.length()-7);
			str=str+".tar.bz2";
			input->setText(str);
		}
		else if (ext2=="tar.z")
		{
			str=str.left(str.length()-6);
			str=str+".tar.bz2";
			input->setText(str);
		}
		else if (ext1=="tgz" || ext1=="bz2" || ext1=="tar" || ext1=="zip" || ext1=="7z" || ext1=="rar" || ext1=="lzh" || ext1=="arj")
		{
			str=str.left(str.length()-4);
			str=str+".tar.bz2";
			input->setText(str);
		}
		else if (ext1=="gz")
		{
			str=str.left(str.length()-3);
			str=str+".tar.bz2";
			input->setText(str);
		}
		else if (ext1=="z")
		{
			str=str.left(str.length()-2);
			str=str+".tar.bz2";
			input->setText(str);
		}
		else
		{			
			str=str+".tar.bz2";
			input->setText(str);
		}
	}

	else if (FXSELID(sel)==ID_FORMAT_TAR)
	{
		// Handle different archive formats
		if (ext1=="tar" && ext2!="tar.gz" && ext2!="tar.bz2" && ext2!="tar.z")
		{
			input->setText(str);
		}
		else if (ext2=="tar.bz2")
		{
			str=str.left(str.length()-8);
			str=str+".tar";
			input->setText(str);
		}
		else if (ext2=="tar.gz")
		{
			str=str.left(str.length()-7);
			str=str+".tar";
			input->setText(str);
		}
		else if (ext2=="tar.z")
		{
			str=str.left(str.length()-6);
			str=str+".tar";
			input->setText(str);
		}
		else if (ext1=="tbz2")
		{
			str=str.left(str.length()-5);
			str=str+".tar";
			input->setText(str);
		}
		else if (ext1=="tgz" || ext1=="bz2" || ext1=="zip" || ext1=="7z" || ext1=="rar" || ext1=="lzh" || ext1=="arj")
		{
			str=str.left(str.length()-4);
			str=str+".tar";
			input->setText(str);
		}
		else if (ext1=="gz")
		{
			str=str.left(str.length()-3);
			str=str+".tar";
			input->setText(str);
		}
		else if (ext1=="z")
		{
			str=str.left(str.length()-2);
			str=str+".tar";
			input->setText(str);
		}
		else
		{			
			str=str+".tar";
			input->setText(str);
		}
	}
	
	else if (FXSELID(sel)==ID_FORMAT_TAR_Z)
	{
		// Handle different archive formats
		if (ext2=="tar.Z")
		{
			input->setText(str);
		}
		else if (ext2=="tar.bz2")
		{
			str=str.left(str.length()-8);
			str=str+".tar.Z";
			input->setText(str);
		}
		else if (ext2=="tar.gz")
		{
			str=str.left(str.length()-7);
			str=str+".tar.Z";
			input->setText(str);
		}
		else if (ext1=="tbz2")
		{
			str=str.left(str.length()-5);
			str=str+".tar.Z";
			input->setText(str);
		}
		else if (ext1=="tgz" || ext1=="bz2" || ext1=="tar" || ext1=="zip" || ext1=="7z" || ext1=="rar" || ext1=="lzh" || ext1=="arj")
		{
			str=str.left(str.length()-4);
			str=str+".tar.Z";
			input->setText(str);
		}
		else if (ext1=="gz")
		{
			str=str.left(str.length()-3);
			str=str+".tar.Z";
			input->setText(str);
		}
		else if (ext1=="z")
		{
			str=str.left(str.length()-2);
			str=str+".tar.Z";
			input->setText(str);
		}
		else
		{			
			str=str+".tar.Z";
			input->setText(str);
		}
	}

	else if (FXSELID(sel)==ID_FORMAT_GZ)
	{
		// Handle different archive formats
		if (ext1=="gz" && ext2!="tar.gz")
		{
			input->setText(str);
		}
		else if (ext2=="tar.bz2")
		{
			str=str.left(str.length()-8);
			str=str+".gz";
			input->setText(str);
		}
		else if (ext2=="tar.gz")
		{
			str=str.left(str.length()-7);
			str=str+".gz";
			input->setText(str);
		}
		else if (ext2=="tar.z")
		{
			str=str.left(str.length()-6);
			str=str+".gz";
			input->setText(str);
		}
		else if (ext1=="tbz2")
		{
			str=str.left(str.length()-5);
			str=str+".gz";
			input->setText(str);
		}
		else if (ext1=="tgz" || ext1=="bz2" || ext1=="tar" || ext1=="zip" || ext1=="7z" || ext1=="rar" || ext1=="lzh" || ext1=="arj")
		{
			str=str.left(str.length()-4);
			str=str+".gz";
			input->setText(str);
		}
		else if (ext1=="z")
		{
			str=str.left(str.length()-2);
			str=str+".gz";
			input->setText(str);
		}
		else
		{			
			str=str+".gz";
			input->setText(str);
		}
	}

	else if (FXSELID(sel)==ID_FORMAT_BZ2)
	{
		// Handle different archive formats
		if (ext1=="bz2" && ext2!="tar.bz2")
		{
			str=str.left(str.length()-8);
			input->setText(str);
		}
		if (ext2=="tar.bz2")
		{
			str=str.left(str.length()-8);
			str=str+".bz2";
			input->setText(str);
		}
		else if (ext2=="tar.gz")
		{
			str=str.left(str.length()-7);
			str=str+".bz2";
			input->setText(str);
		}
		else if (ext2=="tar.z")
		{
			str=str.left(str.length()-6);
			str=str+".bz2";
			input->setText(str);
		}
		else if (ext1=="tbz2")
		{
			str=str.left(str.length()-5);
			str=str+".bz2";
			input->setText(str);
		}
		else if (ext1=="tgz" || ext1=="tar" || ext1=="zip" || ext1=="7z" || ext1=="rar" || ext1=="lzh" || ext1=="arj")
		{
			str=str.left(str.length()-4);
			str=str+".bz2";
			input->setText(str);
		}
		else if (ext1=="gz")
		{
			str=str.left(str.length()-3);
			str=str+".bz2";
			input->setText(str);
		}
		else if (ext1=="z")
		{
			str=str.left(str.length()-2);
			str=str+".bz2";
			input->setText(str);
		}
		else
		{			
			str=str+".bz2";
			input->setText(str);
		}
	}

	else if (FXSELID(sel)==ID_FORMAT_Z)
	{
		// Handle different archive formats
		if (ext1=="z" && ext2!="tar.z")
		{
			input->setText(str);
		}
		else if (ext2=="tar.bz2")
		{
			str=str.left(str.length()-8);
			str=str+".Z";
			input->setText(str);
		}
		else if (ext2=="tar.gz")
		{
			str=str.left(str.length()-7);
			str=str+".Z";
			input->setText(str);
		}
		else if (ext2=="tar.z")
		{
			str=str.left(str.length()-6);
			str=str+".Z";
			input->setText(str);
		}
		else if (ext1=="tbz2")
		{
			str=str.left(str.length()-5);
			str=str+".Z";
			input->setText(str);
		}
		else if (ext1=="tgz" || ext1=="bz2" || ext1=="tar" || ext1=="zip" || ext1=="7z" || ext1=="rar" || ext1=="lzh" || ext1=="arj")
		{
			str=str.left(str.length()-4);
			str=str+".Z";
			input->setText(str);
		}
		else if (ext1=="gz")
		{
			str=str.left(str.length()-3);
			str=str+".Z";
			input->setText(str);
		}
		else
		{			
			str=str+".Z";
			input->setText(str);
		}
	}
	
	else if (FXSELID(sel)==ID_FORMAT_ZIP)
	{
		// Handle different archive formats
		if (ext1=="zip")
		{
			input->setText(str);
		}
		else if (ext2=="tar.bz2")
		{
			str=str.left(str.length()-8);
			str=str+".zip";
			input->setText(str);
		}
		else if (ext2=="tar.gz")
		{
			str=str.left(str.length()-7);
			str=str+".zip";
			input->setText(str);
		}
		else if (ext2=="tar.z")
		{
			str=str.left(str.length()-6);
			str=str+".zip";
			input->setText(str);
		}
		else if (ext1=="tbz2")
		{
			str=str.left(str.length()-5);
			str=str+".zip";
			input->setText(str);
		}
		else if (ext1=="tgz" || ext1=="bz2" || ext1=="tar" || ext1=="7z" || ext1=="rar" || ext1=="lzh" || ext1=="arj")
		{
			str=str.left(str.length()-4);
			str=str+".zip";
			input->setText(str);
		}
		else if (ext1=="gz")
		{
			str=str.left(str.length()-3);
			str=str+".zip";
			input->setText(str);
		}
		else if (ext1=="z")
		{
			str=str.left(str.length()-2);
			str=str+".zip";
			input->setText(str);
		}
		else
		{			
			str=str+".zip";
			input->setText(str);
		}
	}

	else if (FXSELID(sel)==ID_FORMAT_7ZIP)
	{
		// Handle different archive formats
		if (ext1=="7z")
		{
			input->setText(str);
		}
		else if (ext2=="tar.bz2")
		{
			str=str.left(str.length()-8);
			str=str+".7z";
			input->setText(str);
		}
		else if (ext2=="tar.gz")
		{
			str=str.left(str.length()-7);
			str=str+".7z";
			input->setText(str);
		}
		else if (ext2=="tar.z")
		{
			str=str.left(str.length()-6);
			str=str+".7z";
			input->setText(str);
		}
		else if (ext1=="tbz2")
		{
			str=str.left(str.length()-5);
			str=str+".7z";
			input->setText(str);
		}
		else if (ext1=="tgz" || ext1=="bz2" || ext1=="tar" || ext1=="zip" || ext1=="rar" || ext1=="lzh" || ext1=="arj")
		{
			str=str.left(str.length()-4);
			str=str+".7z";
			input->setText(str);
		}
		else if (ext1=="gz")
		{
			str=str.left(str.length()-3);
			str=str+".7z";
			input->setText(str);
		}
		else if (ext1=="z")
		{
			str=str.left(str.length()-2);
			str=str+".7z";
			input->setText(str);
		}
		else
		{			
			str=str+".7z";
			input->setText(str);
		}
	}

	else if (FXSELID(sel)==ID_FORMAT_RAR)
	{
		// Handle different archive formats
		if (ext1=="rar")
		{
			input->setText(str);
		}
		else if (ext2=="tar.bz2")
		{
			str=str.left(str.length()-8);
			str=str+".rar";
			input->setText(str);
		}
		else if (ext2=="tar.gz")
		{
			str=str.left(str.length()-7);
			str=str+".rar";
			input->setText(str);
		}
		else if (ext2=="tar.z")
		{
			str=str.left(str.length()-6);
			str=str+".rar";
			input->setText(str);
		}
		else if (ext1=="tbz2")
		{
			str=str.left(str.length()-5);
			str=str+".rar";
			input->setText(str);
		}
		else if (ext1=="tgz" || ext1=="bz2" || ext1=="tar" || ext1=="zip" || ext1=="7z" || ext1=="lzh" || ext1=="arj")
		{
			str=str.left(str.length()-4);
			str=str+".rar";
			input->setText(str);
		}
		else if (ext1=="gz")
		{
			str=str.left(str.length()-3);
			str=str+".rar";
			input->setText(str);
		}
		else if (ext1=="z")
		{
			str=str.left(str.length()-2);
			str=str+".rar";
			input->setText(str);
		}
		else
		{			
			str=str+".rar";
			input->setText(str);
		}
	}

	else if (FXSELID(sel)==ID_FORMAT_LZH)
	{
		// Handle different archive formats
		if (ext1=="lzh")
		{
			input->setText(str);
		}
		else if (ext2=="tar.bz2")
		{
			str=str.left(str.length()-8);
			str=str+".lzh";
			input->setText(str);
		}
		else if (ext2=="tar.gz")
		{
			str=str.left(str.length()-7);
			str=str+".lzh";
			input->setText(str);
		}
		else if (ext2=="tar.z")
		{
			str=str.left(str.length()-6);
			str=str+".lzh";
			input->setText(str);
		}
		else if (ext1=="tbz2")
		{
			str=str.left(str.length()-5);
			str=str+".lzh";
			input->setText(str);
		}
		else if (ext1=="tgz" || ext1=="bz2" || ext1=="tar" || ext1=="zip" || ext1=="7z" || ext1=="rar" || ext1=="arj")
		{
			str=str.left(str.length()-4);
			str=str+".lzh";
			input->setText(str);
		}
		else if (ext1=="gz")
		{
			str=str.left(str.length()-3);
			str=str+".lzh";
			input->setText(str);
		}
		else if (ext1=="z")
		{
			str=str.left(str.length()-2);
			str=str+".lzh";
			input->setText(str);
		}
		else
		{			
			str=str+".lzh";
			input->setText(str);
		}
	}
	else if (FXSELID(sel)==ID_FORMAT_ARJ)
	{
		// Handle different archive formats
		if (ext1=="arj")
		{
			input->setText(str);
		}
		else if (ext2=="tar.bz2")
		{
			str=str.left(str.length()-8);
			str=str+".arj";
			input->setText(str);
		}
		else if (ext2=="tar.gz")
		{
			str=str.left(str.length()-7);
			str=str+".arj";
			input->setText(str);
		}
		else if (ext2=="tar.z")
		{
			str=str.left(str.length()-6);
			str=str+".arj";
			input->setText(str);
		}
		else if (ext1=="tbz2")
		{
			str=str.left(str.length()-5);
			str=str+".arj";
			input->setText(str);
		}
		else if (ext1=="tgz" || ext1=="bz2" || ext1=="tar" || ext1=="zip" || ext1=="7z" || ext1=="rar" || ext1=="lzh")
		{
			str=str.left(str.length()-4);
			str=str+".arj";
			input->setText(str);
		}
		else if (ext1=="gz")
		{
			str=str.left(str.length()-3);
			str=str+".arj";
			input->setText(str);
		}
		else if (ext1=="z")
		{
			str=str.left(str.length()-2);
			str=str+".arj";
			input->setText(str);
		}
		else
		{			
			str=str+".arj";
			input->setText(str);
		}
	}
			
	return 1;
}


// Option
long ArchInputDialog::onUpdOption(FXObject*,FXSelector sel,void*)
{
	// Get extensions of the archive name
	FXString str=input->getText();
	FXString ext1=str.rafter('.',1);
	ext1.lower();
	FXString ext2=str.rafter('.',2);
	ext2.lower();

	// Handle the different archive formats
	if (ext2=="tar.gz" || ext1=="tgz")
	{
		optionmenu->setCurrent(option_tgz);	
	}
	else if (ext2=="tar.bz2" || ext1=="tbz2")
	{
		optionmenu->setCurrent(option_tbz2);	
	}
	else if (ext2=="tar.z" || ext1=="taz")
	{
		optionmenu->setCurrent(option_taz);	
	}
	else if (ext2=="" || ext1=="tar")
	{
		optionmenu->setCurrent(option_tar);	
	}
	else if (ext1=="gz")
	{
		optionmenu->setCurrent(option_gz);	
	}
	else if (ext1=="bz2")
	{
		optionmenu->setCurrent(option_bz2);	
	}
	else if (ext1=="z")
	{
		optionmenu->setCurrent(option_z);	
	}
	else if (ext1=="zip")
	{
		optionmenu->setCurrent(option_zip);	
	}
	else if (ext1=="7z")
	{
		optionmenu->setCurrent(option_7zip);	
	}
	else if (ext1=="rar")
	{
		optionmenu->setCurrent(option_rar);	
	}
	else if (ext1=="lzh")
	{
		optionmenu->setCurrent(option_lzh);	
	}
	else if (ext1=="arj")
	{
		optionmenu->setCurrent(option_arj);	
	}
	else
	{			
		optionmenu->setCurrent(option_tgz);	
	}
			
	return 1;
}

