#include "lib/Document/HtmlDocument.h"
#include "lib/History.h"
#include <iostream>
#include <memory>

int main()
{
	try
	{
		auto historyManager = std::make_shared<CommandHistoryManager>();
		auto document = std::make_shared<HtmlDocument>(*historyManager);
		auto factory
			= std::make_unique<DocumentCommandFactory>(std::cin, document);

		ConsoleCommandExecutor executor(historyManager, std::move(factory));
		executor.Start();
	}
	catch (const std::exception& e)
	{
		std::cerr << "\033[31mFatal error: " << e.what() << "\033[0m"
				  << std::endl;
		return 1;
	}

	return 0;
}

// SetTitle My Photo Gallery
// InsertParagraph 0 Welcome to my photo gallery!
// InsertImage 1 800 600 1.png
// InsertParagraph 2 This is the first beautiful landscape
// InsertImage 3 800 600 2.png
// InsertParagraph 4 Cityscape at night
// InsertImage 5 800 600 3.png
// InsertParagraph 6 Nature and mountains
// List
// Save gallery.html/

