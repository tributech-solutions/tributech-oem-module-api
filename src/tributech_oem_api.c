#define CONFIGURATION_SIZE 4096				// configuration size

#include "tributech_oem_api.h"

char configuration[CONFIGURATION_SIZE];		// received configuration
bool configuration_received;				// bit if configuration still received
uint32_t get_config_transactionnr;			// transaction number of getConfig command
uint32_t transaction_nr_dec;				// transaction number decimal
char transaction_nr_string[7];				// transaction number string

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// build api command - get configuration
int build_get_configuration(char * result, char * transaction_id)
{
	if (strcmp(transaction_id,"") == 0)
	{
		return 0;
	}

	sprintf(result,"{\"TransactionNr\": %s, \"Operation\": \"GetConfiguration\"}\r\n",transaction_id);

	return 1;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// build api command - provide values
int build_provide_values(char * result, char * transaction_id, char * id, char * data, char * timestamp)
{
	if (strcmp(timestamp,"") == 0)
	{
		return 0;
	}

	sprintf(result, "{\"TransactionNr\": %s,\"Operation\": \"ProvideValues\",\"ValueMetadataId\": \"%s\",\"Values\": [{\"Timestamp\": %s,\"Value\": \"%s\"}]}\r\n" , transaction_id, id, timestamp, data);

	return 1;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// parse oem response and save configuration - return true if success
uint8_t parse_oem_response_save_configuration(char * data, uint16_t cmd_len)
{
	uint16_t number_of_tokens = 0;		// number of tokens
	jsmn_parser p;						// parser
	jsmntok_t *t; 						// tokens
	char * key_name;					// object key name
	uint8_t length;						// length of key name
	uint32_t received_transactionnr = 0;	// received transaction number

	if (cmd_len > CONFIGURATION_SIZE)
	{
		return false;
	}

	t = calloc(1024,sizeof(jsmntok_t));

	//+++++++++++++++++++++++++++++++++++++++++++
	// Initial start token
	p.pos = 0;
	p.toknext = 0;
	p.toksuper = -1;

	//+++++++++++++++++++++++++++++++++++++++++++
	// Get tokens by parsing json received text
	number_of_tokens = jsmn_parse(&p, data, cmd_len, t, 1024);

	//+++++++++++++++++++++++++++++++++++++++++++
	// invalid operation by less than 4 tokens
	if(number_of_tokens < 4 || number_of_tokens > 1024)
	{
		free(t);
		return false;
	}

	//+++++++++++++++++++++++++++++++++++++++++++
	// get transaction number
	for(uint8_t i = 1; i<number_of_tokens; i++)
	{
		length = t[i].end-t[i].start;
		key_name = calloc(length+1, sizeof(char));
		memcpy(key_name, &data[t[i].start], length);

		//+++++++++++++++++++++++++++++++++++++++
		// search for transaction number and sources
		if(strcmp(to_lower_case(key_name), "transactionnr") == 0)
		{
			received_transactionnr = (uint32_t)strtol(&data[(t[i+1].start)], NULL, 0);
		}
		if(strcmp(to_lower_case(key_name), "sources") == 0 && received_transactionnr == get_config_transactionnr)
		{
			//+++++++++++++++++++++++++++++++++++++++
			// save configuration as string
			memset(configuration,0x0,CONFIGURATION_SIZE);
			memcpy(configuration,data,cmd_len);
			configuration_received = true;

			free(key_name);
			free(t);
			return true;
		}
		free(key_name);
	}

	free(t);

	return false;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// parse ValueMetaDataID from configuration - return true if success
uint8_t get_valueMetaDataId(char * stream_name, char * id)
{
	uint16_t number_of_tokens = 0;		// number of tokens
	jsmn_parser p;						// parser
	jsmntok_t *t; 						// tokens
	char * key_name;					// object key name
	uint8_t length;						// length of key name
	uint16_t object_stream_end_pos;		// end position of object stream
	uint16_t array_streams_end_pos;		// end position of array streams
	char id_temp[37];					// id temp
	char stream_name_temp[50];			// stream name temp

	t = calloc(1024,sizeof(jsmntok_t));

	//+++++++++++++++++++++++++++++++++++++++++++
	// Initial start token
	p.pos = 0;
	p.toknext = 0;
	p.toksuper = -1;

	//+++++++++++++++++++++++++++++++++++++++++++
	// Get tokens by parsing json received text
	number_of_tokens = jsmn_parse(&p, configuration, strlen(configuration), t, 1024);

	//+++++++++++++++++++++++++++++++++++++++++++
	// invalid operation by less than 4 tokens
	if(number_of_tokens < 4 || number_of_tokens > 1024)
	{
		free(t);
		return false;
	}

	//+++++++++++++++++++++++++++++++++++++++++++
	// get transaction number
	for(uint8_t j = 1; j<number_of_tokens; j++)
	{
		length = t[j].end-t[j].start;
		key_name = calloc(length+1, sizeof(char));
		memcpy(key_name, &configuration[t[j].start], length);

		//+++++++++++++++++++++++++++++++++++++++
		//search for operation
		if(strcmp(to_lower_case(key_name),"streams") == 0)
		{
			j++;

			if (t[j].type == JSMN_ARRAY)
			{
				//++++++++++++++++++++++++++++++++++
				// get end position of array streams
				array_streams_end_pos = t[j].end;
				while(t[j+1].start < array_streams_end_pos && j < number_of_tokens)
				{
					j++;
					object_stream_end_pos = t[j].end;

					while(t[j+1].start < object_stream_end_pos && j < number_of_tokens)
					{
						j++;

						if (t[j].type == JSMN_STRING)
						{
							//++++++++++++++++++++++++++++++++++
							// get object key name
							free(key_name);
							length = t[j].end-t[j].start;
							key_name = calloc(length+1, sizeof(char));
							memcpy(key_name,&configuration[t[j].start],length);	// get object key name

							//++++++++++++++++++++++++++++++++++
							// get twin id
							if (strcmp(to_lower_case(key_name),"id") == 0 )
							{
								j++;
								length = t[j].end-t[j].start;

								memset(id_temp,0x0,37);
								memcpy(id_temp,&configuration[t[j].start],min2_uint32_t(length,36));	// save value meta data id
							}

							//++++++++++++++++++++++++++++++++++
							// get name
							else if (strcmp(to_lower_case(key_name),"name") == 0 )
							{
								j++;
								length = t[j].end-t[j].start;

								memset(stream_name_temp,0x0,50);
								memcpy(stream_name_temp,&configuration[t[j].start],min2_uint32_t(length,49));

								if (strcmp(stream_name_temp,stream_name) == 0 )
								{
									strcpy(id,id_temp);
									free(key_name);
									free(t);
									return true;
								}
							}
						}
					}
				}
			}
		}
		free(key_name);
	}
	free(t);

	return false;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// increase transaction number
void increase_transaction_nr(void)
{
	transaction_nr_dec++;
	if (transaction_nr_dec >= 1000000)
	{
		transaction_nr_dec = 0;
	}
	memset(transaction_nr_string,0x0,7);
	sprintf(transaction_nr_string, "%"PRIu32"",transaction_nr_dec);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// String To UpperCase
char * to_upper_case(char * text)
{
	uint16_t i;

	i=0;
	while(i < strlen(text))
	{
		text[i] = (char) toupper((int)text[i]);
	    i++;
	}

	return text;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// String To LowerCase
char * to_lower_case(char * text)
{
	uint16_t i;

	i=0;
	while(i < strlen(text))
	{
		text[i] = (char) tolower((int)text[i]);
	    i++;
	}

	return text;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// minimum of two uint32_t values
uint32_t min2_uint32_t (uint32_t value1, uint32_t value2)
{
	if (value2 < value1)
	{
		return value2;
	}
	return value1;
}
