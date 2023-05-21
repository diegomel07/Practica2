#include "helpers.hpp"

struct thread_data
{

    Trip t;
    string clienteip;
    int fd;
};

void writeLogFile(string ip, int origen, int destino)
{

    time_t currentTime = time(nullptr);
    tm *localTime = localtime(&currentTime);

    ofstream file("log.txt", std::ios::app);

    if (file.is_open())
    {
        file << localTime->tm_year + 1900 << localTime->tm_mon << localTime->tm_mday << "T" << localTime->tm_hour << localTime->tm_min << localTime->tm_sec << " Cliente " << ip << " busqueda - " << origen << " - " << destino << "\n";
        file.close();
    }
    else
    {
        cout << "Error opening the file.\n";
    }
}

void *
search(void *datos)
{
    int numbytes, r;
    struct thread_data *dataClient = (struct thread_data *)datos;
    char buffer[sizeof(Trip)];

    int fd = dataClient->fd; // Recuperar el descriptor de archivo del cliente

    while (true)
    {
        numbytes = recv(fd, buffer, sizeof(buffer), 0);
        if (numbytes <= 0)
            break;

        memcpy(dataClient, buffer, sizeof(Trip));

        cout << "Datos actuales:\n \torigen: " << dataClient->t.source_id << "\n\tdestino: " << dataClient->t.dstid << "\n \thora: " << dataClient->t.hod << endl;

        if (dataClient->t.mean_travel_time == -2 || dataClient->t.mean_travel_time > 0)
        {
            dataClient->t.mean_travel_time = searchTrip(dataClient->t.source_id, dataClient->t.dstid, dataClient->t.hod);

            cout << "Mean Travel Time: " << dataClient->t.mean_travel_time << endl;

            writeLogFile(dataClient->clienteip, dataClient->t.source_id, dataClient->t.dstid);
        }

        r = send(fd, &(dataClient->t.mean_travel_time), sizeof(int), 0);
        // set register in log file

        if (r < 0)
        {
            perror("error en send");
            exit(-1);
        }
    }

    close(fd);
    delete dataClient;

    pthread_exit(NULL);
}

#define BACKLOG 32
#define PORT 3550

int main(int argc, char const *argv[])
{
    pthread_t threadsId[BACKLOG];

    // Crear el socket del servidor y configurarlo
    int serverfd, r, clientfd;
    struct sockaddr_in server, client;
    socklen_t socklen;

    serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverfd < 0)
    {
        perror("error en socket");
        exit(-1);
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;
    bzero(server.sin_zero, 8);

    r = bind(serverfd, (struct sockaddr *)&server, sizeof(struct sockaddr));
    if (r < 0)
    {
        perror("error en bind");
        exit(-1);
    }

    r = listen(serverfd, BACKLOG);
    if (r < 0)
    {
        perror("error en listen");
        exit(-1);
    }

    // Bucle para aceptar conexiones de clientes y crear hilos para atenderlos
    while (true)
    {
        clientfd = accept(serverfd, (struct sockaddr *)&client, &socklen);

        // Get the client address information
        string ipAddress = inet_ntoa(client.sin_addr);
        cout << ipAddress;

        if (clientfd < 0)
        {
            perror("error en accept");
            exit(-1);
        }

        struct thread_data *clientData = new thread_data;
        clientData->fd = clientfd;
        clientData->clienteip = ipAddress;

        int error = pthread_create(&threadsId[clientfd], NULL, search, clientData);
        if (error != 0)
        {
            perror("No puedo crear hilo");
            exit(-1);
        }
    }

    close(serverfd);

    return 0;
}
