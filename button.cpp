#include "button.h"

Button::Button(byte debounce_time) : debounce_time_(debounce_time)
{
}

void Button::processDebounce()
{
	checkChange();
	if (state_ & InDebounce)
	{
		if (++count_ == debounce_time_)
		{
			count_ = 0;
			bitToggle(state_, State); // only toggle bit because it changed surely(else we need to read)
			bitSet(state_, Changed);
		}
	}
}

void Button::checkChange()
{
	if (pin_.read() != state())
	{
		if (!(state_ & InDebounce))
		{
			bitSet(state_, InDebounce);
			count_ = 0;
		}
	}

	else if (state_ & InDebounce)
	{
		bitClear(state_, InDebounce);
	}
	return;
}