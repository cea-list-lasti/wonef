#ifndef TOOLS_HPP
#define TOOLS_HPP
#include <string>
#include <map>

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/util/TransService.hpp>

using namespace std;

string tolower(string strToConvert);

// parsing xml tools
string _transcode(const XMLCh* const chars,
		  xercesc::XMLTranscoder* theTranscoder);

bool checkAttr(const xercesc::Attributes & attrs, string key, string value,
	       xercesc::XMLTranscoder* theTranscoder);

string getAttrValue(const xercesc::Attributes & attrs, string value,
		    xercesc::XMLTranscoder* theTranscoder);

std::map<std::string, std::string> loadSensemap(std::string pos);

#endif
