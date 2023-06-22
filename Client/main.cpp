
#include "Client.h"
#include "Chat.h"

int main() {

	Sleep(3000);

	std::string ip{ "127.0.0.1" };
	uint16_t port{ 5555 };

	Chat chat(ip, port);
	chat.run();

	return 0;
}