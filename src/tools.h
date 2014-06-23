//
// Description: 
//
//
// Author: Claire Mouton, Guillaume Pitel <claire.mouton@cea.fr, guillaume.pitel@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef TOOLS_H
#define TOOLS_H

#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/types.h>



void initializeDicMap(std::map<ulong, std::string>& dicmap,
    std::string dicfilename);

void initializeDicMapReverse(std::map<std::string, ulong>& dicmapreverse,
    std::string dicfilename,
    bool desax = false);

std::string escape_xml(const std::string &source);

std::string fastDesax(std::map<std::string, std::string>& dict, std::string toReplace) ;
void initDesax(std::map<std::string, std::string>& dict);



typedef struct
{
        int index;
        int count;
} vectorCount;


bool vectorIndexEqual(vectorCount i, vectorCount j);



typedef struct
{
        int index;
        float mi;
} vectorMI;





#endif
