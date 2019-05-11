#pragma once
#include "ConnectionThread.h"

#include <functional>

#include "Runtime/Networking/Public/Networking.h"
#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"

class FListenerThread : FRunnable
{
private:
	static FListenerThread* s_runnable;

	FRunnableThread* m_thread;

	TArray<uint32>* m_data;

	FThreadSafeCounter m_stop_task_counter;
	FIPv4Endpoint m_remote_address_for_connection;

	FSocket* m_listener_socket;

	// Connection socket
	FSocket* m_connection_socket;
	FConnectionThread* m_connection_thread;

	bool m_should_disconnect = false;

	void check_for_connections();
	bool is_finished();

public:
	
	FListenerThread(FSocket* listener_socket);
	virtual ~FListenerThread();

	// FRunnable interface
	virtual bool Init();
	virtual uint32 Run();
	virtual void Stop();
	// 

	std::function<void(const TArray<uint8>&)> data_received;

	void EnsureCompletion();
	void send_data(const TArray<uint8>& data);

	static FListenerThread* JoyInit(FSocket* listener_socket);

	static void Shutdown();
	static bool IsThreadFinished();
};