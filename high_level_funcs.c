/* high_level_funcs.c
 *
 * Set of functions at high level (above mid level but below main)  
 * for the 3662 calculator mini-project.
 * 
 * For documentation, see the corresponding .h file.
 * 
 * Dr Chris Trayner, 2019 September
 */

#include "TExaS.h"
#include "high_level_funcs.h"
#include "mid_level_funcs.h"
#include "low_level_funcs_tiva.h"


// ------------------------ Keyboard functions ---------------------

void ReadAndEchoInput( char *input_buffer, int input_buffer_size )
{
	char key;
	int shift = 0;  // If SHIFT is pressed, it changes to 1
	int input = 1;  // If * is pressed, it changes to 0
	int keypressed_1 = 0;  // If any key be pressed, it changes to 0
	
	for(int i = input_buffer_size; i >= 0;  i--) {  // Clean the input_buffer
        input_buffer[i] = '\0';
			}
	input_buffer_size = 0;  // The input_buffer_size of input_buffer
/*--------------------------------------------------------------------------------------- */	
	while (keypressed_1 == 0) {
	  char read = 'n';
	  read = GetKeyboardChar();
		
	  if ((read != 'n') && (read != '*')) { // If the user typed equals immediately, leave the previous answer to be displayed
		  keypressed_1 = 1;
			ClearDisplay();
			TurnCursorOnOff(1);
		}
		WaitMicrosec(1);  // Wait 1 us
	}  // Wait until one key is pressed and clear the LCD
/*--------------------------------------------------------------------------------------- */
	while (input == 1) {  // User now can input keys into input_buffer until * be pressed
		int keypressed_2 = 0;  // If any key be pressed, it changes to 1
		while (keypressed_2 == 0) {
		  char read = 'n';
		  read = GetKeyboardChar();
		  if (read != 'n') {
			  keypressed_2 = 1;
				key = read;
			}
			WaitMicrosec(1);  // Wait 1 us
		}  // Wait until one key is pressed
	  
		if (key == 'D' && shift == 0) {  // If shift is not actived, active it
			shift = 1;
/*--------------------------------------------------------------------------------------- */
		} else if (key == 'D' && shift == 1) {  // If shift is actived, cancel shift
		  shift = 0;
/*--------------------------------------------------------------------------------------- */			
		} else if (key == '*') {  // If * has be pressed, end the input
			  input = 0;
			  WaitMicrosec(1);
			  TurnCursorOnOff(0);
/*--------------------------------------------------------------------------------------- */			
		} else if (key == 'A' && shift == 0) {  // Get + character
		  if (input_buffer_size < 15) {
		    input_buffer [input_buffer_size] = '+';
			  input_buffer_size += 1;
			} else {
				LCDFlash();
			}
/*--------------------------------------------------------------------------------------- */		
	  } else if (key == 'A' && shift == 1) {  // Get x character
		  if (input_buffer_size < 16) {
			  input_buffer [input_buffer_size] = 'x';
			  input_buffer_size += 1;
			} else {
				LCDFlash();
			}
/*--------------------------------------------------------------------------------------- */		
	  } else if (key == 'B' && shift == 0) {  // Get - character
		  if (input_buffer_size < 16) {
				input_buffer [input_buffer_size] = '-';
			  input_buffer_size += 1;
			} else {
				LCDFlash();
			}
/*--------------------------------------------------------------------------------------- */		
		} else if (key == 'B' && shift == 1) {  // Get / character
			if (input_buffer_size < 16) {
				input_buffer [input_buffer_size] = '/';
				input_buffer_size += 1;
			} else {
				LCDFlash();
			}
/*--------------------------------------------------------------------------------------- */		
		} else if (key == 'C' && shift == 0) {  // Get . character
			if (input_buffer_size < 16) {
				input_buffer [input_buffer_size] = '.';
				input_buffer_size += 1;
			} else {
				LCDFlash();
			}
/*--------------------------------------------------------------------------------------- */		
		} else if (key == 'C' && shift == 1) {  // Get E character
			if (input_buffer_size < 16) {
			  input_buffer [input_buffer_size] = 'E';
				input_buffer_size += 1;
			} else {
				LCDFlash();
			}
/*--------------------------------------------------------------------------------------- */		
		} else if (key == '#' && shift == 0) {  // Get rubout signal
			if (input_buffer_size > 0) {
				input_buffer [input_buffer_size-1] = '\0' ;
				input_buffer_size -= 1;
			} 
/*--------------------------------------------------------------------------------------- */
		} else if (key == '#' && shift == 1) {  // Delete entire entry		 	
		  for(int i = input_buffer_size; i >= 0;  i--) {
        input_buffer[i] = '\0';
			}
			input_buffer_size=0;
/*--------------------------------------------------------------------------------------- */		
		} else {  // Get the normal number (0-9)
		  if (input_buffer_size < 16) {
				input_buffer [input_buffer_size] = key ;
				input_buffer_size += 1;
			} else {
				LCDFlash();
			}
		}
/*--------------------------------------------------------------------------------------- */			
		ClearDisplay();
		PrintString( 1, 1, input_buffer);  // After input one character, print current buffer
		WaitMicrosec(200000); 
	}  // End the input
} // ReadAndEchoInput

// ------------------------ Display functions ------------------------

void DisplayResult( double answer )
{
	char result[20];
	SetPrintPosition(2,1);
	sprintf(result, "%g", answer);  // Using shortest respresention
	PrintString( 2, 1, result);
} // DisplayResult

void DisplayErrorMessage( const char *error_message_line1, 
			  const char *error_message_line2 )
{
	ClearDisplay();
	/*
	int length1 = 0;
	int length2 = 0;
  const char *warning1 = "Line1 too long";
	const char *warning2 = "Line2 too long";
	
	while(*error_message_line1) {  // Check error message 1 length
		error_message_line1++;
		length1++;
	}
	for (int i = 0; i < length1; i++) {  // Renew the error_message_line 1
		error_message_line1--;
	}
	
	while(*error_message_line2) {  // Check error message 2 length
		error_message_line2++;
		length2++;
	}
	for (int i = 0; i < length1; i++) {  // Renew the error_message_line 2
		error_message_line1--;
	}

	if (length1 > 16 && length2 < 17) {  // Line 1 is too long but line 2 is good
		PrintString( 1, 1, warning1);
	  PrintString( 2, 1, error_message_line2);
	} else if (length1 < 17 && length2 > 16) {  // Line 2 is too long but line 1 is good
		PrintString( 1, 1, error_message_line1);
	  PrintString( 2, 1, warning2);
	} else if (length1 > 16 && length2 > 16) {  // Both two line exceed limit
		PrintString( 1, 1, warning1);
	  PrintString( 2, 1, warning2);
	} else if (length1 < 17 && length2 < 17) {  // Perfect, print two strings
		PrintString( 1, 1, error_message_line1);
	  PrintString( 2, 1, error_message_line2);
	} 
	*/
	PrintString( 1, 1, error_message_line1);
	PrintString( 2, 1, error_message_line2);
} // DisplayErrorMessage


