/*
this file is part of Function List Plugin for Notepad++
Copyright (C)2005 Jens Lorenz <jens.plugin.npp@gmx.de>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef DOCKINGDLGINTERFACE_H
#define DOCKINGDLGINTERFACE_H

#include "Docking.h"
#include "StaticDialog.h"

class DockingDlgInterface : public StaticDialog
{
public:
	DockingDlgInterface();
	DockingDlgInterface(int dlgID);
	virtual void init(HINSTANCE hInst, HWND parent);
	virtual void create(tTbData * data, bool isRTL);
	virtual void updateDockingDlg(void);
	virtual void destroy();
	virtual void display(bool toShow);
	virtual TCHAR * DockingDlgInterface::getPluginFileName();

protected :
	virtual BOOL CALLBACK run_dlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

    HWND _HSource;
	tTbData* _data;
	int _dlgID;
	bool _isFloating;
	TCHAR _moduleName[MAX_PATH];
	TCHAR _pluginName[MAX_PATH];
};

#endif // DOCKINGDLGINTERFACE_H
