
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
			  TextWindow.cpp \
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

if STARTUPNOTIFY
xfe_SOURCES += ../libsn/sn-common.c  \
              ../libsn/sn-launchee.c \
              ../libsn/sn-launcher.c \
              ../libsn/sn-list.c \
              ../libsn/sn-monitor.c \
              ../libsn/sn-util.c \
              ../libsn/sn-xmessages.c \
              ../libsn/sn-xutils.c
endif

xfe_LDADD = @LIBINTL@ -lutil

localedir = $(datadir)/locale

INCLUDES = -I. -I$(top_srcdir) -I$(top_srcdir)/intl
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

EXTRA_DIST =  ../libsn/sn-common.h	\
	          ../libsn/sn-internals.h \
	          ../libsn/sn-launchee.h \
			  ../libsn/sn-launcher.h \
	          ../libsn/sn-list.h \
	          ../libsn/sn-monitor.h \
	          ../libsn/sn-util.h \
	          ../libsn/sn-xmessages.h \
			  ../libsn/sn-xutils.h \
			  ../libsn/sn.h \
			  ../xvt/command.h \
			  ../xvt/screen.h \
			  ../xvt/token.h \
			  ../xvt/xsetup.h \
			  ../xvt/ttyinit.h \
			  ../xvt/xvt.h \
			  xfedefs.h \
              icons.h \
			  xfeutils.h \
              help.h \
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
			  TextWindow.h \
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

