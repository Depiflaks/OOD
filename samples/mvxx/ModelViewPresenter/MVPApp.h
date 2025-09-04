
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CMVPApp:
// See ModelViewController.cpp for the implementation of this class
//

class CMVPApp : public CWinApp
{
public:
	CMVPApp();

public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CMVPApp theApp;