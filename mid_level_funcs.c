/* mid_level_funcs.c
 *
 * Set of functions at middle level (above low-level hardware drivers but below the 
 * high level) for the 3662 calculator mini-project.
 * 
 * For documentation, see the documentation in the corresponding .h file.
 * 
 * Dr Chris Trayner, 2019 September
 */

#include "TExaS.h"
#include "mid_level_funcs.h"
#include "low_level_funcs_tiva.h"

// ------------------------ Keyboard functions ------------------------

char GetKeyboardChar()
{
	int row;
	int col;
	KeyboardReadRowCol(&row, &col);
	return KeyboardRowCol2Char(row, col);
} // GetKeyboardChar

void KeyboardReadRowCol( int *row, int *col )
{
	unsigned char nibble;
	unsigned char input = 0x00;
	do{
	for (nibble = 0x01; nibble <= 0x08; nibble = nibble << 1){ 
		WriteKeyboardCol(nibble); 
		input = ReadKeyboardRow();
		if (input == 0x01) {
			*row = 0;
			*col = HexToDeci(nibble);
			break;
		} else if (input == 0x02) {
			*row = 1;
			*col = HexToDeci(nibble);
			break;
		} else if (input == 0x04) {
			*row = 2;
			*col = HexToDeci(nibble);
			break;
		} else if (input == 0x08) {
			*row = 3;
			*col = HexToDeci(nibble);
			break;
		} 
	}
  }while(input == 0x00);
} // KeyboardReadRowCol

char KeyboardRowCol2Char( int row, int col )
{
	int matrix[4][4] = {'1', '2', '3', 'A',
											'4', '5', '6', 'B',
											'7', '8', '9', 'C',
											'*', '0', '#', 'D'};
	
	if((row > 3) || (row < 0) || (col > 3) || (col < 0)) {  // 
		return '?';
	} else {
		char key = matrix[row][col];									
		return key;
	}
} // KeyboardRowCol2Char

// ------------------------ Display functions ------------------------

void PrintString( short int line, short int char_pos, const char *string )
{
	SetPrintPosition(line, char_pos);
	WaitMicrosec(20);
	while (*string) {
		PrintChar (*string);
		string++;
		char_pos++;
		if(char_pos > 16) { 
			break;
		}
	}
} // PrintString

