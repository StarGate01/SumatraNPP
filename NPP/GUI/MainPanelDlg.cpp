//this file is part of notepad++
//Copyright (C)2003 Don HO ( donho@altern.org )
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
#include <Shlobj.h>
#include "MainPanelDlg.h"
#include "PluginDefinition.h"
#include "resource.h"

extern NppData nppData;

BOOL CALLBACK MainPanelDlg::run_dlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
		case WM_INITDIALOG:
		{
			ddeState = Ready;
			break;
		}
		case WM_SIZE :
		{
			RECT* rcClient = new RECT;
			::GetWindowRect(hwnd, rcClient);
			::MapWindowPoints(HWND_DESKTOP, GetParent(hwnd), (LPPOINT)rcClient, 2);
			HWND hChild = ::FindWindowEx(hwnd, nullptr, nullptr, nullptr);
			if (hChild) ::MoveWindow(hChild, rcClient->left, rcClient->top, rcClient->right - rcClient->left, rcClient->bottom - rcClient->top, FALSE);
			break;
		}
		case WM_PAINT :
		{
			RECT* uRect = new RECT;
			::GetUpdateRect(hwnd, uRect, TRUE);
			HWND hChild = ::FindWindowEx(hwnd, nullptr, nullptr, nullptr);
			if (hChild) ::RedrawWindow(hChild, uRect, NULL, RDW_INVALIDATE | RDW_ALLCHILDREN | RDW_INTERNALPAINT);
			break;
		}
		case WM_COPYDATA :
		{
			HWND hChild = ::FindWindowEx(hwnd, nullptr, nullptr, nullptr);
			if (hChild)
			{
				COPYDATASTRUCT *cds = (COPYDATASTRUCT *)lParam;
				if (cds && 0x4C5255 == cds->dwData && (HWND)wParam == hChild)
				{
					char* lpData = (char *)cds->lpData;
					int newSize = strlen(lpData) + 1;
					TCHAR* wideLpData = new TCHAR[newSize];
					size_t convertedChars = 0;
					mbstowcs_s(&convertedChars, wideLpData, newSize, lpData, _TRUNCATE);
					::ShellExecute(hChild, L"open", wideLpData, NULL, NULL, SW_SHOW);
					return TRUE;
				}
			}
		}
		case WM_DDE_ACK :
		{
			handleDDEack(wParam, lParam);
			break;
		}
		case WM_DESTROY :
		{
			disconnectDDE();
			break;
		}
	}
	return DockingDlgInterface::run_dlgProc(hwnd, message, wParam, lParam);
}

MainPanelDlg::MainPanelDlg() : DockingDlgInterface(IDD_PLUGINMAINPANEL_SUMATRANPP)
{
	ddeAppAtom = ::GlobalAddAtom(DPPATOM_APP);
	ddeTopicAtom = ::GlobalAddAtom(DPPATOM_TOPIC);
};

void MainPanelDlg::init(HINSTANCE hInst, HWND parent, int menuCmdId)
{
	cmdId = menuCmdId;
	DockingDlgInterface::init(hInst, parent);
}

void MainPanelDlg::display(bool toShow)
{
	DockingDlgInterface::display(toShow);
	::SendMessage(nppData._nppHandle, NPPM_SETMENUITEMCHECK, cmdId, toShow);
}

LRESULT MainPanelDlg::openPDF(TCHAR * fileName)
{
	ddeState = NotReady;
	HWND hChild = ::FindWindowEx(_hSelf, nullptr, nullptr, nullptr);
	if (hChild)
	{
		disconnectDDE();
		SendMessage(hChild, WM_DESTROY, NULL, NULL);
	}
	TCHAR cmdLine[MAX_PATH+60];
	swprintf(cmdLine, MAX_PATH + 60, L"SumatraPDF.exe -plugin %d \"%s\"", (int)_hSelf, fileName);
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi;
	if (!CreateProcess(0, cmdLine, 0, 0, FALSE, 0, 0, 0, &si, &pi)) return E_FAIL;
	if (::WaitForInputIdle(pi.hProcess, INFINITE) != S_OK) return E_FAIL;
	ddeState = Ready;
	connectDDE();
	return S_OK;
}

void MainPanelDlg::forwardSearch(TCHAR* pdfFile, TCHAR* srcFile, int lineNr, int colNr)
{
	TCHAR buffer[MAX_PATH * 2 + 35];
	swprintf(buffer, MAX_PATH * 2 + 35, L"[ForwardSearch(\"%s\",\"%s\",%d,%d,0,0)]", pdfFile, srcFile, lineNr, colNr);
	return executeDDE(buffer);
}

#pragma region

void MainPanelDlg::connectDDE()
{
	if (ddeState != Ready) return;
	hwnd_sumatra = ::FindWindowEx(_hSelf, nullptr, nullptr, nullptr);
	if (!hwnd_sumatra) return;
	ddeState = WaitForInitAck;
	::SendMessage(hwnd_sumatra, WM_DDE_INITIATE, (WPARAM)_hSelf, MAKELPARAM(ddeAppAtom, ddeTopicAtom));
}

void MainPanelDlg::disconnectDDE()
{
	if (!(ddeState == GotInitAck || ddeState == GotExecAck)) return;
	::GlobalFree(ddePayloadGlobal);
	::PostMessage(hwnd_sumatra, WM_DDE_TERMINATE, (WPARAM)_hSelf, NULL);
}

void MainPanelDlg::handleDDEack(WPARAM wParam, LPARAM lParam)
{
	switch (ddeState)
	{
		case WaitForInitAck :
		{
			if ((HWND)wParam != hwnd_sumatra) return;
			ddeState = GotInitAck;
			return;
		}
		case WaitForExecAck:
		{
			if ((HWND)wParam != hwnd_sumatra) return;
			ddeState = GotExecAck;
			return;
		}
	}
}

void MainPanelDlg::executeDDE(TCHAR* ddePayload)
{
	if (!(ddeState == GotInitAck || ddeState == GotExecAck)) return;
	int bufLen = (wcslen(ddePayload) + 1) * sizeof(TCHAR);
	ddePayloadGlobal = ::GlobalAlloc(GMEM_FIXED, bufLen);
	memcpy(ddePayloadGlobal, ddePayload, bufLen);
	ddeState = WaitForExecAck;
	::PostMessage(hwnd_sumatra, WM_DDE_EXECUTE, (WPARAM)_hSelf, (LPARAM)ddePayloadGlobal);
}

#pragma endregion DDE methods