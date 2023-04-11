/*
 * tributech_oem_api.h
 *
 *  Created on: 23.02.2023
 *      Author: Daniel Hackl
 *     Purpose: simpler usage of Tributech OEM Module API
 *     License: MIT License - Copyright (c) by Tributech Solutions GmbH
 */

#ifndef LIBRARIES_OEMLIB_TRIBUTECHLIB_TRIBUTECH_OEM_API_H_
#define LIBRARIES_OEMLIB_TRIBUTECHLIB_TRIBUTECH_OEM_API_H_

#include "jsmn.h"
#include "stdio.h"
#include "inttypes.h"
#include "string.h"
#include "stdbool.h"
#include "stdlib.h"
#include "ctype.h"

extern bool configuration_received;			// bit if configuration still received
extern uint32_t get_config_transactionnr;	// transaction number of getConfig command
extern char transaction_nr_string[7];		// transaction number string

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// build api command - get configuration
// return value 0..wrong transaction id, 1..success
int build_get_configuration(char * result, char * transaction_nr);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// build api command - provide value
// return value 0..wrong timestamp, 1..success
int build_provide_value(char * result, char * transaction_nr, char * id, char * data, char * timestamp);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// build api command - get_time
// return value 0..wrong transaction id, timestamp, 1..success
int build_get_time(char * result, char * transaction_nr);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// build api command - get_status
// return value 0..wrong transaction id, timestamp, 1..success
int build_get_status(char * result, char * transaction_nr);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// parse oem response and save configuration - return true if success
uint8_t parse_oem_response_save_configuration(char * data, uint16_t cmd_len);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// parse ValueMetaDataID from configuration - return true if success
uint8_t get_valueMetaDataId(char * stream_name, char * id);

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// parse get time
uint64_t parse_get_time(char * data, uint16_t cmd_len);

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// parse get status
uint8_t parse_get_status(char * data, uint16_t cmd_len);

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// increase transaction number
void increase_transaction_nr(void);

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// String To LowerCase and UpperCase
char * to_lower_case(char * text);
char * to_upper_case(char * text);

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// minimum of two uint32_t values
uint32_t min2_uint32_t (uint32_t value1, uint32_t value2);



#endif /* LIBRARIES_OEMLIB_TRIBUTECHLIB_TRIBUTECH_OEM_API_H_ */
