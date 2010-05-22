
bin_PROGRAMS = xfe

xfe_SOURCES = ../xvt/command.c \
              ../xvt/screen.c \
              ../xvt/ttyinit.c \
              ../xvt/xsetup.c \
              ../xvt/xvt.c \
              icons.cpp \
			  xfeutils.cpp \
			  startupnotification.cpp \
			  StringList.cpp \
              File.cpp \
			  FileDict.cpp \
			  IconList.cpp \
			  FileList.cpp \
			  FileDialog.cpp \
			  DirList.cpp \
			  DialogBox.cpp \
			  MessageBox.cpp \
			  Bookmarks.cpp \
			  HistInputDialog.cpp \
			  InputDialog.cpp \
			  OverwriteBox.cpp \
			  CommandWindow.cpp \
              Properties.cpp \
			  Preferences.cpp \
			  FilePanel.cpp \
			  DirPanel.cpp \
			  DirHistBox.cpp \
			  PathLinker.cpp \
			  BrowseInputDialog.cpp \
			  ArchInputDialog.cpp \
			  FontDialog.cpp \
			  TextLabel.cpp \
			  Keybindings.cpp \
			  KeybindingsDialog.cpp \
			  XFileExplorer.cpp \
              main.cpp

xfe_LDADD = @LIBINTL@ -lutil

localedir = $(datadir)/locale

INCLUDES = -I. -I$(top_srcdir) -I$(top_srcdir)/intl
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

EXTRA_DIST =  ../xvt/command.h \
			  ../xvt/screen.h \
			  ../xvt/token.h \
			  ../xvt/xsetup.h \
			  ../xvt/ttyinit.h \
			  ../xvt/xvt.h \
			  xfedefs.h \
              icons.h \
			  xfeutils.h \
			  startupnotification.h \
			  StringList.h \
			  FileDialog.h \
			  FileDict.h \
			  IconList.h \
			  FileList.h \
			  DirList.h \
			  DirPanel.h \
			  Properties.h \
              File.h \
			  DialogBox.h \
			  MessageBox.h \
			  HistInputDialog.h \
			  InputDialog.h \
			  Preferences.h \
			  CommandWindow.h \
			  OverwriteBox.h \
			  FilePanel.h \
			  Bookmarks.h \
			  XFileExplorer.h \
			  XFileImage.h \
			  XFilePackage.h \
			  ViewWindow.h \
			  XFileView.h \
			  EditWindow.h \
			  XFileWrite.h \
			  DirHistBox.h \
			  PathLinker.h \
			  BrowseInputDialog.h \
			  ArchInputDialog.h \
			  FontDialog.h \
			  TextLabel.h \
			  Keybindings.h \
			  KeybindingsDialog.h \
			  foxhacks.cpp
