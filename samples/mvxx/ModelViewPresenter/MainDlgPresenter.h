#pragma once


class CEquationSolver;
class IMainDlgView;

class CMainDlgPresenter
{
public:
	CMainDlgPresenter(CEquationSolver & solver, IMainDlgView & view);
private:
	void InitView();
	void UpdateSolution();
	void UpdateChart();
	CEquationSolver & m_solver;
	IMainDlgView & m_view;
};

