/*! \file mid_level_funcs.h
 *
 * Set of functions at middle level (above low-level hardware drivers but below the 
 * high level) for the 3662 calculator mini-project.
 * 
 * This .h file defines the functions which you must write. Your functions must 
 * stick to these specifications: the function name, parameters (order, type 
 * and what they mean), return type (if any) and what the function does. 
 * You will be marked (amongst other things) on whether your functions meet 
 * these specifications. Thus you must not modify this .h file.
 * 
 * Please note also the following requirement: your code here may not access 
 * any hardware directly. For example, to write to a keyboard column you must not 
 * use the Tiva commands to access the output port. You must do it by calling a 
 * routine in the module \a low_level_funcs_tiva (q.v. for details).
 * If you ignore this requirement it will be impossible to compile your code 
 * when marking it, and you will lose marks.
 * 
 * Dr Chris Trayner, 2019 September
 */


/* The next two lines, and the corresponding #endif at the end of the file, 
 * are a common precaution with .h files. The problem they address is that 
 * sometimes a .h file will #include another .h file, and a .c/.cpp file 
 * which #includes them both will import the same .h file twice. This can 
 * result in the same function (etc.) definitions being imported twice. 
 * Multiple declaratons like this cause compiler errors.
 * #ifndef means 'if the named constant has not been defined', which is true 
 * when the .h file is #included for the first time but not thereafter.
 * The constant name is traditionally the name of the .h file, with all 
 * letters rendered in upper case and the dot of .h replaced with an 
 * underline.
 */
#ifndef MID_LEVEL_FUNCS_H
#define MID_LEVEL_FUNCS_H

//! \name Keyboard functions
//@{

/*! Get the next character from keyboard.
 * 
 * \return The character read.
 * 
 * This function reads from the 16-key keypad. It waits until the user has 
 * pressed a key and then returns it as an ASCII character.
 * 
 * Note that the character returned must be the one marked on the keyboard, 
 * not the one that will be displayed.
 * 
 * In practice, this function will probably work by calling KeyboardReadRowCol() 
 * and KeyboardRowCol2Char().
 */
char GetKeyboardChar();

/*! Waits until a keyboard key is pressed, then returns its row and column numbers.
 * 
 * \param [out] row The row number of the key pressed. 
 * 			Counts from 1 (top) to 4 (bottom).
 * \param [out] col The column number of the key pressed. 
 * 			Counts from 1 (leftmost) to 4 (rightmost).
 * 
 * This assumes that exactly one key is pressed.
 * If none is currently pressed, the function waits until one is pressed and 
 * then returns its co-ordinates.
 * 
 * Extra test for extra marks:
 * If two or more keys are pressed at the same time, the function waits until 
 * only one is pressed and returns that key's row and column.
 */
void KeyboardReadRowCol( int *row, int *col );

/*! Converts keyboard row and column numbers to an ASCII character.
 * 
 * \param [in] row The row number of the key pressed. 
 * 			Counts from 1 (top) to 4 (bottom).
 * \param [in] col The column number  of the key pressed. 
 * 			Counts from 1 (leftmost) to 4 (rightmost).
 * \return	ASCII value of the character.
 * 
 * The character returned must be the one marked on the keyboard.
 * Note that A, B, C and D are upper case (capital letters).
 * 
 * Note that the character returned must be the one marked on the keyboard, 
 * not the one that will be displayed. This is because this display character 
 * cannot always be known at this low software level. For instance, the key 
 * labelled with A might be displayed as + or x, depending on whether the 
 * Shift key has just been pressed.
 * 
 * Extra test for extra marks:
 * Check for invalid row and column numbers. If either is invalid,  DONE
 * return the character '?'.
 */
char KeyboardRowCol2Char( int row, int col );

//@}
// End of Keyboard functions

//! \name Display functions
//@{

/*! Print a string at a specified location on the LCD display.
 * 
 * \param [in] line The line number, 1 for top or 2 for bottom.
 * \param [in] char_pos The character position, counting from 1 at the 
 * 		left to 16 at the right.
 * \param [in] string A C-format string to be displayed.
 * 
 * The string should be displayed on the specified line starting at the 
 * specified position. 
 * If the string does not cover the entire display line, the other 
 * characters must not be changed.
 * No characters should be displayed beyond the end of the display line. 
 * Extra marks will be given for software that checks this.    DONE
 * Any such extra characters should simply be ignored. 
 * 
 * Note that \a line and \a char_pos count from 1 not 0.
 * 
 * This function will call functions in \a low_level_funcs_tiva to set up 
 * the print position and write individual characters.
 */
void PrintString( short int line, short int char_pos, const char *string );

//@}
// End of Display functions

#endif // of #ifndef MID_LEVEL_FUNCS_H
