//
// Created by vladl on 10/15/2020.
//

#ifndef C_COM_STACK_CONN_PRIVATE_H
#define C_COM_STACK_CONN_PRIVATE_H

#include <com_stack_types.h>

#define CONN_HASH_TABLE_SIZE 256U

#include <stdint.h>
#include <netinet/in.h>

#ifndef __cplusplus
#define NEW(_type) ( ( _type * ) malloc ( sizeof ( _type ) ) )
#endif

#if CONN_HASH_TABLE_SIZE <= UINT8_MAX + 1
#define CONN_HASH_KEY uint8
#elif CONN_HASH_TABLE_SIZE <= UINT16_MAX + 1
#define CONN_HASH_KEY uint16
#elif CONN_HASH_TABLE_SIZE <= UINT32_MAX + 1
#define CONN_HASH_KEY uint32
#else
#define CONN_HASH_KEY uint64
#endif

#define SERVER_LISTEN_QUEUE_SIZE 10
#define SOCKET_READ_WRITE_BUFFER_SIZE 65536

#define COM_TEST_CLIENT_MESSAGE "Client OK!"
#define COM_TEST_SERVER_MESSAGE "Server OK!"

#define SOCKET_READ_ERROR -1
#define SOCKET_READ_DISCONNECT 0

#define SOCKET_WRITE_ERROR -1

typedef struct ConnectionNodeData {
    connection_t                ID;
    int                         socket;
    IPV4IP                      address;
    uint16                      port;
    struct sockaddr_in          inetAddr;
    struct ConnectionNodeData * pNext;
} ConnectionNodeData;

static connection_t currentConnectionIndex = NULL_CONNECTION + 1;

static ConnectionNodeData * connectionDataTable [ CONN_HASH_TABLE_SIZE ];
static ConnectionNodeData * getConnectionNode ( connection_t );
static void                 removeConnectionNode ( connection_t );

static int serverSocket = 0U;
static uint16 serverPort = 0U;

static bool isServer = false;

static bool isDigit(char);
static bool isUpper(char);
static bool isLower(char);
static uint32 revUInt32 (uint32);
static uint32 getFirst3Digits (uint32);

static void freeConnections ();

#endif //C_COM_STACK_CONN_PRIVATE_H
