#include "helpers.cpp"

#define PORT 3550
#define MAXDATASIZE 100

int create_connect_socket(char *addr)
{
    int fd, r;
    struct sockaddr_in input_addr;
    struct sockaddr_in server;

    bzero(&input_addr, sizeof(input_addr));

    r = inet_aton(addr, &input_addr.sin_addr);
    if (r == -1)
    {
        perror("inet_aton");
        exit(EXIT_FAILURE);
    }
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        printf("socket() error\n");
        exit(EXIT_FAILURE);
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr = input_addr.sin_addr;
    bzero(&(server.sin_zero), 8);
    r = connect(fd, (struct sockaddr *)&server, sizeof(struct sockaddr));
    if (r == -1)
    {
        printf("connect() error\n");
        exit(EXIT_FAILURE);
    }
    return fd;
}

float sendDataToServer(int fd, Trip t, size_t sz)
{

    int r, numbytes;
    char buffer[MAXDATASIZE], bufferSend[sizeof(Trip)];
    memcpy(bufferSend, &t, sizeof(Trip));

    // enviando datos al servidor
    r = send(fd, bufferSend, sz, 0);
    if (r < 0)
    {
        perror("error en send");
        exit(-1);
    }

    // respuesta del servidor
    float result;
    numbytes = recv(fd, &result, sizeof(float), 0);
    if (numbytes == -1)
    {
        cout << "Error en recv()" << endl;
        exit(-1);
    }

    return result;
}

int main(int argc, char const *argv[])
{
    int fd1, r, numbytes;
    char buffer[MAXDATASIZE];

    // conectando con el servidor
    fd1 = create_connect_socket("127.0.0.1");

    // datos para la busqueda
    int opc, id_origen = -1, id_destino = -1, hora = -1;
    float tiempo_medio = 0, response = 0;

    // estructura temporal que guarda los datos
    Trip trip;
    trip.dstid = -1;
    trip.hod = -1;
    trip.source_id = -1;
    trip.mean_travel_time = -1;

    cout << "*************************************************" << endl;
    cout << "*******************Bienvenido********************" << endl;
    cout << "*************************************************" << endl;

    do
    {

        if (trip.dstid > 0 || trip.source_id > 0 || trip.hod > 0)
        {
            cout << "Datos actuales:\n \torigen: " << trip.source_id << "\n\tdestino: " << trip.dstid << "\n \thora: " << trip.hod << endl;
        }

        cout << "\n1. Ingresar Origen" << endl;
        cout << "2. Ingresar Destino" << endl;
        cout << "3. Ingresar Hora" << endl;
        cout << "4. Buscar tiempo de viaje medio" << endl;
        cout << "5. Salir" << endl;
        cout << "Digite la opcion: ";
        cin >> opc;

        switch (opc)
        {
        case 1:
            do
            {
                cout << "Selecciona el ID de origen (entre 1 - 1160): ";
                cin >> trip.source_id;

            } while (trip.source_id < 1 || trip.source_id > 1160);

            cout << "\033[2J\033[1;1H";

            response = sendDataToServer(fd1, trip, sizeof(Trip));

            break;
        case 2:
            do
            {
                cout << "Selecciona el ID de destino (entre 1 - 1160): ";
                cin >> trip.dstid;

            } while (trip.dstid < 1 || trip.dstid > 1160);

            cout << "\033[2J\033[1;1H";

            response = sendDataToServer(fd1, trip, sizeof(Trip));

            break;
        case 3:
            do
            {
                cout << "Selecciona la hora (entre 0 - 23): ";
                cin >> trip.hod;
            } while (trip.hod < 0 || trip.hod > 23);

            cout << "\033[2J\033[1;1H";

            response = sendDataToServer(fd1, trip, sizeof(Trip));

            break;
        case 4:
        {

            clock_t start_time, end_time;
            double cpu_time_used;

            // al llamar a la funciones lo que hace es enviar los datos a el otro processo
            start_time = clock();

            trip.mean_travel_time = -2;
            trip.mean_travel_time = sendDataToServer(fd1, trip, sizeof(Trip));

            if (trip.mean_travel_time < 0)
            {
                cout << "\n------- NA -------\n"
                     << endl;
            }
            else
            {

                cout << "------- Tiempo de viaje medio: " << trip.mean_travel_time << " -------\n"
                     << endl;
            }

            break;
        }
        case 5:
            break;
        }
    } while (opc != 5);

    close(fd1);

    exit(0);
}
