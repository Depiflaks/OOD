#include "stdafx.h"
#include "EquationSolver.h"


CEquationSolver::CEquationSolver()
{
}


CEquationSolver::~CEquationSolver()
{
}

sig::connection CEquationSolver::DoOnSolutionChange(const SolutionChangeSignal::slot_type & handler)
{
	return m_solutionChanged.connect(handler);
}

EquationRoots CEquationSolver::GetEquationRoots() const
{
	const auto epsilon = std::numeric_limits<double>::epsilon();
	if (std::abs(m_a) > epsilon)
	{
		double discr = m_b * m_b - 4 * m_a * m_c;
		if (discr < 0)
		{
			return NoRealRoots();
		}
		else if (discr < std::numeric_limits<double>::epsilon())
		{
			return (-m_b + std::sqrt(discr)) / (2 * m_a);
		}
		else
		{
			return std::make_pair((-m_b - std::sqrt(discr)) / (2 * m_a),
				(-m_b + std::sqrt(discr)) / (2 * m_a));
		}
	}
	// linear equation b*x + c = 0
	if (std::abs(m_b) >= epsilon)
	{
		return -m_c / m_b;
	}
	else if (std::abs(m_c) >= epsilon)
	{
		return NoRealRoots();
	}
	else // 0*x = 0
	{
		return InfiniteNumberOfRoots();
	}

}

double CEquationSolver::GetQuadraticCoeff() const
{
	return m_a;
}

void CEquationSolver::SetQuadraticCoeff(double a)
{
	if (m_a != a)
	{
		m_a = a;
		m_solutionChanged();
	}
}

double CEquationSolver::GetLinearCoeff() const
{
	return m_b;
}

void CEquationSolver::SetLinearCoeff(double b)
{
	if (m_b != b)
	{
		m_b = b;
		m_solutionChanged();
	}
}

double CEquationSolver::GetConstantCoeff() const
{
	return m_c;
}

void CEquationSolver::SetConstantCoeff(double c)
{
	if (m_c != c)
	{
		m_c = c;
		m_solutionChanged();
	}
}
