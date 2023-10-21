#pragma once
#pragma once
#include "../Engine/State.h"
#include "../Interface/TextList.h"
#include "../Engine/Options.h"
#include "../Savegame/Soldier.h"

namespace OpenXcom
{
	template<typename StateType = State>
	class HighlightLowManaSoldiersMixin : public StateType
	{
	protected:
		void setListRowColor(TextList& list, size_t row, Uint8 color, const Soldier& soldier)
		{
			list.setRowColor(row, color);

			const auto mode = static_cast<Options::OXCEN::HighlightLowManaSoldiersMode>(Options::OXCEN::highlightLowManaSoldiersMode);
			if (StateType::_game->getSavedGame()->isManaUnlocked(StateType::_game->getMod())
				&& mode != Options::OXCEN::HighlightLowManaSoldiersMode::None)
			{
				const auto missingManaColor = soldier.getMissingManaColorForState();
				if (missingManaColor)
				{
					if (mode == Options::OXCEN::HighlightLowManaSoldiersMode::Name)
						list.setCellColor(row, 0, *missingManaColor);
					else if (mode == Options::OXCEN::HighlightLowManaSoldiersMode::Rank)
						list.setCellColor(row, 1, *missingManaColor);
				}
			}
		}
	};
}
