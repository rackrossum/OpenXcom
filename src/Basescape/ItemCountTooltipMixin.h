#pragma once
#include "../Engine/State.h"
#include "ItemCountTooltip.h"
#include "../Engine/InteractiveSurface.h"
#include <memory>
#include "../Engine/Action.h"
#include "../Engine/Options.h"

namespace OpenXcom
{
	template<typename StateType = State>
	class ItemCountTooltipMixin : public StateType
	{
	protected:
		void BindToSurface(InteractiveSurface* surface)
		{
			if (Options::OXCEN::ItemTooltipMode == static_cast<int>(Options::OXCEN::ItemTooltipMode::None))
				return;

			if (Options::OXCEN::ItemTooltipMode == static_cast<int>(Options::OXCEN::ItemTooltipMode::Hover))
			{
				surface->onMouseOver((ActionHandler)&ItemCountTooltipMixin::InitTooltipHover);
				surface->onMouseOut((ActionHandler)&ItemCountTooltipMixin::ClearTooltip);
			}
			else if (Options::OXCEN::ItemTooltipMode == static_cast<int>(Options::OXCEN::ItemTooltipMode::Hotkey))
			{
				surface->onMouseOver((ActionHandler)&ItemCountTooltipMixin::StoreCoords);
				surface->onKeyboardPress((ActionHandler)&ItemCountTooltipMixin::InitTooltipHotkey, Options::OXCEN::ItemTooltipHotkey);
				surface->onKeyboardRelease((ActionHandler)&ItemCountTooltipMixin::ClearTooltip, Options::OXCEN::ItemTooltipHotkey);
				surface->onMouseOut((ActionHandler)&ItemCountTooltipMixin::ClearTooltip);
			}
		}

		void InitTooltipHover(Action* action)
		{
			StoreCoords(action);
			InitTooltip(action, Options::OXCEN::ItemTooltipHoverDelayInTenths * 100u);
		}

		void InitTooltipHotkey(Action* action)
		{
			InitTooltip(action, 0u);
		}

		void InitTooltip(Action* action, uint32_t delay)
		{
			ClearTooltip(nullptr);

			const auto* item = GetItemForTooltip();
			if (!item)
				return;

			StateHandler sh = (StateHandler)&ItemCountTooltipMixin::ShowTooltip;

			_tooltip = std::make_unique<ItemCountTooltip>(item, *GetBase(), *StateType::_game, delay, *this, sh, _x, _y);
			_tooltip->Init();
		}

		virtual const RuleItem* GetItemForTooltip() = 0;
		virtual const Base* GetBase() = 0;

		void ShowTooltip()
		{
			_tooltip->Show();
		}

		void ClearTooltip(Action*)
		{
			_tooltip.reset();
		}

		void StoreCoords(Action* action)
		{
			_x = action->getAbsoluteXMouse();
			_y = action->getAbsoluteYMouse();
		}

		void think() override
		{
			StateType::think();
			if (_tooltip)
				_tooltip->Think(this, nullptr);
		}

	private:
		std::unique_ptr<ItemCountTooltip> _tooltip;
		double _x;
		double _y;
	};

}
