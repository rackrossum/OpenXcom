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

#include "../Engine/Logger.h"
#include "../Engine/Options.h"
#include "../Engine/Sound.h"
#include "../Mod/Armor.h"
#include "../Mod/Mod.h"
#include "../Savegame/BattleUnit.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/Tile.h"
#include "BattlescapeState.h"
#include "Camera.h"
#include "Map.h"
#include "MeleeAttackBState.h"
#include "Pathfinding.h"
#include "TileEngine.h"
#include "UnitFallBState.h"
#include "UnitTeleportingState.h"

namespace OpenXcom
{

/**
 * Sets up an UnitTeleportingState.
 * @param parent Pointer to the Battlescape.
 * @param action Pointer to an action.
 */
UnitTeleportingState::UnitTeleportingState(BattlescapeGame* parent, BattleAction action)
	: BattleState(parent, action), _unit(nullptr), _terrain(nullptr)
{
}


/**
 * Initializes the state.
 */
void UnitTeleportingState::init()
{
	_unit = _action.actor;
	_terrain = _parent->getTileEngine();
	_target = _action.target;
	if (Options::traceAI)
	{
		Log(LOG_INFO) << "Teleporting from: " << _unit->getPosition() << ","
					  << " to " << _target;
	}

	_terrain->addMovingUnit(_unit);
}

/**
 * Deinitalize the state.
 */
void UnitTeleportingState::deinit()
{
	_terrain->removeMovingUnit(_unit);
}

/**
 * Runs state functionality every cycle.
 */
void UnitTeleportingState::think()
{
	if (!_unit->getArmor()->allowsMoving() || _unit->isOut())
	{
		_parent->popState();
		return;
	}

	if (_unit->getPosition() != _target && !_falling)
	{
		_unit->teleport(_parent->getSave()->getTile(_target), _parent->getSave());
	}
	else
	{

	}

	// unit moved from one tile to the other, update the tiles
	if (_unit->getPosition() != _unit->getLastPosition())
	{
		auto* belowTile = _parent->getSave()->getBelowTile(_unit->getTile());
		_fallingWhenStopped = _unit->haveNoFloorBelow()
							  && _unit->getPosition().z != 0
							  && _unit->getMovementType() != MT_FLY
							  && _unit->getWalkingPhase() == 0;

		_falling = _fallingWhenStopped && !(belowTile && belowTile->hasLadder()                                       // we do not have any footing but "jump" from ladder to reach ledge
											&& _unit->getPosition() == _unit->getLastPosition() + Position(0, 0, 1)); // only vertical move from ladder below

		const auto size = _unit->getArmor()->getSize();
		if (_falling)
		{
			for (int x = size; x >= 0; --x)
			{
				for (int y = size; y >= 0; --y)
				{
					Tile* otherTileBelow = _parent->getSave()->getTile(_unit->getPosition() + Position(x, y, -1));
					if (otherTileBelow && otherTileBelow->getUnit())
					{
						_falling = false;
						_fallingWhenStopped = false;
						_parent->getSave()->addFallingUnit(_unit);
						_parent->statePushFront(new UnitFallBState(_parent));
						return;
					}
				}
			}
		}

		if (!_parent->getMap()->getCamera()->isOnScreen(_unit->getPosition(), true, size, false) && _unit->getFaction() != FACTION_PLAYER && _unit->getVisible())
			_parent->getMap()->getCamera()->centerOnPosition(_unit->getPosition());
		// if the unit changed level, camera changes level with
		_parent->getMap()->getCamera()->setViewLevel(_unit->getPosition().z);
	}

	if (_unit->getFaction() != FACTION_PLAYER)
	{
		_unit->setVisible(false);
	}

	_terrain->calculateLighting(LL_UNITS, _unit->getPosition());
	_terrain->calculateFOV(_unit);
	_parent->popState();
	return;
}


/**
 * Handles the stepping sounds.
 */
void UnitTeleportingState::playMovementSound()
{
	auto sound = Mod::FLYING_SOUND;

	if (sound >= 0)
	{
		_parent->getMod()->getSoundByDepth(_parent->getDepth(), sound)->play(-1, _parent->getMap()->getSoundAngle(_unit->getPosition()));
	}
}

}
