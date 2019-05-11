#pragma once

#include <functional>

#include "Runtime/Networking/Public/Networking.h"
#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"

class FConnectionThread : FRunnable
{
private:
	static FConnectionThread* s_runnable;

	FRunnableThread* m_thread;

	TArray<uint32>* m_data;

	FThreadSafeCounter m_stop_task_counter;
	FIPv4Endpoint m_remote_address_for_connection;

	FSocket* m_connection_socket;

	bool m_should_disconnect = false;

	void read_data();
	bool is_finished();

public:

	FConnectionThread(FSocket* connection_socket);
	virtual ~FConnectionThread();

	// FRunnable interface
	virtual bool Init();
	virtual uint32 Run();
	virtual void Stop();
	// 

	std::function<void(const TArray<uint8>&)> data_received;

	void send_data(const TArray<uint8>& data);

	void EnsureCompletion();

	static FConnectionThread* JoyInit(FSocket* connection_socket);

	static void Shutdown();
	static bool IsThreadFinished();
};