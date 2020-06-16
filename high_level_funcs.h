/*! \file high_level_funcs.h
 *
 * Set of functions at high level (above mid level but below main)  
 * for the 3662 calculator mini-project.
 * 
 * This .h file defines the functions which you must write. Your functions must 
 * stick to these specifications: the function name, parameters (order, type 
 * and what they mean), return type (if any) and what the function does. 
 * You will be marked (amongst other things) on whether your functions meet 
 * these specifications. Thus you must not modify this .h file.
 * 
 * Please note also the following requirement: your code here may not access 
 * any hardware directly. For example, to write to a keyboard column you must not 
 * use the Tiva commands to access the output port. You must do it by calling 
 * functions in a lower level module. 
 * If you ignore this requirement it will be impossible to compile your code 
 * when marking it, and you will lose marks.
 * (In practice, this \a high_level_funcs module will call functions in 
 * \a mid_level_funcs, and they will call functions in \a low_level_funcs_tiva 
 * to do the actual I/O. This \a high_level_funcs may also call 
 * \a low_level_funcs_tiva directly to clear the display.)
 * 
 * Dr Chris Trayner, 2019 September
 */


/* The next two lines, and the corresponding #endif at the end of the file, 
 * are a common precaution with .h files. The problem they address is that 
 * sometimes a .h file will #include another .h file, and a .c/.cpp file 
 * which #includes them both will import the same .h file twice. This can 
 * result in the same function (etc.) definitions being imported twice. 
 * Repeated declaratons like this can cause compiler errors.
 * #ifndef means 'if the named constant has not been defined', which is true 
 * when the .h file is #included for the first time but not thereafter.
 * The constant name is traditionally the name of the .h file, with all 
 * letters rendered in upper case and the dot of .h replaced with an 
 * underline.
 */
#ifndef HIGH_LEVEL_FUNCS_H
#define HIGH_LEVEL_FUNCS_H

//! \name Keyboard functions
//@{

/*! Read characters from the keyboard and echo them on the LCD. Assemble them 
 * into a C-format string and return this when the End Input (*) key is pressed.
 * 
 * \param [out] input_buffer An array of \a char to be used as a C-format 
 * 		string. When the function exits, this must contain the 
 * 		calculator input edited by the user.
 * \param [in] input_buffer_size The size of \a input_buffer, including 
 * 		the trailing null.
 * 
 * The function must wait until the first key is pressed. Until then,  
 * the user may be reading the previous answer. Once the first key is pressed, 
 * the display line used to echo input is cleared and a cursor is placed 
 * at the start of that line. (You might decide to clear the other line too.)
 * 
 * Then this function reads characters as they are entered. Each character is 
 * 	- If it is A, B or C, translated into the display character. This 
 * 		depends on whether the Shift key has been pressed.
 * 	- Entered into \a input_buffer. 
 * 	- Displayed on the LCD at the next character position. The cursor 
 * 		should then be moved to the next position, unless this would 
 * 		move it beyond the end of the line.
 * 
 * There are some special cases to consider:
 * 	- The software must keep track of whether the Shift key has 
 * 		just been pressed.
 * 	- If the Rubout key is pressed, the previous character entry 
 * 		must be undone. This means changing both the 
 * 		\a input_buffer and the display. However, if the cursor 
 * 		is already at the starting position nothing should 
 * 		be done. You should also consider what to do if 
 * 		the character to be rubbed out is Shift.
 * 	- If the End Input (*) key is pressed, the input is complete. 
 * 		The function should return.
 * 	- The function should not accept more than 
 * 		\a input_buffer_size-1 characters. The -1 is because one 
 * 		space is reserved for the trailing null. 
 * 		In the simple case, this will set a limit of 16 characters 
 * 		which is all that will fit on one line of display. 
 * 
 * 		If the user types more characters, they should be 
 * 		ignored: neither echoed nor added to \a input_buffer.
 * 		It would be useful to alert the user, e.g. by flashing 
 * 		the display briefly and then re-displaying the 
 * 		previously echoed input.
 * 
 * 		After hitting this limit (and possibly warning the user), 
 * 		one character may be validly entered: Rubout. 
 * 		You might also make it legal to enter End Input. 
 * 
 * Note that the Shift, Rubout and End Input keys should not be entered 
 * in \a input_buffer or echoed on the LCD.
 * 
 * For convenience, here are the character translations. Note  that the x 
 * is lower case.
 * 	| Key	| Unshifted	| Shifted |
 * 	| :--:|   :--:	  |   :--:	|
 * 	|  A	|    +		  |     x 	|
 * 	|  B  |    -	   	|     /  	|
 * 	|  C	|    .	  	|     E  	|
 * D is the shift key, End Input is asterik (*) and Rubout is hash (#).
 * 
 * This function will presumably call functions in \a mid_level_funcs to 
 * read each character from keyboard and print it to the LCD.
 */
void ReadAndEchoInput( char *input_buffer, int input_buffer_size );

//@}
// End of Keyboard functions

//! \name Display functions
//@{

/*! Display the result of the calculation.
 * 
 * \param [in] answer The number to be displayed. 
 * 		Note that this is of type \a double, i.e. higher precision 
 * 		floating point than \a float. This takes more memory space 
 * 		and (on a small processor) more processor time, but normal 
 * 		\a float can have poor enough accuracy and give bad results.
 * 
 * The result should be displayed on the LCD. The text displayed must not 
 * go beyond the end of the display. This probably means that it will be 
 * limited to one line of 16 characters maximum. It could go on to the 
 * second line, but this would make it harder to read and would be of 
 * questionable benefit.
 * 
 * The line(s) where the answer is displayed should be cleared first.
 * No cursor should be displayed on the line with the result.
 * 
 * Some thought should be given to the format of the display, as the 
 * number could be very small or very large. Four possibilities are listed 
 * here, of increasing level of refinement:
 * 	1. Scientific format, e.g. 1.23E4. This will work for all numbers, 
 * 		and will be needed for very large or very small ones.
 * 		However, it is clumsy for middle-size ones, e.g. 
 * 		1.234E1 for 12.34.
 * 	2. Middle-size numbers displayed normally (e.g. 12.34 or -0.678), 
 * 		with very large or small ones displayed in scientific 
 * 		format.
 * 	3. Choosing the number of decimal places to avoid trailing zeroes, 
 * 		e.g. 12.34 not 12.3400000.
 * 	4. Displaying integers as such, e.g. 42 not 42.0.
 * You should also think about the number of significant figures to display.
 * A 16-character display can accommodate a minus, a decimal point and 14 
 * digits. This is equivalent to about 46-bit accuracy, which is probably 
 * more than type double will provide.
 * 
 * Writing software to convert a floating-point number to ASCII characters 
 * would be very time-consuming. You are advised to use the \a sprintf() 
 * function. You can then call PrintString() in \a mid_level_funcs with 
 * the string.
 * 
 * Note that this function only needs to handle valid floating-point 
 * numbers. If the calculation produced anything invalid, e.g. from 
 * dividing by zero, CalculateAnswer() would have issued an error 
 * message.
 */
void DisplayResult( double answer );

/*! Display an error message.
 * 
 * \param [in] error_message_line1 A C-format string containing the first 
 * 			line of the error message. It will have a maximum 
 * 			of 16 characters plus the trailing null.
 * \param [in] error_message_line2 A C-format string containing the second 
 * 			line of the error message. It will have a maximum 
 * 			of 16 characters plus the trailing null.
 * 
 * This should clear the display and then display the two strings on their 
 * respective lines. Since the two parts of the message may each be up to 
 * 16 characters long, they should start at the left-hand end of the line. 
 * No cursor should be displayed.
 * 
 * These error messages are provided by the system and you may assume they 
 * are no more than 17 chars long (including the trailing null). I wrote 
 * these messages, and I never make misstakes.
 * 
 * Actually, do you believe that? If you're a cautious programmer, you 
 * will ignore what I just said and add an error check that the strings 
 * are not too long.
 */
void DisplayErrorMessage( const char *error_message_line1, 
			  const char *error_message_line2 );

//@}
// End of Display functions

#endif // of #ifndef HIGH_LEVEL_FUNCS_H
