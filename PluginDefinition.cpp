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

#include <stdio.h>
#include <windows.h>
#include <CommCtrl.h>
#include <Shlwapi.h>
#include "PluginDefinition.h"
#include "NPP/MenuCmdID.h"
#include "NPP/GUI/MainPanelDlg.h"
#include "NPP/GUI/resource.h"

MainPanelDlg _mainPanel;
HINSTANCE hInst;
FuncItem funcItem[nbFunc];
NppData nppData;
HICON icon;
HBITMAP iconBitmap;

void pluginInit(HANDLE hModule)
{
	hInst = (HINSTANCE)hModule;
	icon = ::LoadIcon((HINSTANCE)hModule, MAKEINTRESOURCE(IDI_ICON_MAIN));
	iconBitmap = ::CreateMappedBitmap((HINSTANCE)hModule, IDB_BITMAP_MAIN, NULL, NULL, 0);
}

void pluginCleanUp()
{
	::DeleteObject(iconBitmap);
}

void commandMenuInit()
{
    setCommand(0, L"Show SumatraNPP window", toggleMainPanelDlg);
	ShortcutKey *shKeyOCP = new ShortcutKey(); //Ctrl + F7
	shKeyOCP->_isCtrl = true;
	shKeyOCP->_key = 0x76;
	setCommand(1, L"Open corresponding PDF", loadCurrentPDF, shKeyOCP);
	setCommand(2, L"Open arbitrary PDF", loadOtherPDF);
	setCommand(3, L"Forward search", forwardSearch);
	setCommand(4, L"About", about);
}

void commandMenuCleanUp()
{
}

void setCommand(UINT32 index, TCHAR *cmdName, PFUNCPLUGINCMD pFunc, ShortcutKey *shKey)
{
	lstrcpy(funcItem[index]._itemName, cmdName);
	funcItem[index]._pFunc = pFunc;
	funcItem[index]._init2Check = false;
	funcItem[index]._pShKey = shKey;
}

#pragma region

TCHAR* getFullCurrentFileName()
{
	LRESULT currentBufferId = ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTBUFFERID, NULL, NULL);
	if (currentBufferId == -1) return NULL;
	int length = ::SendMessage(nppData._nppHandle, NPPM_GETFULLPATHFROMBUFFERID, currentBufferId, NULL);
	TCHAR* fullPathName = new TCHAR[length + 1];
	::SendMessage(nppData._nppHandle, NPPM_GETFULLPATHFROMBUFFERID, currentBufferId, (LPARAM)fullPathName);
	DWORD dwAttrib = ::GetFileAttributes(fullPathName);
	if (dwAttrib == INVALID_FILE_ATTRIBUTES || (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)) return NULL;
	return fullPathName;
}

TCHAR* getPDFFile(TCHAR* fullPathName)
{
	int pathLength = wcslen(fullPathName) + 1;
	TCHAR* fullPathNameBku = new TCHAR[pathLength];
	memcpy(fullPathNameBku, fullPathName, pathLength * sizeof(TCHAR));
	::PathRemoveExtension(fullPathNameBku);
	TCHAR* pdfPathName = new TCHAR[pathLength + 4];
	swprintf(pdfPathName, (pathLength + 4) * sizeof(TCHAR), L"%s.pdf", fullPathNameBku);
	delete[] fullPathNameBku;
	DWORD dwAttrib = ::GetFileAttributes(pdfPathName);
	if (dwAttrib == INVALID_FILE_ATTRIBUTES || (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)) return NULL;
	return pdfPathName;
}

void loadPDFbyName(TCHAR* fullPathName)
{
	setMainPanelDlgEx(SHOWDLG);
	if (_mainPanel.openPDF(fullPathName) != S_OK)
	{
		::MessageBox(nppData._nppHandle,
			L"Cannot start SumatraPDF.exe\nEither the file was not found,\nor the DDE connection failed!",
			ERRTITLE, MB_ICONERROR | MB_OK);
	}
}

void setMainPanelDlgEx(int show)
{
	if (!_mainPanel.isCreated())
	{
		_mainPanel.init(hInst, nppData._nppHandle, funcItem[0]._cmdID);
		tTbData	data = { 0 };
		_mainPanel.create(&data, false);
		data.uMask = DWS_DF_CONT_RIGHT | DWS_ICONTAB;
		data.pszModuleName = _mainPanel.getPluginFileName();
		data.hIconTab = icon;
		data.dlgID = 0;
		::SendMessage(nppData._nppHandle, NPPM_DMMREGASDCKDLG, 0, (LPARAM)&data);
		_mainPanel.display(false);
	}
	if (show == TOGGLEDLG) _mainPanel.display(!_mainPanel.isVisible());
	else _mainPanel.display(show == SHOWDLG);
}

#pragma endregion Helper functions

#pragma region

void toggleMainPanelDlg()
{ 
	setMainPanelDlgEx(TOGGLEDLG);
}

void loadCurrentPDF()
{
	TCHAR* fullPathName = getFullCurrentFileName();
	if (fullPathName)
	{
		TCHAR* pdfFullPathName = getPDFFile(fullPathName);
		if (pdfFullPathName) loadPDFbyName(pdfFullPathName);
		else
		{
			TCHAR buffer[MAX_PATH + 30];
			swprintf(buffer, MAX_PATH + 30, L"PDF file does not exist!\n%s", fullPathName);
			::MessageBox(nppData._nppHandle, buffer, ERRTITLE, MB_ICONERROR | MB_OK);
		}
		delete[] pdfFullPathName;
	}
	delete[] fullPathName;
}

void loadOtherPDF()
{
	TCHAR szFile[MAX_PATH];
	OPENFILENAME ofn = { 0 };
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"PDF Files\0*.PDF\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrTitle = L"SumatraNPP - Open PDF File";
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	if (::GetOpenFileName(&ofn)) loadPDFbyName(ofn.lpstrFile);
}

void forwardSearch()
{
	if (_mainPanel.isCreated() && _mainPanel.isShown() && _mainPanel.getOpenedPDF())
	{
		int cLine = ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTLINE, NULL, NULL);
		TCHAR* fullPathName = getFullCurrentFileName();
		if (fullPathName)
		{
			TCHAR* pdfFullPathName = getPDFFile(fullPathName);
			if (pdfFullPathName && wcscmp(_mainPanel.getOpenedPDF(), pdfFullPathName) == 0) _mainPanel.forwardSearch(pdfFullPathName, fullPathName, cLine + 1, 0);
			delete[] pdfFullPathName;
		}
		delete[] fullPathName;
	}
}

void about()
{
	::MessageBox(nppData._nppHandle, L"\
SumatraNPP - SumatraPDF Plugin for Notepad++\n\
Version 1.0 by Christoph Honal\n\n\
Use the plugin menu when a .tex file is opened.\n\
Double click on the text in one document to jump to the \n\
corresponding position in the other document.",
L"SumatraNPP - About", MB_ICONINFORMATION | MB_OK);
}

#pragma endregion Menu handlers