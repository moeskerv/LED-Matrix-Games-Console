﻿#include "tetris.h"
#include "avr/eeprom.h"
#include "operators.h"

static unsigned int EE_highscore EEMEM = 0;
unsigned int Tetris::highscore_ = eeprom_read_word(&EE_highscore);

Tetris::Tetris(Display *display) : Game(display)
{
	// allocate memory to the section for gamestate without tetromino
	field_ = static_cast<byte *>(malloc(display_->rows()));
	tetromino_ = nullptr;
	setSpeed(2);

	clear();
	display_->clear();
	points_ = 0;
}

Tetris::~Tetris()
{
	if (tetromino_ != nullptr)
	{
		delete tetromino_;
	}
	free(field_);

	points_ = 0;
}

void Tetris::start(Event *event)
{
	newTetromino();

	event->removeAllTimers();
	event->addTimer(general_step_interval_);
	event->addTimer();
	event->setFlag(Event::ProcessPinChanges);
	event->setFlag(Event::ProcessTimerOverflows);
}

void Tetris::render()
{
	display_->clear();
	display_->setArray(field_);
	if (tetromino_ != nullptr)
	{
		Pos positions[4];
		tetromino_->getPositions(positions);
		for (Pos p : positions)
		{
			display_->setPixel(p.pos_x, p.pos_y, true);
		}
	}
}

void Tetris::onStop(Event *event)
{
	event->timer(0).stop();
	event->timer(1).stop();
	Game::onStop(event);
}

void Tetris::onContinue(Event *event)
{
	render();
	event->timer(0).start();
	// for the second timer, there is no need to restart, because its used for button long pressed
}

bool Tetris::rotate()
{
	/*
	 * sheme of work:
	 * 1. get current values
	 * 2. calculate next(possible) direction. Possible is'nt meaning that the brick
	 * dosnt collides
	 * 3. Check if the new direction is valid
	 * 4. if not: try to change to a valid position
	 */

	if (tetromino_ == nullptr)
		return false;

	// 1.	get current values
	Pos tmp_pos = tetromino_->pos();
	const Shape shape = tetromino_->shape();

	// 2. calculate new direction
	const Direction new_dir = Tetromino::rotate(tetromino_->direction(), shape);

	// 3. check valid
	byte valid_output = tetromino_->isValid(shape, new_dir, tmp_pos);

	// if not valid, try to move to make valid
	if (valid_output)
	{
		if (valid_output & OVER_LEFT)
		{
			while (valid_output = tetromino_->isValid(shape, new_dir, tmp_pos) & OVER_LEFT)
			{
				tmp_pos.pos_x++;
			}
		}
		else if (valid_output & OVER_RIGHT)
		{
			while (valid_output = tetromino_->isValid(shape, new_dir, tmp_pos) & OVER_RIGHT)
			{
				tmp_pos.pos_x--;
			}
		}
		if (valid_output & OVER_BELOW)
		{
			while (valid_output = tetromino_->isValid(shape, new_dir, tmp_pos) & OVER_BELOW)
			{
				tmp_pos.pos_y++;
			}
		}
		else if (valid_output & OVER_ABOVE)
		{
			while (valid_output = tetromino_->isValid(shape, new_dir, tmp_pos) & OVER_ABOVE)
			{
				tmp_pos.pos_y--;
			}
		}
		if (valid_output & COLLIDE) // collides with exiting tetromino
		{
			return false;
		}
	}
	tetromino_->setPos(tmp_pos);
	tetromino_->setDirection(new_dir);
	render();
	return true;
}

bool Tetris::right()

{
	if (!tetromino_)
		return false;
	Pos pos = tetromino_->pos();
	pos.pos_x++;
	if (tetromino_->isValid(tetromino_->shape(), tetromino_->direction(), pos))
	{ // not valid
		return false;
	}
	tetromino_->setPos(pos);
	render();
	return true;
}

bool Tetris::left()
{
	if (tetromino_ == nullptr)
		return false;
	Pos pos = tetromino_->pos();
	pos.pos_x--;
	if (tetromino_->isValid(tetromino_->shape(), tetromino_->direction(), pos))
	{
		return false;
	}
	tetromino_->setPos(pos);
	render();
	return true;
}

bool Tetris::down()
{
	if (tetromino_ == nullptr)
		return false;
	Pos pos = tetromino_->pos();
	pos.pos_y--;

	if (tetromino_->isValid(tetromino_->shape(), tetromino_->direction(), pos))
	{
		takeOverTetromino();
		checkRowsFinished();
		if (newTetromino())
			return true;
		return false;
	}
	tetromino_->setPos(pos);
	render();
	return false;
}

void Tetris::clear()
{
	for (byte *ptr = field_; ptr < &field_[display_->rows()]; ptr++)
	{
		*ptr = 0;
	}
}

void Tetris::setSpeed(const byte v)
{
	switch (v)
	{
	case 0:
		general_step_interval_ = 1800;
		general_down_interval_ = 180;
		general_first_move_interval_ = 400;
		general_move_interval_ = 200;
		break;
	case 1:
		general_step_interval_ = 1400;
		general_down_interval_ = 140;
		general_first_move_interval_ = 320;
		general_move_interval_ = 160;
		break;
	case 3:
		general_step_interval_ = 800;
		general_down_interval_ = 80;
		general_first_move_interval_ = 240;
		general_move_interval_ = 120;
		break;
	case 4:
		general_step_interval_ = 500;
		general_down_interval_ = 50;
		general_first_move_interval_ = 200;
		general_move_interval_ = 100;
		break;
	case 2:
		general_step_interval_ = 1000;
		general_down_interval_ = 100;
		general_first_move_interval_ = 280;
		general_move_interval_ = 140;
		break;
	default:
		break;
	}
}

unsigned int Tetris::highscore()
{
	return highscore_;
}

bool Tetris::newTetromino()
{
	if (tetromino_)
	{ // make sure, a tetromino exists before delete
		delete tetromino_;
		tetromino_ = nullptr;
	}
	const Shape shape = randomTetrominoShape();
	tetromino_ =
	    new Tetromino(shape, display_->rows(), field_, randomTetrominoDirection(shape), Pos(4, display_->rows() - 1));
	Pos points[4];
	tetromino_->getPositions(points);
	for (Pos p : points)
	{
		if (p.pos_y > display_->rows() - 1)
		{
			Pos pos = tetromino_->pos();
			pos.pos_y -= (p.pos_y - (display_->rows() - 1));
			tetromino_->setPos(pos);
			tetromino_->getPositions(points);
		}
	}
	render();
	if (tetromino_->isValid() & COLLIDE)
	{ // not valid
		return true;
	}
	return false;
}

void Tetris::takeOverTetromino()
{
	Pos positions[4];
	tetromino_->getPositions(positions);
	for (Pos p : positions)
	{
		bitWrite(field_[p.pos_y], p.pos_x, true);
	}
	render();
}

void Tetris::resetHighscore()
{
	eeprom_write_word(&EE_highscore, highscore_ = 0);
}

bool Tetris::onButtonChange(Event *event)
{
	Timer &move_timer = event->timer(1);
	if (event->controlButtonPressed())
	{
		// Rotation
		if (event->buttonUp().pressed())
		{
			rotate();
		}

		// Down(faster)
		if (event->buttonDown().changed())
		{
			if (event->buttonDown().changed())
			{
				if (down())
				{
					return true;
				}
				event->timer(0).setInterval(general_down_interval_);
				event->timer(0).restart();
			}
			else
			{
				// unset fast down
				event->timer(0).setInterval(general_step_interval_);
			}
		}

		// btn left
		if (event->buttonLeft().changed())
		{
			if (event->buttonLeft().state())
			{
				if (!event->buttonRight().state())
				{
					left();
					move_timer.setInterval(general_first_move_interval_);
					move_timer.start();
				}
			}
			else
			{
				move_timer.stop();
				move_timer.clearOverflow();
			}
		}

		// btn right
		if (event->buttonRight().changed())
		{
			if (event->buttonRight().state())
			{
				if (!event->buttonLeft().state())
				{
					right();
					move_timer.setInterval(general_first_move_interval_);
					move_timer.start();
				}
			}
			else
			{
				move_timer.stop();
				move_timer.clearOverflow();
			}
		}
	}
	return false;
}

bool Tetris::onTimerOverflow(Event *event)
{
	Timer &move_timer = event->timer(1);
	if (move_timer.overflow())
	{
		if (event->buttonLeft().state())
		{
			left();
			move_timer.setInterval(general_move_interval_);
		}

		else if (event->buttonRight().state())
		{
			right();
			move_timer.setInterval(general_move_interval_);
		}
	}

	Timer &down_timer = event->timer(0);
	if (down_timer.overflow())
	{
		if (down()) // end of the game
		{
			return true;
		}
		if (event->buttonDown().state())
			down_timer.setInterval(general_down_interval_);
		else
			down_timer.setInterval(general_step_interval_);
	}
	return false;
}

void Tetris::checkRowsFinished()
{
	for (int i = 0; i < display_->rows(); i++)
	{
		while (field_[i] == 0xFF)
		{ // row is full
			field_[i] = 0;
			points_++;
			for (int j = i; j < display_->rows() - 1; j++)
			{
				field_[j] = field_[j + 1];
			}
		}
	}
	render();
	if (points_ > highscore_)
	{
		highscore_ = points_;
		eeprom_write_word(&EE_highscore, highscore_);
		is_new_highscore_ = true;
	}
}

Shape Tetris::randomTetrominoShape()
{
	return Shape(millis() % 7);
}

Direction Tetris::randomTetrominoDirection(const Shape &shape)
{
	byte directions = Tetromino::possibleDirections(shape);
	byte num = Tetromino::possibleDirectionNum(shape);

	if (num <= 1)
	{
		return TOP;
	}

	byte rand = ((millis()) % (num));
	for (int i = 0, j = 0; i < 4; i++)
	{
		if (bitRead(directions, i))
		{
			if (j == rand)
			{
				return Direction(j);
			}
			j++;
		}
	}
}
