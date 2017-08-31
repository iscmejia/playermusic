/*
Music Available -> List
Playing music continuing
Thread playing Music
!!! send parts
*/
#include <stdlib.h> 
#include <pthread.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <zmqpp/zmqpp.hpp>
#include <SFML/Audio.hpp>
#include "safequeue.cpp"
#include <unistd.h>

using namespace std;
using namespace zmqpp;
using namespace sf;



struct song2play{
	const string& fileName;
	const message& song;
};

SafeQueue<std::string> sq;

void messageToFile(const message& msg, const string& fileName) {
	const void *data;
	msg.get(&data, 1);
	size_t size = msg.size(1);

	ofstream ofs("download/"+fileName, ios::binary);
	ofs.write((char*)data, size);
}

void *playMusic(void* argv){

	song2play *songToPlay = (song2play *) argv;

	const string& fileName = songToPlay->fileName;
 	const message& song = songToPlay->song;

 	messageToFile(song, fileName);


	Music music;
    music.openFromFile("download/"+fileName);
    music.play();

    usleep(1000000);
/******************** PLAYING SONG ********************/
/**/while(music.getStatus()==SoundSource::Playing){}/**/
/******************************************************/
}

void playList(){
	SafeQueue<std::string> sq;
}



int main(int argc, char** argv) {

	struct songAvailable{
		char hashValue[64];
		char fileName[64];
		struct songAvailable *next;
	}*iteratorList, *head, *nuevo;

	std::vector<std::string> v;

	pthread_t hilo;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	message m;
	message answer;

	int action;
	string hashValue;
	string fileName;

	context ctx;

	socket s(ctx, socket_type::req);
	s.connect("tcp://localhost:5555");

	system("clear");
	cout << "   MENU PLAYER MUSIC"<< endl;
	cout << "1. Listar Canciones" << endl;
	cout << "2. Seleccionar Cancion" << endl << endl;
	

	while(true){
		fileName = "";
		hashValue = "";
		cout << "Que desea hacer: ";
		cin >> action;
		if (action == 1) {
			head=NULL;
			m << "list";
			s.send(m);
			s.receive(answer);
			size_t numSongs;
			answer >> numSongs;
			for(int i = 0; i < numSongs; i++) {
				answer >> hashValue;
				answer >> fileName;
				v.push_back(fileName);
			}

			cout << endl << "Lista de canciones Disponibles" << endl;
			for(int j=1; j<=numSongs;j++)
				cout << j << ". "<< v[j-1] << endl;
		} else if (action == 2){
			cout << "Numero de la Cancion a Escuchar: ";
			int x;
			cin >> x;

			m << "play" << v[x-1];
			s.send(m);
			s.receive(answer);
			answer >> fileName;
			song2play songToPlay={fileName, answer};
			pthread_create(&hilo,NULL,playMusic,&songToPlay);
			pthread_join(hilo,NULL);
		} else {
			cout << "Don't know what to do!!!" << endl;
		}
	}
	return 0;
}
