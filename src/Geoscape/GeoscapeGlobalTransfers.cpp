/*
 * Hotkey handler for the Global Transfers overview.
 * Kept in a separate translation unit so the patcher does not need
 * to inject a function body into GeoscapeState.cpp.
 */

#include "GeoscapeState.h"

#include "../Basescape/GlobalTransfersState.h"
#include "../Engine/Game.h"
#include "../Engine/Action.h"

namespace OpenXcom
{

/**
 * Opens the Global Transfers overview from the Geoscape.
 * @param action Pointer to an action.
 */
void GeoscapeState::btnGlobalTransfersClick(Action *)
{
	_game->pushState(new GlobalTransfersState(false));
}

}
