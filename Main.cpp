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
#include <sysLib.h>
#include <time.h>
#include <pthread.h>
#define socklen_t int

/* load on startup */
int alf_entrypoint();
int alf_main();
const int32_t alf_entrystatus = alf_entrypoint();
#endif

struct threadargs_t {
    MODULE_ID moduleID;
};

void *
unloadRobotThread( void* arg )
{
    struct threadargs_t *args = (struct threadargs_t *)arg;
    std::printf( "unldTask: unloading...\n" );

    unldByModuleId( args->moduleID , UNLD_CPLUS_XTOR_AUTO );

    std::printf( "unldTask: done\n" );

    pthread_exit(NULL);

    return NULL;
}

void
unloadRobot(MODULE_ID module_id) {
    pthread_t thread0;
    pthread_t thread1;
    pthread_attr_t attr;
    struct threadargs_t thread0args;
    struct threadargs_t thread1args;
    struct timespec sleepTime;

    /* set up the threadargs */
    thread0args.moduleID = module_id;
    thread1args.moduleID = module_id;

    /* thread attributes */
    pthread_attr_init(&attr);

    /* launch the first thread */
    pthread_create(&thread0, &attr, unloadRobotThread, (void *)&thread0args);

    /* wait a bit... */
    sleepTime.tv_sec = 0;
    sleepTime.tv_nsec = 1000 * 1000 * 1000;
    nanosleep( &sleepTime , NULL );
    sleep( 1 );

    /* launch the second thread */
    pthread_create(&thread1, &attr, unloadRobotThread, (void *)&thread1args);

    /* wait for them to complete */
    pthread_join(thread0, NULL);
    pthread_join(thread1, NULL);

    return;
}

void
rebootRobot()
{
    reboot( BOOT_NORMAL ); // reboot cRIO normally
}

void
reloadRobot()
{
    std::printf( "reloading...\n" );

#if 0
    // Get the task ID of the robot code if it's already running
    INT32 oldId;
    oldId = taskNameToId("pthr1");
    if ( oldId != ERROR ) {
        std::printf( "pthr1 task ID found\n" );
        taskDelete( oldId );
    }

    // Get a list of the first 50 tasks
    int idList[256];
    int numTasks = taskIdListGet( idList , 256 );

    char* name;

    std::printf( "numTasks=%d" , numTasks );

    // Delete any tasks with "FRC_" at the beginning of their names
    for ( int i = 0 ; i < numTasks ; i++ ) {
        // Get the next task name
        name = taskName( idList[i] );

        std::printf( "taskName=%s\n" , name );

        /* If the first four bytes of the name match "FRC_",
         * delete the task b/c the robot code created it
         */
        if ( std::strncmp( name , "FRC_" , 4 ) == 0 ) {
            taskDelete( idList[i] );
        }
    }
#endif

    // Find the FRC robot code module.
    MODULE_ID frcOldCode = moduleFindByName("FRC_UserProgram.out");

    if ( frcOldCode != NULL ) {
#if 0
        taskSpawn( "UnloadRobot", // Task name
                101, // Priority
                VX_FP_TASK, // Task option flag
                0xFFFF, // Stack size
                (FUNCPTR)unloadRobot, // Entry point
                (int)frcOldCode , // 1st function arg
                0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ); // Unused task args

        struct timespec sleepTime;
        sleepTime.tv_sec = 0;
        sleepTime.tv_nsec = 100;
        nanosleep( &sleepTime , NULL );

        taskSpawn( "UnloadRobotForce", // Task name
                101, // Priority
                VX_FP_TASK, // Task option flag
                0xFFFF, // Stack size
                (FUNCPTR)unloadRobot, // Entry point
                (int)frcOldCode , // 1st function arg
                0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ); // Unused task args
#endif
        unloadRobot(frcOldCode);
    }

#if 0
    int program = open( "/ni-rt/system/FRC_UserProgram.out" , O_RDONLY , 0 );
    MODULE_ID frcNewCode = loadModule( program , LOAD_ALL_SYMBOLS | LOAD_CPLUS_XTOR_AUTO );
    close( program );

    if ( frcNewCode != NULL ) {
        printf( "FRC module loaded\n" );
        VOIDFUNCPTR frcFunctionPtr;
        uint8_t symbolType;
        symFindByName( sysSymTbl , "FRC_UserProgram_StartupLibraryInit" , (char**)&frcFunctionPtr , &symbolType );
        printf( "starting robot code\n" );
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
