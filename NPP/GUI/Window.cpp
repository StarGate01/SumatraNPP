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

#include "Window.h"

Window::Window()
{
	_hInst = NULL;
	_hParent = NULL;
	_hSelf = NULL;
};

Window::~Window()
{
};

void Window::init(HINSTANCE hInst, HWND parent)
{
	_hInst = hInst;
	_hParent = parent;
}

void Window::destroy()
{
}

void Window::display(bool toShow) 
{
	::ShowWindow(_hSelf, toShow ? SW_SHOW : SW_HIDE);
};

void Window::reSizeTo(RECT & rc) // should NEVER be const !!!
{
	::MoveWindow(_hSelf, rc.left, rc.top, rc.right, rc.bottom, TRUE);
	redraw(false);
};

void Window::reSizeToWH(RECT & rc) // should NEVER be const !!!
{
	::MoveWindow(_hSelf, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);
	redraw(false);
};

void Window::redraw(bool forceUpdate) 
{
	::InvalidateRect(_hSelf, NULL, TRUE);
	if (forceUpdate) ::UpdateWindow(_hSelf);
};

void Window::getClientRect(RECT & rc)
{
	::GetClientRect(_hSelf, &rc);
};

void Window::getWindowRect(RECT & rc) 
{
	::GetWindowRect(_hSelf, &rc);
};

int Window::getWidth() 
{
	RECT rc;
	::GetClientRect(_hSelf, &rc);
	return (rc.right - rc.left);
};

int Window::getHeight()
{
	RECT rc;
	::GetClientRect(_hSelf, &rc);
	if (::IsWindowVisible(_hSelf) == TRUE) return (rc.bottom - rc.top);
	return 0;
};

bool Window::isVisible()
{
	return (::IsWindowVisible(_hSelf) ? true : false);
};

HWND Window::getHSelf()
{
	return _hSelf;
};

HWND Window::getHParent()
{
	return _hParent;
};

void Window::getFocus()
{
	::SetFocus(_hSelf);
};

HINSTANCE Window::getHinst()
{
	if (!_hInst)
	{
		::MessageBox(NULL, TEXT("_hInst == NULL"), TEXT("class Window"), MB_OK);
		throw int(1999);
	}
	return _hInst;
};