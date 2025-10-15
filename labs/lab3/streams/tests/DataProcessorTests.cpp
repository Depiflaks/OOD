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
