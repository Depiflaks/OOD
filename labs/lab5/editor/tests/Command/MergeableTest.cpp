#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../../src/lib/Command/Mergeable.h"
#include "../../src/lib/History.h"

namespace mergable_tests
{
class MockHistoryManager : public IHistoryManager
{
public:
	MOCK_METHOD(void, Undo, (), (override));
	MOCK_METHOD(void, Redo, (), (override));
	MOCK_METHOD(bool, CanUndo, (), (const, override));
	MOCK_METHOD(bool, CanRedo, (), (const, override));
};

class MergableCommandTest : public ::testing::Test
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

TEST_F(MergableCommandTest, ReplaceTextCommandExecuteAndUnexecuteMultipleTimes)
{
	HtmlDocument doc(mockManager);
	doc.InsertParagraph("Initial text", 0);

	ReplaceTextCommand command(doc, 0, "First change");

	command.Execute();
	ASSERT_EQ(doc.GetItem(0).GetParagraph()->GetText(), "First change");
	ASSERT_TRUE(command.IsExecuted());

	command.Unexecute();
	ASSERT_EQ(doc.GetItem(0).GetParagraph()->GetText(), "Initial text");

	command.Execute();
	ASSERT_EQ(doc.GetItem(0).GetParagraph()->GetText(), "First change");

	command.Unexecute();
	ASSERT_EQ(doc.GetItem(0).GetParagraph()->GetText(), "Initial text");
}

TEST_F(MergableCommandTest, ReplaceTextCommandsMergeAndUnexecuteTogether)
{
	HtmlDocument doc(mockManager);
	doc.InsertParagraph("Initial text", 0);

	auto command1
		= std::make_shared<ReplaceTextCommand>(doc, 0, "First change");
	auto command2
		= std::make_shared<ReplaceTextCommand>(doc, 0, "Second change");

	command1->Execute();
	bool merged = command1->TryReplace(command2);

	ASSERT_TRUE(merged);
	ASSERT_EQ(doc.GetItem(0).GetParagraph()->GetText(), "Second change");

	command1->Unexecute();
	ASSERT_EQ(doc.GetItem(0).GetParagraph()->GetText(), "Initial text");
}

TEST_F(MergableCommandTest, MultipleReplaceTextCommandsWithDifferentParagraphs)
{
	HtmlDocument doc(mockManager);
	doc.InsertParagraph("First initial", 0);
	doc.InsertParagraph("Second initial", 1);

	auto command1
		= std::make_shared<ReplaceTextCommand>(doc, 0, "First changed");
	auto command2
		= std::make_shared<ReplaceTextCommand>(doc, 0, "First changed again");
	auto command3
		= std::make_shared<ReplaceTextCommand>(doc, 1, "Second changed");
	auto command4 = std::make_shared<ReplaceTextCommand>(doc, 0, "First final");

	command1->Execute();
	command1->TryReplace(command2);
	command3->Execute();
	command1->TryReplace(command4);

	ASSERT_EQ(doc.GetItem(0).GetParagraph()->GetText(), "First final");
	ASSERT_EQ(doc.GetItem(1).GetParagraph()->GetText(), "Second changed");

	command1->Unexecute();
	ASSERT_EQ(doc.GetItem(0).GetParagraph()->GetText(), "First initial");
	ASSERT_EQ(doc.GetItem(1).GetParagraph()->GetText(), "Second changed");

	command3->Unexecute();
	ASSERT_EQ(doc.GetItem(0).GetParagraph()->GetText(), "First initial");
	ASSERT_EQ(doc.GetItem(1).GetParagraph()->GetText(), "Second initial");
}

class TestMergableCommand : public MergableCommand
{
public:
	TestMergableCommand(IDocument& document)
		: MergableCommand(document)
	{
	}

	void Execute() override
	{
		m_executed = true;
	}

	void Unexecute() override
	{
		m_executed = false;
	}

	bool TryReplace(std::shared_ptr<MergableCommand> command) override
	{
		return false;
	}
};

TEST_F(MergableCommandTest, DifferentMergableCommandsDoNotMerge)
{
	HtmlDocument doc(mockManager);
	doc.InsertParagraph("Initial text", 0);

	auto command1
		= std::make_shared<ReplaceTextCommand>(doc, 0, "First change");
	auto command2 = std::make_shared<TestMergableCommand>(doc);

	command1->Execute();
	bool merged = command1->TryReplace(command2);

	ASSERT_FALSE(merged);
	ASSERT_EQ(doc.GetItem(0).GetParagraph()->GetText(), "First change");

	command1->Unexecute();
	ASSERT_EQ(doc.GetItem(0).GetParagraph()->GetText(), "Initial text");
}

TEST_F(MergableCommandTest, ReplaceTextCommandThrowsWhenPositionIsImage)
{
	HtmlDocument doc(mockManager);
	CreateTestImage();
	doc.InsertImage("test_image.png", 100, 200, 0);

	ReplaceTextCommand command(doc, 0, "New text");

	ASSERT_THROW(command.Execute(), std::runtime_error);
	ASSERT_FALSE(command.IsExecuted());
}
} // namespace mergable_tests
