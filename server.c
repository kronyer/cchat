#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_CLIENTS 20
#define MAX_MSG 1024
#define NAME_LEN 50


#define PORT 8888

int clients[MAX_CLIENTS];
char client_names[MAX_CLIENTS][MAX_MSG]; //array para armazenar os nomes dos clientes
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;


void broadcast(const char* msg, int sender_fd)
{
    pthread_mutex_lock(&lock); //bloqueia o mutex
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i] != 0 && clients[i] != sender_fd) //se o cliente estiver conectado
        {
            send(clients[i], msg, strlen(msg), 0); //envia a mensagem para o cliente
        }
    }
    pthread_mutex_unlock(&lock); //desbloqueia o mutex
}

void* handleClient(void* arg)
{
    int client_fd = *(int*)arg; // o tipo do argumento é void*, entao tem que fazer o cast para int*
    char buffer[MAX_MSG]; //buffer para receber a mensagem
    ssize_t bytes_received;

    char client_name[NAME_LEN];
    recv(client_fd, client_name, sizeof(client_name), 0); //recebe o nome do cliente
    client_name[strcspn(client_name, "\n")] = 0; //remove o \n do nome do cliente
    printf("Client %s connected\n", client_name); //imprime o nome do cliente

    pthread_mutex_lock(&lock);
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i] == client_fd)
        {
            strncpy(client_names[i], client_name, NAME_LEN);
            break;
        }
    }
    pthread_mutex_unlock(&lock);

    while ((bytes_received = recv(client_fd, buffer, sizeof(buffer),0 )) > 0)
    {
        buffer[bytes_received] = '\0'; // Garante que a mensagem seja uma string válida

        time_t now = time(NULL);
        struct tm* t = localtime(&now);
        char timestamp[10];
        strftime(timestamp, sizeof(timestamp), "%H:%M:%S", t);

        char formatted_msg[MAX_MSG + NAME_LEN + 20];
        snprintf(formatted_msg, sizeof(formatted_msg), "[%s] %s: %s", timestamp, client_name, buffer);
        printf("Broadcasting: %s", formatted_msg); // Log no servidor
        broadcast(formatted_msg, client_fd); // Envia a mensagem para todos os clientes
    }

    close(client_fd); //fecha o socket do cliente

    pthread_mutex_lock(&lock); //bloqueia o mutex
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i] == client_fd) //se o cliente estiver conectado
        {
            clients[i] = 0; //remove o cliente do array
            client_names[i][0] = '\0'; // Limpa o nome do cliente
            printf("Client %s disconnected\n", client_names[i]); //imprime o nome do cliente
            break;
        }
    }
    pthread_mutex_unlock(&lock); //desbloqueia o mutex

    return NULL; //retorna NULL para a thread
}

int main()
{

    int server_fd, client_fd; //abre o socket, o so retorna um valor int do valor do socket

    struct sockaddr_in server_addr, client_addr;
    pthread_t client_thread;
    socklen_t socklen = sizeof(client_addr);


    server_fd = socket(AF_INET, SOCK_STREAM, 0); //cria o socket (ipv4, TCP, 0???)

    server_addr.sin_family = AF_INET; //define o tipo de socket
    server_addr.sin_port = htons(PORT); //define a porta do socket
    server_addr.sin_addr.s_addr = INADDR_ANY; //sem fiultro nenhum

    bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)); //associa o socket a porta e ip
    listen(server_fd, MAX_CLIENTS); //escuta o socket, 5 é o maximo de conexoes simultaneas

    printf("Server started on port %d\n", PORT);

    while(client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &socklen))
    {
        printf("New client connected\n");

        pthread_mutex_lock(&lock); //bloqueia o mutex

        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            if (clients[i] == 0) //se o cliente nao estiver conectado
            {
                clients[i] = client_fd; //adiciona o cliente ao array
                break;
            }
        }

        pthread_mutex_unlock(&lock); //desbloqueia o mutex

        pthread_create(&client_thread, NULL, handleClient, (void*) &client_fd); //cria uma thread para o cliente
        pthread_detach(client_thread); //desanexa a thread, para que ela possa ser finalizada sem esperar
    }


    close(server_fd); //fecha o socket
    printf("Server stopped\n");

    return 0;
}