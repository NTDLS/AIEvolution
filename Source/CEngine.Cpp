#ifndef _CEngine_CPP_
#define _CEngine_CPP_
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Windows.H>
#include <Stdio.H>
#include <Stdlib.H>
#include <CommCtrl.H>
#include <ShlOBJ.H>
#include <Math.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Entry.H"
#include "Init.H"
#include "CEngine.H"
#include "Bitmap.H"

#include "../Dialogs/MainDlg.H"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace NSWFL::Math;
using namespace NSWFL::String;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

char sDbgText[1024];

#define AIE_PI					3.14159265358979323846264338327f // PI
#define AIE_RADIAN_CONV			0.01745329251994329576923690768f // (PI / 180.0f)
#define AIE_OFFSET90DEGREES		1.57079632679489661923132169156f // (180.0f * RADIAN_CONV)						 
#define AIE_FULLCIRCLE			6.28318530717958647692528676624f // (360.0f * RADIAN_CONV)

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Simple function for generating random numbers
float CEngine::rnd(void)
{
	DWORD dwNumber = 0;
	char sGUID[41];
	if(NewGUID(sGUID, sizeof(sGUID)))
	{
		dwNumber = this->crc.FullCRC((unsigned char *)sGUID, strlen(sGUID));
	}
	return (float)(dwNumber % RAND_MAX);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Simple function for generating random numbers
float CEngine::rnd(float low, float high)
{
    return low + (high - low) * this->rnd() / RAND_MAX;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

float CEngine::CalculateDirectionalAngle(CBasicObject *pObject)
{
	float fRadians = atan2(pObject->VelocityY, -pObject->VelocityX);	
	float fDegrees = (fRadians * (180 / AIE_PI));

	if(pObject->ObjectType == OBJECT_TYPE_SPECIMEN)
	{
		sprintf_s(sDbgText, sizeof(sDbgText),
			"Velocity(%.2fx, %.2fy) Angle(%.2fr, %.2fd)",
			pObject->VelocityX, pObject->VelocityY, fRadians, fDegrees);
	}

	return (fDegrees * AIE_RADIAN_CONV) + AIE_OFFSET90DEGREES;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	This function loads an object texture and adds the object to the list of available
		video objects.
*/
CBasicObject *CEngine::LoadObject(int iObjType, const char *sTexture, int iWidth, int iHeight)
{
	this->Lock();

	HBITMAP hTexture = (HBITMAP) LoadImage(NULL, sTexture, IMAGE_BITMAP, iWidth, iHeight, LR_LOADFROMFILE);
	CBasicObject *pObject;

	if(iObjType == OBJECT_TYPE_RESOURCE)
	{
		this->pResources.pObjects = (CBasicObject *)
			realloc(this->pResources.pObjects, sizeof(CBasicObject) * (this->pResources.Count + 1));

		pObject = &this->pResources.pObjects[this->pResources.Count++];
		memset(pObject, 0, sizeof(CBasicObject));
		pObject->ArrayIndex = this->pResources.Count;

		pObject->Attributes = 0;
	}
	else if(iObjType == OBJECT_TYPE_SPECIMEN)
	{
		this->pSpecimens.pObjects = (CBasicObject *)
			realloc(this->pSpecimens.pObjects, sizeof(CBasicObject) * (this->pSpecimens.Count + 1));

		pObject = &this->pSpecimens.pObjects[this->pSpecimens.Count++];
		memset(pObject, 0, sizeof(CBasicObject));
		pObject->ArrayIndex = this->pSpecimens.Count;

		pObject->Attributes = 0;
	}
	else if(iObjType == OBJECT_TYPE_OBSTACLE)
	{
		this->pObstacles.pObjects = (CBasicObject *)
			realloc(this->pObstacles.pObjects, sizeof(CBasicObject) * (this->pObstacles.Count + 1));

		pObject = &this->pObstacles.pObjects[this->pObstacles.Count++];
		memset(pObject, 0, sizeof(CBasicObject));
		pObject->ArrayIndex = this->pObstacles.Count;

		pObject->Attributes = 0;
	}
	else if(iObjType == OBJECT_TYPE_TRANSMITTER)
	{
		this->pTransmitters.pObjects = (CBasicObject *)
			realloc(this->pTransmitters.pObjects, sizeof(CBasicObject) * (this->pTransmitters.Count + 1));

		pObject = &this->pTransmitters.pObjects[this->pTransmitters.Count++];
		memset(pObject, 0, sizeof(CBasicObject));
		pObject->ArrayIndex = this->pTransmitters.Count;

		pObject->Attributes = 0;
	}

	pObject->Texture = hTexture;
	pObject->Width = iWidth;
	pObject->Height = iHeight;
	pObject->ObjectType = iObjType;
	
	this->UnLock();

	return pObject;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CBasicObject *CEngine::GetFreeObjectInPlay(void)
{
	this->Lock();

	for(int iIndex = 0; iIndex < this->pOIP.Allocated; iIndex++)
	{
		if(!this->pOIP.pObjects[iIndex].IsObjectInUse)
		{
			this->pOIP.pObjects[iIndex].IsObjectInUse = true;
			memset(&this->pOIP.pObjects[iIndex], 0, sizeof(CBasicObject));
			this->UnLock();
			return &this->pOIP.pObjects[iIndex];
		}
	}

	this->pOIP.pObjects = (CBasicObject *)
		realloc(this->pOIP.pObjects, sizeof(CBasicObject) * (this->pOIP.Allocated + 1));

	memset(&this->pOIP.pObjects[this->pOIP.Allocated], 0, sizeof(CBasicObject));

	this->UnLock();
	return &this->pOIP.pObjects[this->pOIP.Allocated++];
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CBasicObject *CEngine::DoesObjectIntersect(CBasicObject *inputObject)
{
	return this->DoesObjectIntersect(inputObject, false);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CBasicObject *CEngine::DoesObjectIntersect(CBasicObject *inputObject, bool bIgnoreAntiCollisionAttribute)
{
	this->Lock();

	for(int iIndex = 0; iIndex < this->pOIP.Allocated; iIndex++)
	{
		if(this->pOIP.pObjects[iIndex].IsObjectInUse && &this->pOIP.pObjects[iIndex] != inputObject)
		{
			if((!(this->pOIP.pObjects[iIndex].Attributes &OBJ_ATTRIB_ALLOW_INTERSECT)) || bIgnoreAntiCollisionAttribute)
			{
				if(this->DoObjectsIntersect(inputObject, &this->pOIP.pObjects[iIndex]))
				{
					this->UnLock();
					return &this->pOIP.pObjects[iIndex];
				}
			}
		}
	}

	this->UnLock();
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	This function adds an object to the list of "need to render" / "in play" objects.
*/
CBasicObject *CEngine::AddObject(int iObjType, int iModelIndex, float iGridX, float iGridY)
{
	this->Lock();

	CBasicObject *pObj = this->GetFreeObjectInPlay();

	memcpy(pObj, this->GetBasicObject(iObjType, iModelIndex), sizeof(CBasicObject));

	pObj->X = iGridX;
	pObj->Y = iGridY;

	pObj->IsObjectInUse = true;

	pObj->ModelIndex = iModelIndex;

	if(iObjType == OBJECT_TYPE_SPECIMEN)
	{
		pObj->IsOptimistic = this->FlipCoin();
	}

	this->UnLock();

	return pObj;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	This function adds an object to the list of "need to render" / "in play" objects.
*/
CBasicObject *CEngine::AddObject(int iObjType, int iModelIndex)
{
	this->Lock();

	CBasicObject *pObj = this->AddObject(iObjType, iModelIndex, 0.0f, 0.0f);

	RandomizePosition(pObj);

	this->UnLock();

	return pObj;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CBasicObject *CEngine::GetBasicObject(int iObjType, int iModelIndex)
{
	CBasicObject *pObject;

	if(iObjType == OBJECT_TYPE_RESOURCE)
	{
		pObject = &this->pResources.pObjects[iModelIndex];
	}
	else if(iObjType == OBJECT_TYPE_SPECIMEN)
	{
		pObject = &this->pSpecimens.pObjects[iModelIndex];
	}
	else if(iObjType == OBJECT_TYPE_OBSTACLE)
	{
		pObject = &this->pObstacles.pObjects[iModelIndex];
	}
	else if(iObjType == OBJECT_TYPE_TRANSMITTER)
	{
		pObject = &this->pTransmitters.pObjects[iModelIndex];
	}
	return pObject;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CEngine::DoObjectsIntersect(CBasicObject *pObject1, CBasicObject *pObject2)
{
	this->Lock();

	RECT rcObj1;
	RECT rcObj2;
	RECT rcIntersect;

	rcObj1.left = (int)pObject1->X;
	rcObj1.top = (int)pObject1->Y;
	rcObj1.right = rcObj1.left + pObject1->Width;
	rcObj1.bottom = rcObj1.top + pObject1->Height;

	rcObj2.left = (int)pObject2->X;
	rcObj2.top = (int)pObject2->Y;
	rcObj2.right = rcObj2.left + pObject2->Width;
	rcObj2.bottom = rcObj2.top + pObject2->Height;

	this->UnLock();

	return(IntersectRect(&rcIntersect, &rcObj1, &rcObj2) != 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CEngine::bIsObjectOffScreen(CBasicObject *pObject)
{
	if(pObject->Y + pObject->Height <= 0) //Is the object completely off the top?
	{
		return true;
	}
	else if(pObject->Y >= this->rcVideo.bottom) //Is the object completely off the bottom?
	{
		return  true;
	}
	else if(pObject->X + pObject->Width <= 0) //Is the object completely off the left side?
	{
		return true;
	}
	else if(pObject->X >= this->rcVideo.right) //Is the object completely off the right side?
	{
		return true;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HBITMAP CEngine::RotateBitmap(HDC hDC, HBITMAP hBitmap, float radians, int iDestX, int iDestY)
{
	// Create a memory DC compatible with the display
	HDC destDC = CreateCompatibleDC(hDC);
	HDC sourceDC = CreateCompatibleDC(hDC);

	// Get logical coordinates
	BITMAP bm;
	GetObject(hBitmap, sizeof(bm), &bm);

	float cosine = (float)cos(radians);
	float sine = (float)sin(radians);

	// Compute dimensions of the resulting bitmap
	// First get the coordinates of the 3 corners other than origin
	int x1 = (int)(bm.bmHeight * sine);
	int y1 = (int)(bm.bmHeight * cosine);
	int x2 = (int)(bm.bmWidth * cosine + bm.bmHeight * sine);
	int y2 = (int)(bm.bmHeight * cosine - bm.bmWidth * sine);
	int x3 = (int)(bm.bmWidth * cosine);
	int y3 = (int)(-bm.bmWidth * sine);

	int minx = min(0,min(x1, min(x2,x3)));
	int miny = min(0,min(y1, min(y2,y3)));
	int maxx = max(0,max(x1, max(x2,x3)));
	int maxy = max(0,max(y1, max(y2,y3)));

	int w = maxx - minx;
	int h = maxy - miny;

	// Create a bitmap to hold the result
	HBITMAP hbmResult = CreateCompatibleBitmap(hDC, w, h);

	HGDIOBJ hbmOldSource = SelectObject(sourceDC, hBitmap);
	HGDIOBJ hbmOldDest = SelectObject(destDC, hbmResult);

	// We will use world transform to rotate the bitmap
	SetGraphicsMode(destDC, GM_ADVANCED);
	XFORM xform;
	xform.eM11 = cosine;
	xform.eM12 = -sine;
	xform.eM21 = sine;
	xform.eM22 = cosine;
	xform.eDx = (float)-minx;
	xform.eDy = (float)-miny;

	SetWorldTransform(destDC, &xform);

	// Now do the actual rotating - a pixel at a time
	BitBlt(destDC, 0, 0, bm.bmWidth, bm.bmHeight, sourceDC, 0, 0, SRCCOPY );

	// Restore DCs
	SelectObject(sourceDC, hbmOldSource);
	SelectObject(destDC, hbmOldDest);

	DeleteObject(hbmOldSource);
	DeleteObject(hbmOldDest);
	DeleteDC(sourceDC);
	DeleteDC(destDC);

	return hbmResult;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CEngine::DrawObject(HDC hDC, CBasicObject *pObject)
{
	this->Lock();

	if(!this->bIsObjectOffScreen(pObject))
	{
		int iX = (int)pObject->X;
		int iY = (int)pObject->Y;

		HBITMAP hRotated = RotateBitmap(hDC, pObject->Texture, pObject->Angle, iX, iY);

		BITMAP bm;
		GetObject(hRotated, sizeof(bm), &bm);

		int iHeight = bm.bmHeight;
		int iWidth = bm.bmWidth;

		//Clip bottom side of image.
		if((iY + iHeight) > this->rcVideo.bottom)
		{
			iHeight -= ((iY + iHeight) - this->rcVideo.bottom);
		}

		//Clip right side of image.
		if((iX + iWidth) > this->rcVideo.right)
		{
			iWidth -= ((iX + iWidth) - this->rcVideo.right);
		}

		//Trim left side of image.
		int iOffsetX = 0;
		if(iX < 0)
		{
			iOffsetX = 0 - iX;
			iX = 0;
		}

		//Trim right side of image.
		int iOffsetY = 0;
		if(iY < 0)
		{
			iOffsetY = 0 - iY;
			iY = 0;
		}

		HDC hMemDC = CreateCompatibleDC(hDC);
		SelectObject(hMemDC, hRotated);

		TransparentBlt(
			hDC,				// handle to destination DC
			iX,					// x-coord of upper-left corner
			iY,					// y-coord of upper-left corner
			iWidth - iOffsetX,	// destination width
			iHeight - iOffsetY,	// destination height
			hMemDC,				// handle to source DC
			iOffsetX,			// x-coord of upper-left corner
			iOffsetY,			// y-coord of upper-left corner
			iWidth - iOffsetX,	// source width
			iHeight - iOffsetY,	// source height
			RGB(0,0,0)			// alpha-blending function
		);

		TextOut(hDC, 0, 0, sDbgText, strlen(sDbgText));

		DeleteObject(hRotated);
		DeleteDC(hMemDC);
	}

	this->UnLock();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CEngine::DrawAllObjects(HDC hDC)
{
	this->Lock();

	HDC hdcOffScreen = CreateCompatibleDC(hDC);
	HBITMAP hbmOffScreen = CreateCompatibleBitmap(hDC, this->rcVideo.right, this->rcVideo.bottom);
	HGDIOBJ hOldSource = SelectObject(hdcOffScreen, hbmOffScreen);

	for(int iIndex = 0; iIndex < this->pOIP.Allocated; iIndex++)
	{
		if(this->pOIP.pObjects[iIndex].IsObjectInUse)
		{
			this->DrawObject(hdcOffScreen, &this->pOIP.pObjects[iIndex]);
		}
	}

	//Fill the video board.
	BitBlt(hDC, 0, 0, this->rcVideo.right, this->rcVideo.bottom, hdcOffScreen, 0, 0, SRCCOPY);

	DeleteObject(SelectObject(hdcOffScreen, hOldSource));
	DeleteObject(hbmOffScreen);
	DeleteDC(hdcOffScreen);

	this->UnLock();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CEngine::StepObject(CBasicObject *pObject)
{
	this->Lock();

	int iX = (int) (pObject->X += pObject->VelocityX);
	int iY = (int) (pObject->Y += pObject->VelocityY);

	CBasicObject *pIntersect = NULL;

	if(this->bIsObjectOffScreen(pObject) || (pIntersect = this->DoesObjectIntersect(pObject)))
	{
		bool bChangeDirection = false;

		if(pIntersect && (pIntersect->ObjectType == OBJECT_TYPE_RESOURCE || pIntersect->ObjectType == OBJECT_TYPE_TRANSMITTER))
		{
			if(pIntersect->ObjectType == OBJECT_TYPE_RESOURCE || (pIntersect->ObjectType == OBJECT_TYPE_TRANSMITTER && pIntersect->ModelIndex == TRANSMIT_O_PRESENT))
			{
				short iTexture = pObject->IsOptimistic ? TRANSMIT_O_PRESENT : TRANSMIT_O_NOTHING;

				if(!(pObject->Attributes &OBJ_ATTRIB_ISONRESOURCE))
				{
					pObject->Attributes |= OBJ_ATTRIB_ISONRESOURCE;
				}

				if(!(pObject->Attributes &OBJ_ATTRIB_HASTRANSMITTER))
				{
					//If the specimin found a resource, it will stop and collect that resource.
					//	It will also (based on it's behavior) let nearby specimies know of its find.

					pObject->Attributes |= OBJ_ATTRIB_HASTRANSMITTER;
					StopObject(pObject);

					this->AddObject(OBJECT_TYPE_TRANSMITTER, iTexture,
						(pObject->X - (TRANSMITTER_HEIGHT / 2.0f)) + (SPECIMEN_HEIGHT / 2.0f),
						pObject->Y - (TRANSMITTER_WIDTH / 2.0f)  + (SPECIMEN_WIDTH / 2.0f));
				}
			}
			else if(pIntersect->ObjectType == OBJECT_TYPE_TRANSMITTER && pIntersect->ModelIndex == TRANSMIT_O_NOTHING)
			{
				if(pObject->IsOptimistic)
				{
					bChangeDirection = true;
				}
			}
		}
		else
		{
			bChangeDirection = true;
		}

		if(bChangeDirection)
		{
			//Reverse the last movement to move the object back onto the screen.
			pObject->X -= pObject->VelocityX;
			pObject->Y -= pObject->VelocityY;
	
			//Change direction.
			RandomizeVelocity(pObject);

			if((pObject->Attributes &OBJ_ATTRIB_ISONRESOURCE))
			{
				pObject->Attributes &= ~OBJ_ATTRIB_ISONRESOURCE;
			}
		}

		if((pObject->Attributes &OBJ_ATTRIB_ISONRESOURCE))
		{
			pObject->ResourceCount += ACCUMULATION_RATE;
		}
	}

	this->UnLock();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CEngine::StepAllObjects(void)
{
	this->Lock();

	for(int iIndex = 0; iIndex < this->pOIP.Allocated; iIndex++)
	{
		if(this->pOIP.pObjects[iIndex].IsObjectInUse && this->pOIP.pObjects[iIndex].ObjectType == OBJECT_TYPE_SPECIMEN)
		{
			this->StepObject(&this->pOIP.pObjects[iIndex]);
		}
		this->Rounds++;
	}

	this->UnLock();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CEngine::FlipCoin(void)
{
	return this->rnd(1, 100) > 50;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

float CEngine::FlipCoinNegPos(float iValue)
{
	if(this->FlipCoin())
	{
		return iValue;
	}
	else{
		return 0-iValue;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int CEngine::FlipCoinNegPos(int iValue)
{
	if(this->FlipCoin())
	{
		return iValue;
	}
	else{
		return 0-iValue;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CEngine::StopObject(CBasicObject *pObject)
{
	pObject->VelocityX = 0;
	pObject->VelocityY = 0;
	//pObject->Angle = CalculateDirectionalAngle(pObject);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CEngine::RandomizeVelocity(CBasicObject *pObject)
{
	pObject->VelocityX = FlipCoinNegPos(this->rnd(MIN_VELOCITY, MAX_VELOCITY));
	pObject->VelocityY = FlipCoinNegPos(this->rnd(MIN_VELOCITY, MAX_VELOCITY));

	pObject->Angle = CalculateDirectionalAngle(pObject);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CEngine::RandomizePosition(CBasicObject *pObject)
{
	do {
		pObject->X = this->rnd(0.0f, (float)this->iGridMaxX);
		pObject->Y = this->rnd(0.0f, (float)this->iGridMaxY);
	} while(this->DoesObjectIntersect(pObject, true));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CEngine::MoveToNextGeneration(void)
{
	for(int iIndex = 0; iIndex < this->pOIP.Allocated; iIndex++)
	{
		if(this->pOIP.pObjects[iIndex].IsObjectInUse)
		{
			if(this->pOIP.pObjects[iIndex].ObjectType == OBJECT_TYPE_SPECIMEN)
			{
				this->pOIP.pObjects[iIndex].ResourceCount -= GENERATION_COST;

				if(this->pOIP.pObjects[iIndex].ResourceCount >= 0)
				{
					this->pOIP.pObjects[iIndex].Attributes = 0;
					this->RandomizeVelocity(&this->pOIP.pObjects[iIndex]);
				}
				else {
					this->pOIP.pObjects[iIndex].IsObjectInUse = false; //Kill the object.
				}
			}

			if(this->pOIP.pObjects[iIndex].ObjectType == OBJECT_TYPE_TRANSMITTER)
			{
				this->pOIP.pObjects[iIndex].IsObjectInUse = false;
			}

			if(this->pOIP.pObjects[iIndex].IsObjectInUse)
			{
				this->RandomizePosition(&this->pOIP.pObjects[iIndex]);
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CEngine::SetupInitialLayout(void)
{
	this->Lock();

	int iPopulation = 10;
	int iResources = 2;
	int iObstacles = 20;

	//Add generation 0 specimens.
	for(int i = 0; i < iPopulation; i++)
	{
		CBasicObject *pObject = this->AddObject(OBJECT_TYPE_SPECIMEN, 0);
		RandomizeVelocity(pObject);
	}

	//Add a few resources.
	for(int i = 0; i < iResources; i++)
	{
		this->AddObject(OBJECT_TYPE_RESOURCE, 0);
	}

	//Add a few obstacles.
	for(int i = 0; i < iObstacles; i++)
	{
		this->AddObject(OBJECT_TYPE_OBSTACLE, 0);
	}

	this->UnLock();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CEngine::Lock(void)
{
	EnterCriticalSection(&this->CS);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CEngine::TryLock(void)
{
	return (TryEnterCriticalSection(&this->CS) != 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CEngine::UnLock(void)
{
	LeaveCriticalSection(&this->CS);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CEngine::Destroy(void)
{
	DeleteCriticalSection(&this->CS);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CEngine::Initialize(HWND hInOwner, HWND hInVideo, HWND hInStatusBar)
{
	InitializeCriticalSection(&this->CS);
	this->Lock();

	srand(GetTickCount());

	this->hOwner = hInOwner;
	this->hVideo = hInVideo;
	this->hStatusBar = hInStatusBar;

	memset(&this->pResources, 0, sizeof(BASICOBJECTS));
	memset(&this->pSpecimens, 0, sizeof(BASICOBJECTS));
	memset(&this->pObstacles, 0, sizeof(BASICOBJECTS));
	memset(&this->pTransmitters, 0, sizeof(BASICOBJECTS));

	GetClientRect(this->hVideo, &rcVideo);
	GetWindowRect(this->hOwner, &rcOwner);

	this->LoadObject(OBJECT_TYPE_RESOURCE, "Images\\Resource\\Energy.bmp", RESOURCE_HEIGHT, RESOURCE_WIDTH);

	this->LoadObject(OBJECT_TYPE_SPECIMEN, "Images\\Specimen\\Gen1.bmp", SPECIMEN_HEIGHT, SPECIMEN_WIDTH);
	this->LoadObject(OBJECT_TYPE_SPECIMEN, "Images\\Specimen\\Gen2.bmp", SPECIMEN_HEIGHT, SPECIMEN_WIDTH);
	this->LoadObject(OBJECT_TYPE_SPECIMEN, "Images\\Specimen\\Gen3.bmp", SPECIMEN_HEIGHT, SPECIMEN_WIDTH);
	this->LoadObject(OBJECT_TYPE_SPECIMEN, "Images\\Specimen\\Gen4.bmp", SPECIMEN_HEIGHT, SPECIMEN_WIDTH);
	this->LoadObject(OBJECT_TYPE_SPECIMEN, "Images\\Specimen\\Gen5.bmp", SPECIMEN_HEIGHT, SPECIMEN_WIDTH);

	this->LoadObject(OBJECT_TYPE_OBSTACLE, "Images\\Obstacles\\Block.bmp", OBSTACLE_HEIGHT, OBSTACLE_WIDTH);

	this->LoadObject(OBJECT_TYPE_TRANSMITTER, "Images\\Transmit\\Nothing.bmp", TRANSMITTER_HEIGHT, TRANSMITTER_WIDTH);
	this->LoadObject(OBJECT_TYPE_TRANSMITTER, "Images\\Transmit\\Something.bmp", TRANSMITTER_HEIGHT, TRANSMITTER_WIDTH);

	this->iGridMaxX = rcVideo.right;
	this->iGridMaxY = rcVideo.bottom;

	this->SetupInitialLayout();

	pOldVideoProc = (WNDPROC) SetWindowLongPtr(hVideo, GWLP_WNDPROC, (long) &NewVideoProc);
	SendMessage(this->hVideo, WM_INITCLASS, 0, (LPARAM)this);

	CreateThread(NULL, NULL, VideoThread, this, 0, NULL);

	this->UnLock();
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CEngine::Paint(void)
{
	this->Lock();

	PAINTSTRUCT VideoPS;
	memset(&VideoPS, 0, sizeof(PAINTSTRUCT));

	HDC hDC = BeginPaint(this->hVideo, &VideoPS);

	//FillRect(hDC, &this->rcVideo, (HBRUSH)GetStockObject(BLACK_BRUSH));

	this->DrawAllObjects(hDC);

	EndPaint(this->hVideo, &VideoPS);

	this->UnLock();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DWORD WINAPI VideoThread(LPVOID pVoid)
{
	CEngine *pVE = (CEngine *) pVoid;

	InvalidateRect(pVE->hVideo, &pVE->rcVideo, FALSE);

	while(true)
	{
		for(int iRound = 0; iRound < NUMBER_OF_ROUNDS; iRound++)
		{
			pVE->StepAllObjects();
			InvalidateRect(pVE->hVideo, &pVE->rcVideo, TRUE);
			Sleep(10);
		}

		Sleep(10000);

		pVE->MoveToNextGeneration();
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK NewVideoProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static CEngine *pVE = NULL;

	if(uMsg == WM_INITCLASS)
	{
		pVE = (CEngine *) lParam;
	}
	else if(pVE)
	{
		if(uMsg == WM_GETDLGCODE)
		{
			return (DLGC_WANTALLKEYS | CallWindowProc(pVE->pOldVideoProc, hWnd, uMsg, wParam, lParam));
		}
		else if(uMsg == WM_PAINT)
		{
			pVE->Paint();
		}

		return CallWindowProc(pVE->pOldVideoProc, hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif

