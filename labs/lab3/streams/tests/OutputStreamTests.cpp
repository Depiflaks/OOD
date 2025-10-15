#include "../src/lib/OutputStream.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

class MockOutputStream : public IOutputStream
{
public:
	MOCK_METHOD(void, WriteByte, (uint8_t), (override));
	MOCK_METHOD(void, WriteBlock, (const void*, std::streamsize), (override));
	MOCK_METHOD(void, Close, (), (override));
};

TEST(OutputStreamTest, MockInterface)
{
	MockOutputStream mock;
	EXPECT_CALL(mock, WriteByte(0xAA)).Times(1);
	EXPECT_CALL(mock, WriteBlock(nullptr, 0)).Times(1);
	EXPECT_CALL(mock, Close()).Times(1);

	mock.WriteByte(0xAA);
	mock.WriteBlock(nullptr, 0);
	mock.Close();
}

TEST(FileOutputStreamTest, WriteByte)
{
	auto file = std::make_unique<std::ofstream>("test_write_byte.bin", std::ios::binary);
	FileOutputStream stream(std::move(file));

	stream.WriteByte(0xAB);
	stream.Close();

	std::ifstream inFile("test_write_byte.bin", std::ios::binary);
	uint8_t result;
	inFile.read(reinterpret_cast<char*>(&result), 1);
	inFile.close();

	EXPECT_EQ(result, 0xAB);
	std::remove("test_write_byte.bin");
}

TEST(FileOutputStreamTest, WriteByteClosed)
{
	auto file = std::make_unique<std::ofstream>("test_write_closed.bin", std::ios::binary);
	FileOutputStream stream(std::move(file));
	stream.Close();

	EXPECT_THROW(stream.WriteByte(0xAA), std::runtime_error);
	std::remove("test_write_closed.bin");
}

TEST(FileOutputStreamTest, WriteBlockFourBytes)
{
	auto file = std::make_unique<std::ofstream>("test_write_block.bin", std::ios::binary);
	FileOutputStream stream(std::move(file));

	const uint8_t data[] = { 0x01, 0x02, 0x03, 0x04 };
	stream.WriteBlock(data, 4);
	stream.Close();

	std::ifstream inFile("test_write_block.bin", std::ios::binary);
	uint8_t result[4];
	inFile.read(reinterpret_cast<char*>(result), 4);
	inFile.close();

	EXPECT_EQ(result[0], 0x01);
	EXPECT_EQ(result[1], 0x02);
	EXPECT_EQ(result[2], 0x03);
	EXPECT_EQ(result[3], 0x04);
	std::remove("test_write_block.bin");
}

TEST(FileOutputStreamTest, WriteBlockPartial)
{
	auto file = std::make_unique<std::ofstream>("test_write_partial.bin", std::ios::binary);
	FileOutputStream stream(std::move(file));

	const uint8_t data[] = { 0xAA, 0xBB };
	stream.WriteBlock(data, 4);
	stream.Close();

	std::ifstream inFile("test_write_partial.bin", std::ios::binary);
	uint8_t result[4];
	inFile.read(reinterpret_cast<char*>(result), 4);
	inFile.close();

	EXPECT_EQ(result[0], 0xAA);
	EXPECT_EQ(result[1], 0xBB);
	std::remove("test_write_partial.bin");
}

TEST(MemoryOutputStreamTest, WriteByte)
{
	MemoryOutputStream stream;
	stream.WriteByte(0xCD);
	stream.Close();

	const auto& data = stream.GetData();
	EXPECT_EQ(data.size(), 1);
	EXPECT_EQ(data[0], 0xCD);
}

TEST(MemoryOutputStreamTest, WriteByteClosed)
{
	MemoryOutputStream stream;
	stream.Close();

	EXPECT_THROW(stream.WriteByte(0xEF), std::runtime_error);
}

TEST(MemoryOutputStreamTest, WriteBlockFourBytes)
{
	MemoryOutputStream stream;
	const uint8_t data[] = { 0x11, 0x22, 0x33, 0x44 };
	stream.WriteBlock(data, 4);
	stream.Close();

	const auto& result = stream.GetData();
	EXPECT_EQ(result.size(), 4);
	EXPECT_EQ(result[0], 0x11);
	EXPECT_EQ(result[1], 0x22);
	EXPECT_EQ(result[2], 0x33);
	EXPECT_EQ(result[3], 0x44);
}

TEST(MemoryOutputStreamTest, WriteBlockPartial)
{
	MemoryOutputStream stream;
	const uint8_t data[] = { 0x99, 0x88 };
	stream.WriteBlock(data, 4);
	stream.Close();

	const auto& result = stream.GetData();
	EXPECT_EQ(result.size(), 4);
	EXPECT_EQ(result[0], 0x99);
	EXPECT_EQ(result[1], 0x88);
}
