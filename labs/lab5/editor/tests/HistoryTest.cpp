#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <filesystem>
#include <iostream>
#include <optional>

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
	auto doc = std::make_shared<HtmlDocument>(historyManager);

	historyManager.ExecuteAndAddCommand(
		std::make_unique<SetTitleCommand>(doc, "Title 1"));
	historyManager.ExecuteAndAddCommand(
		std::make_unique<InsertParagraphCommand>(doc, 0, "Paragraph 1"));
	historyManager.ExecuteAndAddCommand(
		std::make_unique<SetTitleCommand>(doc, "Title 2"));

	ASSERT_EQ(doc->GetTitle(), "Title 2");
	ASSERT_EQ(doc->GetItemsCount(), 1);
	ASSERT_EQ(doc->GetItem(0).GetParagraph()->GetText(), "Paragraph 1");
}

TEST_F(CommandHistoryManagerTest, MixExecutableAndUnexecutableCommands)
{
	auto doc = std::make_shared<HtmlDocument>(historyManager);

	historyManager.ExecuteAndAddCommand(std::make_unique<HelpCommand>());
	historyManager.ExecuteAndAddCommand(
		std::make_unique<SetTitleCommand>(doc, "Title 1"));
	historyManager.ExecuteAndAddCommand(std::make_unique<HelpCommand>());
	historyManager.ExecuteAndAddCommand(
		std::make_unique<InsertParagraphCommand>(doc, 0, "Paragraph 1"));
	historyManager.ExecuteAndAddCommand(std::make_unique<HelpCommand>());
	historyManager.ExecuteAndAddCommand(
		std::make_unique<SetTitleCommand>(doc, "Title 2"));

	ASSERT_EQ(doc->GetTitle(), "Title 2");
	ASSERT_EQ(doc->GetItemsCount(), 1);

	historyManager.Undo();
	ASSERT_EQ(doc->GetTitle(), "Title 1");

	historyManager.Undo();
	ASSERT_EQ(doc->GetTitle(), "Title 1");
	ASSERT_EQ(doc->GetItemsCount(), 1);
	ASSERT_TRUE(doc->GetItem(0).IsDeleted());

	historyManager.Undo();
	ASSERT_EQ(doc->GetTitle(), "");
}

TEST_F(CommandHistoryManagerTest, CreateObjectsUndoAndExecuteNewCommand)
{
	CreateTestImage("test_image1.png");
	CreateTestImage("test_image2.png");

	auto doc = std::make_shared<HtmlDocument>(historyManager);

	historyManager.ExecuteAndAddCommand(std::make_unique<InsertImageCommand>(
		doc, 0, "test_image1.png", 100, 200));
	historyManager.ExecuteAndAddCommand(std::make_unique<InsertImageCommand>(
		doc, 1, "test_image2.png", 300, 400));

	historyManager.ExecuteAndAddCommand(
		std::make_unique<InsertParagraphCommand>(doc, 2, "Test Paragraph"));

	ASSERT_EQ(doc->GetItemsCount(), 3);
	ASSERT_TRUE(fs::exists("tmp"));

	historyManager.Undo();
	historyManager.Undo();

	ASSERT_EQ(doc->GetItemsCount(), 3);
	ASSERT_TRUE(doc->GetItem(1).IsDeleted());
	ASSERT_TRUE(doc->GetItem(2).IsDeleted());

	historyManager.ExecuteAndAddCommand(
		std::make_unique<InsertParagraphCommand>(
			doc, std::nullopt, "new Paragraph"));

	ASSERT_EQ(doc->GetItemsCount(), 2);
	ASSERT_EQ(doc->GetItem(1).GetParagraph()->GetText(), "new Paragraph");
	ASSERT_FALSE(fs::exists("tmp/test_image2.png"));
}

TEST_F(CommandHistoryManagerTest, CannotUndoWhenNoCommands)
{
	auto doc = std::make_shared<HtmlDocument>(historyManager);

	ASSERT_FALSE(historyManager.CanUndo());
	ASSERT_THROW(historyManager.Undo(), std::runtime_error);
}

TEST_F(CommandHistoryManagerTest, CannotRedoWhenNoCommands)
{
	auto doc = std::make_shared<HtmlDocument>(historyManager);

	ASSERT_FALSE(historyManager.CanRedo());
	ASSERT_THROW(historyManager.Redo(), std::runtime_error);
}

TEST_F(CommandHistoryManagerTest, RedoAfterUndo)
{
	auto doc = std::make_shared<HtmlDocument>(historyManager);

	historyManager.ExecuteAndAddCommand(
		std::make_unique<SetTitleCommand>(doc, "Title 1"));
	historyManager.ExecuteAndAddCommand(
		std::make_unique<SetTitleCommand>(doc, "Title 2"));

	historyManager.Undo();
	ASSERT_EQ(doc->GetTitle(), "Title 1");

	historyManager.Redo();
	ASSERT_EQ(doc->GetTitle(), "Title 2");

	ASSERT_FALSE(historyManager.CanRedo());
}

TEST_F(CommandHistoryManagerTest, CreateTwoTextObjectsUndoAndCreateNew)
{
	auto doc = std::make_shared<HtmlDocument>(historyManager);

	historyManager.ExecuteAndAddCommand(
		std::make_unique<InsertParagraphCommand>(doc, 0, "First paragraph"));
	historyManager.ExecuteAndAddCommand(
		std::make_unique<InsertParagraphCommand>(doc, 1, "Second paragraph"));

	ASSERT_EQ(doc->GetItemsCount(), 2);

	historyManager.Undo();
	historyManager.Undo();

	historyManager.ExecuteAndAddCommand(
		std::make_unique<InsertParagraphCommand>(doc, 0, "Third paragraph"));

	ASSERT_EQ(doc->GetItemsCount(), 1);
	ASSERT_EQ(doc->GetItem(0).GetParagraph()->GetText(), "Third paragraph");
}

TEST_F(CommandHistoryManagerTest, ReplaceTextCommandsMerge)
{
	auto doc = std::make_shared<HtmlDocument>(historyManager);

	historyManager.ExecuteAndAddCommand(
		std::make_unique<InsertParagraphCommand>(doc, 0, "Initial text"));
	historyManager.ExecuteAndAddCommand(
		std::make_unique<ReplaceTextCommand>(doc, 0, "First change"));
	historyManager.ExecuteAndAddCommand(
		std::make_unique<ReplaceTextCommand>(doc, 0, "Second change"));

	ASSERT_EQ(doc->GetItem(0).GetParagraph()->GetText(), "Second change");

	historyManager.Undo();

	ASSERT_EQ(doc->GetItem(0).GetParagraph()->GetText(), "Initial text");
}

TEST_F(CommandHistoryManagerTest,
	ReplaceTextCommandsMergeWithNonMergableCommandInBetween)
{
	auto doc = std::make_shared<HtmlDocument>(historyManager);

	historyManager.ExecuteAndAddCommand(
		std::make_unique<InsertParagraphCommand>(doc, 0, "Initial text"));
	historyManager.ExecuteAndAddCommand(
		std::make_unique<ReplaceTextCommand>(doc, 0, "First change"));
	historyManager.ExecuteAndAddCommand(
		std::make_unique<ReplaceTextCommand>(doc, 0, "Second change"));
	historyManager.ExecuteAndAddCommand(
		std::make_unique<InsertParagraphCommand>(doc, std::nullopt, "123"));
	historyManager.ExecuteAndAddCommand(
		std::make_unique<ReplaceTextCommand>(doc, 0, "Third change"));

	ASSERT_EQ(doc->GetItem(0).GetParagraph()->GetText(), "Third change");

	historyManager.Undo();
	ASSERT_EQ(doc->GetItem(0).GetParagraph()->GetText(), "Second change");

	historyManager.Undo();
	historyManager.Undo();
	ASSERT_EQ(doc->GetItem(0).GetParagraph()->GetText(), "Initial text");
}

TEST_F(CommandHistoryManagerTest, ReplaceTextCommandsDoNotMergeAfterNewInsert)
{
	auto doc = std::make_shared<HtmlDocument>(historyManager);

	historyManager.ExecuteAndAddCommand(
		std::make_unique<InsertParagraphCommand>(doc, 0, "First paragraph"));
	historyManager.ExecuteAndAddCommand(
		std::make_unique<ReplaceTextCommand>(doc, 0, "First changed"));
	historyManager.ExecuteAndAddCommand(
		std::make_unique<InsertParagraphCommand>(doc, 1, "Second paragraph"));
	historyManager.ExecuteAndAddCommand(
		std::make_unique<ReplaceTextCommand>(doc, 0, "First changed again"));

	ASSERT_EQ(doc->GetItemsCount(), 2);
	ASSERT_EQ(doc->GetItem(0).GetParagraph()->GetText(), "First changed again");
	ASSERT_EQ(doc->GetItem(1).GetParagraph()->GetText(), "Second paragraph");

	historyManager.Undo();
	ASSERT_EQ(doc->GetItem(0).GetParagraph()->GetText(), "First changed");
	ASSERT_EQ(doc->GetItem(1).GetParagraph()->GetText(), "Second paragraph");

	historyManager.Undo();
	ASSERT_EQ(doc->GetItem(0).GetParagraph()->GetText(), "First changed");
	ASSERT_EQ(doc->GetItemsCount(), 2);

	historyManager.Undo();
	ASSERT_EQ(doc->GetItem(0).GetParagraph()->GetText(), "First paragraph");
}
