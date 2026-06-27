#pragma once

#include "../Engine/State.h"
#include <vector>

namespace OpenXcom
{

class TextButton;
class Window;
class Text;
class TextList;
class Base;

/**
 * Global Transfers screen that provides overview
 * of incoming transfers in all bases.
 */
class GlobalTransfersState : public State
{
private:
	TextButton *_btnOk;
	Window *_window;
	Text *_txtTitle, *_txtBase, *_txtItem, *_txtQuantity, *_txtArrivalTime;
	TextList *_lstTransfers;
	std::vector<Base*> _bases;
	bool _openedFromBasescape;

public:
	/// Creates the GlobalTransfersState.
	GlobalTransfersState(bool openedFromBasescape = false);
	/// Cleans up the GlobalTransfersState.
	~GlobalTransfersState();
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
	/// Handler for clicking the transfers list.
	void onSelectBase(Action *action);
	/// Updates the transfer list.
	void init() override;
	/// Fills the list with transfers from all bases.
	void fillTransferList();
};

}
