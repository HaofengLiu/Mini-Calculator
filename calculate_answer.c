/*! \file calculate_answer.c 
 * The calculation function and associated constants.
 * 
 * This module is provided for you ready written. You must not modify it. 
 * (This would not gain any extra marks, and might lose you marks.) 
 * Nor do you need to understand how it works; you can simply use it.
 * 
 * This is Version 1.2
 * Changes since 1.1:
 * IdentifyTokens() while loop replaced by for, to prevent infinite loops.
 * Various error traps changed when RUNNING_ON_PC is false:
 * set error code 1 (unidentified error) and return.
 * Commenting improved.
 * 
 * Version 1.1
 * Changes:
 * 2019nov27: Tiva turns out not to have exit(EXIT_FAILURE), so this is not now 
 * used when RUNNING_ON_PC is false (zero). The two instances of exit() have 
 * been moved inside the #if RUNNING_ON_PC statements.
 * Also a correction to MergeNumber() - mistaked extra ) removed.
 */

/* Constants controlling what is available.
 * You may need to change the setting of these if you have trouble compiling. */


/* Output to PC/laptop screen: only possible when debugging on such a 
 * machine, not on the Tiva. */
#define RUNNING_ON_PC		0	/* If we're running on a PC (or Mac 
					 * or laptop or whatever) we can print 
					 * debugging and error messages to screen,
					 * so set this to 1.
					 * Set this to 0 for use on the Tiva.
					 */
#if RUNNING_ON_PC
#define DEBUG			3	/* Levels from 0 (no debugging output) 
					 * to 9 (loadsa waffle).
					 */
#else
#define DEBUG			0	// Not possible.
#endif

/* One particular maths function is used: pow(). This is used to raise 10 to a 
 * power, used with the E key (e.g. 1.23E4 is 1.23 . 10^4, written in C as 
 * 1.23 * pow( 10.0, 4 ) ).
 * If you have trouble finding the function pow(), or it won't compile and link, 
 * try turning the following constant from 1 to 0.
 * This will make the behaviour of the E key wrong - E followed by anything 
 * will have the value 0. E.g. 1.23E4 will become 1.23E0 = 1.23.
 * You may also have to edit Makefile and remove both instances of -lm 
 * (load math module) in the commands to compile calculator and test_calculator.
 */
#define USE_TEN_TO_POWER	1


/* Most of these #includes are needed even if RUNNING_ON_PC is zero, 
 * as they define *e.g.) sscanf (needed to read text into numbers).
 * However, exit(EXIT_FAILURE) for error exits is not available on the Tiva. */
#include <stdio.h>
#if RUNNING_ON_PC
#include <stdlib.h>
#endif
#include <string.h>
#include <math.h>
#include "calculate_answer.h"


#define MAX_NUMS_AND_OPS	20	/* Maximum number of numbers, 
		which is also the maximum number of operators, which function 
		CalculateAnswer() can handle. The densest these can be is with 
		an input buffer of alternating 1-digit numbers and operators, 
		e.g. "1+1+1+1+ .... 1". This depends on the paraameter 
		input_buffer_size, which is a copy of constant INPUT_BUFFER_SIZE 
		in main.c, but this is not known to the present module at 
		compile time. However, with a 2x16 display there cannot be more 
		than 16 numbers and 16 operators. Setting MAX_NUMS_AND_OPS to 
		this, or slightly more for caution (minor effects not properly 
		understood), is safe. */
typedef struct {
	double	number[ MAX_NUMS_AND_OPS ];
	int	n_numbers;
	char	infix_operator[ MAX_NUMS_AND_OPS ];
	int	n_infix_operators;
	char	num_and_op_used[ MAX_NUMS_AND_OPS ]; // char used as Boolean.
} ParsedExpression;


int IsOperator( char ch )
{
	switch (ch) {
		case '+': case '-': case 'x': case '/': case 'E':
			return 1;
		default: return 0;
	}
} // IsOperator

void SyntaxCheckStage1( char *input_buffer, int max_buffer_size, int *error_ref_no )
/* Check for strings which are empty, not null-terminated, too long or 
 * which contain invalid character(s). */
{
int	i, actual_buffer_size;

	// Empty string (should have been handled in main()):
	if (input_buffer[0] == '\0') {
		*error_ref_no = 2; // "SOFT BUG: Empty"
		return; // Only report first error, so don't check for more.
	}
	
	// Null missing or string too long:
	*error_ref_no = 3;
	for (i=0; i < max_buffer_size; i++)
		if (input_buffer[i] == '\0') {
			*error_ref_no = 0;
			break;
		}
	if (error_ref_no != 0)
		return; // Only report first error, so don't check for more.
	actual_buffer_size = strlen( input_buffer );	/* Null found, so 
				strlen is safe and no need for strnlen. */
	
	// Invalid char:
	for (i=0; i < actual_buffer_size; i++)
		switch (input_buffer[i]) {
			case '0': case '1': case '2': case '3': case '4': 
			case '5': case '6': case '7': case '8': case '9':
			case '+': case '-': case 'x': case '/': case '.': case 'E':
				break;
			default:
				*error_ref_no = 4;
				return;
		}
} // SyntaxCheckStage1

void SyntaxCheckStage2( char *input_buffer, int *error_ref_no )
/* Check for bad operators: as first or last chars, two together (with 
 * exceptions) and E only followed by integers. */
{
int	i, buffer_size = strlen( input_buffer );
char	ch1 = input_buffer[0];

	/* First and last characters may not be operators, 
	 * except that the first may be a minus: */
	if (IsOperator( ch1 ) && ch1 != '-') {
		*error_ref_no = 7;
		return; // Only report first error, so don't check for more.
	}
	if (IsOperator( input_buffer[ buffer_size-1 ] ) ) {
		*error_ref_no = 8;
		return; // Only report first error, so don't check for more.
	}
	
	/* There are only 3 valid cases of an operator following immediately  
	 * after another: x- /- and E- . */
	for (i=0; i < buffer_size-1; i++) {
	char	ch1 = input_buffer[i], 
		ch2 = input_buffer[i+1]; /* The loop stops before buffer_size-1, 
				one before the end, so ch2 will not over-run. */
	int	ok = 1;
		if (IsOperator( ch1 ) && IsOperator( ch2 ) ){
			ok = 0;
			if (ch2 == '-' 
			&& (ch1 == 'x' || ch1 == '/' || ch1 == 'E') )
				ok = 1;
		} // outer if
		if (! ok) { // Error: no need to check the rest of the string.
			*error_ref_no = 9; // "Two adjacent" "operators"
			return; // Only report first error, so don't check for more.
		}
	} // for

	/* An E operator must be followed by an integer (not, e.g., 1.2E3.4). 
	 * Check that after an E there are no dots 
	 * until next operator or end of buffer: */
	for (i=0; i < buffer_size-1; i++)
		if (input_buffer[i] == 'E') {
		int	j;
			for (j=i+1; j < buffer_size; j++) {
				if (IsOperator( input_buffer[j] ))
					break; /* From the inner loop.
						* We've met the next operator 
						* before a dot: we're happy.
						* (NB - dot does not count 
						* as an operator for 
						* IsOperator().) */
				if (input_buffer[j] == '.') {
					*error_ref_no = 11; /*
						"E must be foll-" "owed by integer" */
					return; /* Only report first error, 
						   so don't check for more. */
				} // inner if
			} // inner for
		} // outer if
} // SyntaxCheckStage2

void ExtractNumber( char *input_buffer, int *ch_no, int buf_len, 
		     ParsedExpression *parsed_expression, int *error_ref_no )
/* Extract a number from input_buffer.
 * A number is an optional minus, followed by 1 or more digits, with an optional 
 * decimal point (only one) before the end.
 * E.g. the following are valid: 123 -123 123.45 -123.45 0.2 .2 -0.2 -.2
 * but the following are invalid: --123 123.4.5 ..0
 * (but note that, e.g., -123-4 would be valid as -123 followed by another operator).
 * General algorithm: copy the number into char array num_as_string 
 * then use sscanf to convert it froom string to double.
 */
{
char	num_as_string[100] = ""; // Far more than enough.
int	next_ch_no = 0,		// Position within num_as_string.
	n_dots_found = 0, num_is_ended = 0, n_convs;
double	number_read;
	
#if DEBUG >= 8
	printf( "ExtractNumber() starts with \"%s\"\n", 
				&(input_buffer[*ch_no]) );
#endif
	if (parsed_expression->n_numbers >= MAX_NUMS_AND_OPS) { /* 
		Check for software bug. Should not be possible because 
		MAX_NUMS_AND_OPS is large enough for as many operators and 
		numbers as can be entered in a 2x16 char display. See 
		the definition of MAX_NUMS_AND_OPS above. */
#if RUNNING_ON_PC
		puts( "\nFATAL: software bug: too many numbers "
			"(increase MAX_NUMS_AND_OPS in calculate_answer.c)" );
		exit( EXIT_FAILURE );
#else
		// The best we can do to flag an error:
		*error_ref_no = 1; // Unidentified error.
		return;
#endif
	} // if
	
	/* Copy chars into array num_as_string. This ends when a non-number 
	 * character is met. */
	
	// If it starts with a minus, copy this into num_as_string.
	if (input_buffer[*ch_no] == '-') 
		num_as_string[ next_ch_no++ ] = input_buffer[ (*ch_no)++ ];
	
	/* Copy number into array num_as_string 
	 * (i.e. while digit or dot, and string not ended): */
	do {
	char	this_ch = input_buffer[ *ch_no ];
		if (*ch_no > buf_len) break;
		switch ( this_ch ) {
			case '.':
				n_dots_found ++;
				/* No break, so continue and perform all 
				 * the digit stuff: */
			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9': 
				num_as_string[ next_ch_no++ ] = this_ch;
				(*ch_no) ++; /* Only if still going, note, 
						not in the default: below. */
				break;
			default: // Number is ended.
				num_is_ended = 1;
				break; /* This just breaks from the switch, 
					* not from the do/while. The fact that 
					* num_ended = 1 will cause the 
					* while to end. */
		} // switch
	} while (! num_is_ended );
	num_as_string[ next_ch_no ] = '\0'; // Add end-of-string null.
	/* ANALYSIS of whether this could loop infinitely:
	 * The switch will either
	 * 1) If dot or digit, increment *ch_no. This will ultimately make 
	 * 	*ch_no > buf_len, so the if at the start of the do/while 
	 * 	will cause a break from the do/while.
	 * OR
	 * 2) If not dot or digit, num_is_ended will be set to 1, so the 
	 * 	do/while will exit.
	 */
#if DEBUG >= 7
	printf( "\tNumber %d read as \"%s\"\n", 
		parsed_expression->n_numbers, num_as_string );
#endif
	
	// Was it a valid number?
	if (n_dots_found > 1) { // Multiple decimal points.
		*error_ref_no = 5; // "Number with > 1" "decimal point"
		return;
	}
	
	// Valid number: convert from string to double:
	n_convs = sscanf( num_as_string, "%lf", &number_read );
	if (n_convs != 1) { // Invalid number
		*error_ref_no = 6; // "Invalid number" ""
		return;
	}
	
	// Valid: record it:
	parsed_expression->number[ parsed_expression->n_numbers++ ] = number_read;
} // ExtractNumber

void ExtractOperator( char *input_buffer, int *ch_no, int buf_len, 
		     ParsedExpression *parsed_expression, int *error_ref_no )
// Extract the next char in the input_buffer, which should be an operator.
{
	char	this_ch;
	
#if DEBUG >= 8
	printf( "ExtractOperator() starts with \"%s\"\n", 
				&(input_buffer[*ch_no]) );
#endif
	
	this_ch = input_buffer[ (*ch_no)++ ];
	if (*ch_no > buf_len) return; /* Should never happen - this should have been 
					checked before this function was called - 
					so this is an extra precaution. */
	switch ( this_ch ) {
		case '+': case '-': case 'x': case '/': case 'E':
			parsed_expression->
				infix_operator[ 
					parsed_expression->n_infix_operators++ ] 
					= this_ch; // Reecord the operator.
			break;
		default:
			*error_ref_no = 1; // Unknown error - should never happen.
	} // switch
#if DEBUG >= 7
	printf( "\tOperator %d read as %c\n", 
		parsed_expression->n_infix_operators-1, /* -1 because already 
							incremented above. */
		parsed_expression->
			infix_operator[ parsed_expression->n_infix_operators-1 ] );
#endif
} // ExtractOperator

#if RUNNING_ON_PC
void PrintTokens( ParsedExpression parsed_expression )
// Useful debugging routine.
{
int	i;
	printf( "%d numbers:", parsed_expression.n_numbers );
	for (i=0; i < parsed_expression.n_numbers; i++ ) {
		/* Print the number, but with square brackets round it 
		 * if it has been used. */
	char	brack1 = '[', brack2 = ']';
		if (! parsed_expression.num_and_op_used[i])
			brack1 = brack2 = ' ';
		printf( "\t%c%g%c", brack1, parsed_expression.number[i], brack2 );
	}
	printf( "\n%d operators:", parsed_expression.n_infix_operators );
	for (i=0; i < parsed_expression.n_infix_operators; i++ )
		printf( "\t    \'%c\'", parsed_expression.infix_operator[i] );
	putchar( '\n' );
} // PrintTokens
#endif

void IdentifyTokens( char *input_buffer, int *error_ref_no, 
		     ParsedExpression *parsed_expression )
{
int	i, ch_no = 0, buf_len = strlen( input_buffer ); /* SyntaxCheckStage1 
			has already thrown out any input_buffer without a 
			trailing null, so using strlen not strnlen is safe. */
	
	// Extract alternate numbers and operators:
	/* This is basically a while(1) loop which breaks or returns from the inside.
	 * As an extra precaution against looping infinitely, however, it is 
	 * a for loop which quits if it goes round too often. The maximum 
	 * number of times it should be able to go round is for one number 
	 * or operator per character of the input buffer. That buffer can be at 
	 * most 32 chars (2x16 display). The for loop limit is slightly more 
	 * than this. */
	for (i=1; i < 35; i++) { // Normally breaks from inside.
	//while(1) { // Breaks from inside.
		ExtractNumber( input_buffer, &ch_no, buf_len, 
			       parsed_expression, error_ref_no );
		if (*error_ref_no != 0) return;	// No point in further processing.
		if (ch_no >= buf_len) break;	// Input finished.
		
		ExtractOperator( input_buffer, &ch_no, buf_len, 
				 parsed_expression, error_ref_no );
		if (*error_ref_no != 0) return;	// No point in further processing.
		if (ch_no >= buf_len) break;	// Input finished.
	} // for
	if (i >= 35) { // Software bug: loop ended.
#if RUNNING_ON_PC
		puts( "SOFTWARE BUG: calculate_answer/IdentifyTokens() "
			"loop ends." )
		exit( EXIT_FAILURE );
#endif
		*error_ref_no = 1; // Unidentified error.
		return;
	}
	
#if DEBUG >= 5
	PrintTokens( *parsed_expression );
#endif
} // IdentifyTokens

void SyntaxCheckStage3( ParsedExpression parsed_expression, int *error_ref_no )
/* Highly specific check: for two adjacent Es.
 * NB - whereas the previous SyntaxChecks examined the input buffer, 
 * this one examines the parsed_expression list of operators.
 */
{
int	i;
	for (i=0; i < parsed_expression.n_infix_operators-1; i++)
		if (  parsed_expression.infix_operator[i] == 'E'
		   && parsed_expression.infix_operator[i+1] == 'E') {
			*error_ref_no = 10;
			return;
		}
} // SyntaxCheckStage3

void MergeNumbers( ParsedExpression *parsed_expression, 
		   int this_index, int next_index, char op )
/* This is part of the heart of the algorithm: it applies one of the four 
 * arithmetic operators to two numbers. 
 * E.g. it replaces 5+3 with 8.
 * In more detail, the number with the higher index into parsed_expression 
 * is so replaced. The lower one is left intact but will never be looked at 
 * again. E.g. in 3+5, the 3 5 is replaced by 3 8.
 * NB - the order of assessment is not sorted out here but in the function 
 * which calls it. This outer mechanism has set up the indices to the two 
 * numbers, which are provided as the two parameters this_index and next_index.
 */
{
double	num1 = parsed_expression->number[ this_index ], 
	num2 = parsed_expression->number[ next_index ];
	switch (op) {
		case '+':
			parsed_expression->number[ next_index ] = num1 + num2;
			break;
		case '-':
			parsed_expression->number[ next_index ] = num1 - num2;
			break;
		case 'x':
			parsed_expression->number[ next_index ] = num1 * num2;
			break;
		case '/':
			parsed_expression->number[ next_index ] = num1 / num2;
			break;
		case 'E': 
			parsed_expression->number[ next_index ] = num1 *
/* TO-DO: replace this by loop:	/* There is now an annoying detail.
				 * This operator requires a way of raising 
				 * 10 to an integer power. The simple way is 
				 * to use the pow() function. But in case that 
				 * is not available on the embedded processor, 
				 * there is the option of ignoring it. This 
				 * results in a poor result but at least 
				 * it works. 
				 * See the definition of USE_TEN_TO_POWER 
				 * above. */
#if USE_TEN_TO_POWER
							 pow( 10.0, num2 ); 
#else
							 1.0;
#endif
			break;
	} // switch
	
	parsed_expression->num_and_op_used[ this_index ] = 1; // Record as used.
	
#if DEBUG >= 6
	printf( "After merge t=%d, n=%d, %c:\n", this_index, next_index, op );
	PrintTokens( *parsed_expression );
#endif
} // MergeNumbers

void EvaluateExpressionForOneOperator( ParsedExpression *parsed_expression, char op, 
			       int *error_ref_no )
{
int	this_index, next_index, found;

	for (this_index=0; this_index < parsed_expression->n_infix_operators; this_index++) {
		// Is it the right operator?
		if (parsed_expression->infix_operator[ this_index] != op)
			continue;

		// Find the next unused number to merge into it:
		found = 0;
		for (next_index=this_index+1; 
		     		next_index <= parsed_expression->n_numbers;
		     				next_index++) {
			if (! parsed_expression->num_and_op_used[ next_index ] ) {
				found = 1;
				break;
			} // if
		} // inner for
		
		// Error check:
		if (! found) {
#if RUNNING_ON_PC
			puts( "\nFATAL: software bug: failed to find "
				"next number to merge." );
			exit( EXIT_FAILURE );
#endif
			*error_ref_no = 1; // Unidentified error.
			return;
		}
		
		/* We have now found the next two numbers to merge by 
		 * performing the arithmetic operation: */
		MergeNumbers( parsed_expression, this_index, next_index, op );
	} // outer for
} // EvaluateExpressionForOneOperator

double EvaluateExpression( ParsedExpression parsed_expression, int *error_ref_no )
/* The algorithm is to go along the set of numbers linked by binary operators 
 * repeatedly, merging two adjacent numbers with the operator.
 * E.g. 2+3 becomes <used>+5.
 * Numbers which have already been used are ignored; the fact that they 
 * are used is known by an extra array (also in parsed_expression) called 
 * num_and_op_used.
 * It applies to each number and the following operator (which has the same 
 * array index.)
 * This is done in the order of execution: E, /, x, +, -.
 * Within each of these it is left-to-right.
 */
{
int	i;
	// Init the number_used array to show no numbers have been used.
	for (i=0; i < MAX_NUMS_AND_OPS/*Init all, not just those used.*/; i++) 
		parsed_expression.num_and_op_used[i] = 0;
	
	// Evaluate in order:
	EvaluateExpressionForOneOperator( &parsed_expression, 'E', error_ref_no );
	EvaluateExpressionForOneOperator( &parsed_expression, '/', error_ref_no );
	EvaluateExpressionForOneOperator( &parsed_expression, 'x', error_ref_no );
	EvaluateExpressionForOneOperator( &parsed_expression, '+', error_ref_no );
	EvaluateExpressionForOneOperator( &parsed_expression, '-', error_ref_no );
	
	/* There should now be nothing left except the number in the last 
	 * element of parsed_expression.number, which is the answer.
	 */
	return parsed_expression.number[ parsed_expression.n_numbers-1 ];
} // EvaluateExpression

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
 * For the functional specification, see the documentation of this function 
 * in calculate_answer.h.
 */
double CalculateAnswer( char *input_buffer, int input_buffer_size, int *error_ref_no )
{
double	answer = 0.0;	// Initialise to something safe.
ParsedExpression	parsed_expression;
	
	*error_ref_no = 0; // Presumed innocent until found guilty.

	// Basic syntax checks:
	SyntaxCheckStage1( input_buffer, input_buffer_size, error_ref_no );
	if (*error_ref_no != 0)
		return 0.0; // Even if it won't be used, the result should be defined.
	
	// No operator errors (e.g. two together):
	SyntaxCheckStage2( input_buffer, error_ref_no );
	if (*error_ref_no != 0)
		return 0.0; // Even if it won't be used, the result should be defined.
	
	/* Parse the input string into tokens (representing numbers 
	 * and operators such as +, x): */
	parsed_expression.n_numbers = parsed_expression.n_infix_operators = 0; /* /* CHK: are both needed? */
	IdentifyTokens( input_buffer, error_ref_no, &parsed_expression );
	if (*error_ref_no != 0)
		return 0.0; // Even if it won't be used, the result should be defined.
	
	/* There should not be two E operators following each other 
	 * (e.g. 12.E3E4). This is easier to test once the input has been 
	 * parsed into tokens: */
	SyntaxCheckStage3( parsed_expression, error_ref_no );
	
	// The input string is now known to be valid, so evaluate it:
	answer = EvaluateExpression( parsed_expression, error_ref_no );
	
	return answer;
} // CalculateAnswer

