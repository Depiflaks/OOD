
// MainDlg.h : header file
//
#pragma once

#include "IMainDlgView.h"
#include "ChartView.h"


class CEquationSolver;
class IMainDlgController;

class CMainDlg 
	: public CDialogEx
	, public IMainDlgView
{
public:
	CMainDlg(CWnd* pParent = NULL);	// standard constructor

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MODELVIEWCONTROLLER_DIALOG };
#endif

	void SetCoeffs(double a, double b, double c) final;
	void SetNoSolution() final;
	void SetInfiniteSolutions() final;
	void SetSingleSolution(double solution) final;
	void SetTwoRootsSolutuion(double root1, double root2) final;
	IChartView & GetChartView() final;

	sig::connection DoOnCoeffAChange(const CoeffChangeSignal::slot_type & handler) final;
	sig::connection DoOnCoeffBChange(const CoeffChangeSignal::slot_type & handler) final;
	sig::connection DoOnCoeffCChange(const CoeffChangeSignal::slot_type & handler) final;
	sig::connection DoOnInit(const InitSignal::slot_type & handler) final;

protected:
	BOOL PreTranslateMessage(MSG *msg) override;
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
private:
	void SetSolutionText(const std::wstring & text);
	void SetEquationText(const std::wstring & text);
	void UpdateEquation();
	void OnChangeCoeffA();
	void OnChangeCoeffB();
	void OnChangeCoeffC();

	void OnOK()final;

	afx_msg void OnKillfocusCoeffA();
	afx_msg void OnKillfocusCoeffB();
	afx_msg void OnKillfocusCoeffC();

	double m_coeffA = 0;
	double m_coeffB = 0;
	double m_coeffC = 0;

	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	
	CoeffChangeSignal m_coeffAChanged;
	CoeffChangeSignal m_coeffBChanged;
	CoeffChangeSignal m_coeffCChanged;
	InitSignal m_init;

private:
	CChartView m_chart;
};
