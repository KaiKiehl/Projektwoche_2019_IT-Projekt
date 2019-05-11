#pragma once

#include <cstdint>
#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"

#define MODBUS_FC_READ_COILS 0x01
#define MODBUS_FC_WRITE_SINGLE_COIL 0x05

#define MODBUS_MAX_READ_BITS 2000
#define MODBUS_MAX_WRITE_BITS 1968

#define MODBUS_MAX_PDU_LENGTH 253

#define MODBUS_HEADER_LENGTH 7

typedef struct _modbus_header {
	uint16_t trans_id; // Transaction ID: 2 bytes, copied
	uint16_t protocol_id; // Protocol ID: 2 bytes, copied
	uint16_t length; // Length: 2 bytes
	uint8_t unit_id; // Length: 1 bytes, copied
} modbus_header_t;

typedef struct _modbus_request {
	uint8_t func_code; // Function code
	virtual ~_modbus_request(); // polymorphic
} modbus_request_t;

typedef struct _modbus_read_request : public _modbus_request {
	uint16_t start_addr; // Starting address
	uint16_t quant_regs; // Quantity of registers
} modbus_read_request_t;

typedef struct _modbus_write_request : public _modbus_request {
	uint16_t addr; // Address to write to
	uint16_t value; // Value to be written
} modbus_write_request_t;

typedef struct _modbus_response {
	uint8_t func_code; // Function code, copied from request

	virtual ~_modbus_response(); // polymorphic
} modbus_response_t;

typedef struct _modbus_read_response : public modbus_response_t {
	uint8_t byte_count; // read bytes
	TArray<uint8> coil_status_array; // contains coil stati, n Byte, n = N, N + 1 if byte_count % 8 != 0
} modbus_read_response_t;

typedef struct _modbus_write_response : public modbus_response_t {
	uint16_t addr; // Address written to
	uint16_t value; // Value written
} modbus_write_response_t;

typedef struct _modbus_request_paket {
	modbus_header_t* header;
	modbus_request_t* request;
} modbus_request_paket_t;

typedef struct _modbus_response_paket {
	modbus_header_t* header;
	modbus_response_t* response;
} modbus_response_paket_t;

modbus_request_paket_t* modbus_parse(const TArray<uint8>& data);
modbus_header_t* modbus_get_header(const TArray<uint8>& data);
modbus_request_t* modbus_get_request_body(const TArray<uint8>& data);

TArray<uint8> modbus_response_to_array(const modbus_response_paket_t* response);

void modbus_free(const modbus_request_paket_t* paket);
void modbus_free(const modbus_response_paket_t* paket);