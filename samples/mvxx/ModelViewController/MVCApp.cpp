
// ModelViewController.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "MVCApp.h"
#include "MainDlg.h"
#include "MainDlgController.h"
#include "EquationSolver.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMVCApp

BEGIN_MESSAGE_MAP(CMVCApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CMVCApp construction

CMVCApp::CMVCApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

CMVCApp theApp;

BOOL CMVCApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// Activate "Windows Native" visual manager for enabling themes in MFC controls
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// Standard initialization
	CEquationSolver solver;
	CMainDlgController controller(solver);
	m_pMainWnd = &controller.GetDialog();
	controller.ShowDialog();

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

