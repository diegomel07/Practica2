#ifndef HELPERS_HPP 
#define HELPERS_HPP


#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <cstring>
#include <list>
#include <cstddef>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>

using namespace std;

const int TABLE_SIZE = 2000; 


// Estructura que guarda un viaje
struct Trip {
  int source_id;
  int dstid;
  int hod;
  float mean_travel_time;
};

// clase que implementa hash table
class HashTable
{
  private:
    vector<vector<Trip>> table;
    
    // funcion para obtner el hash de que un valor recibido
    int hashFunction(int n) 
    {
      static const int prime = 31;
      int hash = 0;
      hash = prime * hash + n;
      return hash % TABLE_SIZE;
    } 

  public:
    HashTable()
    {
      table.resize(TABLE_SIZE);
    }

    // agregar un nuevo bucket a la hash table
    void insert(int key, Trip value)
    {
      int index = hashFunction(key);
      table[index].push_back(value);
      
      // evitar colisiones al insertar
      /*
      int index = hashFunction(key);
      int i = 0;

      while(!table[index].empty() && i < TABLE_SIZE)
      {
        index = (index + 1) % TABLE_SIZE;
        i++;
      }

      if(i < TABLE_SIZE)
      {
        table[index].push_back(value);
      } else {
        cerr << "HashTable is full, could not insert Person" << endl;
      }
      */
    }
    

    // encontrar un bucker dentro de la hash table
    /*
    vector<Trip> find(int key)
    {
      int index = hashFunction(key);
     
      return table[index];
    }
    */

    // devolver la tabla
    const vector<vector<Trip>>& getTable() const {
      return table;
    }
};

int hashFunction(int n);
float searchTrip(int bucket_index, int dstid, int hod);
float searcInBucket(vector<Trip> bucket, int dstid, int hod);
float pipe_process(int id_origen, int id_destino, int hora);
void menu(const char* pipe_name);

#endif
