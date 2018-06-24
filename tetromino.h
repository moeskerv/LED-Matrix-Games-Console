#pragma once
#include "avr.h"
#include "matrixdisplay.h"

namespace tetromino {
enum TETROMINO : byte {
	I,J,L,O,S,T,Z
};
enum DIRECTION : byte {
	TOP =  1 << 0,
	RIGHT = 1 << 2,
	BOTTOM = 1 << 1,
	LEFT = 1 << 3,
};

struct POS {
	char pos_x;
	char pos_y;
};

inline byte getX(byte xy){
	return (xy & 0xF0) >> 4;
}

inline byte getY(byte xy){
	return xy & 0x0F;
}

inline byte setXy(byte x, byte y){
	return (y & 0x0F) | ((x & 0xF0) << 4);
}

inline POS getPos(byte xy){
	POS pos;
	pos.pos_x = (xy & 0xF0) >> 4;
	pos.pos_y = xy & 0x0F;
	return pos;
}

struct SPECIFICATIONS{
	byte directions;
	byte shape[4];

};
const PROGMEM SPECIFICATIONS tetrominos[7] = {
   {TOP | RIGHT                ,{setXy(0,1),setXy(0,0),setXy(0,2),setXy(0,3)}},
   {TOP | RIGHT | BOTTOM | LEFT,{setXy(1,1),setXy(1,0),setXy(1,2),setXy(0,2)}},
   {TOP | RIGHT | BOTTOM | LEFT,{setXy(0,1),setXy(0,0),setXy(0,2),setXy(1,2)}},
   {TOP                        ,{setXy(0,0),setXy(0,1),setXy(1,0),setXy(1,1)}},
   {TOP | RIGHT                ,{setXy(1,0),setXy(1,1),setXy(0,1),setXy(2,0)}},
   {TOP | RIGHT| BOTTOM | LEFT ,{setXy(0,1),setXy(0,0),setXy(0,2),setXy(1,1)}},
   {TOP | RIGHT                ,{setXy(1,0),setXy(0,0),setXy(1,1),setXy(2,1)}},
};
}

class Tetromino
{
public:

	Tetromino(tetromino::TETROMINO shape, byte heigth, byte *field, tetromino::DIRECTION direction, tetromino::POS pos);

	void setPos(tetromino::POS pos);
	tetromino::POS getPos() {return pos_;}

	void setDirection(tetromino::DIRECTION direction);
	tetromino::DIRECTION getDirection() {return direction_;}

	bool getPositions(tetromino::POS(&positions)[4]);
	bool getPositions(tetromino::POS(&positions)[4],tetromino::TETROMINO shape, tetromino::DIRECTION direction, tetromino::POS pos);

	byte isValid();
	byte isValid(tetromino::TETROMINO shape, tetromino::DIRECTION direction, tetromino::POS pos);


private:
	const tetromino::TETROMINO shape_;
	tetromino::DIRECTION direction_;
	tetromino::POS pos_;
	const byte heigth_;
	byte * field_;
};

