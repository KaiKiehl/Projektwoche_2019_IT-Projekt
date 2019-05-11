#include "ModbusParser.h"

modbus_request_paket_t* modbus_parse(const TArray<uint8>& data)
{
	modbus_request_paket_t* paket = new modbus_request_paket_t();
	paket->header = modbus_get_header(data);
	paket->request = modbus_get_request_body(data);

	return paket;
}

modbus_header_t* modbus_get_header(const TArray<uint8>& data)
{
	modbus_header_t* header = new modbus_header_t();
	header->trans_id = static_cast<uint16_t>((data[0] << 8) + (uint16_t)data[1]);
	uint16_t protocol_id = static_cast<uint16_t>((data[2] << 8) + data[3]);
	header->protocol_id = protocol_id;
	uint16_t length = static_cast<uint16_t>((data[4] << 8) + data[5]);
	header->length = length;
	header->unit_id = data[6];
	return header;
}

modbus_request_t* modbus_get_request_body(const TArray<uint8>& data)
{
	modbus_request_t* req = nullptr;

	switch (data[7])
	{
		case MODBUS_FC_READ_COILS:
		{
			modbus_read_request_t* read_req = new modbus_read_request_t();
			read_req->func_code = data[7];
			read_req->start_addr = static_cast<uint16_t>((data[8] << 8) + data[9]);
			read_req->quant_regs = static_cast<uint16_t>((data[10] << 8) + data[11]);
			req = read_req;
		}
		break;

		case MODBUS_FC_WRITE_SINGLE_COIL:
		{
			modbus_write_request_t* write_req = new modbus_write_request_t();
			write_req->func_code = data[7];
			write_req->addr = static_cast<uint16_t>((data[8] << 8) + data[9]);
			write_req->value = static_cast<uint16_t>((data[10] << 8) + data[11]);
			req = write_req;
		}
		break;
	}
	return req;
}

TArray<uint8> modbus_response_to_array(const modbus_response_paket_t* response)
{
	TArray<uint8> array;

	modbus_read_response_t* read_body = dynamic_cast<modbus_read_response_t*>(response->response);
	modbus_write_response_t* write_body = dynamic_cast<modbus_write_response_t*>(response->response);

	if (read_body)
		array.Init(0, MODBUS_HEADER_LENGTH + 1 + read_body->byte_count + 3);
	else if (write_body)
		array.Init(0, MODBUS_HEADER_LENGTH + 5);
	// header
	array[0] = response->header->trans_id >> 8;
	array[1] = response->header->trans_id & 0x00FF;
	array[2] = response->header->protocol_id >> 8;
	array[3] = response->header->protocol_id & 0x00FF;
	array[4] = response->header->length >> 8;
	array[5] = response->header->length & 0x00FF;
	array[6] = response->header->unit_id;
	//

	if (!response->response) return array;
	
	// response body
	array[7] = response->response->func_code;
	if (read_body)
	{
		array[8] = read_body->byte_count;
		for (int i = 0; i < read_body->byte_count; i++)
			array[9 + i] = read_body->coil_status_array[i];
	}
	else if (write_body)
	{
		array[8] = write_body->addr >> 8;
		array[9] = write_body->addr & 0x00FF;
		array[10] = write_body->value >> 8;
		array[11] = write_body->value & 0x00FF;
	}
	//

	return array;
}

void modbus_free(const modbus_request_paket_t* paket)
{
	if (!paket) return;

	if (paket->header) delete paket->header;
	if (paket->request) delete paket->request;
	
	delete paket;
}

void modbus_free(const modbus_response_paket_t* paket)
{
	if (!paket) return;

	if (paket->header) delete paket->header;

	modbus_read_response_t* read_body = dynamic_cast<modbus_read_response_t*>(paket->response);
	
	if (paket->response) delete paket->response;

	delete paket;
}

_modbus_response::~_modbus_response()
{

}

_modbus_request::~_modbus_request()
{
}
