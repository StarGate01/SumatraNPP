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
#include "PluginDefinition.h"

extern FuncItem funcItem[nbFunc];
extern NppData nppData;
extern HBITMAP iconBitmap;

BOOL APIENTRY DllMain( HANDLE hModule, DWORD  reasonForCall, LPVOID lpReserved )
{
    switch (reasonForCall)
    {
		case DLL_PROCESS_ATTACH :
		{
			pluginInit(hModule);
			break;
		}
		case DLL_PROCESS_DETACH :
		{
			commandMenuCleanUp();
			pluginCleanUp();
			break;
		}
		case DLL_THREAD_ATTACH : 
		{
			break;
		}
		case DLL_THREAD_DETACH : 
		{
			break;
		}
    }
    return TRUE;
}

extern "C" __declspec(dllexport) void setInfo(NppData notpadPlusData)
{
	nppData = notpadPlusData;
	commandMenuInit();
}

extern "C" __declspec(dllexport) const TCHAR * getName()
{
	return NPP_PLUGIN_NAME;
}

extern "C" __declspec(dllexport) FuncItem * getFuncsArray(int *nbF)
{
	*nbF = nbFunc;
	return funcItem;
}

extern "C" __declspec(dllexport) void beNotified(SCNotification *notifyCode)
{
	switch (notifyCode->nmhdr.code)
	{
		case NPPN_TBMODIFICATION :
		{
			toolbarIcons tbiFtp;
			tbiFtp.hToolbarBmp = iconBitmap;
			tbiFtp.hToolbarIcon = NULL;
			SendMessage(nppData._nppHandle, NPPM_ADDTOOLBARICON, (WPARAM)funcItem[0]._cmdID, (LPARAM)&tbiFtp);
			break;
		}
		case SCN_DOUBLECLICK :
		{
			forwardSearch();
			break;
		}
	}
}

extern "C" __declspec(dllexport) LRESULT messageProc(UINT Message, WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

#ifdef UNICODE
extern "C" __declspec(dllexport) BOOL isUnicode()
{
    return TRUE;
}
#endif //UNICODE
