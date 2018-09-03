#include "tetromino.h"

Tetromino::Tetromino(tetromino::SHAPE shape, byte heigth, byte * field, tetromino::DIRECTION direction, tetromino::POS pos)
    :shape_(shape), heigth_(heigth), field_(field), direction_(direction),pos_(pos)
{

}

bool Tetromino::getPositions(tetromino::POS (&positions)[4])
{
	return getPositions(positions,shape_,direction_, pos_);
}

bool Tetromino::getPositions(tetromino::POS (&positions)[4], tetromino::SHAPE shape, tetromino::DIRECTION direction, tetromino::POS pos)
{
	tetromino::SPECIFICATIONS brick;
	memcpy_P(&brick, &tetromino::tetrominos + shape, sizeof(tetromino::SPECIFICATIONS));
	if(!(direction & brick.directions)){
		return false; // failure: direction isnt available
	}
	tetromino::POS pos_flip = tetromino::getPos(brick.shape[0]);
	for(int i = 0; i < 4; i++)
	{
		tetromino::POS rotated_brick_part_pos, brick_position;

		brick_position.pos_x = tetromino::getPos(brick.shape[i]).pos_x - pos_flip.pos_x;
		brick_position.pos_y = tetromino::getPos(brick.shape[i]).pos_y - pos_flip.pos_y;

		if(direction == tetromino::TOP){
			rotated_brick_part_pos = brick_position;
		}
		else if(direction == tetromino::RIGHT){
			rotated_brick_part_pos.pos_x = brick_position.pos_y;
			rotated_brick_part_pos.pos_y = -brick_position.pos_x;
		}
		else if(direction == tetromino::BOTTOM){
			rotated_brick_part_pos.pos_x = -brick_position.pos_x;
			rotated_brick_part_pos.pos_y = -brick_position.pos_y;
		}
		else if(direction == tetromino::LEFT){
			rotated_brick_part_pos.pos_x = -brick_position.pos_y;
			rotated_brick_part_pos.pos_y = brick_position.pos_x;
		}
		positions[i].pos_x = pos.pos_x + rotated_brick_part_pos.pos_x;
		positions[i].pos_y = pos.pos_y + rotated_brick_part_pos.pos_y;
	}
	return true;
}

byte Tetromino::possibleDirections(){
	return possibleDirections(shape_);
}


byte Tetromino::possibleDirections(tetromino::SHAPE shape){
	tetromino::SPECIFICATIONS brick;
	memcpy_P(&brick, &tetromino::tetrominos + shape, sizeof(tetromino::SPECIFICATIONS));
	return brick.directions;
}

void Tetromino::rotate()
{

}

byte Tetromino::isValid()
{
	 return isValid(shape_,direction_, pos_);
}

byte Tetromino::isValid(tetromino::SHAPE shape, tetromino::DIRECTION direction, tetromino::POS pos)
{
	tetromino::POS positions[4];
	byte valid_errors;
	getPositions(positions,shape,direction,pos);
	for(int i = 0; i < 4; i++){
		if(positions[i].pos_y > heigth_)
		{
			valid_errors |= tetromino::OVER_ABOVE;
		}
		else if(positions[i].pos_y < 0)
		{
			valid_errors |= tetromino::OVER_BELOW; // is over below
		}
		else if(positions[i].pos_x < 0)
		{
			valid_errors |= tetromino::OVER_LEFT; // is left over
		}

		else if(positions[i].pos_x > 7)
		{
			valid_errors |= tetromino::OVER_RIGHT; // is right over
		}

		else if(bitRead(*(field_ + positions[i].pos_x),byte(positions[i].pos_y)))
		{
			valid_errors |= tetromino::COLLIDE; // collides with exiting tetromino part
		}
	}
	return valid_errors; // no collisions - valid
}
