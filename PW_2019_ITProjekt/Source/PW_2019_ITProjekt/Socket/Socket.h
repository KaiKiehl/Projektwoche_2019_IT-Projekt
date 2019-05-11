#pragma once

#include "ListenerThread.h"
#include <functional>

#include "Runtime/Networking/Public/Networking.h"
#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"

class Socket {
public:
	Socket(FString& socket_name);
	bool start(const FString& ip_addr, const uint32& port);
	void stop();
	~Socket();
	std::function<void(const TArray<uint8>&)> data_received;
	void send_data(const TArray<uint8>& data);

private:
	FListenerThread* m_listener_thread;

	// Socket
	FSocket* m_listener_socket;
	bool start_tcp_receiver(const FString& ip, const int32& port);
	void create_tcp_connection_listener(const FString& socket_name, const FString& ip, const int32 port, const int32 receiveBufferSize = 2 * 1024 * 1024);

	FString m_socket_name;
	bool format_ip4_to_number(const FString& ip, uint8(&Out)[4]);
};
