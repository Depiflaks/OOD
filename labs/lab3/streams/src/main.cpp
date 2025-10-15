#include "lib/FileTransformer.h"

int main(int argc, char* argv[])
{
	FileTransformer transformer;
	transformer.Transform(argc, argv);
	return 0;
}
