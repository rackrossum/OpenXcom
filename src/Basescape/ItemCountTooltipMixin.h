#pragma once
#include "../Engine/State.h"
#include "ItemCountTooltip.h"
#include "../Engine/InteractiveSurface.h"
#include <memory>
#include "../Engine/Action.h"

namespace OpenXcom
{
	template<typename StateType = State>
	class ItemCountTooltipMixin : public StateType
	{
	protected:
		void BindToSurface(InteractiveSurface* surface)
		{
			surface->onMouseOver((ActionHandler)&ItemCountTooltipMixin::InitTooltip);
			surface->onMouseOut((ActionHandler)&ItemCountTooltipMixin::ClearTooltip);
		}

		void InitTooltip(Action* action)
		{
			ClearTooltip(nullptr);

			const auto* item = GetItemForTooltip();
			if (!item)
				return;

			StateHandler sh = (StateHandler)&ItemCountTooltipMixin::ShowTooltip;
			const auto x = action->getAbsoluteXMouse();
			const auto y = action->getAbsoluteYMouse();

			_tooltip = std::make_unique<ItemCountTooltip>(item, *GetBase(), *StateType::_game, 1500u, *this, sh, x, y);
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

		void think() override
		{
			StateType::think();
			if (_tooltip)
				_tooltip->Think(this, nullptr);
		}

	private:
		std::unique_ptr<ItemCountTooltip> _tooltip;
	};

}
