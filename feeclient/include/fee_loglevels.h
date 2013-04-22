#ifndef FEE_LOGLEVELS_H
#define FEE_LOGLEVELS_H

/**
 * Define for the even - type "INFO" .
 */
#define MSG_INFO    1
/**
 * Define for the even - type "WARNING" .
 */
#define MSG_WARNING 2
/**
 * Define for the even - type "ERROR" .
 */
#define MSG_ERROR   4
/**
 * Define for the even - type "FAILURE_AUDIT" .
 */
#define MSG_FAILURE_AUDIT 8
/**
 * Define for the even - type "SUCCESS_AUDIT" .
 */
#define MSG_SUCCESS_AUDIT 16
/**
 * Define for the even - type "DEBUG" .
 */
#define MSG_DEBUG   32
/**
 * Define for the even - type "ALARM" .
 */
#define MSG_ALARM   64

/**
 * Defines the maximum value of cumulated event - types.
 */
#define MSG_MAX_VAL 127


/**
 * Size of the detector field in messages (in byte).
 */
#define MSG_DETECTOR_SIZE       4
/**
 * Size of the source field in messages (in byte).
 */
#define MSG_SOURCE_SIZE         256
/**
 * Size of the description field in messages (in byte).
 */
#define MSG_DESCRIPTION_SIZE    256
/**
 * Size of the date field in messages (in byte).
 */
#define MSG_DATE_SIZE           20

/**
 * The structure of a message in DIM service.
 */
#define MSG_DIM_STRUCTURE       "I:1;C:4;C:256;C:256;C:20;"

#endif
