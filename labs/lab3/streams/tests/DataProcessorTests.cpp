#include "../src/lib/DataProcessor.h"
#include <gmock/gmock.h>

TEST(EncryptionTest, NormalKey)
{
	EncodingDataProcessor encoder(12345);
	DecodingDataProcessor decoder(12345);

	uint8_t original = 0xAB;
	uint8_t encrypted = encoder.ProcessByte(original);
	uint8_t decrypted = decoder.ProcessByte(encrypted);

	EXPECT_EQ(original, decrypted);
	EXPECT_NE(original, encrypted);
}

TEST(EncryptionTest, NegativeKey)
{
	EncodingDataProcessor encoder(-12345);
	DecodingDataProcessor decoder(-12345);

	uint8_t original = 0xCD;
	uint8_t encrypted = encoder.ProcessByte(original);
	uint8_t decrypted = decoder.ProcessByte(encrypted);

	EXPECT_EQ(original, decrypted);
	EXPECT_NE(original, encrypted);
}

TEST(EncryptionTest, EncryptThenDecrypt)
{
	EncodingDataProcessor encoder(999);
	DecodingDataProcessor decoder(999);

	std::vector<uint8_t> original = { 0x01, 0x02, 0x03, 0x04, 0x05 };
	std::vector<uint8_t> buffer = original;

	encoder.ProcessDataBlock(buffer.data(), buffer.size());
	decoder.ProcessDataBlock(buffer.data(), buffer.size());

	EXPECT_EQ(buffer, original);
}

TEST(EncryptionTest, DoubleEncryptDoubleDecrypt)
{
	EncodingDataProcessor encoder1(111);
	EncodingDataProcessor encoder2(222);
	DecodingDataProcessor decoder2(222);
	DecodingDataProcessor decoder1(111);

	uint8_t original = 0xEF;

	uint8_t encrypted1 = encoder1.ProcessByte(original);
	uint8_t encrypted2 = encoder2.ProcessByte(encrypted1);
	uint8_t decrypted2 = decoder2.ProcessByte(encrypted2);
	uint8_t decrypted1 = decoder1.ProcessByte(decrypted2);

	EXPECT_EQ(original, decrypted1);
}

TEST(CompressionTest, SameCompressionForSameData)
{
	PackingDataProcessor packer;

	std::vector<uint8_t> data1 = { 0xAA, 0xAA, 0xAA, 0xAA, 0xBB, 0xBB, 0xBB };
	std::vector<uint8_t> data2 = { 0xAA, 0xAA, 0xAA, 0xAA, 0xBB, 0xBB, 0xBB };

	std::vector<uint8_t> compressed1 = data1;
	std::vector<uint8_t> compressed2 = data2;

	std::streamsize size1 = packer.ProcessDataBlock(compressed1.data(), compressed1.size());
	std::streamsize size2 = packer.ProcessDataBlock(compressed2.data(), compressed2.size());

	EXPECT_EQ(size1, size2);
	EXPECT_TRUE(std::equal(compressed1.begin(), compressed1.begin() + size1,
		compressed2.begin(), compressed2.begin() + size2));
}

TEST(CompressionTest, SameDecompressionForSameData)
{
	UnpackingDataProcessor unpacker;

	const std::vector<uint8_t> compressed_data = { 0xFF, 0xAA, 0x04, 0xFF, 0xBB, 0x03 };

	std::vector<uint8_t> buffer1(1024);
	std::vector<uint8_t> buffer2(1024);

	std::copy(compressed_data.begin(), compressed_data.end(), buffer1.begin());
	std::copy(compressed_data.begin(), compressed_data.end(), buffer2.begin());

	std::streamsize size1 = unpacker.ProcessDataBlock(buffer1.data(), compressed_data.size());
	std::streamsize size2 = unpacker.ProcessDataBlock(buffer2.data(), compressed_data.size());

	EXPECT_EQ(size1, size2);
	EXPECT_TRUE(std::equal(buffer1.begin(), buffer1.begin() + size1,
		buffer2.begin(), buffer2.begin() + size2));
}

TEST(CompressionTest, CompressThenDecompress)
{
	PackingDataProcessor packer;
	UnpackingDataProcessor unpacker;

	std::vector<uint8_t> original = { 0x11, 0x11, 0x11, 0x22, 0x22, 0x33, 0x33, 0x33, 0x33 };
	std::vector<uint8_t> buffer(1024);
	std::copy(original.begin(), original.end(), buffer.begin());

	std::streamsize compressedSize = packer.ProcessDataBlock(buffer.data(), original.size());
	std::streamsize decompressedSize = unpacker.ProcessDataBlock(buffer.data(), compressedSize);

	buffer.resize(decompressedSize);

	EXPECT_EQ(original.size(), decompressedSize);
	EXPECT_EQ(original, buffer);
}

TEST(CompressionTest, DoubleCompressDoubleDecompress)
{
	PackingDataProcessor packer;
	UnpackingDataProcessor unpacker;

	std::vector<uint8_t> original = { 0x44, 0x44, 0x44, 0x55, 0x55, 0x66, 0x66, 0x66, 0x66 };
	std::vector<uint8_t> buffer(1024);
	std::copy(original.begin(), original.end(), buffer.begin());

	std::streamsize size1 = packer.ProcessDataBlock(buffer.data(), original.size());
	std::streamsize size2 = packer.ProcessDataBlock(buffer.data(), size1);
	std::streamsize size3 = unpacker.ProcessDataBlock(buffer.data(), size2);
	std::streamsize size4 = unpacker.ProcessDataBlock(buffer.data(), size3);

	buffer.resize(size4);

	EXPECT_EQ(original.size(), size4);
	EXPECT_EQ(original, buffer);
}
