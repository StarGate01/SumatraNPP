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

#ifndef WINDOW_CONTROL_H
#define WINDOW_CONTROL_H

#include <windows.h>

class Window
{
public:
	Window();
	virtual ~Window();
	virtual void init(HINSTANCE hInst, HWND parent);
	virtual void destroy();
	virtual void display(bool toShow);
	virtual void reSizeTo(RECT & rc);
	virtual void reSizeToWH(RECT & rc);
	virtual void redraw(bool forceUpdate);
	virtual void getClientRect(RECT & rc);
	virtual void getWindowRect(RECT & rc);
	virtual int getWidth();
	virtual int getHeight();
	virtual bool isVisible();
	virtual HWND getHSelf();
	virtual HWND getHParent();
	virtual void getFocus();
	virtual HINSTANCE getHinst();

protected:
	HINSTANCE _hInst;
	HWND _hParent;
	HWND _hSelf;
};

#endif //WINDOW_CONTROL_H


