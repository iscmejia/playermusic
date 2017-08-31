CC = g++ -std=c++11
ZMQ_PATH = /home/usuario/zmq
ZMQ_INC = $(ZMQ_PATH)/include
ZMQ_LIB = $(ZMQ_PATH)/lib

all: playMusic server

playMusic: playMusic.cpp
	$(CC) -L$(ZMQ_LIB) -I$(ZMQ_INC)  playMusic.cpp -o playMusic -lzmq -lzmqpp -lsfml-audio -lpthread

server: server.cpp
	$(CC) -L$(ZMQ_LIB) -I$(ZMQ_INC) server.cpp -o server -lzmq -lzmqpp -lsfml-audio
