#pragma comment(lib, "Ws2_32.lib")
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <thread>
std::string name;

int send0(SOCKET s) {
	std::string buf;
	do {
		std::cout << name << " : ";
		std::cin >> buf;
		if (buf.length() > 0) {
			buf = name + ": " + buf;
			if (send(s, buf.c_str(), strlen(buf.c_str()), 0) == SOCKET_ERROR) {
				std::cout << "send failed: " << WSAGetLastError() << std::endl;
				closesocket(s);
				WSACleanup();
				getchar();
				return 1;
			}
		}
	} while (buf.length() > 0);
	return 0;
}

int receive(SOCKET s) {
	while (true) {
		char recvbuf[4096];
		ZeroMemory(recvbuf, 4096);
		int result = recv(s, recvbuf, 4096, 0);
		if (result > 0) {
			std::cout << '\r';
			std::cout << recvbuf << std::endl;
			std::cout << name << ": ";
		}
		else if (result == 0) {
			std::cout << "Connection closed" << std::endl;
			return 1;
		}
		else {
			std::cout << "recv failed: " << WSAGetLastError() << std::endl;
			closesocket(s);
			WSACleanup();
			getchar();
			return 2;
		}
	}
}

int main() {
	int result;
	WSADATA wsaData;
	result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0) {
		std::cout << "Initialize Winsock Fail: " << result << std::endl;
		getchar();
		return 1;
	}
	else
	{
		std::cout << "Initialize Winsock OK" << std::endl;
	}

	// Resolve the server address and port
	addrinfo hints, * res;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	std::string host = "127.0.0.1"; 
	std::string port = "13337";


	std::cout << "Host: 127.0.0.1 (default)" << std::endl;
	std::cout << "Port: 13337 (default)" << std::endl;



	result = getaddrinfo(host.c_str(), port.c_str(), &hints, &res);
	if (result != 0) {
		std::cout << "getaddrinfo failed: " << result << std::endl;
		getchar();
		return 1;
	}
	else
	{
		std::cout << "getaddrinfo OK" << std::endl;
	}

	// Create connecting Socket
	SOCKET sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (sock == INVALID_SOCKET) {
		std::cout << "Create connect sock failed: " << WSAGetLastError() << std::endl;
		WSACleanup();
		getchar();
		return 1;
	}
	else std::cout << "Create connect sock OK" << std::endl;

	// Connect 
	result = connect(sock, res->ai_addr, (int)res->ai_addrlen);
	if (result != 0) {
		std::cout << "connect failed: " << WSAGetLastError() << std::endl;
		getchar();
		closesocket(sock);
		WSACleanup();
		return 1;
	}
	else
	{
		std::cout << "connect OK" << std::endl;
	}

	freeaddrinfo(res);
	std::cout << std::endl << std::endl;

	std::cout << "What is your name? "; 
	std::cin >> name;
	send(sock, name.c_str(), strlen(name.c_str()) + 1, 0);


	// Send thread
	std::thread send_thread(send0, sock);
	send_thread.detach();

	// Receive thread
	std::thread receive_thread(receive, sock);
	receive_thread.join();

	// Cleanup
	closesocket(sock);
	WSACleanup();
	return 0;
}

