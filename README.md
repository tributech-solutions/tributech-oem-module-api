# tributech-oem-module-api

Tributech OEM Module API is a C library to use the OEM Module UART API in a simpler way.

# API

## get configuration
To receive the oem module configuration with all streams you have to send the GetConfiguration command.
```C
int send_get_configuration(char * result, char * transaction_id);
```
* The parameter 'result' is the build string command which must be send via uart to the oem module.
* The parameter 'transaction_id' is the transaction id.
* The return value is '1' if success or '0' if the transaction_id is wrong.

To convert from base64 to plain the following function is used:

## provide values
To send a stream value to the oem module you have to send the provideValues command.
```C
int send_provide_values(char * result, char * transaction_id, char * id, char * data, char * timestamp);
```
* The parameter 'result' is the build string command which must be send via uart to the oem module.
* The parameter 'transaction_id' is the transaction id.
* The parameter 'id' is the valueMetaDataId of the stream.
* The parameter 'data' is the BASE64 string of the value.
* The parameter 'timestamp' is the timestamp. If '0' then the oem module uses it's own actual time.
* The return value is '1' if success or '0' if the timestamp is wrong.

## parse and save received configuration
To save the configuration you have to execute this command for parsing and saving the configuration from the response.
This command only has to be executed if the 'get_valueMetaDataId' command is used.
```C
uint8_t parse_oem_response_save_configuration(char * data, uint16_t cmd_len);
```
* The parameter 'data' is the response of the oem module.
* The parameter 'cmd_len' is the length of the response message.
* The return value is '1' if success or '0' if error.

## parse and save received configuration
This command searches in the saved configuration for the stream name and returns the stream id.
```C
uint8_t get_valueMetaDataId(char * stream_name, char * id);
```
* The parameter 'stream_name' is the name of the stream.
* The parameter 'id' is the valueMetaDataId of the stream.
* The return value is '1' if success or '0' if error.

## lowercase and uppercase string
These function converts uppercase letters to lowercase and lowercase letters to uppercase.
```C
char * to_lower_case(char * text);
char * to_upper_case(char * text);
```
* The parameter 'text' is the response of the oem module.
* Returns the result string.

## minimum of two values
This function gets the minium value of the two input values.
```C
uint32_t min2_uint32_t (uint32_t value1, uint32_t value2);
```
* The parameters 'value1' and 'value2' are the values which will be compared.
* Returns the minimum value of the two.

## example without parsing the configuration for stream ids
```C
int main(void)
{
	time_t last_command_sent;

	char *base64_string;      			// pointer to base64 string
	char *provide_values_message;		// provide values output message
	float temperature = 15.5;
	bool send_temperature_next = true;
	
	while(1U)
	{
		  //++++++++++++++++++++++++++++++++++++++++++++++++++++
		  // ids received -> publish values
		  if(last_command_sent + 10 < get_time())
		  {
				//++++++++++++++++++++++++++++++++++++++++++++++++++++
				// increase transaction number
				increase_transaction_nr();

				//++++++++++++++++++++++++++++++++++++++++++++++++++++
				// build base64 strings from values and build send string
				provide_values_message = calloc(200,sizeof(char));

				base64_string = calloc(20,sizeof(char));
				bintob64(base64_string,&temperature, sizeof(float));
				send_provide_values(provide_values_message,transaction_nr_string,"3b619323-7a61-465b-88df-24297efd5dda",base64_string,"0");


				//++++++++++++++++++++++++++++++++++++++++++++++++++++
				// output via uart
				uart_output(&UART_OEM,provide_values_message);

				delay_ms(100);
				free(base64_string);
				free(provide_values_message);

				//++++++++++++++++++++++++++++++++++++++++++++++++++++
				// save timestamp
				last_command_sent = get_time();
		  }
	  }
	}
}
```

## example with parsing the configuration for stream ids
```C
int main(void)
{
	time_t last_command_sent;
	bool stream_ids_received;
	bool new_uart_receive;
	char valuemetadataid_temperature[37] = "";	// ValueMetaDataId 1
	char valuemetadataid_pressure[37] = "";		// ValueMetaDataId 2

	char *base64_string;      			// pointer to base64 string
	char *provide_values_message;		// provide values output message
	char send_message[50] = "";			// getConfiguration message
	float temperature = 15.5;
	float pressure = 1561;
	bool send_temperature_next = true;
	
	while(1U)
	{
		  //++++++++++++++++++++++++++++++++++++++++++++++++++++
		  // if no configuration received -> send getConfiguration command
		  if (!configuration_received)
		  {
			  if (last_command_sent + 10 < get_time())
			  {
				  send_get_configuration(send_message,"1");		// generate getConfiguration command
				  uart_output(&UART_OEM,send_message);			// send to uart
				  get_config_transactionnr = 1;					// save getConfiguration transaction number
				  last_command_sent = get_time();
			  }
		  }
		  //++++++++++++++++++++++++++++++++++++++++++++++++++++
		  // configuration received -> parse ids
		  else if (configuration_received && !stream_ids_received)
		  {
			  get_valueMetaDataId("Temperature", valuemetadataid_temperature);
			  get_valueMetaDataId("Pressure", valuemetadataid_pressure);

			  if(strcmp(valuemetadataid_temperature,"") != 0 && strcmp(valuemetadataid_pressure,"") != 0)
			  {
				  stream_ids_received = true;
			  }
		  }
		  //++++++++++++++++++++++++++++++++++++++++++++++++++++
		  // ids received -> publish values
		  else if(last_command_sent + 10 < get_time() && stream_ids_received)
		  {
				//++++++++++++++++++++++++++++++++++++++++++++++++++++
				// increase transaction number
				increase_transaction_nr();

				//++++++++++++++++++++++++++++++++++++++++++++++++++++
				// build base64 strings from values and build send string
				provide_values_message = calloc(200,sizeof(char));
				if (send_temperature_next)
				{
					base64_string = calloc(20,sizeof(char));
					bintob64(base64_string,&temperature, sizeof(float));
					send_provide_values(provide_values_message,transaction_nr_string,valuemetadataid_temperature,base64_string,"0");

					send_temperature_next = false;
				}
				else
				{
					base64_string = calloc(20,sizeof(char));
					bintob64(base64_string,&pressure, sizeof(float));
					send_provide_values(provide_values_message,transaction_nr_string,valuemetadataid_pressure,base64_string,"0");

					send_temperature_next = true;
				}

				//++++++++++++++++++++++++++++++++++++++++++++++++++++
				// output via uart
				uart_output(&UART_OEM,provide_values_message);

				delay_ms(100);
				free(base64_string);
				free(provide_values_message);

				//++++++++++++++++++++++++++++++++++++++++++++++++++++
				// save timestamp
				last_command_sent = get_time();
		  }
	  }

	  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	  // uart receive
	  if (new_uart_receive == true)
	  {
		  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		  // parse oem response and save configuration
		  parse_oem_response_save_configuration(uart_buffer, strlen(uart_buffer));

		  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		  // reset uart receive buffer
		  memset(uart_buffer,0x0,UART_RECEIVE_BUFFER_SIZE);
		  
		  new_uart_receive = false;
	  }
	}
}
	
	
				  