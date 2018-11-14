#include "position.h"

Pos::Pos(char x, char y) : pos_x(x), pos_y(y){

}

Pos Pos::operator + (const Pos& pos)
{

	return Pos(pos.pos_x+pos_x,pos.pos_y+pos_y);

}

Pos& Pos::operator += (const Pos& pos)
{
	pos_x += pos.pos_x;
	pos_y += pos.pos_y;
}

Pos Pos::operator- (const Pos& pos)
{
	return Pos(pos_x-pos.pos_x,pos_y-pos.pos_y);
}

Pos& Pos::operator-= (const Pos& pos)
{
	pos_x -= pos.pos_x;
	pos_y -= pos.pos_y;
}

bool Pos::operator==(const Pos& pos){
	return(pos_x == pos.pos_x && pos_y == pos.pos_y);
}

Pos& Pos::operator=(SmartPos &pos)
{
	pos_x = pos.x();
	pos_y = pos.y();
	return *this;
}

bool Pos::operator !=(const Pos& pos){
	return !operator ==(pos);
}

SmartPos::SmartPos(unsigned char x, unsigned char y) {setXy(x,y);}

Pos SmartPos::pos(){
	return Pos(x(),y());
}

unsigned char SmartPos::x(){
	return (compact_pos_ & 0xF0) >> 4;
}

unsigned char SmartPos::y(){
	return compact_pos_ & 0x0F;
}

unsigned char SmartPos::setXy(unsigned char x, unsigned char y){
	return (y & 0x0F) | ((x & 0x0F) << 4);
}