#pragma once

#pragma once
/*
 * Copyright 2010-2016 OpenXcom Developers.
 *
 * This file is part of OpenXcom.
 *
 * OpenXcom is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenXcom is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenXcom.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "BattleState.h"
#include "BattlescapeGame.h"
#include "Position.h"

namespace OpenXcom
{

class BattleUnit;
class TileEngine;

/**
 * State for teleporting units.
 */
class UnitTeleportingState : public BattleState
{
private:
	Position _target;
	BattleUnit* _unit;
	TileEngine* _terrain;

	/// Unit will fall down always.
	bool _falling = false;
	/// Allow to move over some tiles that normally should fall down.
	bool _fallingWhenStopped = false;

	/// Handles the stepping sounds.
	void playMovementSound();

  public:
	/// Creates a new UnitTeleportingState class.
	UnitTeleportingState(BattlescapeGame* parent, BattleAction _action);
	/// Cleans up the UnitTeleportingState.
	~UnitTeleportingState() = default;
	/// Initializes the state.
	void init() override;
	/// Deinitializes the state.
	void deinit() override;
	/// Handles a cancels request.
	void cancel() override{};
	/// Runs state functionality every cycle.
	void think() override;
};

}
