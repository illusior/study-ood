#include "../../include/pch.h"

#include "../../include/Document/Commands/common.h"
#include "../../include/Editor/Editor.h"

using namespace document_commands;

namespace editor_commands
{

constexpr auto DELETE_CMND = DELETE_DOCUMENT_ITEM_COMMAND_NAME;
constexpr auto EXIT_CMND = "Exit";
constexpr auto HELP_CMND = "Help";
constexpr auto INSERT_PARAGRAPH_CMND = "InsertParagraph";
constexpr auto LIST_CMND = "List";
constexpr auto REDO_CMND = "Redo";
constexpr auto RENAME_CMND = SET_TITLE_COMMAND_NAME;
constexpr auto SAVE_CMND = "Save";
constexpr auto UNDO_CMND = "Undo";

constexpr auto INSERT_END_ARG = "end";

namespace descriptions
{

constexpr auto DELETE_CMND_DSCRP = "Deletes item at certain <pos>. Args: {pos}";
constexpr auto EXIT_CMND_DSCRP = "Stops editing document";
constexpr auto HELP_CMND_DSCRP = "Shows list of available commands";
constexpr auto INSERT_PARAGRAPH_CMND_DSCRP = "Inserts a paragraph before <pos>. Args: {end|<pos>}";
constexpr auto LIST_CMND_DSCRP = "Lists document's content";
constexpr auto REDO_CMND_DSCRP = "Redo undone action";
constexpr auto RENAME_CMND_DSCRP = "Changes title. Args: <new-title>";
constexpr auto SAVE_CMND_DSCRP = "Saves document. Args: <path>";
constexpr auto UNDO_CMND_DSCRP = "Undo previous action";

}; // namespace descriptions

}; // namespace editor_commands

Editor::Editor(IDocumentPtr&& document, std::istream& inputS, std::ostream& outputS)
	: m_document(std::move(document))
	, m_menu(inputS, outputS)
	, m_outputEcho(outputS)
{
	using namespace editor_commands;
	using namespace descriptions;

	m_menu.AddItem(DELETE_CMND, DELETE_CMND_DSCRP, [this](std::istream& is) {
		DeleteItem(is);
	});
	m_menu.AddItem(EXIT_CMND, EXIT_CMND_DSCRP, [&menu = m_menu](std::istream&) {
		menu.Exit();
	});
	m_menu.AddItem(HELP_CMND, HELP_CMND_DSCRP, [&menu = m_menu](std::istream&) {
		menu.ShowInstructions();
	});
	m_menu.AddItem(LIST_CMND, LIST_CMND_DSCRP, [this](std::istream& is) {
		ListDocument();
	});
	m_menu.AddItem(REDO_CMND, REDO_CMND_DSCRP, [this](std::istream&) {
		Redo();
	});
	m_menu.AddItem(INSERT_PARAGRAPH_CMND, INSERT_PARAGRAPH_CMND_DSCRP, [this](std::istream& is) {
		InsertParagparh(is);
	});
	m_menu.AddItem(RENAME_CMND, RENAME_CMND_DSCRP, [this](std::istream& is) {
		SetTitle(is);
	});
	m_menu.AddItem(SAVE_CMND, SAVE_CMND_DSCRP, [this](std::istream& is) {
		Save(is);
	});
	m_menu.AddItem(UNDO_CMND, UNDO_CMND_DSCRP, [this](std::istream&) {
		Undo();
	});
}

constexpr auto BAD_STREAM_MSG = "Failed to read argument. Input stream is bad\n";

void Editor::DeleteItem(std::istream& is)
{
	if (!std::istream::sentry(is))
	{
		m_outputEcho << BAD_STREAM_MSG;
		return;
	}

	std::string index{};
	if (!(is >> index))
	{
		m_outputEcho << "Failed to read position argument\n";
		return;
	}

	try
	{
		m_document->DeleteItem(IEqualStrings(index, editor_commands::INSERT_END_ARG)
				? m_document->GetItemsCount() - 1
				: std::stoi(index));
	}
	catch (const std::exception& e)
	{
		m_outputEcho << e.what() << std::endl;
	}
}

void Editor::Start()
{
	m_menu.Run();
}

void Editor::Redo()
{
	if (!m_document->CanRedo())
	{
		m_outputEcho << "Can't redo\n";
		return;
	}

	m_document->Redo();
}

void Editor::Undo()
{
	if (!m_document->CanUndo())
	{
		m_outputEcho << "Can't undo\n";
		return;
	}

	m_document->Undo();
}

void Editor::InsertParagparh(std::istream& is)
{
	if (!std::istream::sentry(is))
	{
		m_outputEcho << BAD_STREAM_MSG;
		return;
	}

	std::string text{}, paragraphIndex{};
	if (!(is >> paragraphIndex) || !(is >> text))
	{
		m_outputEcho << "Failed to read text or insert position arguments\n";
		return;
	}

	try
	{
		m_document->InsertParagraph(text,
			IEqualStrings(paragraphIndex, editor_commands::INSERT_END_ARG)
				? m_document->GetItemsCount()
				: std::stoi(paragraphIndex));
	}
	catch (std::exception& exception)
	{
		m_outputEcho << exception.what() << std::endl;
	}
}

std::string GetDetailInfoAboutDocumentItem(const DocumentItem& item)
{
	using namespace document_item_types;

	bool isImage = item.GetImage() != nullptr;
	bool isParagraph = item.GetParagraph() != nullptr;

	std::string res{};
	if (isImage)
	{
		auto ptr = item.GetImage();

		res += std::string(IMAGE_TYPE_STR) + ": " + std::to_string(ptr->GetWidth()) + " " + std::to_string(ptr->GetHeight()) + " " + ptr->GetPath().generic_string();
	}
	else if (isParagraph)
	{
		auto ptr = item.GetParagraph();

		res = std::string(PARAGRAPH_TYPE_STR) + ": " + ptr->GetText();
	}

	return res;
}

void Editor::ListDocument()
{
	m_outputEcho << "Title: " << m_document->GetTitle() << '\n';
	const auto itemsCount = m_document->GetItemsCount();
	for (size_t i = 0; i < itemsCount; ++i)
	{
		m_outputEcho << i << ". " << GetDetailInfoAboutDocumentItem(m_document->GetItem(i)) << '\n';
	}
}

void Editor::Save(std::istream& is)
{
	if (!std::istream::sentry(is))
	{
		m_outputEcho << BAD_STREAM_MSG;
		return;
	}

	StdPath path;
	if (!(is >> path))
	{
		m_outputEcho << "Failed to read path\n";
		return;
	}

	m_document->Save(path);
}

void Editor::SetTitle(std::istream& is)
{
	if (!std::istream::sentry(is))
	{
		m_outputEcho << BAD_STREAM_MSG;
		return;
	}

	std::string title;
	if (!(is >> title))
	{
		m_outputEcho << "Failed to read title\n";
		return;
	}

	m_document->SetTitle(title);
}