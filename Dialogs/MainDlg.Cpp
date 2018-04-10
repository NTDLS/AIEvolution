#ifndef _MAINDLG_CPP
#define _MAINDLG_CPP
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Windows.H>
#include <Stdio.H>
#include <Stdlib.H>
#include <CommCtrl.H>
#include <ShlOBJ.H>

#include "../Source/Entry.H"
#include "../Source/CEngine.H"

#include "../Dialogs/MainDlg.H"
#include "../Dialogs/NetLogoDlg.H"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace NSWFL::Windows;
using namespace NSWFL::Conversion;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CEngine cVE;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK MainDialog(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HWND hVideo = NULL;
	static HWND hExactLocation = NULL;
	static HWND hLogicalLocation = NULL;
	static HWND hStatusBar = NULL;

    //--------------------------------------------------------------------------

    if(uMsg == WM_INITDIALOG) // Received an Initilize Dialog Message
    {
        SendMessage(hWnd, (UINT)WM_SETTEXT, (WPARAM)0, (LPARAM)gsTitleCaption);
        SendMessage(hWnd, WM_SETICON, TRUE, (LPARAM) LoadIcon(ghAppInstance, MAKEINTRESOURCE(IDI_MAIN)));

        hVideo				= GetDlgItem(hWnd, IDC_VIDEOBOARD);
        hExactLocation		= GetDlgItem(hWnd, IDC_EXACTLOCATION);
        hLogicalLocation	= GetDlgItem(hWnd, IDC_LOCIGALOCATION);
				
		hStatusBar = CreateStatusWindow(WS_CHILD|WS_VISIBLE, "Ready", hWnd, 1);
		int iWidths[] = {200, -1};
		SendMessage(hStatusBar, (UINT)SB_SETPARTS, (WPARAM)2, (LPARAM)&iWidths);

		RECT rcHwnd;
		GetClientRect(hWnd, &rcHwnd);
		MoveWindow(hStatusBar, rcHwnd.right-20, rcHwnd.bottom-20, rcHwnd.right, rcHwnd.bottom, TRUE);

		CenterWindow(hWnd);

		cVE.Initialize(hWnd, hVideo, hStatusBar);

		return TRUE; // Return TRUE to set the keyboard focus, Otherwise return FALSE.
    }
	//--------------------------------------------------------------------------
	else if(uMsg == WM_GETMINMAXINFO)
	{
		MINMAXINFO *pMMI = (MINMAXINFO *)lParam;

		pMMI->ptMinTrackSize.x = 500;
		pMMI->ptMinTrackSize.y = 500;
		return 0;
	}
	//--------------------------------------------------------------------------
	/*
	else if(uMsg == WM_MOUSEMOVE)
	{
		RECT rcWindow;
		RECT rcVideo;

		memset(&rcVideo, 0, sizeof(rcVideo));
		memset(&rcWindow, 0, sizeof(rcVideo));

		GetWindowRect(hVideo, &rcVideo);
		GetWindowRect(hWnd, &rcWindow);

		int iX = (LOWORD(lParam) - (rcVideo.left - rcWindow.left)) + 5;
		int iY = ((HIWORD(lParam) - (rcVideo.top - rcWindow.top)) + GetSystemMetrics(SM_CYCAPTION)) + 5;

		if(iX > 0 && iY > 0 && iX < rcVideo.right - rcVideo.left && iY < rcVideo.bottom - rcVideo.top)
		{
			char sText[255];
			sprintf_s(sText, sizeof(sText), "x%d / y%d", iX, iY);

			SendMessage(hStatusBar, (UINT)SB_SETTEXT, (WPARAM)1, (LPARAM)sText);
			SendMessage(hStatusBar, (UINT)SB_SETTEXT, (WPARAM)0, (LPARAM)"Selected: 0 Files, 0.00 B");
		}
		else{
			SendMessage(hStatusBar, (UINT)SB_SETTEXT, (WPARAM)1, (LPARAM)"");
		}
	}
	*/
    //--------------------------------------------------------------------------
    else if(uMsg == WM_CLOSE) //- Received a close message.
    {
        EndDialog(hWnd, 0);
        DestroyWindow(hWnd);
        return TRUE;
    }
    //--------------------------------------------------------------------------

    return FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif

