#include "ListenerThread.h"

FListenerThread* FListenerThread::s_runnable = nullptr;

FListenerThread::FListenerThread(FSocket* listener_socket) : m_listener_socket(listener_socket)
{
	this->m_thread = FRunnableThread::Create(this, TEXT("FListenerThread"), 0, TPri_BelowNormal);
}

FListenerThread::~FListenerThread()
{
	if (this->m_thread) delete this->m_thread;
	this->m_thread = nullptr;
}

bool FListenerThread::Init()
{
	return true;
}

uint32 FListenerThread::Run()
{
	while (this->m_stop_task_counter.GetValue() == 0)
	{
		this->check_for_connections();

		FPlatformProcess::Sleep(0.01f);
	}

	return 0;
}

void FListenerThread::Stop()
{
	this->m_stop_task_counter.Increment();

	if (!this->m_connection_socket) return;
	
	this->m_connection_socket->Close();
	ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(this->m_connection_socket);
	this->m_connection_socket = nullptr;

	FConnectionThread::Shutdown();
}

bool FListenerThread::is_finished()
{
	if (!this->m_connection_socket) return true;

	return false;
}

FListenerThread* FListenerThread::JoyInit(FSocket* listener_socket)
{
	if (!s_runnable && FPlatformProcess::SupportsMultithreading())
		s_runnable = new FListenerThread(listener_socket);

	return s_runnable;
}

void FListenerThread::EnsureCompletion()
{
	this->Stop();
	this->m_thread->WaitForCompletion();
}

void FListenerThread::send_data(const TArray<uint8>& data)
{
	if (!this->m_connection_thread) return;

	this->m_connection_thread->send_data(data);
}

void FListenerThread::Shutdown()
{
	if (!s_runnable) return;

	s_runnable->EnsureCompletion();
	delete s_runnable;
	s_runnable = nullptr;
}

bool FListenerThread::IsThreadFinished()
{
	if (!s_runnable) return true;

	return s_runnable->is_finished();
}

void FListenerThread::check_for_connections()
{
	if (!this->m_listener_socket) return;

	TSharedRef<FInternetAddr> remote_address = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	bool pending;

	if (!this->m_listener_socket->HasPendingConnection(pending) || !pending) return;

	if (this->m_connection_socket) // Close previous
	{
		this->m_connection_socket->Close();

		FConnectionThread::Shutdown();
		
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(this->m_connection_socket);
		this->m_connection_socket = nullptr;
	}

	this->m_connection_socket = this->m_listener_socket->Accept(*remote_address, TEXT("Received Socket Connection"));

	if (this->m_connection_socket == NULL) return;

	this->m_remote_address_for_connection = FIPv4Endpoint(remote_address);

	UE_LOG(LogTemp, Display, TEXT("Received TCP Connection"));

	auto conn = FConnectionThread::JoyInit(this->m_connection_socket);

	if (!this->data_received) UE_LOG(LogTemp, Warning, TEXT("Data received null: Listener thread"));
	conn->data_received = this->data_received;
	this->m_connection_thread = conn;
}