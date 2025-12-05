//
// Created by smmm on 05.12.2025.
//
#include "../src/lib/Image.h"
#include <gtest/gtest.h>

class TestImage : public Image
{
public:
	using Image::Image;
	std::vector<CoW<Tile>>& Tiles()
	{
		return GetTiles();
	}
};

TEST(ImageTilesTest, SixteenBySixteenSingleUnderlyingTile)
{
	const Color baseColor = 0x010203;
	TestImage img({ 16, 16 }, baseColor);
	auto& tiles = img.Tiles();
	ASSERT_EQ(4u, tiles.size());

	for (auto& t : tiles)
	{
		EXPECT_EQ(4, t.GetInstanceCount());
	}

	EXPECT_EQ(baseColor, img.GetPixel({ 0, 0 }));
	EXPECT_EQ(baseColor, img.GetPixel({ 15, 15 }));

	Point p{ 0, 0 };
	int tileX = p.x / Tile::SIZE;
	int tileY = p.y / Tile::SIZE;
	int index = tileY * 2 + tileX;
	Point local{ p.x % Tile::SIZE, p.y % Tile::SIZE };
	EXPECT_EQ(baseColor, tiles[static_cast<size_t>(index)]->GetPixel(local));
}

TEST(ImageTilesTest, SeventeenBySeventeenNineTiles)
{
	const Color baseColor = 0x0A0B0C;
	TestImage img({ 17, 17 }, baseColor);
	auto& tiles = img.Tiles();
	EXPECT_EQ(9u, tiles.size());

	for (auto& t : tiles)
	{
		EXPECT_EQ(9, t.GetInstanceCount());
	}

	EXPECT_EQ(baseColor, img.GetPixel({ 0, 0 }));
	EXPECT_EQ(baseColor, img.GetPixel({ 16, 16 }));

	Point p{ 0, 0 };
	int tileX = p.x / Tile::SIZE;
	int tileY = p.y / Tile::SIZE;
	int index = tileY * 3 + tileX;
	Point local{ p.x % Tile::SIZE, p.y % Tile::SIZE };
	EXPECT_EQ(baseColor, tiles[static_cast<size_t>(index)]->GetPixel(local));
}

TEST(ImageTilesTest, SixteenBySixteenModifyOneTileCreatesOneUnique)
{
	const Color baseColor = 0x000000;
	const Color newColor = 0xFFFFFF;
	TestImage img({ 16, 16 }, baseColor);
	auto& tiles = img.Tiles();
	ASSERT_EQ(4u, tiles.size());

	for (auto& t : tiles)
	{
		ASSERT_EQ(4, t.GetInstanceCount());
	}

	Point p{ 0, 0 };
	img.SetPixel(p, newColor);

	EXPECT_EQ(newColor, img.GetPixel(p));
	EXPECT_EQ(baseColor, img.GetPixel({ 8, 0 }));
	EXPECT_EQ(baseColor, img.GetPixel({ 0, 8 }));
	EXPECT_EQ(baseColor, img.GetPixel({ 8, 8 }));

	int tileX = p.x / Tile::SIZE;
	int tileY = p.y / Tile::SIZE;
	int index = tileY * 2 + tileX;
	Point local{ p.x % Tile::SIZE, p.y % Tile::SIZE };
	EXPECT_EQ(newColor, tiles[static_cast<size_t>(index)]->GetPixel(local));

	int count1 = 0;
	int count3 = 0;
	for (auto& t : tiles)
	{
		int c = t.GetInstanceCount();
		if (c == 1)
		{
			++count1;
		}
		else if (c == 3)
		{
			++count3;
		}
		else
		{
			FAIL() << "Unexpected instance count: " << c;
		}
	}
	EXPECT_EQ(1, count1);
	EXPECT_EQ(3, count3);
	EXPECT_EQ(1, tiles[static_cast<size_t>(index)].GetInstanceCount());
}

TEST(ImageTilesTest, SixteenBySixteenModifySameTileTwiceKeepsSharingPattern)
{
	const Color baseColor = 0x101010;
	const Color color1 = 0x202020;
	const Color color2 = 0x303030;
	TestImage img({ 16, 16 }, baseColor);
	auto& tiles = img.Tiles();
	ASSERT_EQ(4u, tiles.size());

	for (auto& t : tiles)
	{
		ASSERT_EQ(4, t.GetInstanceCount());
	}

	Point p1{ 0, 0 };
	Point p2{ 1, 1 };
	img.SetPixel(p1, color1);
	img.SetPixel(p2, color2);

	EXPECT_EQ(color2, img.GetPixel(p2));

	int tileX = p1.x / Tile::SIZE;
	int tileY = p1.y / Tile::SIZE;
	int index = tileY * 2 + tileX;
	Point local1{ p1.x % Tile::SIZE, p1.y % Tile::SIZE };
	Point local2{ p2.x % Tile::SIZE, p2.y % Tile::SIZE };

	EXPECT_EQ(color1, tiles[static_cast<size_t>(index)]->GetPixel(local1));
	EXPECT_EQ(color2, tiles[static_cast<size_t>(index)]->GetPixel(local2));

	EXPECT_EQ(baseColor, img.GetPixel({ 8, 0 }));
	EXPECT_EQ(baseColor, img.GetPixel({ 0, 8 }));
	EXPECT_EQ(baseColor, img.GetPixel({ 8, 8 }));

	int count1 = 0;
	int count3 = 0;
	for (auto& t : tiles)
	{
		int c = t.GetInstanceCount();
		if (c == 1)
		{
			++count1;
		}
		else if (c == 3)
		{
			++count3;
		}
		else
		{
			FAIL() << "Unexpected instance count: " << c;
		}
	}
	EXPECT_EQ(1, count1);
	EXPECT_EQ(3, count3);
	EXPECT_EQ(1, tiles[static_cast<size_t>(index)].GetInstanceCount());
}
