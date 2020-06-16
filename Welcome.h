/*! \file Welcome.h
*/

/*! Display the welcome page and access to password mode
 *
 */
void Welcome( void );

/*! Using password to access the calculator and allow user to reset it
 *
 */
void Password( void );

/*! Address in flash where the password is stored
 *
 */
#define PASSWORD_ADDRESS	0x10

/*! Write password to flash memory
 *
 */
void WritePasswordToFlash( int number );

/*! Read password from flash
 *
 */
int ReadPasswordFromFlash( void );

/*! Reset the password
 *
 */
void Reset( void );

/*! Check the password whether valid, 4 numbers only
 *
 */
int CheckValid(char *word);