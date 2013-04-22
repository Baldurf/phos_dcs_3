// $Id: rcu_issue.h,v 1.21 2006/05/15 08:46:59 richter Exp $

/************************************************************************
**
**
** This file is property of and copyright by the Experimental Nuclear 
** Physics Group, Dep. of Physics and Technology
** University of Bergen, Norway, 2004
** This file has been written by Matthias Richter,
** Please report bugs to Matthias.Richter@ift.uib.no
**
** Permission to use, copy, modify and distribute this software and its  
** documentation strictly for non-commercial purposes is hereby granted  
** without fee, provided that the above copyright notice appears in all  
** copies and that both the copyright notice and this permission notice  
** appear in the supporting documentation. The authors make no claims    
** about the suitability of this software for any purpose. It is         
** provided "as is" without express or implied warranty.                 
**
*************************************************************************/
#ifndef __RCU_ISSUE_H
#define __RCU_ISSUE_H

#include <linux/types.h>   // for the __u32 type

#ifdef __cplusplus
extern "C" {
#endif
/***************************************************************************/

/**
 * @defgroup rcu_issue Command interpretation for the RCU FeeServer
 * This group describes the command interface to the RCU-like FeeServer ControlEngine.
 *
 * The <i>issue</i> method of the CE API expects the input either as encoded in 
 * message buffer format or as a ControlEngine command.
 * The FEE API supports currently one DIM command channel of type char array. The RCU CE
 * treats the first 4 byte as a 32bit header word.
 * <b>Note:</b> The architecture of the ARM linux is <b>little endian</b>, which means that
 * the least significant byte comes first. 
 * All feeserver commands start with <b>0xf</b> in there most significant bits, anything else
 * will be treated as a BLOB encoded in the message buffer format. 
 * @see dcscMsgBufferInterface/dcscMsgBufferInterface.h for detailed 
 * description of the message buffer interface to the RCU motherboard.
 * FEESERVER_CMD is predefined as 0xf0000000 (dcscMsgBufferInterface/dcscMsgBufferInterface.h)
 *
 * @author Matthias Richter
 * @ingroup feesrv_ce
 */

/**
 * @name Layout of FeeServer command
 * The header of a FeeServer command has the following format: <pre>
 * Bit 32 - 28 | 27 - 24 | 23 - 16 | 15 -0 
 *     1 1 1 1   cmd id    sub id    parameter 
 *                                      ^------- 16 bit user parameter passed to the handler function 
 *                            ^-----------------  8 bit command sub id passed to the handler function 
 *                   ^--------------------------  4 bit command id switches to the handler function 
 *        ^-------------------------------------  4 bit code for FeeServer command 
 * </pre>
 *
 * A data block is following, its content depends on the specific command. <br>
 * The whole command is terminated by an end-marker, which also contains the version encoded in the
 * lower 16 bits. The CE issue handler always treats the last 4 bytes of the incoming command block as
 * the 32 bit trailer. The command @ref CE_RELAX_CMD_VERS_CHECK can be used to disable the check.
 * @ingroup rcu_issue
 */

/**
 * CE command ctrl sequence mask
 * @ingroup rcu_issue
 */
#define FEESERVER_CMD_MASK         0xf0000000

/**
 * the command id mask
 * @ingroup rcu_issue
 */
#define FEESERVER_CMD_ID_MASK      0x0f000000

/**
 * the command sub id mask
 *l rcu_issue
 */
#define FEESERVER_CMD_SUB_MASK     0x00ff0000

/**
 * the command parameter mask
 * @ingroup rcu_issue
 */
#define FEESERVER_CMD_PARAM_MASK   0x0000ffff

/**
 * bit shift of general CE command ctrl sequence
 * @ingroup rcu_issue
 */
#define FEESERVER_CMD_BITSHIFT     28

/**
 * bit shift of the command id
 * @ingroup rcu_issue
 */
#define FEESERVER_CMD_ID_BITSHIFT  24

/**
 * bit shift of the command sub id
 * @ingroup rcu_issue
 */
#define FEESERVER_CMD_SUB_BITSHIFT 16

/**
 * bit shift of the parameter
 * @ingroup rcu_issue
 */
#define FEESERVER_CMD_PARAM_BTSHFT  0

/**
 * end marker mask
 * @ingroup rcu_issue
 */
#define CE_CMD_EM_MASK   0xffff0000

/**
 * end version mask
 * @ingroup rcu_issue
 */
#define CE_CMD_VS_MASK   0x0000ffff

/**
 * version of this command set.
 * - 1: initial version
 * - 2: command set extension for configuration and RCU register access
 * @ingroup rcu_issue
 */
#define CE_CMD_VERSION   0x0002

/**
 * the end marker
 * @ingroup rcu_issue
 */
#define CE_CMD_ENDMARKER 0xdd330000

/**
 * the trailer composed out of end marker and version id
 * @ingroup rcu_issue
 */
#define CE_CMD_TAILER    (CE_CMD_ENDMARKER | CE_CMD_VERSION)

// FEESERVER_CMD is defined in dcscMsgBufferInterface.h if the file is part of a project
// including the message buffer interface. If the latter header file is not available, 
// take the default value
#ifndef FEESERVER_CMD
#define FEESERVER_CMD 0xf0000000
#endif //FEESERVER_CMD 

/* command defines
 * the number of additional 32 bit data words is given in {}
 */ 

/****************************************************************************************/

/**
 * @name Group 1: Commands for the ControlEngine.
 * @ingroup rcu_issue
 */

/**
 * ID of group 1.
 * 0xf1xxxxxx
 * @ingroup rcu_issue
 */
#define FEESERVER_CE_CMD  (0x01000000 | FEESERVER_CMD)

/**
 * set the maximum size for the command buffer debug print, by default set to 0.
 * payload: 1 32bit word 
 * @ingroup rcu_issue
 */
#define CEDBG_SET_BUF_PRINT_SIZE (0x010000 | FEESERVER_CE_CMD)

/**
 * turn on/off the update of the services.
 * parameter: 0x0 off, 0xffff all on, 0x<bitfield> selection on <br>
 * payload: 0
 * @ingroup rcu_issue
 */
#define CEDBG_EN_SERVICE_UPDATE  (0x030000 | FEESERVER_CE_CMD)

/**
 * set the value of a service.
 * parameter: length of the service name string including the terminating 0<br>
 * payload: 
 * - 1 32bit word
 * - char string for the service name, zero terminated and padded to 32 bit
 *
 * result: none <br>
 * @ingroup rcu_issue
 */
#define CEDBG_SET_SERVICE_VALUE  (0x040000 | FEESERVER_CE_CMD)

/* #define           CEDBG_SSD_SERVICE_NO_MASK  0x00ff // bit 0-7 service no */
/* #define           CEDBG_SSD_FEC_NO_MASK      0x1f00 // bit 8-12 FEC no */
/* #define           CEDBG_SSD_RCU_REG_MASK     0x2000 // bit 13=1 rcu register (FEC no not valid) */

/**
 * returns a list of all possible services.
 * parameter: ignored <br>
 * payload: 0
 * @ingroup rcu_issue
 */
#define CE_READ_DEFAULT_SERVICES (0x050000 | FEESERVER_CE_CMD)

/**
 * returns a list of the valid Front end cards.
 * parameter: ignored <br>
 * payload: 0
 * @ingroup rcu_issue
 */
#define CE_READ_VALID_FECS       (0x060000 | FEESERVER_CE_CMD)

/**
 * relax the version checking for commands.
 * parameter: 1 relax, 0 strict <br>
 * payload: 0
 * @ingroup rcu_issue
 */
#define CE_RELAX_CMD_VERS_CHECK  (0x070000 | FEESERVER_CE_CMD)

/**
 * set the logging level (@ref rcu_ce_base_logging).
 * parameter: log level <br>
 * payload: 0
 * @ingroup rcu_issue
 */
#define CE_SET_LOGGING_LEVEL     (0x080000 | FEESERVER_CE_CMD)

/**
 * Get a list of available high-level commands.
 * parameter: ignored <br>
 * payload: none <br>
 * return: char buffer<br>
 * <b>Note:</b> defined in command set version 2 <br>
 * @ingroup rcu_issue
 */
#define CE_GET_HIGHLEVEL_CMDS    (0x090000 | FEESERVER_CE_CMD)

/**
 * Force update of a service channel.
 * parameter: number of bytes in the payload <br>
 * payload: char buffer containing service name <br>
 * return: none<br>
 * <b>Note:</b> defined in command set version 2 <br>
 * @ingroup rcu_issue
 */
#define CE_FORCE_CH_UPDATE       (0x0a0000 | FEESERVER_CE_CMD)

/**
 * Trigger transition for one of the state machines.
 * parameter: number of bytes in the payload <br>
 * payload: char buffer containing service name belonging to the state
 * machine followed by the action separated by a blank.<br>
 * return: none<br>
 * <b>Note:</b> defined in command set version 2 <br>
 * @ingroup rcu_issue
 */
#define CE_TRIGGER_TRANSITION    (0x0b0000 | FEESERVER_CE_CMD)

/**
 * Get a description of states for one of the state machines.
 * parameter: number of bytes in the payload <br>
 * payload: char buffer containing service name belonging to the state
 * machine.<br>
 * return: char buffer with state description<br>
 * <b>Note:</b> defined in command set version 2 <br>
 * @ingroup rcu_issue
 */
#define CE_GET_STATES            (0x0c0000 | FEESERVER_CE_CMD)

/**
 * Get a description of transitions for one of the state machines.
 * parameter: number of bytes in the payload <br>
 * payload: char buffer containing service name belonging to the state
 * machine.<br>
 * return: char buffer with state description<br>
 * <b>Note:</b> defined in command set version 2 <br>
 * @ingroup rcu_issue
 */
#define CE_GET_TRANSITIONS       (0x0d0000 | FEESERVER_CE_CMD)

/**
 * A configure command for the FEE.
 * The command encapsulates and arbitrary sequence of other commands. The 
 * 'Hardware address' (HW address) is a unique ID specifying the target
 * device/partition to receive the configuration command.<br>
 * The layout of registers, the memory sizes aso might be different between RCU
 * firmware versions. In order to adapt the CE to a firmware
 * version, different code books are used to define the memory and register 
 * layout. Currently, there is no version number available in the RCU firmware
 * and the CE is therefor "hard-wired" to a certain firmware version.<br>
 * parameter: number of commands inside the block (including @ref FEE_VERIFICATION 
 * and @ref FEE_CONFIGURE_END)<br>
 * payload: <br>
 * - 32 bit HW address, related to the ALTRO addressing<br>
 *   bit 11-0   ALTRO address<br>
 *   bit 15-12  Device: 0 - CE, 1 - RCU, 2 - FEC, 3 - Altro, 4 - Channel <br>
 *   bit 31-16  configuration id to distinguish blocks to the same device/partition<br>
 * - 32 bit checksum for this configuration command
 * - first command of the block 
 * - ...
 * - last configuration command of the block
 * - @ref FEE_VERIFICATION command
 * - @ref FEE_CONFIGURE_END command
 *
 * The @ref FEE_VERIFICATION command can be empty but is mandatory. If not empty 
 * it is added to the list for the verification daemon.<br>
 * The @ref FEE_CONFIGURE_END command signals the end of the configuration for
 * a device/partition.<br>
 * return: the output of all the sub-commands<br>
 * <b>Note:</b> defined in command set version 2 <br>
 * @ingroup rcu_issue
 */
#define FEE_CONFIGURE           (0x200000 | FEESERVER_CE_CMD)

/**
 * Signal the end of the configuration for a device specified by the HW address.
 * The end of a configuration for a device/partition will cause it to change its state. <br>
 * parameter: ignored <br>
 * payload: 32 bit HW address as for the @ref FEE_CONFIGURE command<br>
 * result: none <br>
 *
 * <b>Note:</b> defined in command set version 2 <br>
 * @ingroup rcu_issue
 */
#define FEE_CONFIGURE_END       (0x210000 | FEESERVER_CE_CMD)

/**
 * Specify a verification sequence.
 * The command can only occur inside a @ref FEE_CONFIGURE command and will be connected to
 * this. The verification daemon periodically checks the configuration with the help of
 * verification sequences. The daemon doesn't know about the operations of the verification 
 * and neither the result. 
 * It treats the sub-commands as normal CE commands and calculates the checksum from the
 * result.<br>
 * <b>Note:</b> This implies a modulation of the hardware when preparing the
 * verification command block.<br>
 * <br>
 * parameter: number of commands inside the block <br>
 * payload: <br>
 * - 32 bit checksum for this verification command block
 * - 32 bit checksum for the result of the verification sequence
 * - first command of the block 
 * - ...
 * - last command of the block
 *
 * <b>Note:</b> An empty verification command (parameter zero) does not have any payload, not even
 * the checksums.<br>
 * <br>
 * return: the output of all the sub-commands<br>
 *
 * <b>Note:</b> defined in command set version 2 <br>
 * @ingroup rcu_issue
 */
#define FEE_VERIFICATION        (0x220000 | FEESERVER_CE_CMD)

/**
 * Signal the need for external configuration data.
 * The command can only occur inside a @ref FEE_CONFIGURE command and will abort the interpretation
 * of the configuration sequence. The device will go into state @ref CONF_EXTERNAL and wait for a
 * <i>closeConfExternal</i> action. After this action the device is considered to be completely
 * configured and it wil go to state @ref STBY_CONFIGURED. <br>
 * <br>
 * parameter: lower 16 bit of the device HW address described in @ref FEE_CONFIGURE <br>
 * payload: none <br>
 * return: none<br>
 *
 * <b>Note:</b> defined in command set version 2 <br>
 * @ingroup rcu_issue
 */
#define FEE_EXTERNAL_CONFIGURATION      (0x230000 | FEESERVER_CE_CMD)

/****************************************************************************************/

/**
 * @name Group 2: commands for the dcscMsgBufInterface library
 * @ingroup rcu_issue
 */

/**
 * ID of group 2.
 * 0xf2xxxxxx
 * @ingroup rcu_issue
 */
#define FEESVR_DCSC_CMD    (0x02000000 | FEESERVER_CMD)

/**
 * set a debug option flag of the dcscRCUaccess library
 * parameter: bit flied of flags <br>
 * payload: 0
 * @ingroup rcu_issue
 */
#define DCSC_SET_OPTION_FLAG     (0x010000 | FEESVR_DCSC_CMD)

/**
 * clear a debug option flag of the dcscRCUaccess library
 * parameter: bit flied of flags <br>
 * payload: 0
 * @ingroup rcu_issue
 */
#define DCSC_CLEAR_OPTION_FLAG   (0x020000 | FEESVR_DCSC_CMD)

/**
 * enable/disable single write/read mode for MsgBuffer interface.
 * parameter: 0 disable, 1 enable <br>
 * payload: 0
 * @ingroup rcu_issue
 */
#define DCSC_USE_SINGLE_WRITE    (0x030000 | FEESVR_DCSC_CMD)

/****************************************************************************************/

/**
 * @name Group 3: command set for the RCU
 * @ingroup rcu_issue
 */

/**
 * ID of group 3
 * 0xf3xxxxxx
 * @ingroup rcu_issue
 */
#define FEESVR_CMD_RCU    (0x03000000 | FEESERVER_CMD)

/**
 * send the execution command to run the sequence written to rcu instruction memory.
 * parameter: 8 bit start address in the instruction memory <br>
 * payload: 0
 * <b>Note:</b> The start address is available from command set version 2, in version 1 the parameter is just ignored <br>
 * @ingroup rcu_issue
 */
#define RCU_EXEC              (0x010000 | FEESVR_CMD_RCU)

/**
 * stop the execution of the RCU sequencer.
 * parameter: ignored <br>
 * payload: 0
 * @ingroup rcu_issue
 */
#define RCU_STOP_EXEC         (0x020000 | FEESVR_CMD_RCU)

// the following commands allow to write a block of data to rcu instruction or pattern memory
// the number of 32 bit words is given by the command parameter

/**
 * write to rcu instruction memory.
 * parameter: number of 32 bit words in the payload <br>
 * payload: 32 bit data words
 * @ingroup rcu_issue
 */
#define RCU_WRITE_INSTRUCTION (0x030000 | FEESVR_CMD_RCU)

/**
 * write to rcu instruction memory and send the execution command.
 * parameter: number of 32 bit words in the payload <br>
 * payload: 32 bit data words
 * @ingroup rcu_issue
 */
#define RCU_EXEC_INSTRUCTION  (0x040000 | FEESVR_CMD_RCU)

/**
 * write 8 bit data to rcu pattern memory.
 * parameter: number of 8 bit words in the payload <br>
 * payload: 8 bit data words
 * @ingroup rcu_issue
 */
#define RCU_WRITE_PATTERN8    (0x050000 | FEESVR_CMD_RCU)

/**
 * write 16 bit data to rcu pattern memory.
 * parameter: number of 16 bit words in the payload <br>
 * payload: 16 bit data words
 * @ingroup rcu_issue
 */
#define RCU_WRITE_PATTERN16   (0x060000 | FEESVR_CMD_RCU)

/**
 * write 32 bit data to rcu pattern memory.
 * parameter: number of 32 bit words in the payload <br>
 * payload: 32 bit data words
 * @ingroup rcu_issue
 */
#define RCU_WRITE_PATTERN32   (0x070000 | FEESVR_CMD_RCU)

/**
 * write 10 bit compressed data to rcu pattern memory.
 * three 10 bit data words stored in 4 bytes in a 'little endian' representation. <br> 
 * parameter: number of 10 bit words in the payload <br>
 * payload: 10 bit compressed data words
 * @ingroup rcu_issue
 */
#define RCU_WRITE_PATTERN10   (0x080000 | FEESVR_CMD_RCU) // write 10 bit compressed data to rcu pattern memory {parameter}

// the following commands allow to read the content of the rcu instruction or pattern memory
// the number of 32 bit words is given by the command parameter

/**
 * read from rcu instruction memory.
 * parameter: number of 32 bit words to read <br>
 * payload: 0
 * @ingroup rcu_issue
 */
#define RCU_READ_INSTRUCTION  (0x090000 | FEESVR_CMD_RCU)

/**
 * read from rcu pattern memory.
 * parameter: number of 32 bit words to read <br>
 * payload: 0
 * @ingroup rcu_issue
 */
#define RCU_READ_PATTERN      (0x0a0000 | FEESVR_CMD_RCU)

// the following commands allow to read and write from/to an rcu memory location
// parameter is a 16 bit address

/**
 * read from rcu memory location.
 * parameter: 16 bit address <br>
 * payload: 0
 * @ingroup rcu_issue
 */
#define RCU_READ_MEMORY       (0x0b0000 | FEESVR_CMD_RCU)

/**
 * write to rcu memory location.
 * parameter: 16 bit address <br>
 * payload: 1 32 bit word
 * @ingroup rcu_issue
 */
#define RCU_WRITE_MEMORY      (0x0c0000 | FEESVR_CMD_RCU)

/**
 * write to rcu result memory.
 * parameter: number of 32 bit words in the payload <br>
 * payload: 32 bit data words
 * @ingroup rcu_issue
 */
#define RCU_WRITE_RESULT      (0x0d0000 | FEESVR_CMD_RCU)

/**
 * read from rcu result memory.
 * parameter: number of 32 bit words to read <br>
 * payload: 0
 * @ingroup rcu_issue
 */
#define RCU_READ_RESULT       (0x0e0000 | FEESVR_CMD_RCU)

/**
 * write block to rcu memory.
 * parameter: number of 32 bit words to write <br>
 * payload: the first 32 bit word of the payload is the address to write to, the 32 bit data follows
 * @ingroup rcu_issue
 */
#define RCU_WRITE_MEMBLOCK    (0x100000 | FEESVR_CMD_RCU)

/**
 * read block from rcu memory.
 * parameter: number of 32 bit words to read <br>
 * payload: address
 * @ingroup rcu_issue
 */
#define RCU_READ_MEMBLOCK     (0x110000 | FEESVR_CMD_RCU)

/**
 * read the ERRST register of the RCU.
 * parameter: ignored <br>
 * payload: none <br>
 * result: 32 bit register content <br> 
 * <b>Note:</b> defined in command set version 2 <br>
 * @ingroup rcu_issue
 */
#define RCU_READ_ERRST        (0x120000 | FEESVR_CMD_RCU)

/**
 * write the TRGCFG register of the RCU.
 * parameter: ignored <br>
 * payload: 32bit data (only bit 0 to 28 valid) <br>
 * result: none <br>
 * <b>Note:</b> defined in command set version 2 <br>
 * @ingroup rcu_issue
 */
#define RCU_WRITE_TRGCFG      (0x130000 | FEESVR_CMD_RCU)

/**
 * read the TRGCFG register of the RCU.
 * parameter: ignored <br>
 * payload: none <br>
 * result: 32 bit register content <br>
 * <b>Note:</b> defined in command set version 2 <br>
 * @ingroup rcu_issue
 */
#define RCU_READ_TRGCFG       (0x140000 | FEESVR_CMD_RCU)

/**
 * write the PMCFG register of the RCU.
 * <b>Note:</b>The PMCFG register is 20 bit wide.
 * parameter: ignored <br>
 * payload: 32bit data <br>
 * result: none <br>
 * <b>Note:</b> defined in command set version 2 <br>
 * @ingroup rcu_issue
 */
#define RCU_WRITE_PMCFG       (0x150000 | FEESVR_CMD_RCU)

/**
 * read the PMCFG register of the RCU.
 * <b>Note:</b>The PMCFG register is 20 bit wide.
 * parameter: ignored <br>
 * payload: none <br>
 * result: 32 bit register content <br>
 * <b>Note:</b> defined in command set version 2 <br>
 * @ingroup rcu_issue
 */
#define RCU_READ_PMCFG        (0x160000 | FEESVR_CMD_RCU)

/**
 * check the error register of the RCU.
 * The command can be used to check the status of the error register during
 * a command sequence and abort if there is an error. <br>
 * parameter: ignored <br>
 * result: none, the command fails if the error register of the RCU is non-zero.<br>
 * <b>Note:</b> defined in command set version 2 <br>
 * @ingroup rcu_issue
 */
#define RCU_CHECK_ERROR       (0x200000 | FEESVR_CMD_RCU)

/**
 * enable/disable automatic error checking after each sequencer execution.
 * parameter: 0 disable, 1 enable <br>
 * payload: 0 <br>
 * result: none <br>
 * <b>Note:</b> defined in command set version 2 <br>
 * @ingroup rcu_issue
 */
#define RCU_EN_AUTO_CHECK     (0x210000 | FEESVR_CMD_RCU)

/**
 * Set the Altro Bus Master
 * parameter: 0 DDL SIU, 1 DCS board <br>
 * payload: 0 <br>
 * result: none <br>
 * <b>Note:</b> defined in command set version 2 <br>
 * @ingroup rcu_issue
 */
#define RCU_SET_ABM           (0x220000 | FEESVR_CMD_RCU)

/**
 * check if the CE matches a given version number of the RCU firmware.
 * The RCU firmware can differ from version to version in the layout of the
 * registers, the memory sizes aso. In order to adapt the CE to a firmware
 * version, different code books are used to define the memory and register 
 * layout. Currently, there is no version number available in the RCU firmware
 * and the CE is therefor "hard-wired" to a certain firmware version.
 * parameter: version number <br>
 * payload: 0 <br>
 * return: 32 bit value: 1 if version matches, 0 if not<br>
 * <b>Note:</b> defined in command set version 2 <br>
 * @ingroup rcu_issue
 */
#define RCU_CHECK_VERSION     (0x230000 | FEESVR_CMD_RCU)


/****************************************************************************************/

/**
 * @name Group 4: command set for the RCU configuration.
 * The command group is not yet implemented
 * @ingroup rcu_issue
 */

/**
 * ID of group 4.
 * 0xf4xxxxxx
 * @ingroup rcu_issue
 */
#define FEESVR_CMD_RCUCONF  (0x04000000 | FEESERVER_CMD)

/**
 * write a configuration to the RCU FPGA
 * @ingroup rcu_issue
 */
#define RCU_WRITE_FPGA_CONF   (0x010000 | FEESVR_CMD_RCU)

/**
 * read the configuration of the RCU FPGA
 * @ingroup rcu_issue
 */
#define RCU_READ_FPGA_CONF    (0x020000 | FEESVR_CMD_RCU)

/**
 * write a file to the Flash
 * @ingroup rcu_issue
 */
#define RCU_WRITE_FLASH       (0x030000 | FEESVR_CMD_RCU)

/**
 * read a file from the Flash
 * @ingroup rcu_issue
 */
#define RCU_READ_FLASH        (0x040000 | FEESVR_CMD_RCU)

/****************************************************************************************/

/**
 * @name Group 5 set the value for a data point in the HW.
 * Set the data for a corresponding service.
 * The handler function has to be specified during the registration of the service.
 * The payload is expected to be a 8, 16, or 32 bit value followed by the name of the corresponding service.
 * The length of the string (including the terminating 0!) is specified by the command parameter.
 * The string should be aligned to 4 by adding additional zeros.
 * @ingroup rcu_issue
 */

/**
 * ID of group 5.
 * 0xf5xxxxxx
 * @ingroup rcu_issue
 */
#define FEESVR_SET_FERO_DATA  (0x05000000 | FEESERVER_CMD)

/**
 * set 8 bit data.
 * @ingroup rcu_issue
 */
#define FEESVR_SET_FERO_DATA8   (0x010000 | FEESVR_SET_FERO_DATA)

/**
 * set 16 bit data.
 * @ingroup rcu_issue
 */
#define FEESVR_SET_FERO_DATA16  (0x020000 | FEESVR_SET_FERO_DATA)

/**
 * set 32 bit data.
 * @ingroup rcu_issue
 */
#define FEESVR_SET_FERO_DATA32  (0x030000 | FEESVR_SET_FERO_DATA)

/**
 * set float data.
 * @ingroup rcu_issue
 */
#define FEESVR_SET_FERO_DFLOAT  (0x040000 | FEESVR_SET_FERO_DATA)

/****************************************************************************************/

/**
 * @name Group 6 data readout from the rcu data buffer.
 * The command group is not yet implemented.
 * @ingroup rcu_issue
 */

/**
 * ID of group 6.
 * 0xf6xxxxxx
 * @ingroup rcu_issue
 */
#define FEESVR_CMD_DATA_RO    (0x06000000 | FEESERVER_CMD)

/****************************************************************************************/

/**
 * @name Group 7: shell execution.
 * 
 * @ingroup rcu_issue
 */

/**
 * ID of group 7.
 * 0xf7xxxxxx
 * @ingroup rcu_issue
 */
#define FEESVR_CMD_SHELL    (0x07000000 | FEESERVER_CMD)

/**
 * execute a script/program on the DCS board.
 * parameter: length of the string in the payload <br>
 * payload: char buffer containing shell command and arguments<br>
 * <br>
 * The command is only available in <i>master mode</i>.
 * @ingroup rcu_issue
 */
#define FEESRV_EXECUTE_PGM    (0x010000 | FEESVR_CMD_SHELL)

/**
 * Send a shell script down and execute it.
 * The char buffer will be treated as a script and executed with <tt>sh</tt> (<b>Note:</b> not
 * <tt>bash</tt> which might be unavailable on the embedded system).
 * An additional zero terminated string containing the command line arguments can preceed
 * the script. The length is specified as <tt>parameter</tt>.<br>
 * parameter: length of the command line arguments in byte including a terminating 0 <br>
 * <tt>Note: without additional command line arguments, the parameter has to be zero. </tt><br>
 * payload: char buffer containing the command line arguments and the shell script <br>
 * <tt>Note: This command can not be part of a sequence, i.e. can only be sent as a single 
 * command.</tt><br>
 * <!-- maybe we come to that specification in the future
 * <tt>Note: The script must be zero terminated if this command is part of a sequence. This
 * restriction does not apply as long as this command is sent as a single command. </tt><br>
 * -->
 * <br>
 * The command is only available in <i>master mode</i>.
 * @ingroup rcu_issue
 */
#define FEESRV_EXECUTE_SCRIPT (0x020000 | FEESVR_CMD_SHELL)

/**
 * Send a binary program to the DCS board and execute it.
 * An additional zero terminated string containing the command line arguments can preceed
 * the program. The length is specified as <tt>parameter</tt>.<br>
 * parameter: length of the command line arguments in byte including a terminating 0 <br>
 * payload: char buffer containing the command line arguments and the binary program <br>
 * <tt>Note: This command can not be part of a sequence, i.e. can only be sent as a single 
 * command.</tt><br>
 * <br>
 * The command is only available in <i>master mode</i>.
 * @ingroup rcu_issue
 */
#define FEESRV_BINARY_PGM     (0x030000 | FEESVR_CMD_SHELL)

/**
 * Send an rcu-sh script down and execute it through rcu-sh interpreter.
 * An additional zero terminated string containing the command line arguments can preceed
 * the script. The length is specified as <tt>parameter</tt>.<br>
 * parameter: length of the command line arguments in byte including a terminating 0 <br>
 * <tt>Note: without additional command line arguments, the parameter has to be zero. </tt><br>
 * payload: char buffer containing the command line arguments and the rcu-sh script <br>
 * <tt>Note: This command can not be part of a sequence, i.e. can only be sent as a single 
 * command.</tt><br>
 * <!-- maybe we come to that specification in the future
 * <tt>Note: The script must be zero terminated if this command is part of a sequence. This
 * restriction does not apply as long as this command is sent as a single command. </tt>
 * -->
 * @ingroup rcu_issue
 */
#define FEESRV_RCUSH_SCRIPT   (0x040000 | FEESVR_CMD_SHELL)

/****************************************************************************************/

/**
 * @name Groups 8 and 9: reserved.
 * @ingroup rcu_issue
 */

/**
 * reserved ID of group 8.
 * @ingroup rcu_issue
 */
#define FEESVR_RSVD8_CMD    (0x08000000 | FEESERVER_CMD)

/**
 * reserved ID of group 9.
 * @ingroup rcu_issue
 */
#define FEESVR_RSVD9_CMD    (0x09000000 | FEESERVER_CMD)

/****************************************************************************************/

/**
 * @name Groups 10 to 15: user defined function codes.
 * Introduce convenient define-ids if appropriate!
 * @ingroup rcu_issue
 */

/**
 * command set for the TPC CE
 * @ingroup rcu_issue
 */
#define FEESVR_CMD_TPC  (0x0a000000 | FEESERVER_CMD)

/**
 * command set for the PHOS CE
 * @ingroup rcu_issue
 */
#define FEESVR_CMD_PHOS (0x0b000000 | FEESERVER_CMD)

//#define FEESVR_USER3   (0x0c000000 | FEESERVER_CMD)
//#define FEESVR_USER4   (0x0d000000 | FEESERVER_CMD)


/*******************************************************************************************/

/**
 * @name internal helper functions
 * @ingroup rcu_issue
 */

/**
 * mask a value and shift it according to the mask
 * @param val  data word to mask and shift
 * @param mask data mask
 * @return the processed data word
 * @ingroup rcu_issue
 */
int MASK_SHIFT(int val, int mask);

/**
 * check if the provided header represents a FeeServer command or not
 * @param header the header word to check
 * @return 1 yes, 0 no
 * @ingroup rcu_issue
 */
int checkFeeServerCommand(__u32 header);

/**
 * check if the provided header represents a Msg Buffer command or not
 * @param header the header word to check
 * @return 1 yes, 0 no
 * @ingroup rcu_issue
 */
int checkMsgBufferCommand(__u32 header);

/**
 * extract command id from the provided header.
 * @param header
 * @return 0 - 15 command id, -1 no FeeServer command
 * @ingroup rcu_issue
 */
int extractCmdId(__u32 header);

/* extract command sub id from the provided header
 * @param header
 * @return 0 - 255 command id, -1 no FeeServer command
 * @ingroup rcu_issue
 */
int extractCmdSubId(__u32 header);

/*******************************************************************************************/

/**
 * @name API function entries
 * @ingroup rcu_issue
 */

/**
 * issue handler for the RCU like CE.
 * @ingroup rcu_issue
 */
int RCUce_Issue(char* command, char** result, int* size);

/**
 * internal cleanup of the issue handler.
 * @ingroup rcu_issue
 */
int RCUce_CleanIssue();

/**
 * set the result pointer of the current ce issue process.
 * @param buffer pointer to result data
 * @param size   size of the buffer
 * @param bClone if 1, the result buffer is automatically duplicated and thus made persistent,
 *             all memory handling is carried out by the function and the framework
 * @ingroup rcu_issue
 */
//int RCUce_IssueResult(char* buffer, int size, int bDuplicate);

/**
 * wrapper to the above IssueResult method for static result buffers.
 * the provided buffer has to be valid at least until the next call of issue by the Feeserver 
 * @param buffer pointer to result data
 * @param size   size of the buffer
 * @ingroup rcu_issue
 */
//int RCUce_SetResult(char* buffer, int size);

/**
 * wrapper to the above IssueResult method for volatile result buffers.
 * the provided buffer will be duplicated
 * @param buffer pointer to result data
 * @param size   size of the buffer
 * @ingroup rcu_issue
 */
//int RCUce_CloneResult(char* buffer, int size);

/**
 * return result buffer and size.
 * @result >0 valid buffer with size n
 *         =0 no result buffer
 *         <0 error code 
 * @ingroup rcu_issue
 */
//int RCUce_GetResult(const char** target);

#ifdef __cplusplus
}
#endif
#endif //__RCU_ISSUE_H
