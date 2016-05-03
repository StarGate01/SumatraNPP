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

#include "MainPanelDlg.h"
#include "PluginDefinition.h"
#include <stdio.h>
#include <Shlobj.h>

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
		case WM_DDE_ACK :
		{
			handleDDEack(wParam, lParam);
			break;
		}
	}
	return DockingDlgInterface::run_dlgProc(hwnd, message, wParam, lParam);
}

void MainPanelDlg::openPDF(TCHAR * fileName)
{
	HWND hChild = ::FindWindowEx(_hSelf, nullptr, nullptr, nullptr);
	if (hChild) SendMessage(hChild, WM_DESTROY, NULL, NULL);
	TCHAR cmdLine[MAX_PATH+30];
	swprintf(cmdLine, L"-plugin %d \"%s\"", (int)_hSelf, fileName);
	HINSTANCE res = ::ShellExecute(_hSelf, L"open", L"SumatraPDF.exe", cmdLine, nullptr, SW_SHOW);
	if ((int)res <= 32)
	{
		::MessageBox(_hSelf, L"Cannot start SumatraPDF.exe", L"SumatraNPP - Error", MB_ICONERROR | MB_OK);
		return;
	}
	ddeState = Ready;
}

#pragma region

LRESULT MainPanelDlg::sendDDE(char * command)
{
	if (ddeState != Ready) return E_ABORT;
	ddePayload = command;
	hwnd_sumatra = ::FindWindowEx(_hSelf, nullptr, nullptr, nullptr);
	if (!hwnd_sumatra) return E_FAIL;
	ddeState = WaitForInitAck;
	return ::SendMessage(hwnd_sumatra, WM_DDE_INITIATE, (WPARAM)_hSelf, MAKELPARAM(ddeAppAtom, ddeTopicAtom));
}

LRESULT MainPanelDlg::handleDDEack(WPARAM wParam, LPARAM lParam)
{
	switch (ddeState)
	{
		case WaitForInitAck :
		{
			if ((HWND)wParam != hwnd_sumatra) return E_FAIL;
			if (LOWORD(lParam) != ddeAppAtom)
			{
				ddeState = Ready;
				return E_FAIL;
			}
			ddeState = GotInitAck;
			return executeDDE();
		}
		case WaitForExecAck:
		{
			if ((HWND)wParam != hwnd_sumatra) return E_FAIL;
			if((HGLOBAL)HIWORD(lParam) != ddePayloadGlobal || ((DDEACK *)LOWORD(lParam))->fAck == 0)
			{
				ddeState = Ready;
				return E_FAIL;
			}
			ddeState = GotExecAck;
			return terminateDDE();
		}
	}
	return E_FAIL;
}

LRESULT MainPanelDlg::executeDDE()
{
	if (ddeState != GotInitAck) return E_FAIL;
	ddePayloadGlobal = ::GlobalAlloc(GMEM_FIXED, strlen(ddePayload) + 1);
	memcpy(ddePayloadGlobal, ddePayload, strlen(ddePayload) + 1);
	ddeState = WaitForExecAck;
	return ::PostMessage(hwnd_sumatra, WM_DDE_EXECUTE, (WPARAM)_hSelf, (LPARAM)ddePayloadGlobal);
}

LRESULT MainPanelDlg::terminateDDE()
{
	if (!(ddeState == GotInitAck || ddeState == GotExecAck)) return E_FAIL;
	::GlobalFree(ddePayloadGlobal);
	ddePayloadGlobal = nullptr;
	return ::PostMessage(hwnd_sumatra, WM_DDE_TERMINATE, (WPARAM)_hSelf, NULL);
}

#pragma endregion DDE methods