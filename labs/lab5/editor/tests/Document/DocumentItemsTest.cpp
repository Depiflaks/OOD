#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>

#include "../../src/lib/Document/DocumentItems.h"

TEST(ParagraphTest, TextOperations)
{
	auto paragraph = std::make_shared<Paragraph>();
	paragraph->SetText("Test text");
	ASSERT_EQ(paragraph->GetText(), "Test text");

	paragraph->SetText("New text");
	ASSERT_EQ(paragraph->GetText(), "New text");
}

TEST(ImageTest, ImageOperations)
{
	auto image = std::make_shared<Image>();
	image->Resize(100, 200);
	ASSERT_EQ(image->GetWidth(), 100);
	ASSERT_EQ(image->GetHeight(), 200);

	image->Resize(300, 400);
	ASSERT_EQ(image->GetWidth(), 300);
	ASSERT_EQ(image->GetHeight(), 400);
}

TEST(ConstDocumentItemTest, ParagraphItem)
{
	auto paragraph = std::make_shared<Paragraph>();
	paragraph->SetText("Const paragraph");
	ConstDocumentItem item(paragraph, true);

	ASSERT_EQ(item.GetParagraph()->GetText(), "Const paragraph");
	ASSERT_TRUE(item.IsDeleted());
	ASSERT_EQ(item.GetImage(), nullptr);
}

TEST(ConstDocumentItemTest, ImageItem)
{
	auto image = std::make_shared<Image>();
	image->Resize(150, 250);
	ConstDocumentItem item(image, false);

	ASSERT_EQ(item.GetImage()->GetWidth(), 150);
	ASSERT_EQ(item.GetImage()->GetHeight(), 250);
	ASSERT_FALSE(item.IsDeleted());
	ASSERT_EQ(item.GetParagraph(), nullptr);
}

TEST(DocumentItemTest, ParagraphItemModification)
{
	auto paragraph = std::make_shared<Paragraph>();
	paragraph->SetText("Original");
	DocumentItem item(paragraph);

	item.GetParagraph()->SetText("Modified");
	ASSERT_EQ(item.GetParagraph()->GetText(), "Modified");
	ASSERT_FALSE(item.IsDeleted());

	item.SetIsDeleted(true);
	ASSERT_TRUE(item.IsDeleted());
}

TEST(DocumentItemTest, ImageItemModification)
{
	auto image = std::make_shared<Image>();
	image->Resize(100, 100);
	DocumentItem item(image);

	item.GetImage()->Resize(200, 300);
	ASSERT_EQ(item.GetImage()->GetWidth(), 200);
	ASSERT_EQ(item.GetImage()->GetHeight(), 300);
	ASSERT_FALSE(item.IsDeleted());

	item.SetIsDeleted(true);
	ASSERT_TRUE(item.IsDeleted());
}
