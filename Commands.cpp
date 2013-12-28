/*=============================================================================
File Name: Commands.cpp
Description: Contains functions that can be executed as commands via Telnet
Author: FRC Team 3512, Spartatroniks
=============================================================================*/

#include "Commands.h"

#include <iostream>
#include <fstream>

extern "C"
void alf_copy( const char* fromName , const char* toName ) {
        std::cout << "Copying " << fromName << " to " << toName << "...";

        std::ifstream from( fromName , std::fstream::binary );

        if ( !from.is_open() ) {
                std::cout << " failed to open input file\n";
        }
        else {
                std::ofstream to( toName , std::fstream::trunc | std::fstream::binary );

                to << from.rdbuf();

                from.close();
                to.close();

                std::cout << " done\n";
        }
}

