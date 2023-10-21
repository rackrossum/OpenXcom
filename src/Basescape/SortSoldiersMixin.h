#pragma once
#include <vector>
#include <string>
#include "../Engine/State.h"
#include "SoldierSortUtil.h"
#include "../Engine/Options.h"
#include "../Interface/ComboBox.h"

namespace OpenXcom
{
	template<typename StateType = State>
	class SortSoldiersMixin : public StateType
	{
	public:
		SortSoldiersMixin(Base* base)
			: _base(base)
		{}

	protected:
		void FillSorters(std::vector<SortFunctor*>& sorters, ComboBox& sortingCombobox, ActionHandler comboBoxChangeHandler)
		{
			std::map<Options::OXCEN::DefaultSoldiersSorter, size_t> sortersToIndexes;
			std::vector<std::string> sortingNames;

			sortingNames.push_back(State::tr("STR_ORIGINAL_ORDER"));
			sorters.push_back(nullptr);
			sortersToIndexes[Options::OXCEN::DefaultSoldiersSorter::Original] = sorters.size() - 1;
			
#define PUSH_IN(strId, functor) \
		sortingNames.push_back(State::tr(strId)); \
		sorters.push_back(new SortFunctor(State::_game, functor));

			PUSH_IN("STR_ID", idStat);
			PUSH_IN("STR_NAME_UC", nameStat);
			PUSH_IN("STR_CRAFT", craftIdStat);
			PUSH_IN("STR_SOLDIER_TYPE", typeStat);
			PUSH_IN("STR_RANK", rankStat);
			PUSH_IN("STR_IDLE_DAYS", idleDaysStat);
			PUSH_IN("STR_MISSIONS2", missionsStat);
			PUSH_IN("STR_KILLS2", killsStat);
			sortersToIndexes[Options::OXCEN::DefaultSoldiersSorter::KillCount] = sorters.size() - 1;

			PUSH_IN("STR_WOUND_RECOVERY2", woundRecoveryStat);
			if (State::_game->getMod()->isManaFeatureEnabled() && !State::_game->getMod()->getReplenishManaAfterMission())
			{
				PUSH_IN("STR_MANA_CURRENT", currentManaStat);
				sortersToIndexes[Options::OXCEN::DefaultSoldiersSorter::CurrentMana] = sorters.size() - 1;

				PUSH_IN("STR_MANA_MISSING", manaMissingStat);
			}

			PUSH_IN("STR_TIME_UNITS", tuStat);
			PUSH_IN("STR_STAMINA", staminaStat);
			PUSH_IN("STR_HEALTH", healthStat);
			PUSH_IN("STR_BRAVERY", braveryStat);
			PUSH_IN("STR_REACTIONS", reactionsStat);
			PUSH_IN("STR_FIRING_ACCURACY", firingStat);
			sortersToIndexes[Options::OXCEN::DefaultSoldiersSorter::FiringAccuracy] = sorters.size() - 1;

			PUSH_IN("STR_THROWING_ACCURACY", throwingStat);
			PUSH_IN("STR_MELEE_ACCURACY", meleeStat);
			PUSH_IN("STR_STRENGTH", strengthStat);
			if (State::_game->getMod()->isManaFeatureEnabled())
			{
				// "unlock" is checked later
				PUSH_IN("STR_MANA_POOL", manaStat);
			}
			PUSH_IN("STR_PSIONIC_STRENGTH", psiStrengthStat);
			PUSH_IN("STR_PSIONIC_SKILL", psiSkillStat);

#undef PUSH_IN

			const auto defaultSorter = static_cast<Options::OXCEN::DefaultSoldiersSorter>(Options::OXCEN::defaultSoldiersSorter);
			auto it = sortersToIndexes.find(defaultSorter);

			size_t index = 0;
			if (it != sortersToIndexes.end())
				index = it->second;

			DoSort(index, sorters[index]);

			sortingCombobox.setOptions(sortingNames);
			sortingCombobox.setSelected(0);
			sortingCombobox.onChange(comboBoxChangeHandler);
			sortingCombobox.setText(State::tr("STR_SORT_BY"));
		}

		void DoSort(int sortIndex, const SortFunctor* sortFunctor)
		{
			if (!sortFunctor || !sortFunctor->_getStatFn)
				return;

			if (sortIndex == 2)
			{
				std::stable_sort(_base->getSoldiers()->begin(), _base->getSoldiers()->end(),
								 [](const Soldier* a, const Soldier* b)
								 {
									 return Unicode::naturalCompare(a->getName(), b->getName());
								 });
			}
			else if (sortIndex == 3)
			{
				std::stable_sort(_base->getSoldiers()->begin(), _base->getSoldiers()->end(),
								 [](const Soldier* a, const Soldier* b)
								 {
									 if (a->getCraft())
									 {
										 if (b->getCraft())
										 {
											 if (a->getCraft()->getRules() == b->getCraft()->getRules())
											 {
												 return a->getCraft()->getId() < b->getCraft()->getId();
											 }
											 else
											 {
												 return a->getCraft()->getRules() < b->getCraft()->getRules();
											 }
										 }
										 else
										 {
											 return true; // a < b
										 }
									 }

									 return false; // b > a
								 });
			}
			else
			{
				std::stable_sort(_base->getSoldiers()->begin(), _base->getSoldiers()->end(), *sortFunctor);
			}

			if (State::_game->isShiftPressed())
			{
				std::reverse(_base->getSoldiers()->begin(), _base->getSoldiers()->end());
			}
		}

		void ChangeSorter(getStatFn_t& getter)
		{
			const auto defaultSorter = static_cast<Options::OXCEN::DefaultSoldiersSorter>(Options::OXCEN::defaultSoldiersSorter);
			if (defaultSorter == Options::OXCEN::DefaultSoldiersSorter::Original)
				getter = nullptr;
			else if (defaultSorter == Options::OXCEN::DefaultSoldiersSorter::KillCount)
				getter = killsStat;
			else if (defaultSorter == Options::OXCEN::DefaultSoldiersSorter::FiringAccuracy)
				getter = firingStat;
			else if (defaultSorter == Options::OXCEN::DefaultSoldiersSorter::CurrentMana)
				getter = currentManaStat;
		}

	protected:
		Base* _base;
	};

}
