#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../../src/lib/Command/Command.h"

namespace command_tests
{

class MockHistoryManager : public IHistoryManager
{
public:
	MOCK_METHOD(void, Undo, (), (override));
	MOCK_METHOD(void, Redo, (), (override));
	MOCK_METHOD(bool, CanUndo, (), (const, override));
	MOCK_METHOD(bool, CanRedo, (), (const, override));
};

class CommandTest : public ::testing::Test
{
protected:
	void SetUp() override
	{
		std::cout.rdbuf(m_coutBuffer.rdbuf());
	}

	void TearDown() override
	{
		std::cout.rdbuf(m_originalCoutBuffer);
	}

	std::stringstream m_coutBuffer;
	std::streambuf* m_originalCoutBuffer = std::cout.rdbuf();
};

TEST_F(CommandTest, UndoRedoCommandsCallDocumentMethods)
{
	MockHistoryManager mockManager;
	HtmlDocument doc(mockManager);

	EXPECT_CALL(mockManager, CanUndo()).WillOnce(testing::Return(true));
	EXPECT_CALL(mockManager, Undo()).Times(1);
	EXPECT_CALL(mockManager, CanRedo()).WillOnce(testing::Return(true));
	EXPECT_CALL(mockManager, Redo()).Times(1);

	UndoCommand undoCommand(doc);
	RedoCommand redoCommand(doc);

	undoCommand.Execute();
	redoCommand.Execute();
}

TEST_F(CommandTest, UndoCommandThrowsWhenCannotUndo)
{
	MockHistoryManager mockManager;
	HtmlDocument doc(mockManager);

	EXPECT_CALL(mockManager, CanUndo()).WillOnce(testing::Return(false));

	UndoCommand undoCommand(doc);

	ASSERT_THROW(undoCommand.Execute(), std::runtime_error);
}

TEST_F(CommandTest, RedoCommandThrowsWhenCannotRedo)
{
	MockHistoryManager mockManager;
	HtmlDocument doc(mockManager);

	EXPECT_CALL(mockManager, CanRedo()).WillOnce(testing::Return(false));

	RedoCommand redoCommand(doc);

	ASSERT_THROW(redoCommand.Execute(), std::runtime_error);
}

TEST_F(CommandTest, ListCommandOutputsAllItemTypesCorrectly)
{
	MockHistoryManager mockManager;
	HtmlDocument doc(mockManager);

	doc.SetTitle("Test Document");
	doc.InsertParagraph("First paragraph");
	doc.InsertParagraph("Second paragraph");

	auto& item1 = doc.GetItem(0);
	item1.SetIsDeleted(true);

	ListCommand listCommand(doc);
	listCommand.Execute();

	std::string output = m_coutBuffer.str();

	ASSERT_TRUE(output.find("Title: Test Document") != std::string::npos);
	ASSERT_TRUE(output.find("1. Paragraph: First paragraph [DELETED]")
		!= std::string::npos);
	ASSERT_TRUE(output.find("[DELETED]") != std::string::npos);
}

TEST_F(CommandTest, HelpCommandOutputsHelpText)
{
	HelpCommand helpCommand;
	helpCommand.Execute();

	std::string output = m_coutBuffer.str();

	ASSERT_TRUE(output.find("Available commands:") != std::string::npos);
	ASSERT_TRUE(output.find("InsertParagraph") != std::string::npos);
	ASSERT_TRUE(output.find("InsertImage") != std::string::npos);
	ASSERT_TRUE(output.find("SetTitle") != std::string::npos);
	ASSERT_TRUE(output.find("List") != std::string::npos);
	ASSERT_TRUE(output.find("Undo") != std::string::npos);
	ASSERT_TRUE(output.find("Redo") != std::string::npos);
	ASSERT_TRUE(output.find("Save") != std::string::npos);
}

TEST_F(CommandTest, SaveCommandCallsDocumentSave)
{
	MockHistoryManager mockManager;
	HtmlDocument doc(mockManager);

	SaveCommand saveCommand(doc, "test.html");

	ASSERT_NO_THROW(saveCommand.Execute());
	ASSERT_TRUE(fs::exists("test.html"));
	fs::remove("test.html");
}
} // namespace command_tests
