#include "helpers.hpp"


int hashFunction(int n) 
{
  static const int prime = 31;
  int hash = 0;
  hash = prime * hash + n;
  return hash % TABLE_SIZE;
} 


// Funcion para realizar la busqueda de un indice o bucket de la hash table
float searchTrip(int bucket_index, int dstid, int hod) {
  vector<Trip> bucket;

  string filename = "test.dat";

  // Aprtura del archivo
  ifstream fin(filename, ios::binary);
  if (!fin) {
    cerr << "Error: Unable to open file \"" << filename << "\" for reading." << endl;
    return -1.0;
  }

  // leemos la cantidad de buckets que tiene la hash table
  int num_buckets;
  fin.read(reinterpret_cast<char*>(&num_buckets), sizeof(num_buckets));

  // si el indice proporcionado es mayor que la cantidad de buckets que tiene la hash table
  if (bucket_index >= num_buckets) {
    cerr << "Error: Bucket index out of range." << endl;
    return -1.0;
  }

  // realizamos la busqueda del bucket
  for (int i = 0; i < bucket_index; i++) {
    int bucket_size;
    fin.read(reinterpret_cast<char*>(&bucket_size), sizeof(bucket_size));
    fin.seekg(bucket_size * sizeof(Trip), ios::cur);
  }

  // leemos el tamanio del bucket y redimensionamos un vector para poder guardar los datos 
  int bucket_size;
  fin.read(reinterpret_cast<char*>(&bucket_size), sizeof(bucket_size));
  bucket.resize(bucket_size);

  // leemos y guardamos en un vector los datos del bucket leido del archivo
  for (Trip& trip : bucket) {
    fin.read(reinterpret_cast<char*>(&trip), sizeof(trip));
  }

  fin.close();

  //return bucket;
  return searcInBucket(bucket, dstid, hod);
}

float searcInBucket(vector<Trip> bucket, int dstid, int hod)
{
  // Se realiza ahora la busqueda de el viaje exacto que se necesita
  for (Trip t : bucket)
  {
    if (t.dstid == dstid && t.hod == hod)
    {
      return t.mean_travel_time;
    }
  }
  return -1.0;
}


void menu(const char* pipe_name)
{
  int opc, id_origen = -1, id_destino = -1, hora = -1;
  float tiempo_medio = 0;

  cout<<"*************************************************"<<endl;
  cout<<"*******************Bienvenido********************"<<endl;
  cout<<"*************************************************"<<endl;

  do
  {

    if (id_origen > 0 || id_destino > 0 || hora > 0)
    {
      cout << "Datos actuales:\n \torigen: " << id_origen << "\n\tdestino: " << id_destino << "\n \thora: " << hora << endl;
    }

    cout<<"\n1. Ingresar Origen"<<endl;
    cout<<"2. Ingresar Destino"<<endl;
    cout<<"3. Ingresar Hora"<<endl;
    cout<<"4. Buscar tiempo de viaje medio"<<endl;
    cout<<"5. Salir"<<endl;
    cout << "Digite la opcion: ";
    cin  >> opc;

    switch(opc){
      case 1:
        do
        {
          cout << "Selecciona el ID de origen (entre 1 - 1160): ";
          cin >> id_origen;
        } while(id_origen < 1 || id_origen > 1160);

        cout << "\033[2J\033[1;1H";
        break;
      case 2:
        do
        {
          cout << "Selecciona el ID de destino (entre 1 - 1160): ";
          cin >> id_destino;
        } while(id_destino < 1 || id_destino > 1160);

        cout << "\033[2J\033[1;1H";
        break;
      case 3:
        do
        {
          cout << "Selecciona la hora (entre 0 - 23): ";
          cin >> hora;
        } while(hora < 0 || hora > 23);

        cout << "\033[2J\033[1;1H";
        break;
      case 4:
        {

          //cout << "\033[2J\033[1;1H";

          //clock_t start_time, end_time;
          //double cpu_time_used;


          // al llamar a la funciones lo que hace es enviar los datos a el otro processo
          //start_time = clock();
          Trip t;
          t.source_id = id_origen;
          t.dstid = id_destino;
          t.hod = hora;
          t.mean_travel_time = -1;

          // abriendo la tuberia para escritura
          int pipe_fd = open(pipe_name, O_WRONLY);

          // escribiendo datos en la tuberia
          write(pipe_fd, &t, sizeof(Trip));

          // cerrando la tuberia
          close(pipe_fd);
          opc = 5;
          break;
        }
      case 5:
        break;
    }
  } while(opc != 5);

  _exit(0);
}
