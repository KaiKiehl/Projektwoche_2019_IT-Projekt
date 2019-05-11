#include "Socket.h"
#include "Engine.h"

Socket::Socket(FString& socket_name) : m_socket_name(socket_name)
{	
	this->m_listener_socket = nullptr;
}

Socket::~Socket()
{
	this->m_listener_socket->Close();
	FListenerThread::Shutdown();
	ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(this->m_listener_socket);
}

void Socket::send_data(const TArray<uint8>& data)
{
	this->m_listener_thread->send_data(data);
}

bool Socket::start(const FString& ip, const uint32& port)
{
	return this->start_tcp_receiver(ip, port);
}

void Socket::stop()
{
	this->~Socket();
}

bool Socket::start_tcp_receiver(const FString& ip, const int32& port)
{
	this->create_tcp_connection_listener(this->m_socket_name, ip, port);

	if (!this->m_listener_socket)
	{
		if (!GEngine) return false;

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("start>> Listen socket could not be created ~> %s %d"), *ip, port));

		return false;
	}

	this->m_listener_thread = FListenerThread::JoyInit(this->m_listener_socket);
	this->m_listener_thread->data_received = this->data_received;
	return true;
}

void Socket::create_tcp_connection_listener(const FString& socket_name, const FString& ip, const int32 port, const int32 receiveBufferSize)
{
	uint8 ip4_nums[4];

	if (!this->format_ip4_to_number(ip, ip4_nums)) throw new std::exception("invalid ip");

	FIPv4Endpoint endpoint(FIPv4Address(ip4_nums[0], ip4_nums[1], ip4_nums[2], ip4_nums[3]), port);


	this->m_listener_socket = FTcpSocketBuilder(*socket_name)
		.AsReusable()
		.BoundToEndpoint(endpoint)
		.Listening(8);

	int32 newSize = 0;
	this->m_listener_socket->SetReceiveBufferSize(receiveBufferSize, newSize);
}

bool Socket::format_ip4_to_number(const FString& ip, uint8(&Out)[4])
{
	ip.Replace(TEXT(" "), TEXT(""));

	TArray<FString> parts;

	ip.ParseIntoArray(parts, TEXT("."), true);

	for (int32 i = 0; i < 4; ++i) Out[i] = FCString::Atoi(*parts[i]);

	return true;
}