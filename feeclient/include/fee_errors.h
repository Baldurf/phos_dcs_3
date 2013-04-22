#ifndef FEE_ERRORS_H
#define FEE_ERRORS_H

/**
 * Here are some error codes and condition defines used in the feeserver.
 * There are different areas for every party on this project: <pre>
 * -1		to	-19	belongs to FEE-Communications (ZTT) [starts with FEE_...]
 * -20		to	-39	belongs to TPC [starts with TPC_...]
 * ...
 * -60      to  -79 belongs to InterCom - Layer [starts with INTERCOM_...]
 *
 * RESERVED IS:
 *  -99	for FEE_UNKNOWN_RETVAL
 *    0	for FEE_OK
 *  99		for FEE_MAX_RETVAL
 * <pre>
 *
 * The common used rule is: <pre>
 * value <  0	means error code or failure condition
 * value == 0	means execution was successful
 * value > 0 	means some addtional information is provided
 * </pre>
 *
 * @author Christian Kofler, Sebastian Bablok
 */

/**
 * Unexpected return value encountered
 */
#define FEE_UNKNOWN_RETVAL -99

/**
 * Define for signaling lost link to ACK channel
 */
#define FEE_ACK_LINK_DOWN -11

/**
 * Define for received ACK data is to short for FeePacket.
 */
#define FEE_ACK_CORRUPTED -10

/**
 * Command data corrupted, wrong checksum.
 */
#define FEE_CHECKSUM_FAILED -7

/**
 * Initialisation of CE failed
 */
#define FEE_CE_NOTINIT -6

/**
 * Execution simply failed
 */
//#define FEE_FAILED -5 // ONLY for FeeServer internals

/**
 * Operation was called in wrong state
 */
//#define FEE_WRONG_STATE -4 // ONLY for FeeServer internals

/**
 * One of incoming parameters is invalid
 */
#define FEE_INVALID_PARAM -3

/**
 * Nullpointer exception encountered
 */
#define FEE_NULLPOINTER -2

/**
 * A timeout occured
 */
#define FEE_TIMEOUT -1

/**
 * Execution was successful
 */
#define FEE_OK 0

/**
 * Maximum value of Feeserver return value. (only two figures are allowed)
 */
#define FEE_MAX_RETVAL 99


#endif


