#include <filesystem>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../../src/lib/Command/Unexecutable.h"
#include "../../src/lib/Document/HtmlDocument.h"
#include "../../src/lib/History.h"

namespace unexecutable_tests
{

class MockHistoryManager : public IHistoryManager
{
public:
	MOCK_METHOD(void, Undo, (), (override));
	MOCK_METHOD(void, Redo, (), (override));
	MOCK_METHOD(bool, CanUndo, (), (const, override));
	MOCK_METHOD(bool, CanRedo, (), (const, override));
};

namespace fs = std::filesystem;

class UnexecutableCommandTest : public ::testing::Test
{
protected:
	void SetUp() override
	{
		fs::remove_all("tmp");
	}

	void TearDown() override
	{
		fs::remove_all("tmp");
		if (fs::exists("test_image.png"))
		{
			fs::remove("test_image.png");
		}
	}

	void CreateTestImage()
	{
		std::ofstream file("test_image.png");
		file << "fake image content";
		file.close();
	}

	MockHistoryManager mockManager;
};

TEST_F(UnexecutableCommandTest, SetTitleCommandExecuteAndUnexecute)
{
	HtmlDocument doc(mockManager);
	doc.SetTitle("Initial Title");

	SetTitleCommand command(doc, "New Title");

	command.Execute();
	ASSERT_EQ(doc.GetTitle(), "New Title");
	ASSERT_TRUE(command.IsExecuted());

	command.Unexecute();
	ASSERT_EQ(doc.GetTitle(), "Initial Title");

	command.Execute();
	ASSERT_EQ(doc.GetTitle(), "New Title");

	command.Unexecute();
	ASSERT_EQ(doc.GetTitle(), "Initial Title");
}

TEST_F(UnexecutableCommandTest, ResizeImageCommandExecuteAndUnexecute)
{
	HtmlDocument doc(mockManager);
	CreateTestImage();
	doc.InsertImage("test_image.png", 100, 200, 0);

	ResizeImageCommand command(doc, 0, 300, 400);

	command.Execute();
	ASSERT_EQ(doc.GetItem(0).GetImage()->GetWidth(), 300);
	ASSERT_EQ(doc.GetItem(0).GetImage()->GetHeight(), 400);
	ASSERT_TRUE(command.IsExecuted());

	command.Unexecute();
	ASSERT_EQ(doc.GetItem(0).GetImage()->GetWidth(), 100);
	ASSERT_EQ(doc.GetItem(0).GetImage()->GetHeight(), 200);

	command.Execute();
	ASSERT_EQ(doc.GetItem(0).GetImage()->GetWidth(), 300);
	ASSERT_EQ(doc.GetItem(0).GetImage()->GetHeight(), 400);
}

TEST_F(UnexecutableCommandTest, InsertParagraphCommandExecuteAndUnexecute)
{
	HtmlDocument doc(mockManager);

	InsertParagraphCommand command(doc, std::nullopt, "Test Paragraph");

	command.Execute();
	ASSERT_EQ(doc.GetItemsCount(), 1);
	ASSERT_EQ(doc.GetItem(0).GetParagraph()->GetText(), "Test Paragraph");
	ASSERT_FALSE(doc.GetItem(0).IsDeleted());
	ASSERT_TRUE(command.IsExecuted());

	command.Unexecute();
	ASSERT_EQ(doc.GetItemsCount(), 1);
	ASSERT_TRUE(doc.GetItem(0).IsDeleted());

	command.Execute();
	ASSERT_EQ(doc.GetItemsCount(), 1);
	ASSERT_FALSE(doc.GetItem(0).IsDeleted());

	command.Unexecute();
	ASSERT_TRUE(doc.GetItem(0).IsDeleted());
}

TEST_F(UnexecutableCommandTest, InsertImageCommandExecuteAndUnexecute)
{
	HtmlDocument doc(mockManager);
	CreateTestImage();

	InsertImageCommand command(doc, std::nullopt, "test_image.png", 100, 200);

	command.Execute();
	ASSERT_EQ(doc.GetItemsCount(), 1);
	ASSERT_EQ(doc.GetItem(0).GetImage()->GetWidth(), 100);
	ASSERT_EQ(doc.GetItem(0).GetImage()->GetHeight(), 200);
	ASSERT_FALSE(doc.GetItem(0).IsDeleted());
	ASSERT_TRUE(command.IsExecuted());

	command.Unexecute();
	ASSERT_EQ(doc.GetItemsCount(), 1);
	ASSERT_TRUE(doc.GetItem(0).IsDeleted());

	command.Execute();
	ASSERT_EQ(doc.GetItemsCount(), 1);
	ASSERT_FALSE(doc.GetItem(0).IsDeleted());
}

TEST_F(UnexecutableCommandTest, DeleteItemCommandExecuteAndUnexecute)
{
	HtmlDocument doc(mockManager);
	doc.InsertParagraph("Test Paragraph", 0);

	DeleteItemCommand command(doc, 0);

	command.Execute();
	ASSERT_TRUE(doc.GetItem(0).IsDeleted());
	ASSERT_TRUE(command.IsExecuted());

	command.Unexecute();
	ASSERT_FALSE(doc.GetItem(0).IsDeleted());

	command.Execute();
	ASSERT_TRUE(doc.GetItem(0).IsDeleted());
}

TEST_F(
	UnexecutableCommandTest, UnexecuteWithoutExecuteThrowsNoErrorButDoesNothing)
{
	HtmlDocument doc(mockManager);
	doc.SetTitle("Initial Title");

	SetTitleCommand command(doc, "New Title");

	ASSERT_NO_THROW(command.Unexecute());
	ASSERT_EQ(doc.GetTitle(), "Initial Title");
	ASSERT_FALSE(command.IsExecuted());
}

TEST_F(UnexecutableCommandTest, InsertParagraphCommandDestructorDeletesItem)
{
	HtmlDocument doc(mockManager);
	{
		auto command = std::make_unique<InsertParagraphCommand>(
			doc, std::nullopt, "Test Paragraph");
		command->Execute();
		command->Unexecute();
		ASSERT_EQ(doc.GetItemsCount(), 1);
		ASSERT_TRUE(doc.GetItem(0).IsDeleted());
	}

	ASSERT_EQ(doc.GetItemsCount(), 0);
}

TEST_F(UnexecutableCommandTest, InsertImageCommandDestructorDeletesItem)
{
	HtmlDocument doc(mockManager);
	{
		CreateTestImage();
		auto command = std::make_unique<InsertImageCommand>(
			doc, std::nullopt, "test_image.png", 100, 200);
		command->Execute();
		command->Unexecute();
		ASSERT_EQ(doc.GetItemsCount(), 1);
		ASSERT_TRUE(doc.GetItem(0).IsDeleted());
	}

	ASSERT_EQ(doc.GetItemsCount(), 0);
}

TEST_F(UnexecutableCommandTest, DeleteItemCommandDestructorDeletesItem)
{
	HtmlDocument doc(mockManager);
	{
		doc.InsertParagraph("Test Paragraph", 0);
		auto command = std::make_unique<DeleteItemCommand>(doc, 0);
		command->Execute();
		ASSERT_TRUE(doc.GetItem(0).IsDeleted());
	}

	ASSERT_EQ(doc.GetItemsCount(), 0);
}

TEST_F(UnexecutableCommandTest, CreateUnexecuteCreateNewObject)
{
	HtmlDocument doc(mockManager);

	auto command1
		= std::make_unique<InsertParagraphCommand>(doc, 0, "First Paragraph");
	command1->Execute();
	command1->Unexecute();

	auto command2
		= std::make_unique<InsertParagraphCommand>(doc, 0, "Second Paragraph");
	command2->Execute();

	ASSERT_EQ(doc.GetItemsCount(), 2);
	ASSERT_TRUE(doc.GetItem(1).IsDeleted());
	ASSERT_EQ(doc.GetItem(1).GetParagraph()->GetText(), "First Paragraph");
	ASSERT_FALSE(doc.GetItem(0).IsDeleted());
	ASSERT_EQ(doc.GetItem(0).GetParagraph()->GetText(), "Second Paragraph");
}

TEST_F(
	UnexecutableCommandTest, MultipleCommandsExecuteAndUnexecuteInReverseOrder)
{
	HtmlDocument doc(mockManager);
	CreateTestImage();

	doc.SetTitle("Initial Title");
	doc.InsertParagraph("Initial Paragraph", 0);

	std::string initialTitle = doc.GetTitle();
	std::string initialParagraph = doc.GetItem(0).GetParagraph()->GetText();
	size_t initialCount = doc.GetItemsCount();

	{
		auto command1 = std::make_unique<SetTitleCommand>(doc, "New Title 1");
		auto command2
			= std::make_unique<InsertParagraphCommand>(doc, 0, "New Paragraph");
		auto command3 = std::make_unique<SetTitleCommand>(doc, "New Title 2");
		auto command4 = std::make_unique<InsertImageCommand>(
			doc, std::nullopt, "test_image.png", 100, 200);
		auto command5 = std::make_unique<DeleteItemCommand>(doc, 1);

		command1->Execute();
		command2->Execute();
		command3->Execute();
		command4->Execute();
		command5->Execute();

		command5->Unexecute();
		command4->Unexecute();
		command3->Unexecute();
		command2->Unexecute();
		command1->Unexecute();
	}

	ASSERT_EQ(doc.GetTitle(), initialTitle);
	ASSERT_EQ(doc.GetItemsCount(), initialCount);
	ASSERT_EQ(doc.GetItem(0).GetParagraph()->GetText(), initialParagraph);
	ASSERT_FALSE(doc.GetItem(0).IsDeleted());
}
} // namespace unexecutable_tests
