//
// Created by vladl on 10/16/2020.
//

#include <connection.h>
#include <stdio.h>

int main(){
    printf( "%s\n", comStackErrorToString( initComModule() ) );
    connection_t serverConnection;

    printf( "%s\n", comStackErrorToString( createIPV4ConnectionToServer( & serverConnection, LOCALHOST, 6000 ) ) );
    printf( "%s\n", comStackErrorToString( connectToServer( serverConnection ) ) );
    printf( "%s\n", comStackErrorToString( testConnection( serverConnection ) ) );
    printf( "%s\n", comStackErrorToString( disconnect( serverConnection ) ) );
    printf( "%s\n", comStackErrorToString( stopComModule() ) );
}