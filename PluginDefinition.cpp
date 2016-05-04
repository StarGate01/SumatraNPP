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
TCHAR* lastPDFFile = new TCHAR[0];

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
	setCommand(1, L"Open corresponding PDF", loadCurrentPDF);
	setCommand(2, L"Open arbitrary PDF", loadOtherPDF);
	setCommand(3, L"Reload current PDF", reloadLastPDF);
	setCommand(4, L"Forward search", forwardSearch);
	setCommand(5, L"About", about);
}

void commandMenuCleanUp()
{
}

void setCommand(UINT32 index, TCHAR *cmdName, PFUNCPLUGINCMD pFunc)
{
	lstrcpy(funcItem[index]._itemName, cmdName);
	funcItem[index]._pFunc = pFunc;
	funcItem[index]._init2Check = false;
	funcItem[index]._pShKey = NULL;
}

#pragma region

TCHAR* getFullCurrentFileName()
{
	LRESULT currentBufferId = ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTBUFFERID, NULL, NULL);
	if (currentBufferId == -1) return NULL;
	int length = ::SendMessage(nppData._nppHandle, NPPM_GETFULLPATHFROMBUFFERID, currentBufferId, NULL);
	TCHAR* fullPathName = new TCHAR[length + 1];
	::SendMessage(nppData._nppHandle, NPPM_GETFULLPATHFROMBUFFERID, currentBufferId, (LPARAM)fullPathName);
	return fullPathName;
}

BOOL getPDFFile(TCHAR* fullPathName)
{
	::PathRemoveExtension(fullPathName);
	TCHAR* pdfPathName = new TCHAR[wcslen(fullPathName) + 4];
	swprintf(pdfPathName, (wcslen(fullPathName) + 4) * sizeof(TCHAR), L"%s.pdf", fullPathName);
	realloc(fullPathName, wcslen(pdfPathName) * sizeof(TCHAR) + 1);
	memcpy(fullPathName, pdfPathName, wcslen(pdfPathName) * sizeof(TCHAR) + 1);
	DWORD dwAttrib = ::GetFileAttributes(pdfPathName);
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
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
	realloc(lastPDFFile, wcslen(fullPathName) * sizeof(TCHAR) + 1);
	wcscpy(lastPDFFile, fullPathName);
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
	if (show == TOGGLEDLG)
	{
		_mainPanel.display(!_mainPanel.isVisible());
	}
	else
	{
		_mainPanel.display(show == SHOWDLG);
	}
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
	if(getPDFFile(fullPathName))
	{
		loadPDFbyName(fullPathName);
	}
	else
	{
		TCHAR buffer[MAX_PATH + 30];
		swprintf(buffer, MAX_PATH + 30, L"PDF file does not exist!\n%s", fullPathName);
		::MessageBox(nppData._nppHandle, buffer, ERRTITLE, MB_ICONERROR | MB_OK);
	}
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
	if (::GetOpenFileName(&ofn))
	{
		loadPDFbyName(ofn.lpstrFile);
	}
}

void reloadLastPDF()
{
	if (lastPDFFile)
	{
		DWORD dwAttrib = ::GetFileAttributes(lastPDFFile);
		if(dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
		{
			loadPDFbyName(lastPDFFile);
		}
	}
}

void forwardSearch()
{
	if (_mainPanel.isCreated())
	{
		int cLine = ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTLINE, NULL, NULL);
		int cCol = ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTCOLUMN, NULL, NULL);
		TCHAR* fullPathName = getFullCurrentFileName();
		DWORD dwAttrib = ::GetFileAttributes(fullPathName);
		if (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
		{
			TCHAR* pdfFullPathName = new TCHAR[wcslen(fullPathName)];
			memcpy(pdfFullPathName, fullPathName, (wcslen(fullPathName) + 1) * sizeof(TCHAR));
			if (getPDFFile(pdfFullPathName)) 
			{
				_mainPanel.forwardSearch(pdfFullPathName, fullPathName, cLine, cCol);
			}
			/*else
			{
				TCHAR buffer[MAX_PATH + 30];
				swprintf(buffer, L"PDF file does not exist!\n%s", fullPathName);
				::MessageBox(nppData._nppHandle, buffer, ERRTITLE, MB_ICONERROR | MB_OK);
			}*/
		}
		/*else
		{
			TCHAR buffer[MAX_PATH + 30];
			swprintf(buffer, L"TEX file does not exist!\n%s", fullPathName);
			::MessageBox(nppData._nppHandle, buffer, ERRTITLE, MB_ICONERROR | MB_OK);
		}*/
	}
}

void about()
{
	::MessageBox(nppData._nppHandle, L"\
SumatraNPP - SumatraPDF Plugin for Notepad++\n\
Version 1.0 by Christoph Honal\n\n\
Use the plugin menu when a .tex file is opened.\n\
Double click on the text to jump to the \n\
corresponding position in the PDF.",
L"SumatraNPP - About", MB_ICONINFORMATION | MB_OK);
}

#pragma endregion Menu handlers