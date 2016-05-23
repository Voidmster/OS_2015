CC=g++ -std=c++11

all: main.o io_service.o local_server.o posix_socket.o utils.o rsh_daemon.o file_descriptor.o data_buffer.o
	$(CC) main.o io_service.o local_server.o posix_socket.o utils.o rsh_daemon.o file_descriptor.o data_buffer.o -o rshd

main.o: main.cpp
	$(CC) -c main.cpp

io_service.o: io_service.cpp
	$(CC) -c io_service.cpp

local_server.o: local_server.cpp
	$(CC) -c local_server.cpp

posix_socket.o: posix_socket.cpp
	$(CC) -c posix_socket.cpp

utils.o: utils.cpp
	$(CC) -c utils.cpp

rsh_daemon.o: rsh_daemon.cpp
	$(CC) -c rsh_daemon.cpp

file_descriptor.o: file_descriptor.cpp
	$(CC) -c file_descriptor.cpp

data_buffer.o: data_buffer.cpp
	$(CC) -c data_buffer.cpp

clean:
	rm -rf *.o rshd