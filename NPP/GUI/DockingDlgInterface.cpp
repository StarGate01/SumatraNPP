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
#include <shlwapi.h>
#include "../Notepad_plus_msgs.h"
#include "DockingResource.h"
#include "DockingDlgInterface.h"

BOOL CALLBACK DockingDlgInterface::run_dlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_NOTIFY:
		{
			LPNMHDR	pnmh = (LPNMHDR)lParam;
			if (pnmh->hwndFrom == _hParent)
			{
				switch (LOWORD(pnmh->code))
				{
					case DMN_CLOSE:
					{
						break;
					}
					case DMN_FLOAT:
					{
						_isFloating = true;
						break;
					}
					case DMN_DOCK:
					{
						_isFloating = false;
						break;
					}
					default:
					{
						break;
					}
				}
			}
			break;
		}
		default:
		{
			break;
		}
	}
	return FALSE;
};

DockingDlgInterface::DockingDlgInterface() : StaticDialog()
{
};

DockingDlgInterface::DockingDlgInterface(int dlgID) : StaticDialog()
{
	_dlgID = dlgID;
};

void DockingDlgInterface::init(HINSTANCE hInst, HWND parent)
{
	StaticDialog::init(hInst, parent);
	::GetModuleFileName((HMODULE)hInst, _moduleName, MAX_PATH);
	lstrcpy(_moduleName, PathFindFileName(_moduleName));
}

void DockingDlgInterface::create(tTbData * data, bool isRTL = false)
{
	StaticDialog::create(_dlgID, isRTL);
	::GetWindowText(_hSelf, _pluginName, sizeof(_pluginName));
	data->hClient = _hSelf;
	data->pszName = _pluginName;
	data->uMask = 0;
	data->pszAddInfo = NULL;
	_data = data;
};

void DockingDlgInterface::updateDockingDlg(void)
{
	::SendMessage(_hParent, NPPM_DMMUPDATEDISPINFO, 0, (LPARAM)_hSelf);
}

 void DockingDlgInterface::destroy()
{
};

void DockingDlgInterface::display(bool toShow = true)
{
	::SendMessage(_hParent, toShow ? NPPM_DMMSHOW : NPPM_DMMHIDE, 0, (LPARAM)_hSelf);
};

TCHAR * DockingDlgInterface::getPluginFileName()
{
	return _moduleName;
};
