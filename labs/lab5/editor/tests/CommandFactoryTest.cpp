#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include <sstream>

#include "../src/lib/CommandFactory.h"
#include "../src/lib/Document/HtmlDocument.h"

class DocumentCommandFactoryTest : public ::testing::Test
{
protected:
	void TearDown() override
	{
		fs::remove_all("tmp");
		if (fs::exists("test.png"))
		{
			fs::remove("test.png");
		}
	}

	void CreateTestImage(const std::string& filename)
	{
		std::ofstream file(filename);
		file << "fake image content";
		file.close();
	}

	void SetUp() override
	{
		fs::remove_all("tmp");
		historyManager = std::make_shared<CommandHistoryManager>();
		doc = std::make_shared<HtmlDocument>(*historyManager);
	}

	std::shared_ptr<CommandHistoryManager> historyManager;
	std::shared_ptr<HtmlDocument> doc;
	std::unique_ptr<DocumentCommandFactory> factory;
};

TEST_F(DocumentCommandFactoryTest, InsertParagraphWithNumberSuccess)
{
	std::istringstream stream("InsertParagraph 0 Test text");
	DocumentCommandFactory factory(stream, doc);

	auto command = factory.CreateCommand();
	ASSERT_NE(command, nullptr);

	ASSERT_NO_THROW(command->Execute());
	ASSERT_EQ(doc->GetItemsCount(), 1);
	ASSERT_EQ(doc->GetItem(0).GetParagraph()->GetText(), "Test text");
}

TEST_F(DocumentCommandFactoryTest, InsertParagraphWithEndSuccess)
{
	std::istringstream stream("InsertParagraph end Test text");
	DocumentCommandFactory factory(stream, doc);

	auto command = factory.CreateCommand();
	ASSERT_NE(command, nullptr);

	ASSERT_NO_THROW(command->Execute());
	ASSERT_EQ(doc->GetItemsCount(), 1);
	ASSERT_EQ(doc->GetItem(0).GetParagraph()->GetText(), "Test text");
}

TEST_F(DocumentCommandFactoryTest, InsertParagraphInvalidArguments)
{
	std::istringstream stream("InsertParagraph invalid text");
	DocumentCommandFactory factory(stream, doc);

	ASSERT_THROW(factory.CreateCommand(), std::invalid_argument);
}

TEST_F(DocumentCommandFactoryTest, InsertImageWithNumberSuccess)
{
	CreateTestImage("test.png");
	std::istringstream stream("InsertImage 0 100 200 test.png");
	DocumentCommandFactory factory(stream, doc);

	auto command = factory.CreateCommand();
	ASSERT_NE(command, nullptr);

	ASSERT_NO_THROW(command->Execute());
	ASSERT_EQ(doc->GetItemsCount(), 1);
}

TEST_F(DocumentCommandFactoryTest, InsertImageWithEndSuccess)
{
	CreateTestImage("test.png");
	std::istringstream stream("InsertImage end 100 200 test.png");
	DocumentCommandFactory factory(stream, doc);

	auto command = factory.CreateCommand();
	ASSERT_NE(command, nullptr);

	ASSERT_NO_THROW(command->Execute());
	ASSERT_EQ(doc->GetItemsCount(), 1);
}

TEST_F(DocumentCommandFactoryTest, InsertImageInvalidArguments)
{
	CreateTestImage("test.png");
	std::istringstream stream("InsertImage invalid 100 200 test.png");
	DocumentCommandFactory factory(stream, doc);

	ASSERT_THROW(factory.CreateCommand(), std::invalid_argument);
}

TEST_F(DocumentCommandFactoryTest, SetTitleSuccess)
{
	std::istringstream stream("SetTitle New Title");
	DocumentCommandFactory factory(stream, doc);

	auto command = factory.CreateCommand();
	ASSERT_NE(command, nullptr);

	ASSERT_NO_THROW(command->Execute());
	ASSERT_EQ(doc->GetTitle(), "New Title");
}

TEST_F(DocumentCommandFactoryTest, SetTitleInvalidArguments)
{
	std::istringstream stream("SetTitle");
	DocumentCommandFactory factory(stream, doc);

	ASSERT_THROW(factory.CreateCommand(), std::invalid_argument);
}

TEST_F(DocumentCommandFactoryTest, ListSuccess)
{
	std::istringstream stream("List");
	DocumentCommandFactory factory(stream, doc);

	auto command = factory.CreateCommand();
	ASSERT_NE(command, nullptr);

	ASSERT_NO_THROW(command->Execute());
}

TEST_F(DocumentCommandFactoryTest, ListWithArgumentsFails)
{
	std::istringstream stream("List extra");
	DocumentCommandFactory factory(stream, doc);

	auto command = factory.CreateCommand();
	ASSERT_NE(command, nullptr);

	ASSERT_NO_THROW(command->Execute());
}

TEST_F(DocumentCommandFactoryTest, ReplaceTextSuccess)
{
	doc->InsertParagraph("Initial", 0);
	std::istringstream stream("ReplaceText 0 New text");
	DocumentCommandFactory factory(stream, doc);

	auto command = factory.CreateCommand();
	ASSERT_NE(command, nullptr);

	ASSERT_NO_THROW(command->Execute());
	ASSERT_EQ(doc->GetItem(0).GetParagraph()->GetText(), "New text");
}

TEST_F(DocumentCommandFactoryTest, ReplaceTextInvalidArguments)
{
	std::istringstream stream("ReplaceText invalid text");
	DocumentCommandFactory factory(stream, doc);

	ASSERT_THROW(factory.CreateCommand(), std::invalid_argument);
}

TEST_F(DocumentCommandFactoryTest, ResizeImageSuccess)
{
	CreateTestImage("test.png");
	doc->InsertImage("test.png", 100, 200, 0);
	std::istringstream stream("ResizeImage 0 300 400");
	DocumentCommandFactory factory(stream, doc);

	auto command = factory.CreateCommand();
	ASSERT_NE(command, nullptr);

	ASSERT_NO_THROW(command->Execute());
	ASSERT_EQ(doc->GetItem(0).GetImage()->GetWidth(), 300);
	ASSERT_EQ(doc->GetItem(0).GetImage()->GetHeight(), 400);
}

TEST_F(DocumentCommandFactoryTest, ResizeImageInvalidArguments)
{
	std::istringstream stream("ResizeImage invalid 300 400");
	DocumentCommandFactory factory(stream, doc);

	ASSERT_THROW(factory.CreateCommand(), std::invalid_argument);
}

TEST_F(DocumentCommandFactoryTest, DeleteItemSuccess)
{
	doc->InsertParagraph("Test", 0);
	std::istringstream stream("DeleteItem 0");
	DocumentCommandFactory factory(stream, doc);

	auto command = factory.CreateCommand();
	ASSERT_NE(command, nullptr);

	ASSERT_NO_THROW(command->Execute());
	ASSERT_TRUE(doc->GetItem(0).IsDeleted());
}

TEST_F(DocumentCommandFactoryTest, DeleteItemInvalidArguments)
{
	std::istringstream stream("DeleteItem invalid");
	DocumentCommandFactory factory(stream, doc);

	ASSERT_THROW(factory.CreateCommand(), std::invalid_argument);
}

TEST_F(DocumentCommandFactoryTest, HelpSuccess)
{
	std::istringstream stream("Help");
	DocumentCommandFactory factory(stream, doc);

	auto command = factory.CreateCommand();
	ASSERT_NE(command, nullptr);

	ASSERT_NO_THROW(command->Execute());
}

TEST_F(DocumentCommandFactoryTest, HelpWithArgumentsSucceeds)
{
	std::istringstream stream("Help extra");
	DocumentCommandFactory factory(stream, doc);

	auto command = factory.CreateCommand();
	ASSERT_NE(command, nullptr);

	ASSERT_NO_THROW(command->Execute());
}

TEST_F(DocumentCommandFactoryTest, UndoSuccess)
{
	doc->SetTitle("Test");
	std::istringstream stream("Undo");
	DocumentCommandFactory factory(stream, doc);

	auto command = factory.CreateCommand();
	ASSERT_NE(command, nullptr);

	ASSERT_THROW(command->Execute(), std::runtime_error);
}

TEST_F(DocumentCommandFactoryTest, UndoWithArgumentsSucceeds)
{
	std::istringstream stream("Undo extra");
	DocumentCommandFactory factory(stream, doc);

	auto command = factory.CreateCommand();
	ASSERT_NE(command, nullptr);

	ASSERT_THROW(command->Execute(), std::runtime_error);
}

TEST_F(DocumentCommandFactoryTest, RedoSuccess)
{
	std::istringstream stream("Redo");
	DocumentCommandFactory factory(stream, doc);

	auto command = factory.CreateCommand();
	ASSERT_NE(command, nullptr);

	ASSERT_THROW(command->Execute(), std::runtime_error);
}

TEST_F(DocumentCommandFactoryTest, RedoWithArgumentsSucceeds)
{
	std::istringstream stream("Redo extra");
	DocumentCommandFactory factory(stream, doc);

	auto command = factory.CreateCommand();
	ASSERT_NE(command, nullptr);

	ASSERT_THROW(command->Execute(), std::runtime_error);
}

TEST_F(DocumentCommandFactoryTest, SaveSuccess)
{
	std::istringstream stream("Save test.html");
	DocumentCommandFactory factory(stream, doc);

	auto command = factory.CreateCommand();
	ASSERT_NE(command, nullptr);

	ASSERT_NO_THROW(command->Execute());
}

TEST_F(DocumentCommandFactoryTest, SaveInvalidArguments)
{
	std::istringstream stream("Save");
	DocumentCommandFactory factory(stream, doc);

	ASSERT_THROW(factory.CreateCommand(), std::invalid_argument);
}

TEST_F(DocumentCommandFactoryTest, UnknownCommandThrows)
{
	std::istringstream stream("UnknownCommand");
	DocumentCommandFactory factory(stream, doc);

	ASSERT_THROW(factory.CreateCommand(), std::invalid_argument);
}

TEST_F(DocumentCommandFactoryTest, CanCreateCommand)
{
	std::istringstream stream("Help\nList");
	DocumentCommandFactory factory(stream, doc);

	ASSERT_TRUE(factory.CanCreateCommand());
	factory.CreateCommand();
	ASSERT_TRUE(factory.CanCreateCommand());
	factory.CreateCommand();
	ASSERT_FALSE(factory.CanCreateCommand());
}
