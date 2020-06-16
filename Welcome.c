#include "Welcome.h"
#include "TExaS.h"
#include "low_level_funcs_tiva.h"
#include "mid_level_funcs.h"
#include "high_level_funcs.h"

// ------------------- Flash memory definitions ----------------------
//Define Flash
#define FMA       (*((volatile unsigned long *)0x400FD000))  // Page 542, Base 0x400F.D000, Offset 0X000, flash memory address
#define FMD       (*((volatile unsigned long *)0x400FD004))  // Page 543, Base 0x400F.D000, Offset 0x004, flash memory data
#define FMC       (*((volatile unsigned long *)0x400FD008))  // Page 544, Base 0x400F.D000, Offset 0x008, flash memory control
#define BOOTCFG   (*((volatile unsigned long *)0x400FE1D0))  // Page 581, Base 0x400F.E000, Offset 0x1D0, boot configuration
#define INPUT_SIZE	17

void Welcome()
{
	TurnCursorOnOff(0);
	int menu = 1;
  PrintString( 1, 1, ">>>Welcome To<<<");
	PrintString( 2, 1, ">>>Calculator<<<");
	WaitMicrosec(3000000);  // Wait 3 s
	ClearDisplay();
	
	int password = 6666;  // Set default password
	WritePasswordToFlash(password);
	Password();

	while(menu == 1) {
		PrintString( 1, 1, "1->Calculator");
		PrintString( 2, 1, "2->Reset PW");	
		char key = GetKeyboardChar();	
		
		if(key == '1') {
			menu = 0;
			ClearDisplay();
		} else if (key == '2') {
			Reset();
		}
	}
}

void Password()
{
	int pass = 1;
	char input[INPUT_SIZE];
	int word = ReadPasswordFromFlash();
	char password[5];
	
	sprintf( password, "%i", word );
	TurnCursorOnOff(0);
	PrintString( 1, 1, "Enter Password,");
	PrintString( 2, 1, " Use * to end. ");
	
	while(pass == 1) {
		int check = 0;
		for(int i = INPUT_SIZE; i >= 0;  i--) {  // Clean the input
        input[i] = '\0';
		}

		ReadAndEchoInput(input, INPUT_SIZE);
		check = CheckValid(input);
		if (check == 1) {
/*--------------------------------------------------------------------------------------- */	
			if (strncmp(input, password) == 0) {
				pass = 0;
				ClearDisplay();
			} else { 
				ClearDisplay();
				TurnCursorOnOff(0);
				PrintString( 1, 1, "Wrong Password,");
				PrintString( 2, 1, "  Input again  ");
		  }
/*--------------------------------------------------------------------------------------- */	
	  } else { 
			ClearDisplay();
			TurnCursorOnOff(0);
			PrintString( 1, 1, "Wrong Password,");
			PrintString( 2, 1, "  Input again  ");
		}
	}
	WaitMicrosec(10);  // Wait 10 us
}

void Reset()
{
	int success = 0;
	int valid = 0;
	ClearDisplay();
	PrintString( 1, 1, "Enter new one");
	PrintString( 2, 1, " Use * to end.");
	
	while(success == 0) {
		int buffer = 0;
		char reset[INPUT_SIZE];
		ReadAndEchoInput(reset, INPUT_SIZE);
		valid = CheckValid(reset);
		
		if (valid == 1) {
			success = 1;
			buffer = atoi(reset);
			WriteDoubleToFlash(buffer);
			WaitMicrosec(200);
		} else if (valid == 2) {
			ClearDisplay();
			PrintString( 1, 1, "  Numbers Only ");
			PrintString( 2, 1, "  Input again  ");
			WaitMicrosec(20);
		} else if (valid == 3) {
			ClearDisplay();
			PrintString( 1, 1, " 4 Numbers Only");
			PrintString( 2, 1, "  Input again  ");
			WaitMicrosec(20);
		}
	}
}

int CheckValid(char *word)
{
	int result;
	int count;
	int num;
	int digit = 1;
	
	for (count = 0; word[count] != '\0'; ++count)
	for (int i = 0; i <= count; i++) {
	  num = isdigit(word[i]);
		if (num == 0) {
			digit = 0;
			break;
		}
	}
	
	if (digit == 0) {
		result = 2;
	} else if (count != 4) {
		result = 3;
	} else {
		result = 1;
	}
	return result;
}

void WritePasswordToFlash( int number )
{
	FMD = number;  // Write the number to the FMD
	Done_Check(3);
	FMA = PASSWORD_ADDRESS;  // Choose the flash memory address to store to number
	FMC |= 0xA4420001;  // Write the value into WRKEY and WRITE field to process the write operation
	Done_Check(1);
} // WriteFloatToFlash

int ReadPasswordFromFlash()
{
	int read;
	FMA |= PASSWORD_ADDRESS;  // Choose the flash memory adress
	Done_Check(1);
	read = FMD;
	return read;       // Return the number
} // ReadFloatFromFlash