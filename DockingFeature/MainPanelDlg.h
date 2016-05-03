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

#ifndef MAINPANEL_DLG_H
#define MAINPANEL_DLG_H

#include "DockingDlgInterface.h"
#include "resource.h"

#define DPPATOM_APP L"SUMATRA"
#define DPPATOM_TOPIC L"control"

class MainPanelDlg : public DockingDlgInterface
{

public :
	enum DDEState
	{
		NotReady, Ready, WaitForInitAck, GotInitAck, WaitForExecAck, GotExecAck
	};

	MainPanelDlg() : DockingDlgInterface(IDD_PLUGINMAINPANEL_SUMATRANPP)
	{
		ddeAppAtom = ::GlobalAddAtom(DPPATOM_APP);
		ddeTopicAtom = ::GlobalAddAtom(DPPATOM_TOPIC);
	};

    virtual void display(bool toShow = true) const 
	{
        DockingDlgInterface::display(toShow);
    };

	void setParent(HWND parent2set)
	{
		_hParent = parent2set;
	};

	virtual void openPDF(TCHAR* fileName);

protected :
	virtual BOOL CALLBACK run_dlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

private :
	HWND hwnd_sumatra;
	ATOM ddeAppAtom, ddeTopicAtom;
	DDEState ddeState = NotReady;
	char * ddePayload;
	HGLOBAL ddePayloadGlobal = nullptr;
	virtual HRESULT sendDDE(char * command);
	virtual LRESULT handleDDEack(WPARAM wParam, LPARAM lParam);
	virtual LRESULT terminateDDE();
	virtual LRESULT executeDDE();

};

#endif //MAINPANEL_DLG_H
