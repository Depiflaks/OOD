#ifndef FILETRANSFORMER_H
#define FILETRANSFORMER_H

#include "lib/MakeDecorator.h"
#include "lib/StreamDecorator.h"
#include <cstddef>
#include <string>
#include <vector>

class StreamsData
{
public:
	IInputStreamPtr m_inputStream;
	IOutputStreamPtr m_outputStream;
};

class FileTransformer
{
public:
	void Transform(int argc, char* args[])
	{
		ValidateArguments(argc, args);
		auto fileNames = ExtractFileNames(argc, args);
		auto arguments = ExtractArguments(argc, args);

		StreamsData streams = CreateStreams(fileNames.first, fileNames.second);
		DecorateStreams(streams, arguments);
		TransferData(streams);
	}

private:
	const std::string k_encryptArg{ "--encrypt" };
	const std::string k_decryptArg{ "--decrypt" };
	const std::string k_compressArg{ "--compress" };
	const std::string k_decompressArg{ "--decompress" };

	void ValidateArguments(int argc, char* args[]) const
	{
		CheckArgumentCount(argc);
		CheckArgumentTypes(argc, args);
	}

	void CheckArgumentCount(int argc) const
	{
		if (argc < 3)
		{
			throw std::runtime_error("Invalid number of arguments. Usage: transform [options] <input-file> <output-file>");
		}
	}

	void CheckArgumentTypes(int argc, char* args[]) const
	{
		std::vector<std::string> arguments = ExtractArguments(argc, args);

		for (size_t i = 0; i < arguments.size(); ++i)
		{
			const std::string& arg = arguments[i];
			CheckParameterIsExpected(arg);

			if (arg == k_encryptArg || arg == k_decryptArg)
			{
				if (i + 1 >= arguments.size())
				{
					throw std::runtime_error("Missing key for argument " + arg);
				}
				CheckArgumentWithIntParameter(arg, arguments[i + 1]);
				i++;
			}
		}
	}

	void CheckParameterIsExpected(const std::string& arg) const
	{
		if (
			arg != k_encryptArg
			&& arg != k_decompressArg
			&& arg != k_compressArg
			&& arg != k_decryptArg)
		{

			throw std::runtime_error("Unknown argument: " + arg);
		}
	}

	void CheckArgumentWithIntParameter(const std::string& arg, const std::string& next) const
	{
		try
		{
			std::stoi(next);
		}
		catch (const std::invalid_argument&)
		{
			throw std::runtime_error("Invalid key for argument " + arg + ". Key must be an integer.");
		}
	}

	std::pair<std::string, std::string> ExtractFileNames(int argc, char* args[]) const
	{
		return { args[argc - 2], args[argc - 1] };
	}

	std::vector<std::string> ExtractArguments(int argc, char* args[]) const
	{
		std::vector<std::string> arguments;

		for (int i = 1; i < argc - 2; ++i)
		{
			arguments.push_back(args[i]);
		}
		return arguments;
	}

	StreamsData CreateStreams(const std::string& inputFileName, const std::string& outputFileName)
	{
		auto inputFile = std::make_unique<std::ifstream>(inputFileName, std::ios::binary);
		auto outputFile = std::make_unique<std::ofstream>(outputFileName, std::ios::binary);

		StreamsData streams;
		streams.m_inputStream = std::make_unique<FileInputStream>(std::move(inputFile));
		streams.m_outputStream = std::make_unique<FileOutputStream>(std::move(outputFile));

		return streams;
	}

	void DecorateStreams(StreamsData& streams, const std::vector<std::string>& args)
	{
		for (size_t i = 0; i < args.size(); ++i)
		{
			const std::string& arg = args[i];
			if (arg == k_encryptArg)
			{
				int key = std::stoi(args[++i]);
				streams.m_outputStream = std::move(streams.m_outputStream) << MakeDecorator<EncodingOutputStreamDecorator>(key);
			}
			else if (arg == k_decryptArg)
			{
				int key = std::stoi(args[++i]);
				streams.m_inputStream = std::move(streams.m_inputStream) << MakeDecorator<DecodingInputStreamDecorator>(key);
			}
			else if (arg == k_compressArg)
			{
				streams.m_outputStream = std::move(streams.m_outputStream) << MakeDecorator<PackingOutputStreamDecorator>();
			}
			else if (arg == k_decompressArg)
			{
				streams.m_inputStream = std::move(streams.m_inputStream) << MakeDecorator<UnpackingInputStreamDecorator>();
			}
		}
	}

	void TransferData(StreamsData& streams)
	{
		std::vector<uint8_t> buffer(4096);
		while (!streams.m_inputStream->IsEOF())
		{
			std::streamsize bytesRead = streams.m_inputStream->ReadBlock(buffer.data(), buffer.size());
			if (bytesRead > 0)
			{
				streams.m_outputStream->WriteBlock(buffer.data(), bytesRead);
			}
		}
	}
};
#endif /* FILETRANSFORMER_H */
