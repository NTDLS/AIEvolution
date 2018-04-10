#ifndef _CEngine_H_
#define _CEngine_H_
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define WM_INITCLASS	WM_USER + 667

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CBasicObject.H"
#include "../../@Libraries/CCRC32/CCRC32.H"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct _tag_Basic_Objects {
	int Count;
	CBasicObject *pObjects;
} BASICOBJECTS, *LPBASICOBJECTS;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct _tag_Objects_In_Play {
	int Allocated;
	CBasicObject *pObjects;
} OBJECTSINPLAY, *LPOBJECTSINPLAY;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CEngine {

public:
	WNDPROC pOldVideoProc;
	HWND hOwner;
	HWND hVideo;
	HWND hStatusBar;

	RECT rcVideo;
	RECT rcOwner;

	int iGridMaxX;
	int iGridMaxY;

private:
	BASICOBJECTS pResources;
	BASICOBJECTS pSpecimens;
	BASICOBJECTS pObstacles;
	BASICOBJECTS pTransmitters;
	OBJECTSINPLAY pOIP;

	CRITICAL_SECTION CS;

public:
	bool Initialize(HWND hInOwner, HWND hInVideo, HWND hInStatusBar);

	void Paint(void);
	void StepObject(CBasicObject *pObject);
	void StepAllObjects(void);

	void Destroy(void);
	void Lock(void);
	bool TryLock(void);
	void UnLock(void);

	int Rounds;

	CCRC32 crc;

private:
	float CalculateDirectionalAngle(CBasicObject *pObject);
	CBasicObject *LoadObject(int iObjType, const char *sTexture, int iWidth, int iHeight);
	void DrawAllObjects(HDC hDC);
	void DrawObject(HDC hDC, CBasicObject *pObject);
	CBasicObject *AddObject(int iObjType, int iModelIndex, float iX, float iY);
	CBasicObject *AddObject(int iObjType, int iModelIndex);
	CBasicObject *GetFreeObjectInPlay(void);
	CBasicObject *GetBasicObject(int iObjType, int iModelIndex);
	void SetupInitialLayout(void);
	void MoveToNextGeneration(void);
	bool FlipCoin(void);
	int FlipCoinNegPos(int iValue);
	float FlipCoinNegPos(float iValue);
	bool DoObjectsIntersect(CBasicObject *pObject1, CBasicObject *pObject2);
	CBasicObject *DoesObjectIntersect(CBasicObject *inputObject, bool bIgnoreAntiCollisionAttribute);
	CBasicObject *DoesObjectIntersect(CBasicObject *inputObject);
	void RandomizePosition(CBasicObject *pObject);
	void RandomizeVelocity(CBasicObject *pObject);
	void StopObject(CBasicObject *pObject);
	bool bIsObjectOffScreen(CBasicObject *pObject);
	HBITMAP RotateBitmap(HDC hDC, HBITMAP hBitmap, float radians, int iDestX, int iDestY);
	float rnd(float low, float high);
	float rnd(void);

	friend LRESULT CALLBACK NewVideoProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	friend DWORD WINAPI VideoThread(LPVOID pVoid);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif

