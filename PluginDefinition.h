//this file is part of notepad++
//Copyright (C)2003 Don HO <donho@altern.org>
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#ifndef PLUGINDEFINITION_H
#define PLUGINDEFINITION_H

#include "NPP/PluginInterface.h"

#define MENUID_TEST 0
#define MENUID_SHOWDLG 1
#define TOGGLEDLG -1
#define SHOWDLG 1
#define HIDEDLG 0
#define ERRTITLE L"SumatraNPP - Error"

const TCHAR NPP_PLUGIN_NAME[] = L"SumatraNPP";
const int nbFunc = 5;

void pluginInit(HANDLE hModule);
void pluginCleanUp();
void commandMenuInit();
void commandMenuCleanUp();
void setCommand(UINT32 index, TCHAR *cmdName, PFUNCPLUGINCMD pFunc, ShortcutKey *shKey = NULL);

TCHAR* getFullCurrentFileName();
TCHAR* getPDFFile(TCHAR* fullPathName);
void loadPDFbyName(TCHAR* fullPathName);
void setMainPanelDlgEx(int show);

void loadCurrentPDF();
void loadOtherPDF();
void toggleMainPanelDlg();
void forwardSearch();
void about();

#endif //PLUGINDEFINITION_H
