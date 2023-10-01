#pragma once
#include <memory>
#include <map>
#include <utility>
#include "../Engine/Timer.h"

namespace OpenXcom
{
	class Surface;
	class TextList;
	class State;
	class Window;
	class RuleItem;
	class Base;
	class Game;
	class Font;

	class ItemCountTooltip
	{
	public:
		ItemCountTooltip(const RuleItem*,
						 const Base& curBase,
						 const Game&,
						 uint32_t tooltipDelay,
						 State&,
						 StateHandler showCaller,
						 double coordX,
						 double coordY);

		~ItemCountTooltip();
		void Init();
		TextList* Show();
		void Think(State* state, Surface* surf);

	private:
		uint16_t calculateBaseNameColumnWidth(const std::string& headerName) const;
		uint16_t calculateBaseItemsCountColumnWidth(const std::string& headerName) const;
		uint16_t calculateCraftItemsCountColumnWidth(const std::string& headerName) const;
		uint16_t calculateTransferItemsCountColumnWidth(const std::string& headerName) const;
		uint16_t calculateHeight() const;

		uint16_t calculateStringWidth(const std::string& str, const Font& font) const;

	private:
		const Game& _game;
		uint32_t _tooltipDelay;
		std::unique_ptr<Timer> _timer;
		State& _state;
		StateHandler _showCaller;
		double _coordX;
		double _coordY;

		struct ItemCountRecord
		{
			uint32_t onBase;
			uint32_t onCraft;
			uint32_t transfered;
		};

		std::map<std::string, ItemCountRecord> _basesToCounts;
		std::map<std::string, ItemCountRecord>::const_iterator _curBaseIt;

		bool _enableItemsOnCraftColumn = false;
		bool _enableTransferedItemsColumn = false;
		TextList* _text = nullptr;
		Window* _window = nullptr;
	};
}

