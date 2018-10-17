#include "matrixdisplay.h"
#include "ATmega-master/include/spi.h"

// class for driving display


// ... X X X X
//  3  X X X X
//  2  X X X X
//  1  X X X X
//  0  X X X X
//     0 1 2 ...

#define FIRST_LETTER (' ' - 10)
#define LAST_LETTER '~'
const byte LETTERS[] PROGMEM = {
   0b00001110, 0b00010001, 0b00100001, 0b01000010, 0b00100001, // smiley
   0b00010001, 0b00001110,													// smiley
   0b00111100, 0b01000010, 0b10010101, 0b10100001, 0b10100001, // smiley
   0b10010101, 0b01000010, 0b00111100,									// smiley
   0b00111100, 0b01000010, 0b10100101, 0b10100001, 0b10100001, // smiley
   0b10100101, 0b01000010, 0b00111100,									// smiley
   0b00111100, 0b01000010, 0b10100101, 0b10010001, 0b10010001, // smiley
   0b10100101, 0b01000010, 0b00111100,									// smiley
   0b01111101, 0b00010010, 0b00010010, 0b01111101,					// Ä = 26
   0b00111101, 0b01000010, 0b01000010, 0b00111101,					// Ö = 27
   0b00111100, 0b01000001, 0b01000001, 0b01111100,					// Ü = 28
   0b00100000, 0b01010101, 0b01010101, 0b01111000,					// ä = 29
   0b00111000, 0b01000101, 0b01000101, 0b00111000,					// ö = 30
   0b00111100, 0b01000001, 0b01000001, 0b01111100,					// ü = 31
   0b00000000, 0b00000000, 0b00000000,									// space = 32
   0b01011111,																	// !
   0b00000011, 0b00000000, 0b00000011,									// "
   0b00010100, 0b00111110, 0b00010100, 0b00111110, 0b00010100, // #
   0b00100100, 0b01101010, 0b00101011, 0b00010010,					// $
   0b01100011, 0b00010011, 0b00001000, 0b01100100, 0b01100011, // %
   0b00110110, 0b01001001, 0b01010110, 0b00100000, 0b01010000, // &
   0b00000011,																	// '
   0b00011100, 0b00100010, 0b01000001,									// (
   0b01000001, 0b00100010, 0b00011100,									// )
   0b00101000, 0b00011000, 0b00001110, 0b00011000, 0b00101000, // *
   0b00001000, 0b00001000, 0b00111110, 0b00001000, 0b00001000, // +
   0b10110000, 0b01110000,													// ,
   0b00001000, 0b00001000, 0b00001000,									// -
   0b01100000, 0b01100000,													// .
   0b01100000, 0b00011000, 0b00000110, 0b00000001,					// /
   0b00111110, 0b01000001, 0b01000001, 0b00111110,					// 0
   0b01000010, 0b01111111, 0b01000000,									// 1
   0b01100010, 0b01010001, 0b01001001, 0b01000110,					// 2
   0b00100010, 0b01000001, 0b01001001, 0b00110110,					// 3
   0b00011000, 0b00010100, 0b00010010, 0b01111111,					// 4
   0b00100111, 0b01000101, 0b01000101, 0b00111001,					// 5
   0b00111110, 0b01001001, 0b01001001, 0b00110000,					// 6
   0b01100001, 0b00010001, 0b00001001, 0b00000111,					// 7
   0b00110110, 0b01001001, 0b01001001, 0b00110110,					// 8
   0b00000110, 0b01001001, 0b01001001, 0b00111110,					// 9
   0b00101000, 0b00000000,													// :
   0b10000000, 0b01010000,													// ;
   0b00010000, 0b00101000, 0b01000100,									// <
   0b00101000, 0b00101000, 0b00101000,									// =
   0b01000100, 0b00101000, 0b00010000,									// >
   0b00000010, 0b01011001, 0b00001001, 0b00000110,					// ?
   0b00111110, 0b01001001, 0b01010101, 0b01011101, 0b00001110, // @
   0b01111110, 0b00010001, 0b00010001, 0b01111110,					// A
   0b01111111, 0b01001001, 0b01001001, 0b00110110,					// B
   0b00111110, 0b01000001, 0b01000001, 0b00100010,					// C
   0b01111111, 0b01000001, 0b01000001, 0b00111110,					// D
   0b01111111, 0b01001001, 0b01001001, 0b01000001,					// E
   0b01111111, 0b00001001, 0b00001001, 0b00000001,					// F
   0b00111110, 0b01000001, 0b01001001, 0b01111010,					// G
   0b01111111, 0b00001000, 0b00001000, 0b01111111,					// H
   0b01000001, 0b01111111, 0b01000001,									// I
   0b00110000, 0b01000000, 0b01000001, 0b00111111,					// J
   0b01111111, 0b00001000, 0b00010100, 0b01100011,					// K
   0b01111111, 0b01000000, 0b01000000, 0b01000000,					// L
   0b01111111, 0b00000010, 0b00001100, 0b00000010, 0b01111111, // M
   0b01111111, 0b00000100, 0b00001000, 0b00010000, 0b01111111, // N
   0b00111110, 0b01000001, 0b01000001, 0b00111110,					// O
   0b01111111, 0b00001001, 0b00001001, 0b00000110,					// P
   0b00111110, 0b01000001, 0b01000001, 0b10111110,					// Q
   0b01111111, 0b00001001, 0b00001001, 0b01110110,					// R
   0b01000110, 0b01001001, 0b01001001, 0b00110010,					// S
   0b00000001, 0b00000001, 0b01111111, 0b00000001, 0b00000001, // T
   0b00111111, 0b01000000, 0b01000000, 0b00111111,					// U
   0b00001111, 0b00110000, 0b01000000, 0b00110000, 0b00001111, // V
   0b00111111, 0b01000000, 0b00111000, 0b01000000, 0b00111111, // W
   0b01100011, 0b00010100, 0b00001000, 0b00010100, 0b01100011, // X
   0b00000111, 0b00001000, 0b01110000, 0b00001000, 0b00000111, // Y
   0b01100001, 0b01010001, 0b01001001, 0b01000111,					// Z
   0b01111111, 0b01000001,													// [
   0b00000001, 0b00000110, 0b00011000, 0b01100000,					// \ backslash
   0b01000001, 0b01111111,													// ]
   0b00000010, 0b00000001, 0b00000010,									// hat
   0b01000000, 0b01000000, 0b01000000, 0b01000000,					// _
   0b00000001, 0b00000010,													// `
   0b00100000, 0b01010100, 0b01010100, 0b01111000,					// a
   0b01111111, 0b01000100, 0b01000100, 0b00111000,					// b
   0b00111000, 0b01000100, 0b01000100, 0b00101000,					// c
   0b00111000, 0b01000100, 0b01000100, 0b01111111,					// d
   0b00111000, 0b01010100, 0b01010100, 0b00011000,					// e
   0b00000100, 0b01111110, 0b00000101,									// f
   0b10011000, 0b10100100, 0b10100100, 0b01111000,					// g
   0b01111111, 0b00000100, 0b00000100, 0b01111000,					// h
   0b01000100, 0b01111101, 0b01000000,									// i
   0b01000000, 0b10000000, 0b10000100, 0b01111101,					// j
   0b01111111, 0b00010000, 0b00101000, 0b01000100,					// k
   0b01000001, 0b01111111, 0b01000000,									// l
   0b01111100, 0b00000100, 0b01111100, 0b00000100, 0b01111000, // m
   0b01111100, 0b00000100, 0b00000100, 0b01111000,					// n
   0b00111000, 0b01000100, 0b01000100, 0b00111000,					// o
   0b11111100, 0b00100100, 0b00100100, 0b00011000,					// p
   0b00011000, 0b00100100, 0b00100100, 0b11111100,					// q
   0b01111100, 0b00001000, 0b00000100, 0b00000100,					// r
   0b01001000, 0b01010100, 0b01010100, 0b00100100,					// s
   0b00000100, 0b00111111, 0b01000100,									// t
   0b00111100, 0b01000000, 0b01000000, 0b01111100,					// u
   0b00011100, 0b00100000, 0b01000000, 0b00100000, 0b00011100, // v
   0b00111100, 0b01000000, 0b00111100, 0b01000000, 0b00111100, // w
   0b01000100, 0b00101000, 0b00010000, 0b00101000, 0b01000100, // x
   0b10011100, 0b10100000, 0b10100000, 0b01111100,					// y
   0b01100100, 0b01010100, 0b01001100,									// z
   0b00001000, 0b00110110, 0b01000001,									// {
   0b01111111,																	// |
   0b01000001, 0b00110110, 0b00001000,									// }
   0b00001000, 0b00000100, 0b00001000, 0b00000100,					// ~
};

const int PTN_LETTERS[] PROGMEM{
	0,
	7,
	15,
	23,
	31,
	35,
	39,
	43,
	47,
	51,
	55,
	58,
	59,
	62,
	67,
	71,
	76,
	81,
	82,
	85,
	88,
	93,
	98,
	100,
	103,
	105,
	109,
	113,
	116,
	120,
	124,
	128,
	132,
	136,
	140,
	144,
	148,
	150,
	152,
	155,
	158,
	161,
	165,
	170,
	174,
	178,
	182,
	186,
	190,
	194,
	///////
	198,
	202,
	205,
	209,
	213,
	217,
	222,
	227,
	231,
	235,
	239,
	243,
	247,
	252,
	256,
	261,
	266,
	271,
	276,
	280,
	282,
	286,
	288,
	291,
	295,
	297,
	301,
	305,
	309,
	313,
	317,
	320,
	324,
	328,
	331,
	335,
	339,
	342,
	347,
	351,
	355,
	359,
	363,
	367,
	371,
	374,
	378,
	383,
	388,
	393,
	397,
	400,
	403,
	404,
	407,
	411,
};


const byte col_order[] = {5,0,2,3,1,4,6,7};
const byte row_order[] = {7,6,5,4,3,2,1,0};

MatrixDisplay::MatrixDisplay(byte height, byte width)
   :height_(height), width_(width)
{
	// allocate memory for (columnwi0,se) display content
	rows_ = static_cast<byte*>(malloc(height_));
	//	latch_.setPins(PortPin::B,2);
	//	latch_.output();

	//	// declare selected pins of Port C as OUTPUT
	//	// set all eight pins of PortD as output
	DDRD = 0xFF;
	bitSet(DDRB,2);

	SPI_Init();
	clear();

	// for calibration
#if 0
	for(int i = 0; i < 8; i++){
		setRow(i,0x00FF << i);
		//		setPixel(i,i,1);
	}
#endif
}

MatrixDisplay::~MatrixDisplay(){
	free(rows_);
}

void MatrixDisplay::show(){
	static byte row = 0;
	bitClear(PORTB, 2); // clear latch
	for(byte *col = rows_ + row, *end = rows_ + height_ + row; col != end; col+=8){
		SPI_SendByte(~*col);
	}
	//	SPI_SendByte(*(rows_+row));
	//	SPI_SendByte(*(rows_+row+8));

	PORTD = 0; // avoid glowing of prev/next row
	bitSet(PORTB, 2); // set latch
	PORTD = _BV(row_order[row]);
	// select row to be displayed
	row++;
	if(row >= 8){
		row = 0;

	}
}

void MatrixDisplay::clear(){
	for(int i = 0; i < height_; i++){
		setRow(i,0);
	}
}

void MatrixDisplay::setPixel(byte col, byte row, bool value)
{
	bitWrite(rows_[row],col_order[col],value);
}

byte MatrixDisplay::orderCols(byte value){
	byte out;
	for(byte i = 0; i<8;i++){
		bitWrite(out,col_order[i],bitRead(value,i));
	}
	return out;
}

void MatrixDisplay::setRow(byte row, int value)
{
	if(row > height_)
		return;
	rows_[row] = orderCols(value);
}

void MatrixDisplay::disable()
{
	for(byte i = 0; i < 2; i++){
		SPI_SendByte(0xFF);
	}
	PORTD = 0;
}

byte MatrixDisplay::mapCol(byte row){
	return ((row / 8) * 8) +  col_order[row % 8];
}

void MatrixDisplay::setColumn(byte column, byte value,byte offset)
{
	if(column > width_)
		return;
	for(int r = 0; r < 8; r++){
		setPixel(column,r+offset,bitRead(value,7-r));
	}
}

void MatrixDisplay::setArray(byte *array)
{
	for(int i = 0; i < 16; i++)
	{
		setRow(i,array[i]);
	}
}


// clear content of columns start to end
void MatrixDisplay::clearRows(byte start, byte end)
{
	if (start < 0) start = 0;
	if (end > height_) end = height_;

	for(; start < end; ++start)
		setRow(start,0);
}

// set the column content of given column to given byte value

inline const byte* MatrixDisplay::letterStart(char ch) {
	if (ch < FIRST_LETTER || ch > LAST_LETTER) ch = '?';
	return LETTERS + pgm_read_word(PTN_LETTERS+ch-FIRST_LETTER);
}

const byte MatrixDisplay::letterWidth(char ch){
	if (ch < FIRST_LETTER || ch > LAST_LETTER) ch = '?';
	return pgm_read_word(PTN_LETTERS+ch-FIRST_LETTER+1) - pgm_read_word(PTN_LETTERS+ch-FIRST_LETTER);
}

// write a sing-le char, starting at column
byte MatrixDisplay::setChar(char ch, int column, byte offset)
{

	const byte *end;
	const byte *start = letterStart(ch);
	int width = letterWidth(ch);
	end = start + width;
	for (; start != end; ++start, ++column)
		setColumn(column, pgm_read_byte(start),offset);
	return width;
}

int MatrixDisplay::setString(const char *s, int column, char cursor_pos, char spacing, byte offset) {
	int char_pos = 0;
	while (*s != 0 && column < width_) {
		byte char_width = setChar(*s, column, offset);
		if (char_pos++ == cursor_pos){
			for (byte col = column, end = col + char_width; col != end; ++col){
				setPixel(7, col, 1);
			}
		}
		column += char_width;
		clearRows(column, column+spacing);
		column += spacing;
		++s;
	}
	if (*s) column += 1; // there is more text following that we clipped
	return column;
}

int MatrixDisplay::width(char ch)
{
	return *letterStart(ch);
}

// determine the width of the given string
int MatrixDisplay::width(const char *s, char spacing)
{
	int column = 0;
	while (*s != 0) {
		column += spacing + width(*s);
		++s;
	}
	return column;
}

// converts an integer to a string
char* MatrixDisplay::formatInt(char* digits, byte size, int value)
{
	if (size < 3) return digits;

	digits[--size] = '\0'; // terminating '\0'
	digits[--size] = '0'; // zero display if value == 0

	bool neg = false;
	if (value < 0) {
		neg = true;
		value = -value;
	} else if (value == 0)
		--size;

	for (; size != 0 && value != 0; --size) {
		digits[size] = '0' + (value % 10);
		value = value / 10;
	}

	if (neg) digits[size] = '-';
	else ++size;

	return digits + size;
}

