
// ModelViewController.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CMVCApp:
// See ModelViewController.cpp for the implementation of this class
//

class CMVCApp : public CWinApp
{
public:
	CMVCApp();

public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CMVCApp theApp;