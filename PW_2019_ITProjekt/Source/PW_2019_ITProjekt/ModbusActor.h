// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Socket/Socket.h"
#include "Modbus/ModbusParser.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ModbusActor.generated.h"

UCLASS()
class PW_2019_ITPROJEKT_API AModbusActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AModbusActor();
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, DisplayName = "State")
	TArray<uint8> state;

	UPROPERTY(EditAnywhere, DisplayName = "IP Address")
	FString ip = "0.0.0.0";

	UPROPERTY(EditAnywhere, DisplayName = "Port")
	uint32 port = 502;

	UPROPERTY(EditAnywhere, DisplayName = "Socket Name")
	FString socket_name = "modbus_socket";

	UPROPERTY(EditAnywhere, DisplayName = "Array Size")
	uint32 array_size = 18;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	Socket* m_socket;
	void on_data_received_handler(const TArray<uint8> data);

	modbus_response_paket_t* build_response(const modbus_request_paket_t* request);

	bool try_read(const uint16& location, const uint16& count, TArray<uint8>& result);
	bool try_write(const uint16& location, const uint16& value);

	TArray<uint8> deflate_array(TArray<uint8>& uintArray, int& byte_count);
};
