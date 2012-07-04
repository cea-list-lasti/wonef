#include "Tools.hpp"
#include <algorithm>

using namespace std;

string tolower(string strToConvert)
{//change each element of the string to lower case
   for(unsigned int i=0;i<strToConvert.length();i++)
   {
      strToConvert[i] = tolower(strToConvert[i]);
   }
   return strToConvert;//return the converted string
}

// parsing XML tools
string _transcode(const XMLCh* const chars,
		  xercesc::XMLTranscoder* theTranscoder) {
  XMLSize_t length = xercesc::XMLString::stringLen(chars);
  const XMLCh* it = chars;
  while (*it!=0) {
    if (it[0]>=0xa0) {
      length++;
    }
    it++;
  }
  XMLByte buff[65536];
  XMLSize_t nbTranscoded = 0;
  theTranscoder->transcodeTo (chars, length, buff, 16384, nbTranscoded,
			      xercesc::XMLTranscoder::UnRep_RepChar);
  buff[length] = 0;
  char *buff2 = (char*) buff;
  xercesc::XMLString::trim(buff2);
  string res = buff2;
  remove(res.begin(), res.end(), '*');
  remove(res.begin(), res.end(), '[');
  remove(res.begin(), res.end(), ']');
  remove(res.begin(), res.end(), '\'');
  while(res != "" && (res.find(" ") == 0 || res.rfind(" ") == (res.length()-1))) {
    res = res.substr(res.find(" ")+1).substr(0, res.rfind(" "));
  }
  return res;
}

bool checkAttr(const xercesc::Attributes & attrs, string key, string value,
	       xercesc::XMLTranscoder* theTranscoder) {
  XMLCh * _key = xercesc::XMLString::transcode(key.c_str());
  bool res = _transcode(attrs.getValue(_key), theTranscoder).compare(value) == 0;
  xercesc::XMLString::release(&_key);
  return (res);
}

string getAttrValue(const xercesc::Attributes & attrs, string value,
		    xercesc::XMLTranscoder* theTranscoder) {
  XMLCh * _key =  xercesc::XMLString::transcode(value.c_str());
  if (attrs.getValue(_key) == NULL) {
    xercesc::XMLString::release(&_key);
    return "";
  }
  string res = _transcode(attrs.getValue(_key), theTranscoder);
  xercesc::XMLString::release(&_key);
  return   res;
}