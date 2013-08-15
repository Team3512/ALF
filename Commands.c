/*=============================================================================
File Name: Commands.c
Description: Contains functions that can be executed as commands via Telnet
Author: FRC Team 3512, Spartatroniks
=============================================================================*/

#include "Commands.h"

#include <stdio.h>

/* Used for opening files in VxWorks */
#include <ioLib.h>

/* Defines MODULE_ID typedef */
#include <moduleLib.h>

/* For FindBySymbol() function */
#include <private/symLibP.h>

/* For SYMREF_* values */
#include <private/symbolP.h>

/* Used for loading modules */
#include <loadLib.h>

/* Used for unloading modules */
#include <unldLib.h>

/* Provides functions for manipulating tasks */
#include <taskLib.h>

/* Defines reboot() function */
#include <rebootLib.h>

/* Defines BOOT_NORMAL flag */
#include <sysLib.h>

/* Provides access to global symbol table and manipulation functions */
#include <sysSymTbl.h>

/* Note: for internal use only */
MODULE_ID moduleFindBySymbolName( char* symbolName ) {
    SYMBOL* sym = NULL;

    /* When 'name' is passed, the 'value', 'type', and 'mask' are ignored */
    if ( symFindSymbol( sysSymTbl , symbolName , 0 , 0 , 0 , &sym ) !=
            ERROR ) {
        unsigned int* ref = (unsigned int*)sym->symRef;
        if ( ref == SYMREF_SHELL || ref == SYMREF_KERNEL ||
                ref == SYMREF_WTX ) {
            return NULL;
        }
        else {
            return (MODULE_ID)sym->symRef;
        }
    }
    else {
        return NULL;
    }
}

void rebootRobot() {
    /* reboot cRIO normally */
    reboot( BOOT_NORMAL );
}

void reloadRobot() {
    printf( "Reloading...\n" );

    /* Find FRC robot code module */
    char symbolName[] = "FRC_UserProgram_StartupLibraryInit";
    MODULE_ID frcOldCode = moduleFindBySymbolName( symbolName );

    /* If FRC robot code was found, unload it */
    if ( frcOldCode != NULL ) {
        printf( "Unloading robot code module...\n" );
        //unldByModuleId( frcOldCode , 0 );
        unldByModuleId( frcOldCode , UNLD_CPLUS_XTOR_AUTO );
        printf( "Robot code module unloaded\n" );
    }
    else {
        printf( "Robot code module not found\n" );
    }

    /* Load the new FRC code module */
    printf( "Reloading robot code module...\n" );
    int program = open( "/ni-rt/system/FRC_UserProgram.out" , O_RDONLY , 0 );
    MODULE_ID frcNewCode = loadModule( program , LOAD_ALL_SYMBOLS | LOAD_CPLUS_XTOR_AUTO );
    close( program );

    /* If the FRC code module loaded correctly, call its entry point */
    if ( frcNewCode != NULL ) {
        printf( "Robot code module loaded\n" );

        VOIDFUNCPTR frcFunctionPtr;
        uint8_t symbolType;

        char strEntryPointName[] = "FRC_UserProgram_StartupLibraryInit";
        symFindByName( sysSymTbl , strEntryPointName , (char**)&frcFunctionPtr , &symbolType );
        printf( "Starting robot code...\n" );
        frcFunctionPtr();
    }
    else {
        printf( "Robot code module failed to load\n" );
    }
}

void listTasks() {
    int idList[256];

    int numTasks = taskIdListGet( idList , 256 );

    printf( "Task ID : Name\n" );
    int i;
    for ( i = 0 ; i < numTasks ; i++ ) {
        printf( "%d: %s\n" , idList[i] , taskName( idList[i] ) );
    }
}
