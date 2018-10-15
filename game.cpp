﻿#include "game.h"
#include "string.h"

Game::Game(Display *display):
   display_(display)
{
	// allocate memory to the section for gamestate without tetromino
	field_ = static_cast<byte*>(malloc(display_->rows()));
	tetromino_ = new Tetromino(tetromino::I, display_->rows(), field_, tetromino::LEFT,tetromino::Pos{2,5});
}

Game::~Game()
{
	if(tetromino_ != nullptr){
		delete tetromino_;
	}
	free(field_);
}

void Game::render()
{
	display_->clear();
	display_->setArray(field_);
	if(tetromino_ != nullptr){
		tetromino::Pos positions[4];
		tetromino_->getPositions(positions);
		for(Pos p : positions){
			display_->setPixel(p.pos_y,p.pos_x, true);
		}
	}
}

bool Game::rotate()
{
	/*
	 * function scheme:
	 * 1. get current values
	 * 2. calculate next(possible) direction. Possible means not that the brick dosnt collides
	 * 3. Check if the new direction is valid
	 *    4. if not: try to change to a valid position
	 *
	*/

	tetromino::DIRECTION direction = tetromino_->getDirection();
	tetromino::DIRECTION new_direction;
	tetromino::Pos position = tetromino_->getPos();
	tetromino::Pos new_position = position;
	tetromino::SHAPE shape = tetromino_->getShape();

	byte possible_directions = tetromino_->possibleDirections();
	new_direction = Tetromino::rotate(direction,shape);

	// position is not valid
	byte valid_output = tetromino_->isValid(shape,new_direction,position);
	if(valid_output != 0)
	{
		if(valid_output & tetromino::OVER_LEFT) // left over
		{
			++new_position.pos_x;
			while(tetromino_->isValid(shape,new_direction,new_position) & tetromino::OVER_LEFT){
				new_position.pos_x++;
			}
		}
		else if(valid_output & tetromino::OVER_RIGHT) // right over
		{
			new_position.pos_x --;
			while(tetromino_->isValid(shape,new_direction,new_position) & tetromino::OVER_RIGHT){
				new_position.pos_x--;
			}
		}
		if(valid_output & tetromino::OVER_BELOW) // over below
		{
			new_position.pos_y ++;
			while(tetromino_->isValid(shape,new_direction,new_position) & tetromino::OVER_BELOW){
				new_position.pos_y++;
			}
		}
		else if(valid_output & tetromino::OVER_ABOVE) // above over
		{
			new_position.pos_y --;
			while(tetromino_->isValid(shape,new_direction,new_position) & tetromino::OVER_ABOVE){
				new_position.pos_y--;
			}
		}
		if(valid_output & tetromino::COLLIDE) // collides with exiting tetromino
		{

		}
	}
}

bool Game::right()
{
	Pos pos = tetromino_->getPos();
	pos.pos_x++;
	if(tetromino_->isValid(tetromino_->getShape(),tetromino_->getDirection(),pos)){  // not valid
		return false;
	}
	tetromino_->setPos(pos);
	return true;
}

bool Game::left()
{
	Pos pos = tetromino_->getPos();
	pos.pos_x--;
	if(tetromino_->isValid(tetromino_->getShape(),tetromino_->getDirection(),pos)){  // not valid
		return false;
	}
	tetromino_->setPos(pos);
	return true;
}

bool Game::step()
{
	Pos pos = tetromino_->getPos();
	pos.pos_y--;
	byte valid_output = tetromino_->isValid(tetromino_->getShape(),tetromino_->getDirection(),pos);
	if(valid_output){  // not valid
		if(valid_output & tetromino::OVER_ABOVE){
			return true; // game ends
		}
		newTetromino();
		checkRowsFinished();
	}
	tetromino_->setPos(pos);
	return false;
}

void Game::reset()
{
	if(tetromino_ != nullptr)
		delete(tetromino_);
	this->clear();
	display_->clear();
}

void Game::clear()
{
	for(byte * ptr = field_; ptr < &field_[display_->rows()];ptr++){
		*ptr = 0;
	}
}

void Game::begin()
{
	newTetromino();
}

bool Game::newTetromino()
{
	if(tetromino_ != nullptr){
		delete tetromino_;
		tetromino_ = nullptr;
	}
	tetromino::SHAPE shape = randomTetrominoShape();
	tetromino_ = new Tetromino(shape, display_->rows(), field_, randomTetrominoDirection(shape),Pos(4,char(display_->rows())));

	tetromino_->setPos(Pos(4,4));
//	tetromino::POS points[4];
//	tetromino_->getPositions(points);

//	for(POS p : points){
//		if(p.pos_y > display_->rows()){
//			tetromino::POS pos = tetromino_->getPos();
//			pos.pos_y -= (p.pos_y - display_->rows());
//			tetromino_->setPos(pos);
//		}
//	}
	render();
	if(tetromino_->isValid() != 0){ // not valid
		return true;

	}
	return false;
}

void Game::checkRowsFinished()
{
	for(int i = 0; i < display_->rows(); i++){
		if(field_[i] == 0xFF){ // row is full
			points_++;
			for(int j = display_->rows() - 1; i > i; j--){
				field_[j] = field_[j+1];
			}
			field_[display_->rows()] = 0;
		}
	}
}

tetromino::SHAPE Game::randomTetrominoShape()
{
	return tetromino::SHAPE(millis() % 7);
}

tetromino::DIRECTION Game::randomTetrominoDirection(tetromino::SHAPE shape){
	byte directions = Tetromino::possibleDirections(shape);
	int num = 0;
	for(int i = 0; i < 4; i++){
		if(bitRead(directions, i))
			num++;
	}
	if(num <= 1){
		return TOP;
	}
	bitSet(PORTB,1);
	int i_direction = (micros() * 883) % (num-1);
	for(int i = 0, j = 0; i < 4; i++){
		if(bitRead(directions,i)){
			if(j == i_direction){
				tetromino::DIRECTION dir = tetromino::DIRECTION(j);
				return dir;
			}
			j++;
		}
	}
}

