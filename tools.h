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

using namespace std;

void initializeDicMap(map<ulong, string>& dicmap,
    string dicfilename = "/data/text/FastKNN/Ressources/SemMap/words.ids");

void initializeDicMapReverse(  map<string, ulong>& dicmapreverse,
    string dicfilename = "/data/text/FastKNN/Ressources/SemMap/words.ids",
    bool desax = false);

std::string escape_xml(const std::string &source);

string fastDesax(map<string, string>& dict, string toReplace) ;
void initDesax(map<string, string>& dict);



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
