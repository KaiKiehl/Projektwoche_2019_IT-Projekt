#include "ConnectionThread.h"

FConnectionThread* FConnectionThread::s_runnable = nullptr;

FConnectionThread::FConnectionThread(FSocket* connection_socket) : m_connection_socket(connection_socket)
{
	this->m_thread = FRunnableThread::Create(this, TEXT("FConnectionThread"), 0, TPri_BelowNormal);
}

FConnectionThread::~FConnectionThread()
{
	if (this->m_thread) delete this->m_thread;
	this->m_thread = nullptr;
}

bool FConnectionThread::Init()
{
	return true;
}

uint32 FConnectionThread::Run()
{
	FPlatformProcess::Sleep(1.0f);

	while (this->m_stop_task_counter.GetValue() == 0)
	{
		this->read_data();

		FPlatformProcess::Sleep(0.001f);
	}

	return 0;
}

void FConnectionThread::Stop()
{
	this->m_stop_task_counter.Increment();
}

bool FConnectionThread::is_finished()
{
	if (!this->m_connection_socket) return true;

	return false;
}

FConnectionThread* FConnectionThread::JoyInit(FSocket* listener_socket)
{
	if (!s_runnable && FPlatformProcess::SupportsMultithreading())
		s_runnable = new FConnectionThread(listener_socket);

	return s_runnable;
}

void FConnectionThread::send_data(const TArray<uint8>& data)
{
	if (!this->m_connection_socket) return;
	int32 how_much;
	this->m_connection_socket->Send(data.GetData(), data.Num(), how_much);
}

void FConnectionThread::EnsureCompletion()
{
	this->Stop();
	this->m_thread->WaitForCompletion();
}

void FConnectionThread::Shutdown()
{
	if (!s_runnable) return;

	s_runnable->EnsureCompletion();
	delete s_runnable;
	s_runnable = nullptr;
}

bool FConnectionThread::IsThreadFinished()
{
	if (!s_runnable) return true;

	return s_runnable->is_finished();
}

void FConnectionThread::read_data()
{
	if (!this->m_connection_socket) return;

	TArray<uint8> received_data;

	uint32 size = 0;

	while (this->m_connection_socket->HasPendingData(size))
	{
		UE_LOG(LogTemp, Warning, TEXT("Size: %d"), size);

		received_data.Init(0, /*FMath::Min(size,*/ 65507u); // check if this is correct

		int32 read = 0;

		this->m_connection_socket->Recv(received_data.GetData(), received_data.Num(), read);
	}

	if (received_data.Num() <= 0) return;

	if (this->data_received)
	{
		this->data_received(received_data);
	}
	else UE_LOG(LogTemp, Warning, TEXT("data received handler is null"));
}