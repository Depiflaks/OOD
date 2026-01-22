//
// Created by smmm on 03.12.2025.
//

#ifndef OOD_COUTMOCK_H
#define OOD_COUTMOCK_H
#include "gtest/gtest.h"

class GumballMachineTest : public ::testing::Test
{
protected:
	void CaptureOutput()
	{
		m_oldCoutBuffer = std::cout.rdbuf();
		std::cout.rdbuf(m_outputStream.rdbuf());
	}

	void ReleaseOutput()
	{
		std::cout.rdbuf(m_oldCoutBuffer);
	}

	std::string GetOutput()
	{
		return m_outputStream.str();
	}

	void ClearOutput()
	{
		m_outputStream.str("");
	}

private:
	std::stringstream m_outputStream;
	std::streambuf* m_oldCoutBuffer = nullptr;
};

#endif // OOD_COUTMOCK_H
