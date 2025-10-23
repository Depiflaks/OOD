#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <filesystem>

#include "../src/lib/Document/HtmlDocument.h"
#include "../src/lib/History.h"

namespace fs = std::filesystem;

class CommandHistoryManagerTest : public ::testing::Test
{
protected:
	void SetUp() override
	{
		fs::remove_all("tmp");
	}

	void TearDown() override
	{
		fs::remove_all("tmp");
		if (fs::exists("test_image1.png"))
		{
			fs::remove("test_image1.png");
		}
		if (fs::exists("test_image2.png"))
		{
			fs::remove("test_image2.png");
		}
	}

	void CreateTestImage(const std::string& filename)
	{
		std::ofstream file(filename);
		file << "fake image content";
		file.close();
	}

	CommandHistoryManager historyManager;
};

TEST_F(CommandHistoryManagerTest, ExecuteMultipleCommandsAndVerifyChanges)
{
	HtmlDocument doc(historyManager);

	historyManager.ExecuteAndAddCommand(
		std::make_unique<SetTitleCommand>(doc, "Title 1"));
	historyManager.ExecuteAndAddCommand(
		std::make_unique<InsertParagraphCommand>(doc, 0, "Paragraph 1"));
	historyManager.ExecuteAndAddCommand(
		std::make_unique<SetTitleCommand>(doc, "Title 2"));

	ASSERT_EQ(doc.GetTitle(), "Title 2");
	ASSERT_EQ(doc.GetItemsCount(), 1);
	ASSERT_EQ(doc.GetItem(0).GetParagraph()->GetText(), "Paragraph 1");
}

TEST_F(CommandHistoryManagerTest, ExecuteAndUndoMultipleCommands)
{
	HtmlDocument doc(historyManager);

	doc.SetTitle("Initial Title");
	doc.InsertParagraph("Initial Paragraph", 0);

	std::string initialTitle = doc.GetTitle();
	std::string initialParagraph = doc.GetItem(0).GetParagraph()->GetText();

	historyManager.ExecuteAndAddCommand(
		std::make_unique<SetTitleCommand>(doc, "New Title 1"));
	historyManager.ExecuteAndAddCommand(
		std::make_unique<InsertParagraphCommand>(doc, 0, "New Paragraph"));
	historyManager.ExecuteAndAddCommand(
		std::make_unique<SetTitleCommand>(doc, "New Title 2"));
	historyManager.ExecuteAndAddCommand(
		std::make_unique<ReplaceTextCommand>(doc, 0, "Replaced Text"));
	historyManager.ExecuteAndAddCommand(
		std::make_unique<SetTitleCommand>(doc, "Final Title"));

	historyManager.Undo();
	historyManager.Undo();
	historyManager.Undo();
	historyManager.Undo();
	historyManager.Undo();

	ASSERT_EQ(doc.GetTitle(), initialTitle);
	ASSERT_EQ(doc.GetItemsCount(), 1);
	ASSERT_EQ(doc.GetItem(0).GetParagraph()->GetText(), initialParagraph);
}

TEST_F(CommandHistoryManagerTest, MixExecutableAndUnexecutableCommands)
{
	HtmlDocument doc(historyManager);

	historyManager.ExecuteAndAddCommand(std::make_unique<HelpCommand>());
	historyManager.ExecuteAndAddCommand(
		std::make_unique<SetTitleCommand>(doc, "Title 1"));
	historyManager.ExecuteAndAddCommand(std::make_unique<HelpCommand>());
	historyManager.ExecuteAndAddCommand(
		std::make_unique<InsertParagraphCommand>(doc, 0, "Paragraph 1"));
	historyManager.ExecuteAndAddCommand(std::make_unique<HelpCommand>());
	historyManager.ExecuteAndAddCommand(
		std::make_unique<SetTitleCommand>(doc, "Title 2"));

	ASSERT_EQ(doc.GetTitle(), "Title 2");
	ASSERT_EQ(doc.GetItemsCount(), 1);

	historyManager.Undo();
	ASSERT_EQ(doc.GetTitle(), "Title 1");

	historyManager.Undo();
	ASSERT_EQ(doc.GetTitle(), "Title 1");
	ASSERT_EQ(doc.GetItemsCount(), 0);

	historyManager.Undo();
	ASSERT_EQ(doc.GetTitle(), "Title 1");
	ASSERT_EQ(doc.GetItemsCount(), 0);
}

TEST_F(CommandHistoryManagerTest, CreateObjectsUndoAndExecuteNewCommand)
{
	CreateTestImage("test_image1.png");
	CreateTestImage("test_image2.png");

	HtmlDocument doc(historyManager);

	historyManager.ExecuteAndAddCommand(std::make_unique<InsertImageCommand>(
		doc, 0, "test_image1.png", 100, 200));
	historyManager.ExecuteAndAddCommand(std::make_unique<InsertImageCommand>(
		doc, 1, "test_image2.png", 300, 400));
	historyManager.ExecuteAndAddCommand(
		std::make_unique<InsertParagraphCommand>(doc, 2, "Test Paragraph"));

	ASSERT_EQ(doc.GetItemsCount(), 3);
	ASSERT_TRUE(fs::exists("tmp"));

	historyManager.Undo();
	historyManager.Undo();

	ASSERT_EQ(doc.GetItemsCount(), 1);

	historyManager.ExecuteAndAddCommand(
		std::make_unique<ReplaceTextCommand>(doc, 0, "Modified Text"));

	ASSERT_EQ(doc.GetItemsCount(), 1);
	ASSERT_EQ(doc.GetItem(0).GetParagraph()->GetText(), "Modified Text");
	ASSERT_FALSE(fs::exists("tmp/test_image2.png"));
}

TEST_F(CommandHistoryManagerTest, CannotUndoWhenNoCommands)
{
	HtmlDocument doc(historyManager);

	ASSERT_FALSE(historyManager.CanUndo());
	ASSERT_THROW(historyManager.Undo(), std::runtime_error);
}

TEST_F(CommandHistoryManagerTest, CannotRedoWhenNoCommands)
{
	HtmlDocument doc(historyManager);

	ASSERT_FALSE(historyManager.CanRedo());
	ASSERT_THROW(historyManager.Redo(), std::runtime_error);
}

TEST_F(CommandHistoryManagerTest, RedoAfterUndo)
{
	HtmlDocument doc(historyManager);

	historyManager.ExecuteAndAddCommand(
		std::make_unique<SetTitleCommand>(doc, "Title 1"));
	historyManager.ExecuteAndAddCommand(
		std::make_unique<SetTitleCommand>(doc, "Title 2"));

	historyManager.Undo();
	ASSERT_EQ(doc.GetTitle(), "Title 1");

	historyManager.Redo();
	ASSERT_EQ(doc.GetTitle(), "Title 2");

	ASSERT_FALSE(historyManager.CanRedo());
}
