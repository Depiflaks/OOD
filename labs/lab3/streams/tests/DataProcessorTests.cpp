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

	std::vector<uint8_t> compressedBuffer(original.size() * 2);
	std::copy(original.begin(), original.end(), compressedBuffer.begin());
	std::streamsize compressedSize = packer.ProcessDataBlock(compressedBuffer.data(), original.size());
	compressedBuffer.resize(compressedSize);

	std::vector<uint8_t> decompressedBuffer(original.size() * 2);
	std::copy(compressedBuffer.begin(), compressedBuffer.end(), decompressedBuffer.begin());
	std::streamsize decompressedSize = unpacker.ProcessDataBlock(decompressedBuffer.data(), compressedSize);
	decompressedBuffer.resize(decompressedSize);

	EXPECT_EQ(original.size(), decompressedSize);
	EXPECT_EQ(original, decompressedBuffer);
}

TEST(CompressionTest, DoubleCompressDoubleDecompress)
{
	PackingDataProcessor packer1;
	PackingDataProcessor packer2;
	UnpackingDataProcessor unpacker2;
	UnpackingDataProcessor unpacker1;

	std::vector<uint8_t> original = { 0x44, 0x44, 0x44, 0x55, 0x55, 0x66, 0x66, 0x66, 0x66 };

	std::vector<uint8_t> buffer1(original.size() * 2);
	std::copy(original.begin(), original.end(), buffer1.begin());
	std::streamsize size1 = packer1.ProcessDataBlock(buffer1.data(), original.size());
	buffer1.resize(size1);

	std::vector<uint8_t> buffer2(buffer1.size() * 2);
	std::copy(buffer1.begin(), buffer1.end(), buffer2.begin());
	std::streamsize size2 = packer2.ProcessDataBlock(buffer2.data(), size1);
	buffer2.resize(size2);

	std::vector<uint8_t> buffer3(buffer2.size() * 2);
	std::copy(buffer2.begin(), buffer2.end(), buffer3.begin());
	std::streamsize size3 = unpacker2.ProcessDataBlock(buffer3.data(), size2);
	buffer3.resize(size3);

	std::vector<uint8_t> buffer4(buffer3.size() * 2);
	std::copy(buffer3.begin(), buffer3.end(), buffer4.begin());
	std::streamsize size4 = unpacker1.ProcessDataBlock(buffer4.data(), size3);
	buffer4.resize(size4);

	EXPECT_EQ(original.size(), size4);
	EXPECT_EQ(original, buffer4);
}
