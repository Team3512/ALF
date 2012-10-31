//=============================================================================
//File Name: Main.cpp
//Description: Unloads ni-rt/system/FRC_UserProgram.out, then reloads it and
//             exits
//Author: FRC Team 3512, Spartatroniks
//=============================================================================

#include <loadLib.h>
#include <unldLib.h>
#include <fcntl.h>

int main() {
    unldByNameAndPath( "FRC_UserProgram.out" , "ni-rt/system" , UNLD_CPLUS_XTOR_AUTO );

    int program = open( "ni-rt/system/FRC_UserProgram.out" , O_RDONLY , 0 );
    loadModule( program , LOAD_ALL_SYMBOLS | LOAD_CPLUS_XTOR_AUTO );

    return 0;
}
