// ISpringWord.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Menu.h"
#include "Document.h"

using namespace std;
using namespace std::placeholders;

namespace
{

class CEditor
{
public:
	CEditor()  //-V730
		:m_document(make_unique<CDocument>())
	{
		m_menu.AddItem("help", "Help", [this](istream&) { m_menu.ShowInstructions(); });
		m_menu.AddItem("exit", "Exit", [this](istream&) { m_menu.Exit(); });
		AddMenuItem("setTitle", "Changes title. Args: <new title>", &CEditor::SetTitle);
		m_menu.AddItem("list", "Show document", bind(&CEditor::List, this, _1));
		AddMenuItem("undo", "Undo command", &CEditor::Undo);
		AddMenuItem("redo", "Redo undone command", &CEditor::Redo);
	}

	void Start()
	{
		m_menu.Run();
	}

private:
	// Указатель на метод класса CEditor, принимающий istream& и возвращающий void
	typedef void (CEditor::*MenuHandler)(istream & in);

	void AddMenuItem(const string & shortcut, const string & description, MenuHandler handler)
	{
		m_menu.AddItem(shortcut, description, bind(handler, this, _1));
	}

	// TODO: скипнуть первый пробел элегантнее
	void SetTitle(istream & in)
	{
		string head;
		string tail;

		if (in >> head)
		{
			getline(in, tail);
		}
		string title = head + tail;

		m_document->SetTitle(title);
	}

	void List(istream &)
	{
		cout << "-------------" << endl;
		cout << m_document->GetTitle() << endl;
		cout << "-------------" << endl;
	}

	void Undo(istream &)
	{
		if (m_document->CanUndo())
		{
			m_document->Undo();
		}
		else
		{
			cout << "Can't undo" << endl;
		}
	}

	void Redo(istream &)
	{
		if (m_document->CanRedo())
		{
			m_document->Redo();
		}
		else
		{
			cout << "Can't redo" << endl;
		}
	}

	CMenu m_menu;
	unique_ptr<IDocument> m_document;
};

}

int main()
{
	CEditor editor;
	editor.Start();
	return 0;
}

