#include "helpers.cpp"

struct ThreadData
{
    struct Trip *dataClient;
    int fd;
};

void *search(void *datos)
{
    int numbytes, r;
    struct ThreadData *td;
    td = (struct ThreadData *)datos;
    char buffer[sizeof(Trip)];

    while (true)
    {
        numbytes = recv(td->fd, buffer, sizeof(buffer), 0);
        memcpy(td->dataClient, buffer, sizeof(Trip));

        cout << "Datos actuales:\n \torigen: " << td->dataClient->source_id << "\n\tdestino: " << td->dataClient->dstid << "\n \thora: " << td->dataClient->hod << endl;

        r = send(td->fd, "Operacion Exitosa", 17, 0);
        if (r < 0)
        {
            perror("error en send");
            exit(-1);
        }

        if (td->dataClient->mean_travel_time == -2 || td->dataClient->mean_travel_time > 0)
        {
            td->dataClient->mean_travel_time = searchTrip(td->dataClient->source_id, td->dataClient->dstid, td->dataClient->hod);

            cout << "Mean Travel Time: " << td->dataClient->mean_travel_time << endl;
        }
    }
    close(td->fd);
}

#define BACKLOG 32
#define PORT 3550

int main(int argc, char const *argv[])
{
    // hilos que van a responder a los clientes
    Trip dataClients[BACKLOG];
    pthread_t threadsId[BACKLOG];
    int error;
    char *valor_devuelto;

    // se hacen todas las cositas del servidor, jiji
    int serverfd, r, clientfd, numbytes;
    struct sockaddr_in server, client;

    serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverfd < 0)
    {
        perror("error en socket");
        exit(-1);
    }

    socklen_t socklen;

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

    for (int i = 0; i < BACKLOG; i++)
    {
        clientfd = accept(serverfd, (struct sockaddr *)&client, &socklen);
        if (clientfd < 0)
        {
            perror("error en accept");
            exit(-1);
        }

        struct ThreadData threadata;
        threadata.dataClient = &dataClients[i];
        threadata.fd = clientfd;

        cout << "ClientFD" << clientfd << endl;
        error = pthread_create(&threadsId[i], NULL, search, (void *)(&threadata));
        if (error != 0)
        {
            perror("No puedo crear hilo");
            exit(-1);
        }

    }

    for (int i = 0; i < BACKLOG; i++)
    {
        pthread_join(threadsId[i], (void **)&valor_devuelto);
    }

    close(serverfd);
    // se llega al accept
    // y se empieza a repetir con un bucle, pero dentro de ese bucle cada vez que se acepte una nueva conexion se crear un hilo que sera el encargado
    // de responderle a ese cliente

    return 0;
}
