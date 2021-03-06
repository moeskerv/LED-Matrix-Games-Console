/* text.cpp : Support text options for the led matrix (also shifeted)
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

#include "text.h"

Text::Text(MatrixDisplay *display)
	 : display_(display), curser_pos_(-1), shift_mode_(OFF), offset_(0), start_col_(0), end_col_(display->cols()-1), start_row_(0),
		end_row_(display->rows()-1), shift_start_col_(3), alignment_(MIDDLE)
{
	setShiftSpeed(5);
}

void Text::clear()
{
	display_->clearRows(start_row_, end_row_);
	first_ = text_ = nullptr;
	shift_mode_ = OFF;
	Timer::stop();
}

// shift text by one column and start over if nothing is shown anymore
void Text::shift()
{
	--current_shift_start_col_;
	if (display_->setString(first_, current_shift_start_col_, curser_pos_ + text_ - first_, 1, offset_) <= 0)
	{
		first_ = text_;
		current_shift_start_col_ = shift_start_col_;
	}
	else
	{ // should we advance to next start char?
		byte w = display_->width(*first_);
		if (current_shift_start_col_ + w < 0)
		{
			++first_;
			current_shift_start_col_ += w + 1; // char width + 1 column spacing
		}
	}
}

// display (and remember) text (for future shifting)
void Text::setText(const char *text, bool show)
{
	first_ = text_ = text;
	display_->clearRows(start_row_, end_row_);
	computeShiftMode();
	if(show){
		display_->show();
	}
}

void Text::setNumber(const int &value, bool show)
{
	setText(display_->formatInt(number_buffer_, 10, value),show);
}

void Text::computeShiftMode()
{
	int width = display_->width(text_);
	shift_mode_ = (width > (end_col_ - start_col_) + 1 ? SHIFT : NO_SHIFT);

	if (shift_mode_ == SHIFT)
	{ // when shifting, start in column 7
		current_shift_start_col_ = shift_start_col_ + 1;
		shift();
		Timer::start();
	}
	else
	{ // otherwise, start start like the alignment pretends
		if(alignment_ == RIGHT){
			current_shift_start_col_ = end_col_ - width + 1;
		}
		else if (alignment_ == MIDDLE){
			current_shift_start_col_ = start_col_ + (end_col_ - start_col_ + 1 - width) / 2;
		}
		else { // its left
			current_shift_start_col_ = 0;
		}
		display_->setString(text_, current_shift_start_col_, curser_pos_, 1, offset_);
		Timer::stop();
	}
}

void Text::setShiftSpeed(const int speed)
{
	speed_ = speed;
	setInterval(1000/speed_); // from shifts per second to mseconds per shift
}

void Text::setCursor(const char pos)
{
	curser_pos_ = pos;
}

void Text::setOperationRows(const byte start, const byte end)
{
	start_row_ = start;
	end_row_ = end;
}

void Text::onOverflow()
{
	shift();
	Timer::clearOverflow();
	display_->show();
}
