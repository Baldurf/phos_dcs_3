#ifndef FEEPACKET_FLAGS_H
#define FEEPACKET_FLAGS_H

/**
 * Bitset for the huffman flag
 */
#define HUFFMAN_FLAG 0x0001	// dec 1

/**
 * Bitset for the checksum flag
 */
#define CHECKSUM_FLAG 0x0002		// dec 2

/**
 * Bitset to signal command for FeeServer - update FeeServer
 */
#define FEESERVER_UPDATE_FLAG 0x0004	// dec 4

/**
 * Bitset to signal command for FeeServer - restarts the FeeServer
 */
#define FEESERVER_RESTART_FLAG 0x0008		// dec 8

/**
 * Bitset to signal command for FeeServer - reboots the DCS board
 */
#define FEESERVER_REBOOT_FLAG 0x0010	// dec 16

/**
 * Bitset to signal command for FeeServer - shuts down the DCS board
 */
#define FEESERVER_SHUTDOWN_FLAG 0x0020	// dec 32

/**
 * Bitset to signal command for FeeServer - quits the FeeServer
 */
#define FEESERVER_EXIT_FLAG 0x0040		// dec 64

/**
 * Bitset to signal command for FeeServer - sets the deadband for specified item
 */
#define FEESERVER_SET_DEADBAND_FLAG 0x0080	// dec 128

/**
 * Bitset to signal command for FeeServer - gives back the deadband
 */
#define FEESERVER_GET_DEADBAND_FLAG 0x0100		// dec 256

/**
 * Bitset to signal command for FeeServer - set the timeout for the issue call
 */
#define FEESERVER_SET_ISSUE_TIMEOUT_FLAG 0x0200		// dec 512

/**
 * Bitset to signal command for FeeServer - gives back the issue timeout
 */
#define FEESERVER_GET_ISSUE_TIMEOUT_FLAG 0x0400		// dec 1024

/**
 * Bitset to signal command for FeeServer - sets the update rate for monitoring
 */
#define FEESERVER_SET_UPDATERATE_FLAG 0x0800		// dec 2048

/**
 * Bitset to signal command for FeeServer - gives back the update rate for monitoring
 */
#define FEESERVER_GET_UPDATERATE_FLAG 0x1000		// dec 4096

/**
 * Bitset to signal command for FeeServer - set the loglevel for the FeeServer
 */
#define FEESERVER_SET_LOGLEVEL_FLAG 0x2000		// dec 8192

/**
 * Bitset to signal command for FeeServer - gives back the current FeeServer configuration
 */
#define FEESERVER_GET_CONFIGURATION_FLAG 0x4000		// dec 16384

/**
 * Bitset for no flags set
 */
#define NO_FLAGS 0x0000

/**
 * Zero value for no checksum
 */
#define CHECKSUM_ZERO 0x0000


#endif
