//=============================================================================
//File Name: Main.cpp
//Description: Unloads /ni-rt/system/FRC_UserProgram.out, then reloads it and
//             exits
//Author: FRC Team 3512, Spartatroniks
//=============================================================================

#define VxWorks

#ifdef UNIX
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#endif

#ifdef VxWorks
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <hostLib.h>
#include <ioLib.h>
#include <sockLib.h>
#include <signal.h>
#include <selectLib.h>
#include <rebootLib.h>
#include <loadLib.h>
#include <unldLib.h>
#include <moduleLib.h>
#include <sysSymTbl.h>
#include <taskLib.h>
#define socklen_t int

/* load on startup */
int alf_entrypoint();
int alf_main();
const int32_t alf_entrystatus = alf_entrypoint();
#endif

void
rebootRobot()
{
    std::printf("rebooting...\n");

#ifdef VxWorks
    reboot(0);
#endif

    std::printf("...not really!\n");

    return;
}

void
reloadRobot()
{
    // Get the task ID of the robot code if it's already running
    INT32 oldId;
    oldId = taskNameToId("pthr1");
    if ( oldId != ERROR ) {
        std::printf( "pthr1 task ID found\n" );
        taskDelete( oldId );
    }

    // Get a list of the first 50 tasks
    int idList[50];
    int numTasks = taskIdListGet( idList , 50 );

    char* name;
    char taskNameStart[5];
    taskNameStart[4] = '\0';

    // Delete any tasks with "FRC_" at the beginning of their names
    for ( int i = 0 ; i < numTasks ; i++ ) {
        // Copy the first 4 bytes of the task's name
        name = taskName( idList[i] );
        std::memcpy( taskNameStart , name , 4 );

        // If they match "FRC_", delete the task b/c the robot code created it
        if ( std::strcmp( taskNameStart , "FRC_" ) == 0 ) {
            taskDelete( idList[i] );
        }
    }

    // Find the startup code module.
    char moduleName[256];
    //strcpy("FRC_UserProgram-1.out", moduleName);
    MODULE_ID startupModId = moduleFindByName("FRC_UserProgram-1.out");
    if ( startupModId != NULL ) {
        std::printf( "unloading module\n" );
        // Remove the startup code.
        moduleDelete( startupModId );
        unldByModuleId(startupModId, UNLD_FORCE);
    }

#if 0
    int program = open( "/ni-rt/system/FRC_UserProgram-1.out" , O_RDONLY , 0 );
    MODULE_ID frcCodeModule = loadModule( program , LOAD_ALL_SYMBOLS | LOAD_CPLUS_XTOR_AUTO );
    close( program );

    if ( frcCodeModule != NULL ) {
        printf( "frc module loaded\n" );
        VOIDFUNCPTR frcFunctionPtr;
        uint8_t symbolType;
        symFindByName( sysSymTbl , "FRC_UserProgram_StartupLibraryInit" , (char**)&frcFunctionPtr , &symbolType );
        printf( "reloading...\n" );
        frcFunctionPtr();
    }
    else {
        std::printf( "...not really!\n" );
    }
#endif

    return;
}

int
procCommands(int sockfd)
{
    char buf[1024];
    char *cbuf;

    for ( cbuf = buf ; *(cbuf-1) != '\n' ; cbuf++ ) {
        if( recv( sockfd , cbuf , 1 , 0 ) != 1 ) {
            return -1;
        }
    }

    // add the null terminator
    *(cbuf-2) = '\0';

    if( std::strcmp( buf , "reboot" ) == 0 ) {
        rebootRobot();
    }
    else if( std::strcmp( buf , "reload" ) == 0 ) {
        reloadRobot();
    }

    return 0;
}

int alf_entrypoint() {
    taskSpawn( "ALF", // Task name
            101, // Priority
            VX_FP_TASK, // Task option flag
            0xFFFF, // Stack size
            (FUNCPTR)alf_main, // Entry point
            0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ); // Unused task args

    return 0;
}

int
alf_main()
{
    int sockfd, nsockfd;
    int portno;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = 3512;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    signal(SIGPIPE, SIG_IGN);


    while( 1 ) {
        // Accept commands from elsewhere as string and process them
        nsockfd = accept(sockfd, (struct sockaddr *) &cli_addr,
            &clilen);
        while( procCommands(nsockfd) == 0 );
    }

    return 0;
}
