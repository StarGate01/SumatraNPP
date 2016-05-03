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

#include "PluginDefinition.h"
#include "menuCmdID.h"
#include "MainPanelDlg.h"
#include <stdio.h>

MainPanelDlg _mainPanel;
FuncItem funcItem[nbFunc];
NppData nppData;
HICON icon;

void pluginInit(HANDLE hModule)
{
	icon = ::LoadIcon((HINSTANCE)hModule, L"IDI_ICON1");
	_mainPanel.init((HINSTANCE)hModule, NULL);
}

void pluginCleanUp()
{
}

void commandMenuInit()
{
    setCommand(0, L"Show Sumatra window", showMainPanelDlg, 0);
	setCommand(1, L"Load corresponding pdf document", loadCpdf, 0);
}

void commandMenuCleanUp()
{

}

void setCommand(UINT32 index, TCHAR *cmdName, PFUNCPLUGINCMD pFunc, UCHAR cShortCut)
{
    lstrcpy(funcItem[index]._itemName, cmdName);
	funcItem[index]._cmdID = index;
    funcItem[index]._pFunc = pFunc;
    funcItem[index]._init2Check = false;
	funcItem[index]._pShKey = NULL;
}

void showMainPanelDlg() { toggleMainPanelDlgEx(SHOWDLG); }
void toggleMainPanelDlgEx(int show)
{
	_mainPanel.setParent(nppData._nppHandle);
	if (!_mainPanel.isCreated())
	{
		tTbData	data = { 0 };
		_mainPanel.create(&data);
		data.uMask = DWS_DF_CONT_RIGHT | DWS_ICONTAB;
		data.pszModuleName = _mainPanel.getPluginFileName();
		data.hIconTab = icon;
		data.dlgID = 0;
		::SendMessage(nppData._nppHandle, NPPM_DMMREGASDCKDLG, 0, (LPARAM)&data);
	}
	if (show == TOGGLEDLG)
	{
		_mainPanel.display(!_mainPanel.isVisible());
		return;
	}
	_mainPanel.display(show == SHOWDLG);
}

void loadCpdf()
{
	
	LRESULT currentBufferId = ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTBUFFERID, NULL, NULL);
	if (currentBufferId == -1) return;
	int length = ::SendMessage(nppData._nppHandle, NPPM_GETFULLPATHFROMBUFFERID, currentBufferId, NULL);
	TCHAR* fullPathName = new TCHAR[length + 1];
	::SendMessage(nppData._nppHandle, NPPM_GETFULLPATHFROMBUFFERID, currentBufferId, (LPARAM)fullPathName);
	::PathRemoveExtension(fullPathName);
	TCHAR* pdfPathName = new TCHAR[wcslen(fullPathName) + 5];
	swprintf(pdfPathName, L"%s.pdf", fullPathName);
	DWORD dwAttrib = ::GetFileAttributes(pdfPathName);
	if (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
	{
		showMainPanelDlg();
		_mainPanel.openPDF(pdfPathName);
	}
	else
	{
		TCHAR buffer[MAX_PATH + 30];
		swprintf(buffer, L"PDF file does not exist: %s", pdfPathName);
		::MessageBox(nppData._nppHandle, buffer, L"SumatraNPP - Error", MB_ICONERROR | MB_OK);
	}
}