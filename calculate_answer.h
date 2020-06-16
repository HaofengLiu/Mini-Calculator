/*! \file calculate_answer.h
 * Set of functions at the bottom level for the 3662 calculator 
 * mini-project. These are calculation function and associated constants.
 * 
 * The software to perform the actual calculation is provided here: it is the 
 * function CalculateAnswer(). It takes the input string read from keyboard and 
 * 	1. Checks it for any input syntax errors (e.g. two plusses together).
 * 	2. If there are no errors, calculates the answer and returns it as a float.
 * 		(Actually a double-precision float.)
 * 	3. If there are errors, produces an error message.
 * Any error message is indicated by the variable \a error_ref_no. This is used as 
 * the subscript of the two arrays \a error_message_line1 and \a error_message_line2. 
 * Each error message is divided into two lines of 16 characters max, split across 
 * these two arrays, to facilitate display on the 2-line 16-character display.
 * These two arrays are also defined in this .h file, making them available to the 
 * calling program.
 * 
 * This module is provided for you ready written. You must not modify it. 
 * (This would not gain any extra marks, and might lose you marks.)
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
#ifndef CALCULATE_ANSWER_H
#define CALCULATE_ANSWER_H


/*! Parse the input from keyboard and produce either the answer or an error message.
 * 
 * \param [in] input_buffer A string with the characters read from keyboard.
 * 		This should be C-format, i.e. an array of \a char terminated with 
 * 		a null.
 * \param [in] input_buffer_size The size of the \a input_buffer array. The software 
 * 		will not read \a input_buffer beyond this. This is a precaution 
 * 		agains \a input_buffer lacking its terminating null.
 * \param [out] error_ref_no The reference number of the error, if any. 
 * 		If there is no error, this is set to zero. If there is an error, 
 * 		this number can be used to subscript the two arrays 
 * 		\a error_message_line1 and \a error_message_line2 to produce an 
 * 		error message.
 * \return If there was no error, the result of the calculation is returned. 
 * 		If there was an error, 0.0 is returned.
 * 
 * This function handles the input string in \a input_buffer. It will only read 
 * until the terminating null. If this is missing it will not read more than 
 * \a input_buffer_size characters. (The lack of a terminating null is an error and 
 * will be reported as such. Note that this is a software bug, not a user input 
 * error.)
 * 
 * The user will have ended their input string with an equals sign. This must 
 * not be included in \a input_buffer.
 * 
 * The function then parses the input string (checks it for syntax errors). 
 * The string should be a normal valid calculation, so syntax errors are such things 
 * as two plus signs together or two decimal points in the same number.
 * The only valid characters are the digits and '+', '-', 'x' (lower case) and '/'.
 * (The equals sign and rubout should be handled by your keyboard reading 
 * function and not handed to CalculateAnswer().)
 * 
 * If the parsing shows no errors, the result is calculated and returned as the 
 * result of the function. \a error_ref_no is set to zero indicating no error.
 * However, if the calculation is syntactically valid but produces an invalid result (e.g. 
 * divide by zero), this will be reported as an error.
 * 
 * If the parsing shows an error, \a error_ref_no is set to a non-zero number 
 * indicating the error. If more than one error occurred, only one will be 
 * indicated; it is undefined which. The value 0.0 will be returned as the result 
 * of the function.
 * 
 */
double CalculateAnswer( char *input_buffer, int input_buffer_size, int *error_ref_no );


#define MAX_ERROR_MESSAGES	20	//!< Size of the error message arrays.

static const char error_message_line1[MAX_ERROR_MESSAGES][17] = {
	"No error",		//  0
	"Unidentified",		//  1
	"SOFT BUG: Empty",	//  2
	"No null or too",	//  3
	"Invalid char",		//  4
	"Number with > 1",	//  5
	"Invalid number",	//  6
	"May not start",	//  7
	"May not end",		//  8
	"Two adjacent",		//  9
	"Two adjacent",		// 10
	"E must be foll-",	// 11
		
	// 16 chars end ^ here  32 here ^
},
error_message_line2[MAX_ERROR_MESSAGES][17] = {
	"No error",		//  0
	"error",		//  1
	"input string",		//  2
	"long I/P string",	//  3
	"in input string",	//  4
	"decimal point",	//  5
	"",			//  6
	"with +,x,/ or E",	//  7
	"with operator",	//  8
	"operators",		//  9
	"E operators",		// 10
	"owed by integer",	// 11
	
	// 16 chars end ^ here
};

#endif // of #ifndef CALCULATE_ANSWER_H
