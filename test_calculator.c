/* test_calculator.c
 * 
 * Dr Chris Trayer, Novembeer 2019
 */

#define PROG_NAME_VER		"test_calculator v1.1 (2019nov05)"
#define INPUT_BUFFER_SIZE	17
#define AUTO_TEST_RESULTS_LEVEL	3	/* 0: Nothing (silly)
					 * 1: Just totals of tests and successes
					 * 2: Nature and result of each test
					 */
#define AUTO_TEST_ERROR_MARGIN	1.00001	/* To pass, the result must be no different 
					 * (as a ratio) than this from the correct value. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "calculate_answer.h"


void ReadAndEchoInput( char *input_buffer, int input_buffer_size )
{
	printf( "\nEnter keyboard input:\t" );
	fgets( input_buffer, input_buffer_size, stdin );
	input_buffer[ strlen(input_buffer) - 1 ] = '\0'; // Remove <LF> at end.
	printf( "\tRead:\t\t\"%s\", %d chars long\n", 
		input_buffer, (int)strlen( input_buffer ) );
} // ReadAndEchoInput

void DisplayResult( double answer )
{
	printf( "\tAnswer:\t\t%g\n", answer );
} // DisplayResult

void DisplayErrorMessage( int error_ref_no, 
			  const char* error_message_line1, 
			  const char *error_message_line2 )
{
	if (error_ref_no < 0 || error_ref_no >= MAX_ERROR_MESSAGES) {
		printf( "\tINVALID error message no. %d\n", error_ref_no );
		return;
	}
	printf( "\tError message:\t\"%s\"\n", error_message_line1 );
	printf( "\t[No. %2d]\t\t\"%s\"\n", error_ref_no, error_message_line2 );
} // DisplayErrorMessage

void ManualTest( void )
{
	puts( "End with <enter>, not =" );
	puts( "When finished, type ^C to quit.\n" );
	
	while (1) {
	char	input_buffer [INPUT_BUFFER_SIZE];
	int	error_ref_no = 0;	/* Init in case just = is entered 
					 * as the first entry. */
	double	answer = 0.0;
		
		ReadAndEchoInput( input_buffer, INPUT_BUFFER_SIZE ); 

		answer = CalculateAnswer( input_buffer, 
				INPUT_BUFFER_SIZE,  &error_ref_no ); /* In 
							calculate_answer. */
		if (error_ref_no == 0) // Meaning no error.
			DisplayResult( answer ); 
		else	DisplayErrorMessage( error_ref_no, 
				error_message_line1[error_ref_no], 
				error_message_line2[error_ref_no] );
	} // while	
} // ManualTest

void AutomaticTest_Correct_One( const char *input, double correct_ans, 
				int *n_tested, int *n_passed )
{
char	spaces[50] = "                        "; // Used to tab.
double	actual_ans;
int	error_ref_no, passed;
	
	// Parameter correctness check:
	if (strnlen( input, 17 ) >= 17) {
		printf( "\tERROR: input \"%s\" is too long (%d chars)\n", 
			input, (int)strnlen( input, 17 ) );
		return;
	}

	actual_ans = CalculateAnswer( (char*)input, 
				INPUT_BUFFER_SIZE,  &error_ref_no ); /* In 
							calculate_answer. */

	if (error_ref_no != 0) {
		printf( "\tBAD TEST: \"%s\" yields error_ref_no %d\n", 
			input, error_ref_no );
		return;
	}
	
	passed = (  (actual_ans/correct_ans <= AUTO_TEST_ERROR_MARGIN) // Not too large.
		 && (correct_ans/actual_ans <= AUTO_TEST_ERROR_MARGIN) // Not too small.
		 );
	
	(*n_tested) ++;
	if (passed) (*n_passed) ++;
	
	if (AUTO_TEST_RESULTS_LEVEL >= 1)
		printf( "\t\"%s\"%sshould give %g,\tgave %g,\t%s\n", 
			input, &(spaces[ strlen( input ) ]), 
			correct_ans, actual_ans, 
	  		passed ? "passed" : "failed" );
} // AutomaticTest_Correct_One

void AutomaticTest_Correct( void )
{
int	n_tested = 0, n_passed = 0;

	if (AUTO_TEST_RESULTS_LEVEL >= 1)
		printf( "Error margin = %g\n", AUTO_TEST_ERROR_MARGIN );

	// Numbers:
	AutomaticTest_Correct_One( "123.45",		123.45,	&n_tested, &n_passed );
	AutomaticTest_Correct_One( "-456",		-456,	&n_tested, &n_passed );
	AutomaticTest_Correct_One( ".2",		0.2,	&n_tested, &n_passed );
	AutomaticTest_Correct_One( "12.3E3",		12300,	&n_tested, &n_passed );
	AutomaticTest_Correct_One( "23.4E-2",		0.234,	&n_tested, &n_passed );
	// Arithmetic:
	AutomaticTest_Correct_One( "1+2-4",		-1,	&n_tested, &n_passed );
	AutomaticTest_Correct_One( "1+2x10",		21,	&n_tested, &n_passed );
	AutomaticTest_Correct_One( "1.2E3/3x1.25+9-4",	505,	&n_tested, &n_passed );
	AutomaticTest_Correct_One( "0-4+9x6/4E-2",	-1354,	&n_tested, &n_passed );

/*
	AutomaticTest_Correct_One( "",		,	&n_tested, &n_passed );
*/	
	if (AUTO_TEST_RESULTS_LEVEL >= 1)
		printf( "Total of %d tests passed out of %d conducted.\n", 
			n_passed, n_tested );
} // AutomaticTest_Correct

int main( int argc, char* argv[] )
{
int	args_ok = 1;
				 
	printf( "\n%s\n", PROG_NAME_VER );
	puts( "Testing the CalculateAnswer() function." );
	if (argc != 2) 
		ManualTest();
/*		args_ok = 0;*/
	else	switch (argv[1][0]) {
			case 'm':	argv[1][0] = 'M'; // Render UC and carry on.
			case 'M':	ManualTest();
					break;
			case 'a':	argv[1][0] = 'A'; // Render UC and carry on.
			case 'A':	AutomaticTest_Correct();
					//AutomaticTest_Error();
					break;
			default:	args_ok = 0;
		} // switch
	if (! args_ok) {
		puts( "FATAL: usage is test_calculator [<mode>]" );
		puts( "where <mode> is\tA or a for Automatic (set of pre-chosen tests)" );
		puts( "or\t\tM or m for Manual (enter your own input string)\n" );
		puts( "If <mode> is absent, a manual test will be performed." );
		exit( EXIT_FAILURE );
	}
} // main
