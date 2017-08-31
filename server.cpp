#include <zmqpp/zmqpp.hpp>
#include <iostream>
#include <string>
#include <cassert>
#include <unordered_map>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

using namespace std;
using namespace zmqpp;

vector<char> readFileToBytes(const string& fileName){//, int parte) {
  ifstream ifs(fileName, ios::binary | ios::ate);
  ifstream::pos_type pos = ifs.tellg(); //longitud

  //int lenghtSong = ifs.tellg();

  //vector<char> result(524288);
  vector<char> result(pos);


  //int readBeginPos = parte * 524288;
  //int readEndPos = readBeginPos + 524288;

  ifs.seekg(0, ios::beg);
  //if(lenghtSong > readBeginPos){
    //ifs.seekg(readBeginPos, ios::beg);
    //ifs.seekg(0, ios::beg);
    //if(lenghtSong < readEndPos){
     // int lenFinalBlock = lenghtSong - readBeginPos;
    //  ifs.read(result.data(), lenFinalBlock);
    //}
    //else
    //  ifs.read(result.data(), 524288);
      ifs.read(result.data(), pos);
  //}
  return result;
}

void fileToMesage(const string& fileName, message& msg) {//, int parte
  vector<char> bytes = readFileToBytes(fileName);//, parte);
  //msg.add_raw(bytes.data(), bytes.size());
  msg.add_raw(bytes.data(), bytes.size());

}

/* Funcion para retornar lista de archivos ogg*/
unordered_map<std::string,string> musicAvailable (const char *path){
  DIR *dir;
  struct dirent *ent;
  dir = opendir (path);

  unordered_map<std::string,string> songs;
  std::hash<std::string> str_hash;

  while ((ent = readdir (dir)) != NULL) {
      int input_len = strlen (ent->d_name);
      int offset = input_len - 3;
      char extension[4] = {0};

      strncpy (extension, ent->d_name + offset, 3);
      if ((strcmp(ent->d_name, ".")!=0) && (strcmp(ent->d_name, "..")!=0) && (strcmp(extension,"ogg")==0)){
        size_t hashValue = str_hash(ent->d_name);
        char key[64]; 
        sprintf(key,"%u", hashValue);

        std::pair<std::string,std::string> onepair (key,ent->d_name);
        songs.insert(onepair);
      }
    }

  closedir (dir);
  return songs;
}

int main(int argc, char** argv) {
  context ctx;
  socket s(ctx, socket_type::rep);
  s.bind("tcp://*:5555");

  unordered_map<std::string,std::string> songs;

  songs = musicAvailable(argv[1]);

  cout << "Start serving requests!\n";
  while(true) {
    message m;
    s.receive(m);

    string op;
    m >> op;

    cout << "Action:  " << op << endl;
    if (op == "list") {  // Use case 1: Send the songs
      message n;

      n << songs.size();
      for(const auto& p : songs)
        n << p.first << p.second;
      s.send(n);
      
    } else if(op == "play") {
      // Use case 2: Send song file
      std::hash<std::string> str_hash;

      //int parte;
      //m >> parte;   

      string songName;
      m >> songName;

      size_t hashValue = str_hash(songName);
      char key[64]; 
      sprintf(key,"%u", hashValue);

      //string songName = songs[key];
      cout << "sending song " << songName
           << " at " << songName << endl;
      message n;
      n << songName;
      //fileToMesage(argv[1]+songName, parte, n);
      fileToMesage(argv[1]+songName, n);
      s.send(n);
    } else {
      cout << "Invalid operation requested!!\n";
    }
  }

  cout << "Finished\n";
  return 0;
}