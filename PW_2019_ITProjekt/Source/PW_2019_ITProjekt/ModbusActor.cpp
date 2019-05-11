// Fill out your copyright notice in the Description page of Project Settings.


#include "ModbusActor.h"
#include "Modbus/ModbusParser.h"
#include <functional> 

// Sets default values
AModbusActor::AModbusActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	this->m_socket = new Socket(this->socket_name);
}

// Called when the game starts or when spawned
void AModbusActor::BeginPlay()
{
	this->m_socket->data_received = std::function<void(TArray<uint8>)>([this](const TArray<uint8> data)
		{
			this->on_data_received_handler(data);
		});

	this->state.Init(0, this->array_size);

	if (!this->m_socket->start(this->ip, this->port))
		UE_LOG(LogTemp, Error, TEXT("Could not start socket"));

	UE_LOG(LogTemp, Display, TEXT("Created socket"));

	Super::BeginPlay();
	
}

void AModbusActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	this->m_socket->stop();
}

// Called every frame
void AModbusActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AModbusActor::on_data_received_handler(const TArray<uint8> data)
{
	modbus_request_paket_t* modbus_model = nullptr;

	modbus_model = modbus_parse(data);
	
	UE_LOG(LogTemp, Warning, TEXT("Transaction id: %d"), modbus_model->header->trans_id);
	UE_LOG(LogTemp, Warning, TEXT("Protocol id: %d"), modbus_model->header->protocol_id);
	UE_LOG(LogTemp, Warning, TEXT("Length: %d"), modbus_model->header->length);
	UE_LOG(LogTemp, Warning, TEXT("Unit id: %d"), modbus_model->header->unit_id);
	if (modbus_model->request)
	{
		UE_LOG(LogTemp, Warning, TEXT("Function code: %d"), modbus_model->request->func_code);
	}
	modbus_read_request_t* request_read_body = dynamic_cast<modbus_read_request_t*>(modbus_model->request);

	if (request_read_body)
	{
		UE_LOG(LogTemp, Warning, TEXT("Start Address: %d"), request_read_body->start_addr);
		UE_LOG(LogTemp, Warning, TEXT("Register Quantity: %d"), request_read_body->quant_regs);
	}

	modbus_write_request_t* request_write_body = dynamic_cast<modbus_write_request_t*>(modbus_model->request);

	if (request_write_body)
	{
		UE_LOG(LogTemp, Warning, TEXT("Address: %d"), request_write_body->addr);
		UE_LOG(LogTemp, Warning, TEXT("Value: %d"), request_write_body->value);
	}

	modbus_response_paket_t* response = this->build_response(modbus_model);
	UE_LOG(LogTemp, Error, TEXT("Transaction id: %d"), response->header->trans_id);
	UE_LOG(LogTemp, Error, TEXT("Protocol id: %d"), response->header->protocol_id);
	UE_LOG(LogTemp, Error, TEXT("Length: %d"), response->header->length);
	UE_LOG(LogTemp, Error, TEXT("Unit id: %d"), response->header->unit_id);
	UE_LOG(LogTemp, Error, TEXT("Function code: %d"), response->response->func_code);

	modbus_read_response_t* read_body = dynamic_cast<modbus_read_response_t*>(response->response);

	if (read_body)
	{
		UE_LOG(LogTemp, Error, TEXT("Byte Count: %d"), read_body->byte_count);
		UE_LOG(LogTemp, Error, TEXT("Status [0]: %d"), read_body->coil_status_array[0]);
	}
	TArray<uint8> arr = modbus_response_to_array(response);
	modbus_free(response);
	this->m_socket->send_data(arr);

	delete modbus_model;
}

modbus_response_paket_t* AModbusActor::build_response(const modbus_request_paket_t* request)
{
	modbus_response_paket_t* paket = nullptr;

	switch (request->request->func_code)
	{
		case MODBUS_FC_READ_COILS:
		{
			modbus_read_request_t* request_read_body = dynamic_cast<modbus_read_request_t*>(request->request);
			TArray<uint8> results;
			if (!this->try_read(request_read_body->start_addr, request_read_body->quant_regs, results))
			{
				UE_LOG(LogTemp, Warning, TEXT("Could not read requested bytes!!"));
				break;
			}

			int byte_count = 0;
			TArray<uint8> bytes = this->deflate_array(results, byte_count);

			paket = new modbus_response_paket_t();
			modbus_header_t* header = new modbus_header_t();
			header->trans_id = request->header->trans_id;
			header->protocol_id = 0x0; // Modbus
			header->length = 1 /*unit id*/ + 1 /* function code */ + 1 /* byte count */ + byte_count + 1 /* out bytes */;
			header->unit_id = request->header->unit_id;

			modbus_read_response_t* read_response = new modbus_read_response_t();
			read_response->func_code = request->request->func_code;
			read_response->byte_count = request_read_body->quant_regs / 8 + request_read_body->quant_regs % 8 ? 1 : 0;
			read_response->coil_status_array = bytes;

			paket->header = header;
			paket->response = read_response;
		}
		break;

		case MODBUS_FC_WRITE_SINGLE_COIL:
		{
			modbus_write_request_t* request_write_body = dynamic_cast<modbus_write_request_t*>(request->request);

			uint16 value = request_write_body->value;

			if (!this->try_write(request_write_body->addr, value))
			{
				// Handle failure
			}

			paket = new modbus_response_paket_t();
			modbus_header_t* header = new modbus_header_t();
			header->trans_id = request->header->trans_id;
			header->protocol_id = 0x0; // Modbus
			header->length = request->header->length;
			header->unit_id = request->header->unit_id;
			paket->header = header;
			modbus_write_response_t* body = new modbus_write_response_t();
			body->func_code = request->request->func_code;
			body->addr = request_write_body->addr;
			body->value = request_write_body->value;
			paket->response = body;
		}
		break;
	}

	if (!paket)
	{
		paket = new modbus_response_paket_t();
		modbus_header_t* head = new modbus_header_t();
		head->protocol_id = 0x0;
		head->length = 2;
		head->trans_id = request->header->trans_id;
		head->unit_id = request->header->unit_id;
		paket->header = head;
	}

	return paket;
}

TArray<uint8> AModbusActor::deflate_array(TArray<uint8>& uintArray, int& byte_count)
{
	byte_count = 0;
	int readIndex = 0;

	TArray<uint8> returnArray;
	for (auto& item : uintArray)
	{
		if (readIndex % 8 == 0 && readIndex != 0)
		{
			byte_count++;
			readIndex = 0;
		} // only write 8 bits to one byte

		if (returnArray.IsValidIndex(byte_count)) returnArray[byte_count] += item >> 1 * readIndex;
		else returnArray.Add(item);
		readIndex++;
	}

	return returnArray;
}

bool AModbusActor::try_read(const uint16& location, const uint16& count, TArray<uint8>& result)
{
	if (location > this->state.Num() - 1) return false;

	for (int i = 0; i < count; i++)
	{
		result.Add(this->state[location + i] == 255 ? 1 : 0);
	}

	return true;
}

bool AModbusActor::try_write(const uint16& location, const uint16& value)
{
	if (location > this->state.Num() - 1) return false;

	this->state[location] = value >> 8;

	return true;
}