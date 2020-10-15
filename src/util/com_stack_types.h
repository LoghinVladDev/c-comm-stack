//
// Created by vladl on 10/15/2020.
//

#ifndef C_COM_STACK_COM_STACK_TYPES_H
#define C_COM_STACK_COM_STACK_TYPES_H

/**
 * AMD + Intel processors use Little Endian
 *
 * Test processor endianness :

    IPV4IP ip;
    ip.addr = (20U << 24U) | (69U << 16U) | (124U << 8U) | (144U);
    printf("%d.%d.%d.%d\n", ip.firstEight, ip.secondEight, ip.thirdEight, ip.fourthEight );

    If:
        ip.firstEight   = 20
        ip.secondEight  = 69
        ip.thirdEight   = 124
        ip.fourthEight  = 144

        -> little endian

    else if:
        ip.firstEight   = 144
        ip.secondEight  = 124
        ip.thirdEight   = 69
        ip.fourthEight  = 20

        -> big endian

        if big endian, #define BIG_ENDIAN

    else:
        idk man
*/
#if ! defined(LITTLE_ENDIAN) && ! defined(BIG_ENDIAN)
#define LITTLE_ENDIAN
#endif

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;

typedef char sint8;
typedef short sint16;
typedef int sint32;
typedef long long sint64;

#ifndef _STDBOOL_H
#ifndef __cplusplus

typedef uint8 bool;
#define true 1
#define false 0

#endif
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif

#define NULL_CONNECTION ((uint64)0U)

typedef uint64 connection_t;

typedef enum {
    COM_ID_PTR_NULL,
    COM_IP_INVALID,
    COM_PORT_INVALID,

    COM_SOCKET_CREATION_FAILURE,
    COM_SOCKET_BIND_FAILURE,
    COM_SOCKET_LISTEN_FAILURE,

    COM_CONNECT_FAIL,
    COM_ACCEPT_FAIL,

    COM_ERROR_DISCONNECT,
    COM_TRANSMIT_ERROR,

    COM_TEST_CLIENT_NOT_OK,
    COM_TEST_SERVER_NOT_OK,

    COM_SUCCESS
} ComResult;

#if defined(LITTLE_ENDIAN)
typedef union {
    struct {
        uint8 fourthEight;
        uint8 thirdEight;
        uint8 secondEight;
        uint8 firstEight;
    };
    uint32 addr;
} IPV4IP;
#elif defined(BIG_ENDIAN)
typedef union {
    struct {
        uint8 firstEight;
        uint8 secondEight;
        uint8 thirdEight;
        uint8 fourthEight;
    };
    uint32 addr;
} IPV4IP;
#endif

#endif //C_COM_STACK_COM_STACK_TYPES_H
