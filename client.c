/**
 * @file: client.c
 * @date: 2016-11-17
 * @author: autor
 */
#include <stdio.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "memory.h"
#include "debug.h"
#include "common.h"
#include "client_opt.h"

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
    int udp_client_socket;
    if ((udp_client_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        ERROR(21, "Can't create udp_client_socket (IPv4)");

    socklen_t udp_server_endpoint_length = sizeof(struct sockaddr_in);
    struct sockaddr_in udp_server_endpoint;
    memset(&udp_server_endpoint, 0, sizeof(struct sockaddr_in));
    udp_server_endpoint.sin_family = AF_INET;
    switch (inet_pton(AF_INET, args.ip_arg, &udp_server_endpoint.sin_addr.s_addr))
    {
    case 0:
        fprintf(stderr, "[%s@%d] ERROR - Cannot convert IP address (IPv4): Invalid Network Address\n",
                __FILE__, __LINE__);
        exit(22);
    case -1:
        ERROR(22, "Cannot convert IP address (IPv4)");
    }
    udp_server_endpoint.sin_port = htons(args.port_arg);

    char userString[MAX_SIZE];
    char stringReceived[MAX_SIZE];
    strncpy(userString, args.UTF8_arg, MAX_SIZE);

    ssize_t udp_read_bytes, udp_sent_bytes;

    //printf("String user: %s\n", userString);

    printf("CLIENT] Sending data to server\n");
    fflush(stdout);
    if ((udp_sent_bytes = sendto(udp_client_socket, userString, strlen(userString) + 1, 0, (struct sockaddr *)&udp_server_endpoint, udp_server_endpoint_length)) == -1)
        ERROR(24, "Can't sendto server");
    printf("CLIENT] Data sent (%d bytes sent)\n", (int)udp_sent_bytes);

    printf("[CLIENT] Waiting for server's answer");
    fflush(stdout);
    if ((udp_read_bytes = recvfrom(udp_client_socket, stringReceived, MAX_SIZE, 0, (struct sockaddr *)&udp_server_endpoint, &udp_server_endpoint_length)) == -1)
        ERROR(25, "Can't recvfrom server");
    printf("\n\n[CLIENT] Answer 'Number of simbols: %s'\n", stringReceived);

    //printf("String recebida: %s\n", stringReceived);

    if (close(udp_client_socket) == -1)
        ERROR(23, "Can't close udp_client_socket (IPv4)");

    cmdline_parser_free(&args);

    return 0;
}
