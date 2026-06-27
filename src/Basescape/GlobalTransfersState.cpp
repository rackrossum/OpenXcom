/*
 * Global transfer overview for OXCE QOL.
 */

#include "GlobalTransfersState.h"

#include <sstream>

#include "../Engine/Game.h"
#include "../Mod/Mod.h"
#include "../Engine/LocalizedText.h"
#include "../Engine/Options.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Base.h"
#include "../Savegame/Transfer.h"
#include "TransfersState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Global Transfers screen.
 */
GlobalTransfersState::GlobalTransfersState(bool openedFromBasescape) : _openedFromBasescape(openedFromBasescape)
{
	_screen = false;
	// Create objects
	_window = new Window(this, 320, 200, 0, 0);
	_btnOk = new TextButton(304, 16, 8, 176);
	_txtTitle = new Text(310, 17, 5, 8);
	_txtBase = new Text(72, 9, 10, 28);
	_txtItem = new Text(116, 9, 82, 28);
	_txtQuantity = new Text(38, 9, 198, 28);
	_txtArrivalTime = new Text(76, 9, 238, 28);
	_lstTransfers = new TextList(288, 132, 8, 42);

	// Set palette
	setInterface("transferInfo");
	add(_window, "window", "transferInfo");
	add(_btnOk, "button", "transferInfo");
	add(_txtTitle, "text", "transferInfo");
	add(_txtBase, "text", "transferInfo");
	add(_txtItem, "text", "transferInfo");
	add(_txtQuantity, "text", "transferInfo");
	add(_txtArrivalTime, "text", "transferInfo");
	add(_lstTransfers, "list", "transferInfo");

	centerAllSurfaces();

	// Set up objects
	setWindowBackground(_window, "transferInfo");

	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&GlobalTransfersState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&GlobalTransfersState::btnOkClick, Options::keyOk);
	_btnOk->onKeyboardPress((ActionHandler)&GlobalTransfersState::btnOkClick, Options::keyCancel);

	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(tr("STR_TRANSFER_OVERVIEW"));

	_txtBase->setText(tr("STR_BASE_UC"));
	_txtItem->setText(tr("STR_ITEM"));
	_txtQuantity->setText(tr("STR_QUANTITY_UC"));
	_txtArrivalTime->setText(tr("STR_ARRIVAL_TIME_HOURS"));

	_lstTransfers->setColumns(4, 74, 116, 38, 60);
	_lstTransfers->setSelectable(true);
	_lstTransfers->setBackground(_window);
	_lstTransfers->setMargin(2);
	_lstTransfers->setWordWrap(false);
	_lstTransfers->onMouseClick((ActionHandler)&GlobalTransfersState::onSelectBase, SDL_BUTTON_LEFT);
}

/**
 * Deletes the Global Transfers screen.
 */
GlobalTransfersState::~GlobalTransfersState()
{
}

/**
 * Returns to the previous screen.
 */
void GlobalTransfersState::btnOkClick(Action *)
{
	_game->popState();
}

/**
 * Opens the selected base's normal transfers screen.
 */
void GlobalTransfersState::onSelectBase(Action *)
{
	const int row = _lstTransfers->getSelectedRow();
	if (row >= 0 && row < (int)_bases.size())
	{
		Base *base = _bases[row];
		if (base)
		{
			// close this overview
			_game->popState();
			// close Transfers UI if this screen was opened from Basescape
			if (_openedFromBasescape)
			{
				_game->popState();
			}
			_game->pushState(new TransfersState(base));
		}
	}
}

/**
 * Updates the transfers list after returning from other screens.
 */
void GlobalTransfersState::init()
{
	State::init();
	fillTransferList();
}

/**
 * Fills the list with transfers from all bases, grouped per base.
 */
void GlobalTransfersState::fillTransferList()
{
	_bases.clear();
	_lstTransfers->clearList();

	bool anyTransfers = false;
	int row = 0;

	for (Base *xbase : *_game->getSavedGame()->getBases())
	{
		auto *transfers = xbase->getTransfers();
		if (transfers->empty())
		{
			continue;
		}

		std::string baseName = xbase->getName(_game->getLanguage());

		// Header row: base name only.
		_lstTransfers->addRow(4, baseName.c_str(), "", "", "");
		_lstTransfers->setRowColor(row, _lstTransfers->getSecondaryColor());
		_bases.push_back(xbase);
		++row;

		for (const auto* transfer : *transfers)
		{
			std::ostringstream qty;
			std::ostringstream hours;
			qty << transfer->getQuantity();
			hours << transfer->getHours();

			// Item row: base column left empty, indented under its header.
			_lstTransfers->addRow(4,
				"",
				transfer->getName(_game->getLanguage()).c_str(),
				qty.str().c_str(),
				hours.str().c_str());
			_bases.push_back(xbase);
			++row;
			anyTransfers = true;
		}
	}

	if (anyTransfers)
	{
		// Trailing spacer row so the list's bottom-of-scroll clipping
		// (an OpenXcom TextList quirk with grouped rows of mixed height)
		// eats blank space instead of cutting off the very last real item.
		_lstTransfers->addRow(4, "", "", "", "");
		_bases.push_back(0);
		++row;
	}

	if (!anyTransfers)
	{
		_lstTransfers->clearList();
		_bases.clear();
		_lstTransfers->addRow(4, tr("STR_NONE").c_str(), "", "", "");
		_bases.push_back(0);
	}
}

}
