#ifndef _BITMAP_H
#define _BITMAP_H
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define PW_WINDOW             1 
#define PW_CLIENT             2 
#define HDIB HANDLE
#define DIB_HEADER_MARKER     ((WORD) ('M' << 8) | 'B') 
#define WIDTHBYTES(bits)      (((bits) + 31) / 32 * 4) 
#define RECTWIDTH(lpRect)     ((lpRect)->right - (lpRect)->left) 
#define RECTHEIGHT(lpRect)    ((lpRect)->bottom - (lpRect)->top) 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum {       
	ERR_MIN = 0,                     // All error #s >= this value       
	ERR_NOT_DIB = 0,                 // Tried to load a file, NOT a DIB!       
	ERR_MEMORY,                      // Not enough memory!       
	ERR_READ,                        // Error reading file!       
	ERR_LOCK,                        // Error on a GlobalLock()!       
	ERR_OPEN,                        // Error opening a file!       
	ERR_CREATEPAL,                   // Error creating palette.       
	ERR_GETDC,                       // Couldn't get a DC.       
	ERR_CREATEDDB,                   // Error create a DDB.       
	ERR_STRETCHBLT,                  // StretchBlt() returned failure.       
	ERR_STRETCHDIBITS,               // StretchDIBits() returned failure.       
	ERR_SETDIBITSTODEVICE,           // SetDIBitsToDevice() failed.       
	ERR_STARTDOC,                    // Error calling StartDoc().       
	ERR_NOGDIMODULE,                 // Couldn't find GDI module in memory.       
	ERR_SETABORTPROC,                // Error calling SetAbortProc().       
	ERR_STARTPAGE,                   // Error calling StartPage().       
	ERR_NEWFRAME,                    // Error calling NEWFRAME escape.       
	ERR_ENDPAGE,                     // Error calling EndPage().       
	ERR_ENDDOC,                      // Error calling EndDoc().       
	ERR_SETDIBITS,                   // Error calling SetDIBits().       
	ERR_FILENOTFOUND,                // Error opening file in GetDib()       
	ERR_INVALIDHANDLE,               // Invalid Handle       
	ERR_DIBFUNCTION,                 // Error on call to DIB function       
	ERR_MAX                          // All error #s < this value      
};   

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HBITMAP CopyScreenToBitmap(LPRECT lpRect);
HBITMAP WindowCapture(HWND wnd);
HBITMAP CopyWindowToBitmap(HWND hWnd, WORD fPrintArea);
HPALETTE GetSystemPalette(HWND hwnd);
WORD DibNumColors (VOID FAR * pv);
WORD PaletteSize (VOID FAR * pv);
HANDLE AllocRoomForDIB(BITMAPINFOHEADER bi, HBITMAP hBitmap);
HDIB ChangeBitmapFormat(HBITMAP hBitmap, WORD wBitCount, DWORD dwCompression, HPALETTE hPal);
WORD SaveDIB(HDIB hDib, const char *lpFileName);
bool SaveBitmap(HBITMAP hBitMap, int iBitCount, DWORD dwCompression, const char *sFile);
bool DrawTextBitmap(int iHeight, int iWidth, char *sFontName, int iFontSz,
				DWORD dwFontColor, DWORD dwBGColor, char *sText, int iBitCount,
				DWORD dwCompression, const char *sOutFile);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif

