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
