/**
 * @file: server.c
 * @date: 2016-11-17
 * @author: autor
 */
#include <stdio.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "debug.h"
#include "common.h"
#include "server_opt.h"

int main(int argc, char *argv[])
{
    /* Estrutura gerada pelo utilitario gengetopt */
    struct gengetopt_args_info args;

    /* Processa os parametros da linha de comando */
    if (cmdline_parser(argc, argv, &args) != 0)
    {
        exit(ERR_ARGS);
    }

    if (args.port_arg < 1 || args.port_arg > 65535)
    {
        ERROR(2, "Port has to be between 0 and 65535");
    }

    int udp_server_socket;
    if ((udp_server_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        ERROR(31, "Can't create udp_server_socket (IPv4)");

    struct sockaddr_in udp_server_endpoint;
    memset(&udp_server_endpoint, 0, sizeof(struct sockaddr_in));
    udp_server_endpoint.sin_family = AF_INET;
    udp_server_endpoint.sin_addr.s_addr = htonl(INADDR_ANY); // Todas as interfaces de rede
    udp_server_endpoint.sin_port = htons(args.port_arg);     // Server port
    if (bind(udp_server_socket, (struct sockaddr *)&udp_server_endpoint, sizeof(struct sockaddr_in)) == -1)
        ERROR(32, "Can't bind @udp_server_endpoint info");

    socklen_t udp_client_endpoint_length = sizeof(struct sockaddr_in);
    struct sockaddr_in udp_client_endpoint;
    ssize_t udp_read_bytes, udp_sent_bytes;

    char stringReceived[MAX_SIZE];
    char stringSended[MAX_SIZE];
    uint16_t numberOfbytes = 0;
    int simbols = 0;

    while (1)
    {
        printf("[SERVER] Waiting for clients UDP/%d\n", args.port_arg);
        fflush(stdout);
        if ((udp_read_bytes = recvfrom(udp_server_socket, stringReceived, MAX_SIZE, 0, (struct sockaddr *)&udp_client_endpoint, &udp_client_endpoint_length)) == -1)
            ERROR(34, "Can't recvfrom client");
        printf("[SERVER]:  (%d bytes received)('%s')\n", (int)udp_read_bytes, stringReceived);

        //strcpy(stringSended, stringReceived);

        for (int i = 0; i < (int)strlen(stringReceived); i++)
        {
            simbols++;
            //printf("String received: %s, %d\n", stringReceived, i);

            switch ((stringReceived[i] & (1 << 7)))
            {
            case 0:
                numberOfbytes = 1;
                printf("[INFO] symbol %d has %d bytes(s)\n", i + 1, numberOfbytes);
                continue;

            case 1:
                if ((stringReceived[i] & (1 << 5)) == 0)
                {
                    numberOfbytes = 2;
                }
                if ((stringReceived[i] & (1 << 4)) == 0)
                {
                    numberOfbytes = 3;
                }
                if ((stringReceived[i] & (1 << 3)) == 0)
                {
                    numberOfbytes = 4;
                }
                printf("[INFO] symbol %d has %d bytes(s)\n", i + 1, numberOfbytes);
                continue;
            }
        }
        printf("%d symbols in '%s' (%d bytes)\n", simbols, stringReceived, numberOfbytes);

        //printf("Numero de simbolos: %d\n", simbols);

        snprintf(stringSended, MAX_SIZE, "%d", simbols);

        printf("[SERVER] Sending answer to client\n");
        fflush(stdout);
        if ((udp_sent_bytes = sendto(udp_server_socket, stringSended, strlen(stringSended) + 1, 0, (struct sockaddr *)&udp_client_endpoint, udp_client_endpoint_length)) == -1)
            ERROR(35, "Can't sendto client");
        printf("[SERVER] answer sent to client (%d bytes)\n", (int)udp_sent_bytes);

        //printf("String enviada: %s\n", stringSended);
    }

    if (close(udp_server_socket) == -1)
        ERROR(33, "Can't close udp_server_socket (IPv4)");

    cmdline_parser_free(&args);

    return 0;
}
