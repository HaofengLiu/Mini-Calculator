/*! \file main.c 
 * Top level program for the 3362 calculator mini-project.
 * 
 * This program is written for you. 
 * You must not modify this file (except for the constant 
 * \a INPUT_BUFFER_SIZE, which you may change). If you make any 
 * other changes, the software which tests your code may fail 
 * and you will probably lose many marks. 
 * 
 * There is a further detail worth explaining. If the user presses 
 * the equals key immediately, i.e. with no calculation entered, the 
 * calculator displays the last calculated value. While the calculator 
 * remains turned on, this only requires the first \a if statement inside 
 * the loop. However, there is an optional feature which you can add for 
 * extra marks if you wish. This is to record the value of the calculation 
 * in the processor's flash memory. When the calculator is turned off and 
 * then on again, this is read from flash and displayed. This is 
 * accomplished with the two commands WriteFloatToFlash() and 
 * ReadFloatFromFlash() in the code below. These are both in the module 
 * \a low_level_funcs_tiva. 
 * Handling flash memory is a bit complicated, so if you want to write this 
 * extra code you are recommended to leave it until the rest of your 
 * project is working. If you do not want to write it, you need not 
 * modify any code. The two calls here will call the functions, which 
 * exist in \a low_level_funcs_tiva but do nothing.
 */

/*! Size of the array of input chars typed by the user.
 * 
 * As the user presses keys, what they type is entered into an input buffer: 
 * a \a char array used as a C-format string. \a INPUT_BUFFER_SIZE is  
 * this array's size, and includes space for the trailing null. 
 * 
 * You must choose the size of this array, though 17 (one more than the 
 * number of characters on a display line) is the easiest choice.
 * The size will depend on the quality of your input handling, specifically 
 * the maximum number of characters the user can type before they press the  
 * equals key to perform the calculation.
 * 	- The simplest is to limit the input size to one display line. 
 * 		The input is simply echoed on one line.
 * 	- More input can be allowed if the echoed characters overflow 
 * 		from the first display line onto the second.
 * 	- Far more input can be allowed if, once the display line(s) 
 * 		is/are full, it/they scroll allowing more input to be 
 * 		typed at the end. (This means that the first input will 
 * 		be invisible.) This requires more complicated software. 
 * 		\a INPUT_BUFFER_SIZE can now be arbitrarily large; the 
 * 		actual value is a programmer's decision.
 */
#define INPUT_BUFFER_SIZE	17

#include "high_level_funcs.h"
#include "low_level_funcs_tiva.h"
#include "calculate_answer.h"

/*! The entry point when the program is run.
 * 
 * This is written for you, as it implements the overall strategy (the 
 * top-level design) of the program, which has been dictated to you.
 * You must not change it.
 * 
 * Actually, its design is more or less inevitable. 
 * After initialising everything it goes into an infinite loop.
 * This reads from keyboard, calls the calculation function to do the 
 * mathematical bit, then displays the result.
 * 
 * It calls the various functions which do the real work, all but one 
 * of which are written by you.
 */
int main()
{
	double	answer = 0.0 ;/* Initialise in case 
				 * ReadFloatFromFlash() does nothing. */
	
	InitAllHardware();	// In low_level_funcs_tiva.
	
	answer = ReadDoubleFromFlash(); // See note at top. 
	DisplayResult( answer ); // In high_level_funcs.
	
	while (1) {
	char	input_buffer [INPUT_BUFFER_SIZE];
	int	error_ref_no = 0;	/* Init in case just = is entered 
					 * as the first entry. */
		
		ReadAndEchoInput( input_buffer, INPUT_BUFFER_SIZE ); /* In
							high_level_funcs. */
		/* If the user typed equals immediately (indicated by an empty 
		 * buffer), we leave the previous answer to be displayed. 
		 * Otherwise we calculate it. */
		if (input_buffer[0] != '\0')
			answer = CalculateAnswer( input_buffer, 
				INPUT_BUFFER_SIZE,  &error_ref_no ); /* In 
							calculate_answer. */
		if (error_ref_no == 0) { // meaning no error.
			DisplayResult( answer ); // In high_level_funcs.
			WriteDoubleToFlash( answer ); // See note at top.
		} else	DisplayErrorMessage( error_message_line1[error_ref_no], 
				error_message_line2[error_ref_no] ); /* In
						high_level_funcs. */
	} // while
		
	/* Normally you would have a return 0; command here.
	 * Like most embedded processor programs, this one never exits 
	 * (returns from main()): it enters an infinite loop, and when the user 
	 * finishes using it they mererly turn the device off.
	 * You may find the compiler gives a warning message about main() being 
	 * an int function without a return: this warning may be ignored. */
} // main
