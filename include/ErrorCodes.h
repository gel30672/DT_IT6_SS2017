/*
 * Project ErrorCode Header
 * -------------------------
 * Contains all known Error codes
 * Feel free to add new owns
 * Negative Values = Error Code
 * Positive Values = Information return (for example: written bytes after a successful operation)
 * ! DON'T USE VALUE : 0 !
 * Please add commands to your Codes !
 * Created by Kevin Krämer on 08.05.17.
*/

#ifndef READDATA_ERRORCODES_H
#define READDATA_ERRORCODES_H

/* Positive Response */
#define OK 1 //Everything worked like excepted

/* Negative Response */
#define Err -1 // Something went wrong
#define TcGetAttrErr -2 //Can't get information of terminal
#define ReadErr -3 //Can't Read any data from SerialPort
#define ReadNothingErr -4 // SerialPort has no valid value -> Nothing read

#endif //READDATA_ERRORCODES_H
