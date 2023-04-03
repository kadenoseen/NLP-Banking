server: server.cpp databaseHandler.cpp globals.cpp transactionHandler.cpp user.cpp request.cpp session.cpp

	g++ -Wno-psabi server.cpp request.cpp session.cpp databaseHandler.cpp user.cpp transactionHandler.cpp globals.cpp -o server -ljsoncpp -lcurl -pthread -lssl -lcrypto

run:
	./server
clean:
	rm -f server
