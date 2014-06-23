//
// Description: 
//
//
// Author: Claire Mouton, Guillaume Pitel <claire.mouton@cea.fr, guillaume.pitel@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "tools.h"

void initializeDicMap(std::map<ulong, std::string>& dicmap, std::string dicfilename ) {
  /***** initialize dicmap ***********/
  std::ifstream is(dicfilename);
  if (is.fail()) {
    std::cerr << "Oops, " << dicfilename << " doesn't exist. " << __FILE__ << ":" << __LINE__ << std::endl;
    exit(-1);
  }

  std::string s;
  while (getline(is, s)) {
    std::stringstream ss;
    ss << s;
    ulong id;
    int size = s.find(" ");
    char word[size];
    ss.read(word, size);
    ss.ignore(1, ' ');
    ss>> id;
    std::stringstream ss2;
    ss2.write(word, size) ;
    ss2 >>     dicmap[id];
  }
  is.close();

}

void initializeDicMapReverse(std::map<std::string, ulong>& dicmapreverse,   std::string dicfilename , bool desaxIdent ) {
  std::map<std::string, std::string> dict; 
  initDesax(dict);
  /***** initialize dicmap ***********/
  std::ifstream is(dicfilename.c_str());
  std::string s;
  while (getline(is, s)) {
    std::stringstream ss; 
    ss << s;
    ulong id;
    int size = s.find(" ");
    char word[size];
    ss.read(word, size);
    ss.ignore(1, ' ');
    ss>> id;
    std::stringstream ss2;
    ss2.write(word, size) ;
    dicmapreverse[ss2.str()]=id;
    if (desaxIdent) {
      dicmapreverse[fastDesax(dict,ss2.str())]=id;
    }
  }
  is.close();

}

/* If it exists in a library we're using (Boost, XML parser...), then replace it! */
std::string escape_xml(const std::string &source)
{
  std::string escaped(source);
  std::string::size_type pos = 0;
  for (;;)
  {
    pos = escaped.find_first_of("\"&<>", pos);
    if (pos == std::string::npos) break;
    std::string replacement;
    switch (escaped[pos])
    {
      case '\"': replacement = "&quot;"; break;
      case '&':  replacement = "&amp;";  break;
      case '<':  replacement = "&lt;";   break;
      case '>':  replacement = "&gt;";   break;
      default: ;
    }
    escaped.replace(pos, 1, replacement);
    pos += replacement.size();
  };

  return escaped;
}

std::string fastDesax(std::map<std::string, std::string>& dict, std::string toReplace) {
  std::string res = toReplace;
  for (std::map<std::string, std::string>::iterator it= dict.begin(); it != dict.end(); it++)
    {
      std::string strMine=(it->first);
      size_t found=res.find(strMine);
      while (found != std::string::npos)
      {
          std::string strAux=(it->second);
          res.erase(found,2);
          res.insert(found,strAux);
          found=res.find(strMine,found+1);
      }
    } 
  return res;
}


void initDesax(std::map<std::string, std::string>& dict) {
  dict["á"] = "a";
  dict["à"] = "a";
  dict["â"] = "a";
  dict["ä"] = "a";
  dict["ç"] = "c";
  dict["é"] = "e";
  dict["è"] = "e";
  dict["ê"] = "e";
  dict["ë"] = "e";
  dict["É"] = "E";
  dict["î"] = "i";
  dict["í"] = "i";
  dict["ï"] = "i";
  dict["ó"] = "o";
  dict["ô"] = "o";
  dict["ö"] = "o";
  dict["ú"] = "u";
  dict["û"] = "u";
  dict["ü"] = "u";
  dict["ù"] = "u";
  dict["ñ"] = "n";

}


bool vectorIndexEqual ( vectorCount i,vectorCount j )
{
  return i.index == j.index;
}

