//
// Created by vladl on 10/15/2020.
//

#include <connection.h>
#include <connection-private.h>
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

IPV4IP strToIPV4IP ( const char * pStr, char ** ppNext) {
    IPV4IP address;
    memset ( & address, 0, sizeof ( IPV4IP ) );

    while ( ! ( isDigit (* pStr ) ) ) {
        pStr++;
    }

    uint32 firstEight   = 0U;
    uint32 secondEight  = 0U;
    uint32 thirdEight   = 0U;
    uint32 fourthEight  = 0U;

    char * endPtr = NULL;
    firstEight = strtol ( pStr, & endPtr, 10);

    if ( * endPtr != '.' || endPtr - pStr > 3 ) {
        firstEight = getFirst3Digits( firstEight );
        if ( ppNext != NULL )
            (* ppNext) = (endPtr - pStr > 3) ? (char*)pStr + 3 : endPtr;
        goto _ASSIGN_ADDR;
    }

    pStr = endPtr + 1;
    secondEight = strtol ( pStr, & endPtr, 10 );
    if ( * endPtr != '.' || endPtr - pStr > 3 ) {
        secondEight = getFirst3Digits( secondEight );
        if ( ppNext != NULL )
            (* ppNext) = (endPtr - pStr > 3) ? (char*)pStr + 3 : endPtr;
        goto _ASSIGN_ADDR;
    }

    pStr = endPtr + 1;
    thirdEight = strtol ( pStr, & endPtr, 10 );
    if ( * endPtr != '.' || endPtr - pStr > 3 ) {
        if ( ppNext != NULL )
            (* ppNext) = (endPtr - pStr > 3) ? (char*)pStr + 3 : endPtr;
        thirdEight = getFirst3Digits( thirdEight );
        goto _ASSIGN_ADDR;
    }

    pStr = endPtr + 1;
    fourthEight = strtol ( pStr, & endPtr, 10 );

    if ( endPtr - pStr > 3 )
        fourthEight = getFirst3Digits( fourthEight );
    if ( ppNext != NULL )
        (* ppNext) = (endPtr - pStr > 3) ? (char*)pStr + 3 : endPtr;

    _ASSIGN_ADDR:
    address.firstEight  = (uint8) firstEight;
    address.secondEight = (uint8) secondEight;
    address.thirdEight  = (uint8) thirdEight;
    address.fourthEight = (uint8) fourthEight;

    return address;
}

char * IPV4IPToStr ( IPV4IP ip, char * buffer ) {
    sprintf ( buffer, "%d.%d.%d.%d", ip.firstEight, ip.secondEight, ip.thirdEight, ip.fourthEight );
    return buffer;
}

uint32 getFirst3Digits ( uint32 number ) {
    return revUInt32( revUInt32( number ) % 1000);
}

bool isDigit ( char c ) {
    return c >= '0' && c <= '9';
}

bool isUpper ( char c ) {
    return c >= 'A' && c <= 'Z';
}

bool isLower ( char c ) {
    return c >= 'a' && c <= 'z';
}

uint32 revUInt32 ( uint32 number ) {
    uint32 rev = 0U;

    while ( number ) {
        rev = rev * 10 + number % 10;
        number /= 10;
    }

    return rev;
}

ComResult initComModule () {
    return COM_SUCCESS;
}

in_addr_t IPV4IPToInAddr ( IPV4IP ip ) {
    static char buffer [32];
    return inet_addr ( IPV4IPToStr( ip, buffer ) );
}

ConnectionNodeData * getConnectionNode ( connection_t ID ) {
    if ( ID == NULL_CONNECTION )
        return NULL;

    CONN_HASH_KEY key = ( CONN_HASH_KEY ) ( ID % CONN_HASH_TABLE_SIZE );

    __auto_type listHead = connectionDataTable [ key ];

    while ( listHead != NULL ) {
        if ( listHead->ID == ID )
            return listHead;

        listHead = listHead->pNext;
    }

    __auto_type pNewNode = NEW( ConnectionNodeData );

    pNewNode->ID = ID;
    pNewNode->address.addr = 0U;
    pNewNode->port = 0U;
    pNewNode->socket = 0U;
    pNewNode->pNext = listHead;
    pNewNode->connected = false;

    return ( connectionDataTable[ key ] = pNewNode );
}

void removeConnectionNode ( connection_t ID ) {
    if ( ID == NULL_CONNECTION )
        return;

    CONN_HASH_KEY key = ( CONN_HASH_KEY ) ( ID % CONN_HASH_TABLE_SIZE );

    __auto_type listHead = connectionDataTable [ key ];

    if ( listHead == NULL )
        return;

    if ( listHead->pNext == NULL ) {
        if ( listHead->ID == ID ) {
            free(listHead);
            connectionDataTable[key] = NULL;
            return;
        }
    }

    while ( listHead->pNext != NULL ) {
        if ( listHead->pNext->ID == ID ) {
            __auto_type p = listHead->pNext;
            listHead->pNext = listHead->pNext->pNext;
            free(p);
            return;
        }

        listHead = listHead->pNext;
    }
}

ComResult createIPV4ConnectionToServer ( connection_t * pID, IPV4IP ip, uint16 port ) {
    if ( pID == NULL ) return COM_ID_PTR_NULL;
    if ( ip.addr == 0 ) return COM_IP_INVALID;
    if ( port == 0 ) return COM_PORT_INVALID;

    * pID = currentConnectionIndex ++;
    __auto_type dataNode = getConnectionNode( * pID );

    dataNode->port = port;
    dataNode->address = ip;

    dataNode->inetAddr.sin_addr.s_addr = IPV4IPToInAddr( ip );
    dataNode->inetAddr.sin_port = port;
    dataNode->inetAddr.sin_family = AF_INET;

    if ( -1 == ( dataNode->socket = socket( AF_INET, SOCK_STREAM, IPPROTO_IP ) ) )
        return COM_SOCKET_CREATION_FAILURE;

    return COM_SUCCESS;
}

ComResult createIPV4Server ( uint16 port ) {
//    if ( pID == NULL ) return COM_ID_PTR_NULL;
    if ( port == 0 ) return COM_PORT_INVALID;
    serverPort = port;

    struct sockaddr_in sockaddrIn = {
            .sin_port = port,
            .sin_addr.s_addr = INADDR_ANY,
            .sin_family = AF_INET
    };

    if ( -1 == ( serverSocket = socket ( AF_INET, SOCK_STREAM, IPPROTO_IP ) ) )
        return COM_SOCKET_CREATION_FAILURE;
    if ( -1 == ( bind( serverSocket, ( struct sockaddr * ) & sockaddrIn, sizeof ( sockaddrIn ) ) ) )
        return COM_SOCKET_BIND_FAILURE;
    if ( -1 == listen( serverSocket, SERVER_LISTEN_QUEUE_SIZE ) )
        return COM_SOCKET_LISTEN_FAILURE;

    _isServer = true;
    return COM_SUCCESS;
}

ComResult connectToServer ( connection_t ID ) {
    __auto_type pData = getConnectionNode ( ID );
    if ( pData )
        return COM_ID_PTR_NULL;

    if ( -1 == connect( pData->socket, ( struct sockaddr * ) & pData->inetAddr, sizeof ( struct sockaddr_in ) ) )
        return COM_CONNECT_FAIL;

    pData->connected = true;
    return COM_SUCCESS;
}

ComResult acceptClient ( connection_t * pID ) {
    if ( pID == NULL )
        return COM_ID_PTR_NULL;

    * pID = currentConnectionIndex++;
    __auto_type pNode = getConnectionNode( * pID );
    socklen_t peerAddressSize = sizeof ( pNode->inetAddr );

    if ( -1 == ( pNode->socket = accept( serverSocket, (struct sockaddr *) & pNode->inetAddr, & peerAddressSize ) ) )
        return COM_ACCEPT_FAIL;

    pNode->port = serverPort;
    pNode->address = inetAddrToIPV4IP( pNode->inetAddr.sin_addr.s_addr );
    pNode->connected = true;

    return COM_SUCCESS;
}

IPV4IP inetAddrToIPV4IP ( in_addr_t addr ) {
    return (IPV4IP) {.addr = (
            ( ( ( addr >> 0U ) % 256 ) << 24U ) |
            ( ( ( addr >> 8U ) % 256 ) << 16U ) |
            ( ( ( addr >> 16U) % 256 ) << 8U  ) |
            ( ( ( addr >> 24U) % 256 ) << 0U  )
        )
    };
}

void freeConnections () {
    for ( uint64 key = 0U; key < CONN_HASH_TABLE_SIZE; key ++ ) {
        while ( connectionDataTable [ key ] != NULL ) {
            __auto_type p = connectionDataTable[key];
            connectionDataTable[key] = connectionDataTable[key]->pNext;
            free(p);
        }
    }
}

ComResult stopComModule () {
    freeConnections();

    return COM_SUCCESS;
}

ComResult testConnection ( connection_t ID ) {
    __auto_type pNode = getConnectionNode( ID );
    if ( pNode == NULL )
        return COM_ID_PTR_NULL;

    char buffer[ SOCKET_READ_WRITE_BUFFER_SIZE ];
    if ( _isServer ) {
        switch( read ( pNode->socket, buffer, SOCKET_READ_WRITE_BUFFER_SIZE ) ) {
            case SOCKET_READ_ERROR :        return COM_TRANSMIT_ERROR;
            case SOCKET_READ_DISCONNECT :   return COM_ERROR_DISCONNECT;
            default :                       break;
        }

        if ( 0 != strcmp ( buffer, COM_TEST_CLIENT_MESSAGE ) )
            return COM_TEST_CLIENT_NOT_OK;

        strcpy ( buffer, COM_TEST_SERVER_MESSAGE );
        switch ( write ( pNode->socket, buffer, SOCKET_READ_WRITE_BUFFER_SIZE ) ) {
            case SOCKET_WRITE_ERROR :       return COM_TRANSMIT_ERROR;
            default :                       break;
        }
    } else {
        strcpy ( buffer, COM_TEST_CLIENT_MESSAGE );
        switch ( write ( pNode->socket, buffer, SOCKET_READ_WRITE_BUFFER_SIZE ) ) {
            case SOCKET_READ_ERROR :        return COM_TRANSMIT_ERROR;
            default :                       break;
        }

        switch ( read ( pNode->socket, buffer, SOCKET_READ_WRITE_BUFFER_SIZE ) ) {
            case SOCKET_READ_ERROR :        return COM_TRANSMIT_ERROR;
            case SOCKET_READ_DISCONNECT :   return COM_ERROR_DISCONNECT;
            default :                       break;
        }

        if ( 0 != strcmp ( buffer, COM_TEST_SERVER_MESSAGE ) )
            return COM_TEST_SERVER_NOT_OK;
    }
    return COM_SUCCESS;
}

ComResult disconnect ( connection_t ID ) {
    if ( ID == NULL_CONNECTION )
        return COM_ID_PTR_NULL;

    close ( getConnectionNode(ID)->socket );
    removeConnectionNode( ID );

    return COM_SUCCESS;
}

//ComResult disconnectClient ( connection_t ID ) {
//    return disconnect( ID );
//}

ComResult stopServer () {
    close ( serverSocket );
    return COM_SUCCESS;
}


const char * comStackErrorToString ( ComResult errorStatus ) {
    switch ( errorStatus ) {
        case COM_ID_PTR_NULL:               return "Given Pointer is invalid";
        case COM_IP_INVALID:                return "Ip address invalid";
        case COM_PORT_INVALID:              return "Port invalid";
        case COM_SOCKET_CREATION_FAILURE:   return "Socket create failure";
        case COM_SOCKET_BIND_FAILURE:       return "Socket Bind failure";
        case COM_SOCKET_LISTEN_FAILURE:     return "Socket listen failure";
        case COM_CONNECT_FAIL:              return "Connect failure";
        case COM_ACCEPT_FAIL:               return "Accept failure";
        case COM_ERROR_DISCONNECT:          return "Read/Write disconnect";
        case COM_TRANSMIT_ERROR:            return "Read/Write error";
        case COM_TEST_CLIENT_NOT_OK:        return "Test : client message not ok";
        case COM_TEST_SERVER_NOT_OK:        return "Test : server message not ok";
        case COM_SUCCESS:                   return "Success";
        default:                            return "Undefined behaviour";
    }
}

ComResult sendChar ( connection_t ID, char character ) {
    __auto_type pNode = getConnectionNode( ID );
    if ( pNode == NULL )
        return COM_ID_PTR_NULL;

    switch ( write ( pNode->socket, & character, sizeof ( char ) ) ) {
        case SOCKET_WRITE_ERROR:    return COM_TRANSMIT_ERROR;
        default:                    return COM_SUCCESS;
    }
}

ComResult sendInt ( connection_t ID, int value ) {
    __auto_type pNode = getConnectionNode( ID );
    if ( pNode == NULL )
        return COM_ID_PTR_NULL;

    switch ( write ( pNode->socket, & value, sizeof ( int ) ) ) {
        case SOCKET_WRITE_ERROR:    return COM_TRANSMIT_ERROR;
        default:                    return COM_SUCCESS;
    }
}

ComResult sendLong ( connection_t ID, long long value ) {
    __auto_type pNode = getConnectionNode( ID );
    if ( pNode == NULL )
        return COM_ID_PTR_NULL;

    switch ( write ( pNode->socket, & value, sizeof ( long long ) ) ) {
        case SOCKET_WRITE_ERROR:    return COM_TRANSMIT_ERROR;
        default:                    return COM_SUCCESS;
    }
}

ComResult sendString ( connection_t ID, const char * str ) {
    return sendBuffer ( ID, (const void*) str, strlen(str) );
}

ComResult sendBuffer ( connection_t ID, const void * pData, uint32 size ) {
    __auto_type pNode = getConnectionNode( ID );
    if ( pNode == NULL )
        return COM_ID_PTR_NULL;

    void * writeBuffer = NEW_COMM_BUFFER();
    memcpy ( writeBuffer, pData, size > SOCKET_READ_WRITE_BUFFER_SIZE ? SOCKET_READ_WRITE_BUFFER_SIZE : size );

    switch ( write ( pNode->socket, writeBuffer, SOCKET_READ_WRITE_BUFFER_SIZE ) ) {
        case SOCKET_WRITE_ERROR:
            free( writeBuffer );
            return COM_TRANSMIT_ERROR;

        default:
            if ( size > SOCKET_READ_WRITE_BUFFER_SIZE ) {
                free( writeBuffer );
                return COM_WRITE_STRING_TRUNCATED;
            }

            free( writeBuffer );
            return COM_SUCCESS;
    }
}

ComResult readChar ( connection_t ID, char * pCharacter ) {
    __auto_type pNode = getConnectionNode( ID );
    if ( pNode == NULL )
        return COM_ID_PTR_NULL;

    switch ( read ( pNode->socket, pCharacter, sizeof ( char ) ) ) {
        case SOCKET_READ_ERROR:         return COM_TRANSMIT_ERROR;
        case SOCKET_READ_DISCONNECT:    return COM_ERROR_DISCONNECT;
        default:
            return COM_SUCCESS;
    }
}

ComResult readInt ( connection_t ID, int * pValue ) {
    __auto_type pNode = getConnectionNode( ID );
    if ( pNode == NULL )
        return COM_ID_PTR_NULL;

    int bytesRead = read ( pNode->socket, pValue, sizeof ( int ) );

    switch ( bytesRead ) {
        case SOCKET_READ_ERROR:         return COM_TRANSMIT_ERROR;
        case SOCKET_READ_DISCONNECT:    return COM_ERROR_DISCONNECT;
        default:
            if ( bytesRead < sizeof( int ) )
                return COM_READ_VALUE_TRUNCATED;
            return COM_SUCCESS;
    }
}

ComResult readLong ( connection_t ID, long long * pValue ) {
    __auto_type pNode = getConnectionNode( ID );
    if ( pNode == NULL )
        return COM_ID_PTR_NULL;

    int bytesRead = read ( pNode->socket, pValue, sizeof ( long long ) );

    switch ( bytesRead ) {
        case SOCKET_READ_ERROR:         return COM_TRANSMIT_ERROR;
        case SOCKET_READ_DISCONNECT:    return COM_ERROR_DISCONNECT;
        default:
            if ( bytesRead < sizeof( long long ) )
                return COM_READ_VALUE_TRUNCATED;
            return COM_SUCCESS;
    }
}

ComResult readBuffer ( connection_t ID, void * pData, uint32 bufferSize ) {
    __auto_type pNode = getConnectionNode( ID );
    if ( pNode == NULL )
        return COM_ID_PTR_NULL;

    __auto_type buffer = NEW_COMM_BUFFER();

    int bytesRead = read ( pNode->socket, buffer, SOCKET_READ_WRITE_BUFFER_SIZE );
    memcpy ( pData, buffer, bufferSize > SOCKET_READ_WRITE_BUFFER_SIZE ? SOCKET_READ_WRITE_BUFFER_SIZE : bufferSize );

    switch ( bytesRead ) {
        case SOCKET_READ_ERROR:
            free ( buffer );
            return COM_TRANSMIT_ERROR;
        case SOCKET_READ_DISCONNECT:
            free ( buffer );
            return COM_ERROR_DISCONNECT;
        default:
            if ( bytesRead < SOCKET_READ_WRITE_BUFFER_SIZE ) {
                free ( buffer );
                return COM_READ_VALUE_TRUNCATED;
            }

            free ( buffer );
            return COM_SUCCESS;
    }
}

ComResult readString ( connection_t ID, char * pData, uint32 length ) {
    return readBuffer ( ID, (void*) pData, length );
}

bool isServer() {
    return _isServer;
}

bool isConnected ( connection_t ID ) {
    __auto_type pNode = getConnectionNode( ID );
    return pNode == NULL ? false : pNode->connected;
}