#include <string>
#include <iostream>
#include <memory>
#include <boost/optional.hpp>
#include <boost/filesystem.hpp>


using namespace std;

typedef boost::filesystem::path Path;
using boost::optional;
using boost::none;

class IParagraph;
class IImage;

class CConstDocumentItem
{
public:
	shared_ptr<const IImage> GetImage()const;
	shared_ptr<const IParagraph> GetParagraph()const;
	virtual ~CConstDocumentItem() = default;
};

class CDocumentItem : public CConstDocumentItem
{
public:
	shared_ptr<IImage> GetImage();
	shared_ptr<IParagraph> GetParagraph();
};

class IParagraph
{
public:
	virtual string GetText()const = 0;
	virtual void SetText(const string& text) = 0;
	virtual ~IParagraph() = default;
};

class IImage
{
public:
	virtual Path GetPath()const = 0;

	virtual int GetWidth()const = 0;
	virtual int GetHeight()const = 0;
	
	virtual void Resize(int width, int height) = 0;
	
	virtual ~IImage() = default;
};

class IDocument
{
public:
	virtual shared_ptr<IParagraph> InsertParagraph(const string& text, 
		optional<size_t> position = none) = 0;

	virtual shared_ptr<IImage> InsertImage(const Path& path, int width, int height, 
		optional<size_t> position = none) = 0;
	
	virtual size_t GetItemsCount()const = 0;

	virtual CConstDocumentItem GetItem(size_t index)const = 0;
	virtual CDocumentItem GetItem(size_t index) = 0;

	virtual void DeleteItem(size_t index) = 0;

	virtual string GetTitle()const = 0;
	virtual void SetTitle(const string & title) = 0;

	virtual bool CanUndo()const = 0;
	virtual void Undo() = 0;

	virtual bool CanRedo()const = 0;
	virtual void Redo() = 0;

	virtual void Save(const Path& path)const = 0;

	virtual ~IDocument() = default;
};


int main()
{

}
