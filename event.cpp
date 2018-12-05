#include "event.h"

Event::Event() : event_(0), flags_(0)
{
}

void Event::setButtonUpState(bool state)
{
	if (state != buttonUpState())
	{
		bitSet(event_, 6);
		bitWrite(event_, 2, state);
	}
}

void Event::setButtonDownState(bool state)
{
	if (state != buttonDownState())
	{
		bitSet(event_, 5);
		bitWrite(event_, 1, state);
	}
}

void Event::setButtonRightState(bool state)
{
	if (state != buttonRightState())
	{
		bitSet(event_, 7);
		bitWrite(event_, 3, state);
	}
}

void Event::setButtonLeftState(bool state)
{
	if (state != buttonLeftState())
	{
		bitSet(event_, 4);
		bitWrite(event_, 0, state);
	}
}

void Event::clear()
{
	// call after processed all
	event_ &= ~(event_ & CHANGE);
	event_ &= ~ON_ENTRY;
	unsigned int t = millis();
	for (int i = 0; i < timers_.size(); i++)
	{
		timers_.itemAt(i).clearOverflow();
	}
	overflow_ = false;
}

bool Event::process()
{
	processTimers();
	return (flag(ProcessEveryCycle) || (flag(ProcessPinChange) && changed()) || (flag(ProcessTimerOverflows) && overflow_));
}

bool Event::processTimers()
{
	unsigned int t = millis();
	for (int i = 0; i < timers_.size(); i++)
	{
		Timer timer = timers_.itemAt(i);
		overflow_ |= timer.overflow_ = timer.process(t);
	}
	return overflow_;
}

void Event::addTimer(byte index, unsigned int interval)
{
	Timer t(interval);
	timers_.append(t);
}

Timer &Event::getTimer(byte index)
{
	return timers_.itemAt(index);
}

bool Event::getOverflow(byte &index)
{
	timers_.itemAt(index).overflow();
}

void Event::removeTimer(byte &index)
{
	timers_.remove(index);
}

void Event::removeAllTimers()
{
	timers_.removeAll();
}

void Event::setFlag(Event::Flags flag, bool set)
{
	bitWrite(flags_, flag, set);
}

bool Event::flag(Event::Flags flag)
{
	return flags_ & flag;
}

void Event::clearFlags()
{
	flags_ = 0;
}
