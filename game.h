/* game.h : Abstract class for all games
 *            for managing the game from the statemaschine
 *
 * Copyright (C) 2019 Felix Haschke
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see http://www.gnu.org/licenses/.
 */

#pragma once
#include "avr.h"
#include "avr/eeprom.h"
#include "display.h"
#include "event.h"

class Game
{
public:
	Game(Display *display);
	virtual ~Game();
	virtual void start(Event *event) = 0;
	virtual bool process(Event *event);

	virtual void setSpeed(byte v) = 0;

	virtual unsigned int points() const = 0;
	virtual bool isNewHighscore() const
	{
		return is_new_highscore_;
	}

protected:
	virtual bool onButtonChange(Event *event);
	virtual bool onTimerOverflow(Event *event);
	virtual void onStop(Event *event);
	virtual void onContinue(Event *event);
	virtual void render() = 0;

	Display *display_;
	bool is_new_highscore_ = false;
	bool stop_state_;
};
