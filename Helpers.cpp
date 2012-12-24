//=============================================================================
//File Name: Helpers.cpp
//Description: Contains helper functions for commands that are executed
//Author: FRC Team 3512, Spartatroniks
//=============================================================================

#include "Helpers.hpp"

#include <cstdio>
#include <unldLib.h>
#include <pthread.h>

void* unloadCodeThread( void* arg ) {
    struct threadargs_t *args = (struct threadargs_t*)arg;

    std::printf( "unldTask: unloading...\n" );
    unldByModuleId( args->moduleID , UNLD_CPLUS_XTOR_AUTO );
    std::printf( "unldTask: done\n" );

    pthread_exit( NULL );

    return NULL;
}

void unloadCode( MODULE_ID frcCodeModule ) {
    pthread_t unldThread;
    pthread_attr_t attr;
    struct threadargs_t threadArgs;

    /* set up the threadargs */
    threadArgs.moduleID = frcCodeModule;

    /* thread attributes */
    pthread_attr_init( &attr );

    /* launch the first thread */
    pthread_create( &unldThread , &attr , unloadCodeThread , (void*)&threadArgs );

    /* wait for it to complete */
    pthread_join( unldThread , NULL );
}
