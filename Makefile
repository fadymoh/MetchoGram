peer:
	g++ -std=c++14 -pthread Peer.cpp UDPSocket.cpp Message.cpp base64.cpp main.cpp -o peer && ./peer
server:
	g++ -std=c++14 -pthread centralserver.cpp UDPSocket.cpp Message.cpp base64.cpp serverMain.cpp -o server && ./server