#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../../src/lib/Document/Document.h"

class MockHistoryManager : public IHistoryManager
{
public:
	MOCK_METHOD(void, Undo, (), (override));
	MOCK_METHOD(void, Redo, (), (override));
	MOCK_METHOD(bool, CanUndo, (), (const, override));
	MOCK_METHOD(bool, CanRedo, (), (const, override));
};

class HtmlDocumentTest : public ::testing::Test
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
		if (fs::exists("test_save.html"))
		{
			fs::remove("test_save.html");
		}
	}

	void CreateTestImage()
	{
		std::ofstream file("test_image.png");
		file << "fake image content";
		file.close();
	}
};

TEST_F(HtmlDocumentTest, InsertParagraphAtEndWhenEmpty)
{
	MockHistoryManager mockManager;
	HtmlDocument doc(mockManager);

	auto paragraph = doc.InsertParagraph("Test text");

	ASSERT_EQ(doc.GetItemsCount(), 1);
	ASSERT_EQ(doc.GetItem(0).GetParagraph()->GetText(), "Test text");
}

TEST_F(HtmlDocumentTest, InsertParagraphAtIndex0WhenEmpty)
{
	MockHistoryManager mockManager;
	HtmlDocument doc(mockManager);

	auto paragraph = doc.InsertParagraph("Test text", 0);

	ASSERT_EQ(doc.GetItemsCount(), 1);
	ASSERT_EQ(doc.GetItem(0).GetParagraph()->GetText(), "Test text");
}

TEST_F(HtmlDocumentTest, InsertParagraphAtIndex0WithThreeParagraphs)
{
	MockHistoryManager mockManager;
	HtmlDocument doc(mockManager);
	doc.InsertParagraph("First");
	doc.InsertParagraph("Second");
	doc.InsertParagraph("Third");

	auto paragraph = doc.InsertParagraph("New First", 0);

	ASSERT_EQ(doc.GetItemsCount(), 4);
	ASSERT_EQ(doc.GetItem(0).GetParagraph()->GetText(), "New First");
	ASSERT_EQ(doc.GetItem(1).GetParagraph()->GetText(), "First");
}

TEST_F(HtmlDocumentTest, InsertParagraphAtIndex2WithThreeParagraphs)
{
	MockHistoryManager mockManager;
	HtmlDocument doc(mockManager);
	doc.InsertParagraph("First");
	doc.InsertParagraph("Second");
	doc.InsertParagraph("Third");

	auto paragraph = doc.InsertParagraph("New Middle", 2);

	ASSERT_EQ(doc.GetItemsCount(), 4);
	ASSERT_EQ(doc.GetItem(2).GetParagraph()->GetText(), "New Middle");
	ASSERT_EQ(doc.GetItem(3).GetParagraph()->GetText(), "Third");
}

TEST_F(HtmlDocumentTest, InsertParagraphAtEndWithThreeParagraphs)
{
	MockHistoryManager mockManager;
	HtmlDocument doc(mockManager);
	doc.InsertParagraph("First");
	doc.InsertParagraph("Second");
	doc.InsertParagraph("Third");

	auto paragraph = doc.InsertParagraph("New End");

	ASSERT_EQ(doc.GetItemsCount(), 4);
	ASSERT_EQ(doc.GetItem(3).GetParagraph()->GetText(), "New End");
}

TEST_F(HtmlDocumentTest, InsertParagraphAtIndex2WithTwoParagraphs)
{
	MockHistoryManager mockManager;
	HtmlDocument doc(mockManager);
	doc.InsertParagraph("First");
	doc.InsertParagraph("Second");

	auto paragraph = doc.InsertParagraph("Third", 2);

	ASSERT_EQ(doc.GetItemsCount(), 3);
	ASSERT_EQ(doc.GetItem(2).GetParagraph()->GetText(), "Third");
}

TEST_F(HtmlDocumentTest, InsertParagraphAtInvalidIndexThrowsException)
{
	MockHistoryManager mockManager;
	HtmlDocument doc(mockManager);
	doc.InsertParagraph("First");
	doc.InsertParagraph("Second");

	ASSERT_THROW(doc.InsertParagraph("Invalid", 3), std::runtime_error);
}

TEST_F(HtmlDocumentTest, DeleteItemFromTwoParagraphs)
{
	MockHistoryManager mockManager;
	HtmlDocument doc(mockManager);
	doc.InsertParagraph("First");
	doc.InsertParagraph("Second");

	doc.DeleteItem(0);

	ASSERT_EQ(doc.GetItemsCount(), 1);
	ASSERT_EQ(doc.GetItem(0).GetParagraph()->GetText(), "Second");
}

TEST_F(HtmlDocumentTest, SetAndGetTitle)
{
	MockHistoryManager mockManager;
	HtmlDocument doc(mockManager);

	doc.SetTitle("Test Title");

	ASSERT_EQ(doc.GetTitle(), "Test Title");
}

TEST_F(HtmlDocumentTest, UndoRedoCallsHistoryManager)
{
	MockHistoryManager mockManager;
	HtmlDocument doc(mockManager);

	EXPECT_CALL(mockManager, Undo()).Times(1);
	EXPECT_CALL(mockManager, Redo()).Times(1);
	EXPECT_CALL(mockManager, CanUndo()).Times(1);
	EXPECT_CALL(mockManager, CanRedo()).Times(1);

	doc.Undo();
	doc.Redo();
	doc.CanUndo();
	doc.CanRedo();
}

TEST_F(HtmlDocumentTest, InsertImageCopiesFileToTmp)
{
	MockHistoryManager mockManager;
	HtmlDocument doc(mockManager);
	CreateTestImage();

	auto image = doc.InsertImage("test_image.png", 100, 200);

	ASSERT_TRUE(fs::exists("tmp"));
	ASSERT_EQ(image->GetWidth(), 100);
	ASSERT_EQ(image->GetHeight(), 200);
	ASSERT_FALSE(image->GetPath().empty());
}

TEST_F(HtmlDocumentTest, DeleteImageRemovesFileFromTmp)
{
	MockHistoryManager mockManager;
	HtmlDocument doc(mockManager);
	CreateTestImage();

	auto image = doc.InsertImage("test_image.png", 100, 200);
	std::string imagePath = image->GetPath();

	ASSERT_TRUE(fs::exists("tmp/" + imagePath));
	doc.DeleteItem(0);

	ASSERT_FALSE(fs::exists("tmp/" + imagePath));
	ASSERT_TRUE(fs::exists("test_image.png"));
}

TEST_F(HtmlDocumentTest, InsertMultipleImagesCreatesDifferentFiles)
{
	MockHistoryManager mockManager;
	HtmlDocument doc(mockManager);
	CreateTestImage();

	auto image1 = doc.InsertImage("test_image.png", 100, 200);
	auto image2 = doc.InsertImage("test_image.png", 300, 400);

	ASSERT_EQ(doc.GetItemsCount(), 2);
	ASSERT_NE(image1->GetPath(), image2->GetPath());
	ASSERT_TRUE(fs::exists("tmp/" + image1->GetPath()));
	ASSERT_TRUE(fs::exists("tmp/" + image2->GetPath()));
}

TEST_F(HtmlDocumentTest, SaveDocumentWithParagraphAndImage)
{
	MockHistoryManager mockManager;
	HtmlDocument doc(mockManager);
	CreateTestImage();

	doc.SetTitle("Test Document");
	doc.InsertParagraph("Test paragraph");
	doc.InsertImage("test_image.png", 100, 200);

	doc.Save("test_save.html");

	ASSERT_TRUE(fs::exists("test_save.html"));

	std::ifstream file("test_save.html");
	std::string content((std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>());

	ASSERT_TRUE(content.find("Test Document") != std::string::npos);
	ASSERT_TRUE(content.find("Test paragraph") != std::string::npos);
	ASSERT_TRUE(content.find("width=\"100\"") != std::string::npos);
	ASSERT_TRUE(content.find("height=\"200\"") != std::string::npos);
}
