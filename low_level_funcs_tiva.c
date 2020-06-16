/* low_level_funcs_tiva.c
 * 
 * Set of functions at the bottom level for the 3662 calculator mini-project.
 * These are the hardware drivers.
 * 
 * For documentation, see the documentation in the corresponding .h file.
 * 
 * Dr Chris Trayner, 2019 September
 */

#include "TExaS.h"
#include "PLL.h"
#include "Welcome.h"
#include "low_level_funcs_tiva.h"

// =========================== CONSTANTS ============================

/* ----- What you need to do with these #define constants -----
 * 
 * You will need to define many hardware-specific #define constants 
 * to make your program work with the Tiva. They should all be 
 * defined here.
 * 
 * It is your job to decide which constants you need to define.
 * Any which you do define must have the names given here. This is 
 * the same as asking you to use specified names for functions. 
 * Rather than giving you a list of required names, you are given 
 * the definitions but without the values entered. In fact, they 
 * all have a value entered (0x0) to make sure the module will 
 * compile.
 * 
 * The port allocations (i.e. what bit of what port does what job) 
 * are given in Appendix C of the Mini-project Handout. For your 
 * convenience they are copied into the comment at the start of each 
 * port below.
 * 
 * For Ports A, B, D and E and the clocks, all the constants have 
 * been listed for you. This is simpler than deciding which you 
 * will use in your program. For definitions you don't need, 
 * just leave them as they are (with value 0x0) - it's not worth 
 * deleting them. Unused #define definitions do no harm.
 * 
 * These port and clock constants have been given the standard 
 * names, so there is no explanation of what they mean.
 * There are also some special LCD-related definitions which are 
 * not Tiva standards. These are at the end of these constants 
 * and have comments explaining what they are.
 */

// --------------------------- Ports -------------------------

// Port A (bit 2 is EN, bit 3 is RS):
#define GPIO_PORTA_DATA_R       (*((volatile unsigned long *)0x400043FC))
#define GPIO_PORTA_DIR_R        (*((volatile unsigned long *)0x40004400))
#define GPIO_PORTA_AFSEL_R      (*((volatile unsigned long *)0x40004420))
#define GPIO_PORTA_PUR_R        (*((volatile unsigned long *)0x40004510))
#define GPIO_PORTA_PDR_R        (*((volatile unsigned long *)0x40004514))
#define GPIO_PORTA_DEN_R        (*((volatile unsigned long *)0x4000451C))
#define GPIO_PORTA_LOCK_R       (*((volatile unsigned long *)0x40004520))
#define GPIO_PORTA_CR_R         (*((volatile unsigned long *)0x40004524))
#define GPIO_PORTA_AMSEL_R      (*((volatile unsigned long *)0x40004528))
#define GPIO_PORTA_PCTL_R       (*((volatile unsigned long *)0x4000452C))

// Port B (PORTB[2:5] are LCD DB4 to DB7):
#define GPIO_PORTB_DATA_R       (*((volatile unsigned long *)0x400053FC))
#define GPIO_PORTB_DIR_R        (*((volatile unsigned long *)0x40005400))
#define GPIO_PORTB_AFSEL_R      (*((volatile unsigned long *)0x40005420))
#define GPIO_PORTB_PUR_R        (*((volatile unsigned long *)0x40005510))
#define GPIO_PORTB_PDR_R        (*((volatile unsigned long *)0x40005514))
#define GPIO_PORTB_DEN_R        (*((volatile unsigned long *)0x4000551C))
#define GPIO_PORTB_LOCK_R       (*((volatile unsigned long *)0x40005520))
#define GPIO_PORTB_CR_R         (*((volatile unsigned long *)0x40005524))
#define GPIO_PORTB_AMSEL_R      (*((volatile unsigned long *)0x40005528))
#define GPIO_PORTB_PCTL_R       (*((volatile unsigned long *)0x4000552C))

// Port D (PORTD[0:3] are the outputs to the columns):
#define GPIO_PORTD_DATA_R       (*((volatile unsigned long *)0x400073FC))
#define GPIO_PORTD_DIR_R        (*((volatile unsigned long *)0x40007400))
#define GPIO_PORTD_AFSEL_R      (*((volatile unsigned long *)0x40007420))
#define GPIO_PORTD_PUR_R        (*((volatile unsigned long *)0x40007510))
#define GPIO_PORTD_PDR_R        (*((volatile unsigned long *)0x40007514))
#define GPIO_PORTD_DEN_R        (*((volatile unsigned long *)0x4000751C))
#define GPIO_PORTD_LOCK_R       (*((volatile unsigned long *)0x40007520))
#define GPIO_PORTD_CR_R         (*((volatile unsigned long *)0x40007524))
#define GPIO_PORTD_AMSEL_R      (*((volatile unsigned long *)0x40007528))
#define GPIO_PORTD_PCTL_R       (*((volatile unsigned long *)0x4000752C))

// Port E (PORTE[0:3] are the inputs from the rows):
#define GPIO_PORTE_DATA_R       (*((volatile unsigned long *)0x400243FC))
#define GPIO_PORTE_DIR_R        (*((volatile unsigned long *)0x40024400))
#define GPIO_PORTE_AFSEL_R      (*((volatile unsigned long *)0x40024420))
#define GPIO_PORTE_PUR_R        (*((volatile unsigned long *)0x40024510))
#define GPIO_PORTE_PDR_R        (*((volatile unsigned long *)0x40024514))
#define GPIO_PORTE_DEN_R        (*((volatile unsigned long *)0x4002451C))
#define GPIO_PORTE_LOCK_R       (*((volatile unsigned long *)0x40024520))
#define GPIO_PORTE_CR_R         (*((volatile unsigned long *)0x40024524))
#define GPIO_PORTE_AMSEL_R      (*((volatile unsigned long *)0x40024528))
#define GPIO_PORTE_PCTL_R       (*((volatile unsigned long *)0x4002452C))


// --------------------------- Clocks --------------------------
#if 1
/* You are not asked to define the following because they are defined 
 * in PLL.h, which is #included by TExaS.h, which is #included above.
 */

//PLL related Defines
#define SYSCTL_RIS_R		  (*((volatile unsigned long *)0x400FE050))	
#define SYSCTL_RCC_R		  (*((volatile unsigned long *)0x400FE060))
#define SYSCTL_RCC2_R		  (*((volatile unsigned long *)0x400FE070))	
#define SYSCTL_RCGC1_R		(*((volatile unsigned long *)0x400FE104))
#define SYSCTL_RCGC2_R    (*((volatile unsigned long *)0x400FE108))

//SysTick related Defines	
#define NVIC_ST_CTRL_R        	(*((volatile unsigned long *)0xE000E010))
#define NVIC_ST_RELOAD_R      	(*((volatile unsigned long *)0xE000E014))
#define NVIC_ST_CURRENT_R     	(*((volatile unsigned long *)0xE000E018))
#endif

// ------------------- Special definitions ----------------- -----

/* LCD-related definitions
 * 
 * These are explained in Appendix C of the Mini-project Handout;
 * before you write this code you should re-read that part of 
 * that Appendix. The port allocations are copied into the 
 * comments at the start of each port above.
 * These have all been defined here with the simple value 0 (so the 
 * program will compile), but you should think carefully about 
 * them. For instance, if you need to access bit 6 of a port, 
 * should you define the value to be 6? Or should you write a 
 * definition like (*((volatile unsigned long *) ... )) to make 
 * the port access this bit directly? It is your decision.
 */

#define LCD_RS			(*((volatile unsigned long *)0x40004020)) /* Bit 3, connect to PA 3
			 * The single port bit connected to the 
			 * RS (Register Select) pin of the LCD.
			 */
#define LCD_EN			(*((volatile unsigned long *)0x40004010)) /* Bit 2, connect to PA 2
			 * The single port bit connected to the 
			 * EN (ENable data transfer) pin of the LCD.
			 */
#define LCD_DATA		(*((volatile unsigned long *)0x400053FC)) /* Bit 2 - 5, connect to PB2 - PB5
			 * The set of four adjacent bits connected 
			 * to the four data transfer bits (DB4 to 
			 * DB7) of the LCD. */

// ------------------- Flash memory definitions ----------------------
//Define Flash
#define FMA       (*((volatile unsigned long *)0x400FD000))  // Page 542, Base 0x400F.D000, Offset 0X000, flash memory address
#define FMD       (*((volatile unsigned long *)0x400FD004))  // Page 543, Base 0x400F.D000, Offset 0x004, flash memory data
#define FMC       (*((volatile unsigned long *)0x400FD008))  // Page 544, Base 0x400F.D000, Offset 0x008, flash memory control
#define BOOTCFG   (*((volatile unsigned long *)0x400FE1D0))  // Page 581, Base 0x400F.E000, Offset 0x1D0, boot configuration

/* Incidentlly, a  comment on C-writing technique:
 * You will have noticed that the comments above use to old C 
 * comment form starting with slash-star and ending with 
 * star-slash, rather than the newer slash-slash. This is because 
 * a slash-slash comment ends at the line end. When you edit a 
 * program, it is an easy mistake to copy and paste something in the 
 * middle of a comment. It compiles, but the comment now refers to 
 * the wrong lines of code. But with the old form it probably won't 
 * compile, so you find the problem.
 * This is also why the comments above start on the same line as 
 * the #define, even though there is only room for the opening 
 * slash-star. (At least, there would only be room for that if you 
 * used the (*((volatile unsigned long *) ... )) form.)
 * This is one of the personal tricks I have invented over the years 
 * to stop me making mistaakes.
 */

// =========================== FUNCTIONS ============================

extern void SysTick_Init(void);
extern void PLL_Init(void);
extern void SysTick_Wait(unsigned long delay);

// ------------------------ Keyboard functions ------------------------

void InitKeyboardPorts( void )
{
	volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x00000018;      // port D and E clock
  delay = SYSCTL_RCGC2_R;            // delay
	
	GPIO_PORTD_LOCK_R = 0x4C4F434B;    // unlock PortD
  GPIO_PORTD_CR_R |= 0x0F;           // allow changes to PD3-0
  GPIO_PORTD_AMSEL_R &= ~0x0F;       // disable analog function
	GPIO_PORTD_PCTL_R &= ~0x000FFFF0;  // GPIO clear bit PCTL
  GPIO_PORTD_DIR_R |= 0x0F;          // set PD3-0 as output
  GPIO_PORTD_AFSEL_R = ~0x0F;        // no alternate function
  GPIO_PORTD_DEN_R |= 0x0F;          // enable digital pins PD3-0
	GPIO_PORTD_PUR_R = 0x00;           // Disable pullup resistors on PD0-3
	
  GPIO_PORTE_LOCK_R = 0x4C4F434B;    // unlock PortE
  GPIO_PORTE_CR_R |= 0x0F;           // allow changes to PE3-0
  GPIO_PORTE_AMSEL_R &= ~0x0F;       // disable analog function
  GPIO_PORTE_PCTL_R &= ~0x000FFFF0;  // GPIO clear bit PCTL
  GPIO_PORTE_DIR_R &= ~0x0F;         // set PE3-0 are inputs
  GPIO_PORTE_AFSEL_R &= ~0x0F;       // no alternate function
	GPIO_PORTE_PUR_R = 0x00;           // Disable pullup resistors on PE0-3
	GPIO_PORTE_PDR_R = 0x0F;           // enable pull-down resistors on PE3-0
  GPIO_PORTE_DEN_R |= 0x0F;          // enable digital pins PE3-0
} // InitKeyboardPorts, the keyboard uses port D and port E.

void WriteKeyboardCol( unsigned char nibble )
{
	GPIO_PORTD_DATA_R = nibble;
	
	unsigned char check;
	check = GPIO_PORTD_DATA_R;
	if (check != nibble) {  // Check whether exact one bit has been set
		do{
			GPIO_PORTD_DATA_R = nibble;
			check = GPIO_PORTD_DATA_R;
		}while(check != nibble);  // Set nibble to Port D until success
	}
} // WriteKeyboardCol

unsigned char ReadKeyboardRow( void )
{
	unsigned char Read = 0;
	Read = GPIO_PORTE_DATA_R & 0x0F;  // Get the input from port E
	return Read;
} // ReadKeyboardRow

// ------------------------ Display functions ------------------------

void SendDisplayNibble( unsigned char byte, unsigned char instruction_or_data )
{
	if (instruction_or_data == 0) {
		LCD_RS = 0x00;  // Set register select be 0	
	  LCD_DATA = (byte & 0x0F) << 2;  // Send four nibbles to data bus line
	  LCD_EN_Pulse();
	  WaitMicrosec(50);  // Wait 50us
	} else if (instruction_or_data == 1) {
		LCD_RS = 0x08;  // Set register select be 1	
	  LCD_DATA = (byte & 0x0F) << 2;  // Send four nibbles to data bus line
	  LCD_EN_Pulse();
	  WaitMicrosec(50);  // Wait 50us
	}
} // SendDisplayInstruction

void SendDisplayByte( unsigned char byte, unsigned char instruction_or_data )
{
	char nibble_1 = byte & 0x0F;
	char nibble_2 = ((byte & 0xF0) >> 4);
	SendDisplayNibble(nibble_2, instruction_or_data);
	WaitMicrosec(200);  // Wait 200 us
	SendDisplayNibble(nibble_1, instruction_or_data);
	WaitMicrosec(200);  // Wait 200 us
} // SendDisplayInstruction

void InitDisplayPort( void )
{
	volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x00000001;      // port A and B clock
  delay = SYSCTL_RCGC2_R;            // delay  
  GPIO_PORTA_LOCK_R = 0x4C4F434B;    // unlock PortA	
  GPIO_PORTA_CR_R |= 0x0C;           // allow changes to PA3-2
  GPIO_PORTA_AMSEL_R &= 0x00;        // disable analog function
  GPIO_PORTA_PCTL_R &= 0x00000000;   // GPIO clear bit PCTL
  GPIO_PORTA_DIR_R |= 0x0C;          // set PA3-2 are outputs
  GPIO_PORTA_AFSEL_R &= 0x00;        // no alternate function
  GPIO_PORTA_DEN_R |= 0x0C;          // enable digital pins PA3-2
	
	volatile unsigned long delay2;
	SYSCTL_RCGC2_R |= 0x00000002; 
	delay2 = SYSCTL_RCGC2_R;
	GPIO_PORTB_LOCK_R = 0x4C4F434B;    // unlock PortB
  GPIO_PORTB_CR_R |= 0x3C;           // allow changes to PB5-2
  GPIO_PORTB_AMSEL_R &= 0x00;        // disable analog function
  GPIO_PORTB_PCTL_R &= 0x00000000;   // GPIO clear bit PCTL
  GPIO_PORTB_DIR_R |= 0x3C;          // set PB5-2 as output
  GPIO_PORTB_AFSEL_R &= 0x00;        // no alternate function
  GPIO_PORTB_DEN_R |= 0x3C;          // enable digital pins PB5-2
} // InitDisplayPort, the LCD uses port A and port B

void ClearDisplay()
{
	SendDisplayByte(0x01, 0); // Display clear
  /* Refers to the reference
	   RS R/W  DB7  DB6  DB5  DB4  DB3  DB2  DB1  DB0
	   0    0   0    0    0    0    0    0    0    1
	           bit7 bit6 bit5 bit4 bit3 bit2 bit1 bit0
	*/
	
	SendDisplayByte(0x02, 0); // Return home
  /* Refers to the reference
	   RS R/W  DB7  DB6  DB5  DB4  DB3  DB2  DB1  DB0
	   0    0   0    0    0    0    0    0    1    0
	           bit7 bit6 bit5 bit4 bit3 bit2 bit1 bit0
	*/
	
	WaitMicrosec(20000);  // Wait 20 ms
} // ClearDisplay

void TurnCursorOnOff( short int On )
{
	if (On != 0) {
	SendDisplayByte(0x0F, 0); // Sets entire display on, cursor on and blinking of cursor position character
  /* Refers to the reference
	   RS R/W  DB7  DB6  DB5  DB4  DB3  DB2  DB1  DB0
	   0    0   0    0    0    0    1    1    1    1
	           bit7 bit6 bit5 bit4 bit3 bit2 bit1 bit0
	*/
	}
	if (On == 0) {
	SendDisplayByte(0x0C, 0); // Sets entire display on, cursor off and blinking of cursor position character off
  /* Refers to the reference
	   RS R/W  DB7  DB6  DB5  DB4  DB3  DB2  DB1  DB0
	   0    0   0    0    0    0    1    1    0    0
	           bit7 bit6 bit5 bit4 bit3 bit2 bit1 bit0
	*/
	}
} // TurnCursorOnOff

void SetPrintPosition( short int line, short int char_pos )
{
	unsigned char pos;
	unsigned char address;
	
	if((line < 1) || (line > 2)){  // If the row is out of range, set position to first row
		line = 1;
	}
	 if((char_pos < 1) || (char_pos > 16)) {  // If the column is out of range, set position to first column
		 char_pos = 1;
	}
	
	if (line == 1) {
		pos = 0x00 + char_pos - 1; // The first line starts at 0x00
		address = 0x80 + pos;  // DB 7 should be 1 to set DDRAM
		SendDisplayByte(address, 0);
	} else if (line == 2) {
		pos = 0x40 + char_pos - 1; // The second line starts at 0x40
		address = 0x80 + pos;
		SendDisplayByte(address, 0);
	}
} // SetPrintPosition

void PrintChar( char ch )
{
	SendDisplayByte(ch, 1);
} // PrintChar


// ------------------------ Flash memory functions ------------------------

void InitFlash()
{
	FMA |= 0x0000;  // Reset flash memory address
	Done_Check(1);
	FMD |= 0x0000;  // Reset flash memory data
	Done_Check(1);
	FMC |= 0x0000;  // Reset flash memory control
	Done_Check(1);
	BOOTCFG |= 0x0010;  // 0xA442 is used as the WRKEY in the FMC register
	Done_Check(1);
} // InitFlash

void WriteDoubleToFlash( double number )
{
	FMD = number;  // Write the number to the FMD
	Done_Check(3);
	FMA = ANSWER_FLASH_ADDRESS;  // Choose the flash memory address to store to number
	FMC |= 0xA4420001;  // Write the value into WRKEY and WRITE field to process the write operation
	Done_Check(1);
} // WriteFloatToFlash

double ReadDoubleFromFlash()
{
	double read;
	FMA |= ANSWER_FLASH_ADDRESS;  // Choose the flash memory adress
	Done_Check(1);
	read = FMD;
	return read;       // Return the number
} // ReadFloatFromFlash

// ------------------------ Sundry functions ------------------------

void InitAllOther()
{
	PLL_Init();	
	SysTick_Init();
	InitLCD();
	InitFlash();
} // InitAllOther

void InitAllHardware()
{
	InitKeyboardPorts();  // Initial keyborad ports.
	InitDisplayPort();  // Initial LCD ports.
	InitAllOther();
	Welcome();
} // InitAllHardware

void WaitMicrosec( long int wait_microsecs )
{
	for (int i = 0; i < wait_microsecs; i++) {
	  SysTick_Wait(80);
	}

} // WaitMicrosec

void LCD_EN_Pulse()
{
  LCD_EN = 0x04;  // Set EN to 1
	WaitMicrosec(1); // Wait 1 us
	LCD_EN = 0x00;  // set EN to 0
}

void Done_Check( int number )
{
	int check = 1;
	unsigned long read;
	while (check == 1) {
	  if (number == 1) {
		  read = FMC & 0x08;
	  } else if (number == 2) {
	  	read = FMC & 0x02;
  	} else if (number == 3) {
	  	read = FMC & 0x01;
  	}
	
  	if (read == 0x00) {
	  	check =0;
		}	else {
			WaitMicrosec(10); // Wait 10	 us to continue the uncomplete process
		}
	}
}  // Check whether the previous process is done

void LCDFlash( void )
{
  SendDisplayByte(0x08, 0); // Sets entire display off to make flash
  /* Refers to the reference
	   RS R/W  DB7  DB6  DB5  DB4  DB3  DB2  DB1  DB0
	   0    0   0    0    0    0    1    0    0    0
	           bit7 bit6 bit5 bit4 bit3 bit2 bit1 bit0
	*/
	WaitMicrosec(100000);  // Wait 0.5 sec
	SendDisplayByte(0x0F, 0); 
	WaitMicrosec(100000);  // Wait 0.5 sec
	SendDisplayByte(0x08, 0); 
	WaitMicrosec(100000);  // Wait 0.5 sec
	SendDisplayByte(0x0F, 0); 
	WaitMicrosec(100000);  // Wait 0.5 sec
	SendDisplayByte(0x0F, 0); 
}

int HexToDeci( char Hex )
{
	int number;
	if (Hex == 0x01) {         //  bit 0
			number = 0;
		} else if (Hex == 0x02) {  //  bit 1
			number = 1;
		} else if (Hex == 0x04) {  //  bit 2
			number = 2;
		} else if (Hex == 0x08) {  //  bit 3
			number = 3;
		}
	return number;
}

void InitLCD()
{
	LCD_EN =0x00;
	WaitMicrosec(60000);  // Wait 60 ms for voltage rising
	SendDisplayNibble(0x0C, 0); // Send 0x3 to DB, function set
	/* Refers to the reference
	   RS R/W  DB7  DB6  DB5  DB4
	   0    0   0    0    1    1
	           bit3 bit2 bit1 bit0
	*/
	
	WaitMicrosec(50000);  // Wait 50 ms 
	SendDisplayNibble(0x0C, 0); // Send 0x3 to DB, function set
	WaitMicrosec(1000);  // Wait 1 ms
	SendDisplayNibble(0x0C, 0); // Send 0x3 to DB, function set
	WaitMicrosec(37);  // Wait 37 us
	SendDisplayNibble(0x08, 0); // Seng 0x2 to DB to set interface to be 4 bits long
	/* Refers to the reference
	   RS R/W  DB7  DB6  DB5  DB4
	   0    0   0    0    1    0
	           bit3 bit2 bit1 bit0
	*/
	
	WaitMicrosec(37);  // Wait 37 us
	SendDisplayByte(0x28, 0); // 4-bits data, display in 2 lines, 5x8 dots
  /* Refers to the reference
	   RS R/W  DB7  DB6  DB5  DB4  DB3  DB2  DB1  DB0
	   0    0   0    0    1    0    1    0    0    0
	           bit7 bit6 bit5 bit4 bit3 bit2 bit1 bit0
	*/
	
	WaitMicrosec(37);  // Wait 37 us
	SendDisplayByte(0x0F, 0); // Display off
  /* Refers to the reference
	   RS R/W  DB7  DB6  DB5  DB4  DB3  DB2  DB1  DB0
	   0    0   0    0    0    0    1    0    0    0
	           bit7 bit6 bit5 bit4 bit3 bit2 bit1 bit0
	*/
	
	WaitMicrosec(37);  // Wait 37 us
	SendDisplayByte(0x01, 0); // Display clear
  /* Refers to the reference
	   RS R/W  DB7  DB6  DB5  DB4  DB3  DB2  DB1  DB0
	   0    0   0    0    0    0    0    0    0    1
	           bit7 bit6 bit5 bit4 bit3 bit2 bit1 bit0
	*/
	
	WaitMicrosec(37);  // Wait 37 us
	SendDisplayByte(0x06, 0); // Entry mode set
  /* Refers to the reference
	   RS R/W  DB7  DB6  DB5  DB4  DB3  DB2  DB1  DB0
	   0    0   0    0    0    0    0    1    1    0
	           bit7 bit6 bit5 bit4 bit3 bit2 bit1 bit0
	*/
	
	WaitMicrosec(37);  // Wait 37 us
} // Initialize the LCD, reference figure 24 on page 46 of HD44780.pdf
