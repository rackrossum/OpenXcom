#include "ItemCountTooltip.h"
#include "../Interface/TextList.h"
#include "../Interface/Window.h"
#include "../Engine/State.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Base.h"
#include "../Savegame/ItemContainer.h"
#include <vector>
#include "../Engine/Game.h"
#include "../Mod/Mod.h"
#include "../Engine/Font.h"
#include "../Savegame/Craft.h"
#include "../Mod/Armor.h"

namespace
{
	constexpr uint8_t smallWidthOffset = 3;
}

namespace OpenXcom
{
	ItemCountTooltip::ItemCountTooltip(const RuleItem* item,
									   const Base& curBase,
									   const Game& game,
									   uint32_t tooltipDelay,
									   State& state,
									   StateHandler showCaller,
									   double coordX,
									   double coordY)
		: _tooltipDelay(tooltipDelay),
		  _state(state),
		  _showCaller(showCaller),
		  _coordX(coordX),
		  _coordY(coordY),
		  _game(game)
	{
		const auto* bases = game.getSavedGame()->getBases();
		if (!bases)
			return;

		for (const auto* base : *bases)
		{
			uint32_t itemsOnCraftCount = 0;
			if (base->getCrafts())
			{
				for (const auto* craft : *base->getCrafts())
				{
					itemsOnCraftCount += craft->getTotalItemCount(item);
				}
			}

			int32_t itemsOnBaseCount = base->getStorageItems()->getItem(item);
			for (const auto* soldier : base->getSoldiers())
			{
				if (auto* armorItem = soldier->getArmor()->getStoreItem(); armorItem && armorItem->getType() == item->getType())
				{
					if (soldier->getCraft())
						++itemsOnCraftCount;
					else
						++itemsOnBaseCount;
				}
			}

			if (itemsOnCraftCount != 0)
				_enableItemsOnCraftColumn = true;

			auto [it, _] = _basesToCounts.emplace(base->getName(), std::make_pair(itemsOnBaseCount, itemsOnCraftCount));
			
			if (&curBase == base)
				_curBaseIt = it;
		}
	}

	ItemCountTooltip::~ItemCountTooltip()
	{
		_state.remove(_window);
		_state.remove(_text);
	}

	void ItemCountTooltip::Init()
	{
		_timer = std::make_unique<Timer>(_tooltipDelay);
		_timer->onTimer(_showCaller);
		_timer->start();
	}

	void ItemCountTooltip::Think(State* state, Surface* surf)
	{
		if (_timer)
			_timer->think(state, surf);
	}

	TextList* ItemCountTooltip::Show()
	{
		_timer->stop();

		constexpr uint32_t margin = 3;

		static const auto nameStr = _state.tr("STR_NAME");
		static const auto baseStr = _state.tr("STR_BASE");
		static const auto craftStr = _state.tr("STR_CRAFT");

		const uint16_t baseNameColumnWidth = calculateBaseNameColumnWidth(nameStr);
		const uint16_t baseItemsCountColumnWidth = calculateBaseItemsCountColumnWidth(baseStr);
		const uint16_t craftItemsCountColumnWidth = calculateCraftItemsCountColumnWidth(craftStr);
		const uint16_t height = calculateHeight();

		const auto totalWidth = _enableItemsOnCraftColumn
								? baseNameColumnWidth + baseItemsCountColumnWidth + craftItemsCountColumnWidth
								: baseNameColumnWidth + baseItemsCountColumnWidth;

	
		_window = new Window(nullptr, totalWidth + 2 * margin, height + 3 * margin, _coordX, _coordY);
		_state.add(_window);
		_window->invalidate(true);

		_text = new TextList(totalWidth, height, _coordX + margin, _coordY + 1.5 * margin);
		_state.add(_text);

		if (_enableItemsOnCraftColumn)
		{
			_text->setColumns(3, baseNameColumnWidth, baseItemsCountColumnWidth, craftItemsCountColumnWidth);
			_text->setAlign(TextHAlign::ALIGN_RIGHT, 1);
			_text->setAlign(TextHAlign::ALIGN_RIGHT, 2);
			_text->addRow(3, nameStr.c_str(), baseStr.c_str(), craftStr.c_str());
		}
		else
		{
			_text->setColumns(2, baseNameColumnWidth, baseItemsCountColumnWidth);
			_text->setAlign(TextHAlign::ALIGN_RIGHT, 1);
			_text->addRow(2, nameStr.c_str(), baseStr.c_str());
		}
			
		_text->setSelectable(false);
		_text->setBackground(_window);
			
		uint32_t totalBaseCount = 0;
		uint32_t totalCraftCount = 0;
		auto addRow = [this, &totalBaseCount, &totalCraftCount](decltype(_curBaseIt) it)
		{
			const auto baseCount = it->second.first;
			const auto craftCount = it->second.second;

			totalBaseCount += baseCount;
			totalCraftCount += craftCount;

			if (_enableItemsOnCraftColumn)
				_text->addRow(3, it->first.c_str(), std::to_string(baseCount).c_str(), std::to_string(craftCount).c_str());
			else
				_text->addRow(2, it->first.c_str(), std::to_string(baseCount).c_str());
		};

		addRow(_curBaseIt);
		for (auto it = _basesToCounts.cbegin(); it != _basesToCounts.cend(); ++it)
		{
			if (it == _curBaseIt)
				continue;

			addRow(it);
		}

		if (_enableItemsOnCraftColumn)
			_text->addRow(3, _state.tr("STR_GRAND_TOTAL").c_str(), std::to_string(totalBaseCount).c_str(), std::to_string(totalCraftCount).c_str());
		else
			_text->addRow(2, _state.tr("STR_GRAND_TOTAL").c_str(), std::to_string(totalBaseCount).c_str());
		
		_text->invalidate(true);
		return _text;
	}

	uint16_t ItemCountTooltip::calculateBaseNameColumnWidth(const std::string& headerName) const
	{
		const auto* font = _game.getMod()->getFont("FONT_SMALL");
		if (!font)
			return 120;

		uint16_t res = calculateStringWidth(headerName, *font);
		for (const auto& [baseName, _] : _basesToCounts)
			res = std::max(calculateStringWidth(baseName, *font), res);

		res += smallWidthOffset;
		return res;
	}

	uint16_t ItemCountTooltip::calculateBaseItemsCountColumnWidth(const std::string& headerName) const
	{
		const auto* font = _game.getMod()->getFont("FONT_SMALL");
		if (!font)
			return 50;

		uint16_t res = calculateStringWidth(headerName, *font);
		for (const auto& [_, pair] : _basesToCounts)
			res = std::max(calculateStringWidth(std::to_string(pair.first), *font), res);

		res += smallWidthOffset;
		return res;
	}

	uint16_t ItemCountTooltip::calculateCraftItemsCountColumnWidth(const std::string& headerName) const
	{
		const auto* font = _game.getMod()->getFont("FONT_SMALL");
		if (!font)
			return 50;

		uint16_t res = calculateStringWidth(headerName, *font);
		for (const auto& [_, pair] : _basesToCounts)
			res = std::max(calculateStringWidth(std::to_string(pair.second), *font), res);

		res += smallWidthOffset;
		return res;
	}

	uint16_t ItemCountTooltip::calculateHeight() const
	{
		const auto* font = _game.getMod()->getFont("FONT_SMALL");
		if (!font)
			return 80;

		return font->getHeight() * (_basesToCounts.size() + 2); // including header row + summary row
	}

	uint16_t ItemCountTooltip::calculateStringWidth(const std::string& str, const Font& font) const
	{
		const auto converted = Unicode::convUtf8ToUtf32(str);
		uint16_t res = 0;
		for (auto ch : converted)
			res += font.getCharSize(ch).w;

		return res;
	}
}
