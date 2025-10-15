#include "../src/lib/InputStream.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

class MockInputStream : public IInputStream
{
public:
	MOCK_METHOD(bool, IsEOF, (), (override));
	MOCK_METHOD(uint8_t, ReadByte, (), (override));
	MOCK_METHOD(std::streamsize, ReadBlock, (void*, std::streamsize), (override));
};

TEST(InputStreamTest, MockInterface)
{
	MockInputStream mock;
	EXPECT_CALL(mock, IsEOF()).Times(1);
	EXPECT_CALL(mock, ReadByte()).Times(1);
	EXPECT_CALL(mock, ReadBlock(nullptr, 0)).Times(1);

	mock.IsEOF();
	mock.ReadByte();
	mock.ReadBlock(nullptr, 0);
}

TEST(MemoryInputStreamTest, EmptyStream)
{
	const uint8_t data[] = { 0 };
	MemoryInputStream stream(data, 0);

	EXPECT_TRUE(stream.IsEOF());

	uint8_t buffer[10] = { 0xFF, 0xFF, 0xFF, 0xFF };
	std::streamsize result = stream.ReadBlock(buffer, 3);

	EXPECT_EQ(result, 0);
	EXPECT_EQ(buffer[0], 0xFF);
	EXPECT_EQ(buffer[1], 0xFF);
	EXPECT_EQ(buffer[2], 0xFF);

	EXPECT_THROW(stream.ReadByte(), std::runtime_error);
}

TEST(MemoryInputStreamTest, ReadFourBytes)
{
	const uint8_t data[] = { 0x01, 0x02, 0x03, 0x04 };
	MemoryInputStream stream(data, 4);

	EXPECT_FALSE(stream.IsEOF());

	uint8_t byte1 = stream.ReadByte();
	EXPECT_EQ(byte1, 0x01);

	uint8_t buffer[3] = { 0 };
	std::streamsize result = stream.ReadBlock(buffer, 3);

	EXPECT_EQ(result, 3);
	EXPECT_EQ(buffer[0], 0x02);
	EXPECT_EQ(buffer[1], 0x03);
	EXPECT_EQ(buffer[2], 0x04);
	EXPECT_TRUE(stream.IsEOF());
}

TEST(MemoryInputStreamTest, ReadMoreThanAvailable)
{
	const uint8_t data[] = { 0x0A, 0x0B, 0x0C, 0x0D };
	MemoryInputStream stream(data, 4);

	uint8_t byte1 = stream.ReadByte();
	EXPECT_EQ(byte1, 0x0A);

	uint8_t buffer[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
	std::streamsize result = stream.ReadBlock(buffer, 6);

	EXPECT_EQ(result, 3);
	EXPECT_EQ(buffer[0], 0x0B);
	EXPECT_EQ(buffer[1], 0x0C);
	EXPECT_EQ(buffer[2], 0x0D);
	EXPECT_EQ(buffer[3], 0xFF);
	EXPECT_EQ(buffer[4], 0xFF);
	EXPECT_EQ(buffer[5], 0xFF);
	EXPECT_TRUE(stream.IsEOF());
}

TEST(FileInputStreamTest, EmptyFile)
{
	auto file = std::make_unique<std::ifstream>();
	file->open("empty_test_file.bin", std::ios::binary);

	FileInputStream stream(std::move(file));

	uint8_t buffer[10] = { 0xFF, 0xFF, 0xFF, 0xFF };
	std::streamsize result = stream.ReadBlock(buffer, 3);

	EXPECT_EQ(result, 0);
	EXPECT_EQ(buffer[0], 0xFF);
	EXPECT_EQ(buffer[1], 0xFF);
	EXPECT_EQ(buffer[2], 0xFF);

	EXPECT_THROW(stream.ReadByte(), std::runtime_error);
	EXPECT_TRUE(stream.IsEOF());

	stream.Close();
	std::remove("empty_test_file.bin");
}

TEST(FileInputStreamTest, ReadFourBytes)
{
	const uint8_t data[] = { 0x11, 0x22, 0x33, 0x44, 0x55 };

	std::ofstream outFile("four_bytes_test_file.bin", std::ios::binary);
	outFile.write(reinterpret_cast<const char*>(data), 5);
	outFile.close();

	auto file = std::make_unique<std::ifstream>("four_bytes_test_file.bin", std::ios::binary);
	FileInputStream stream(std::move(file));

	EXPECT_FALSE(stream.IsEOF());

	uint8_t byte1 = stream.ReadByte();
	EXPECT_EQ(byte1, 0x11);

	uint8_t buffer[3] = { 0 };
	std::streamsize result = stream.ReadBlock(buffer, 3);

	EXPECT_EQ(result, 3);
	EXPECT_EQ(buffer[0], 0x22);
	EXPECT_EQ(buffer[1], 0x33);
	EXPECT_EQ(buffer[2], 0x44);
	EXPECT_FALSE(stream.IsEOF());

	stream.Close();
	std::remove("four_bytes_test_file.bin");
}

TEST(FileInputStreamTest, ReadMoreThanAvailable)
{
	const uint8_t data[] = { 0xAA, 0xBB, 0xCC, 0xDD };

	std::ofstream outFile("more_bytes_test_file.bin", std::ios::binary);
	outFile.write(reinterpret_cast<const char*>(data), 4);
	outFile.close();

	auto file = std::make_unique<std::ifstream>("more_bytes_test_file.bin", std::ios::binary);
	FileInputStream stream(std::move(file));

	uint8_t byte1 = stream.ReadByte();
	EXPECT_EQ(byte1, 0xAA);

	uint8_t buffer[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
	std::streamsize result = stream.ReadBlock(buffer, 6);

	EXPECT_EQ(result, 3);
	EXPECT_EQ(buffer[0], 0xBB);
	EXPECT_EQ(buffer[1], 0xCC);
	EXPECT_EQ(buffer[2], 0xDD);
	EXPECT_EQ(buffer[3], 0xFF);
	EXPECT_EQ(buffer[4], 0xFF);
	EXPECT_EQ(buffer[5], 0xFF);
	EXPECT_TRUE(stream.IsEOF());

	stream.Close();
	std::remove("more_bytes_test_file.bin");
}
