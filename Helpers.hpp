//=============================================================================
//File Name: Helpers.hpp
//Description: Contains helper functions for commands that are executed
//Author: FRC Team 3512, Spartatroniks
//=============================================================================

#ifndef HELPERS_HPP
#define HELPERS_HPP

#include <moduleLib.h>

typedef struct threadargs_t {
    MODULE_ID moduleID;
} threadargs_t;

// This function shouldn't be called directly
void* unloadCodeThread( void* arg );

void unloadCode( MODULE_ID frcCodeModule );

#endif // HELPERS_HPP
