#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_MSG 1024
#define PORT 8888
#define NAME_LEN 50

void* handleMessages(void* arg)
{
    int client_fd = *(int*)arg; // O tipo do argumento é void*, então tem que fazer o cast para int*
    char buffer[MAX_MSG]; // Buffer para receber a mensagem
    ssize_t size;

    while ((size = recv(client_fd, buffer, MAX_MSG, 0)) > 0)
    {
        buffer[size] = '\0'; // Adiciona o caractere nulo no final da string
        printf("%s\n", buffer); // Imprime a mensagem recebida
    }

    return NULL;
}

int main()
{
    int client_fd;
    struct sockaddr_in sockaddr;
    pthread_t listener;
    char msg[MAX_MSG]; // Buffer para enviar a mensagem
    char name[NAME_LEN];

    printf("Enter your name: ");
    fgets(name, NAME_LEN, stdin);
    name[strcspn(name, "\n")] = '\0'; // Remove o '\n' do final

    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(PORT);
    sockaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(client_fd, (struct sockaddr*)&sockaddr, sizeof(sockaddr));
    printf("Connected to server\n");

    send(client_fd, name, strlen(name), 0);


    // Cria uma cópia do descritor de arquivo para a thread
    int* client_fd_copy = malloc(sizeof(int));
    *client_fd_copy = client_fd;

    pthread_create(&listener, NULL, handleMessages, client_fd_copy); // Cria a thread para receber mensagens

               while (fgets(msg, MAX_MSG, stdin))
        {
            msg[strcspn(msg, "\n")] = '\0'; // Remove o '\n' do final da mensagem
            send(client_fd, msg, strlen(msg), 0); // Envia apenas o texto da mensagem para o servidor
            printf("You: %s\n", msg); // Exibe a mensagem enviada localmente
        }

    close(client_fd); // Fecha o socket do cliente
    free(client_fd_copy); // Libera a memória alocada

    return 0;
}