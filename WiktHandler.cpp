#include "WiktHandler.hpp"
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/TransService.hpp>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>

using namespace xercesc;
using namespace std;



WiktHandler::WiktHandler(const string outputfile, const string _srcLang, const string _tgtLang) 
  : srcLang(_srcLang), tgtLang(_tgtLang), nbPage(0), def("") {
  ofs.open(outputfile.c_str(),  ios_base::out | ios_base::trunc );
  XMLTransService* const  theService =
    XMLPlatformUtils::fgTransService;

  XMLTransService::Codes  theCode;

  theTranscoder =
    theService->makeNewTranscoderFor(
				     "utf-8",
				     theCode,
				     8192);



}

WiktHandler::~WiktHandler() {
  ofs.close();
  delete theTranscoder;
}


string WiktHandler::_transcode(const XMLCh* const chars) {
  
  XMLSize_t length = XMLString::stringLen(chars);
  const XMLCh* it = chars;
  while (*it!=0) {
    if (it[0]>=0xa0) {
      length++;
    }
    it++;    
  }
  XMLByte buff[65536];
  XMLSize_t nbTranscoded = 0;
  theTranscoder->transcodeTo (chars, length, buff, 16384, nbTranscoded, XMLTranscoder::UnRep_RepChar);
  buff[length]=0;
  char *buff2 = (char*) buff;
  XMLString::trim(buff2);
  string res = buff2;
  std::remove(res.begin(), res.end(), '*');
  std::remove(res.begin(), res.end(), '[');
  std::remove(res.begin(), res.end(), ']');
  std::remove(res.begin(), res.end(), '\'');
  while(res!="" && (res.find(" ")==0 || res.rfind(" ")==(res.length()-1))) {
    res=res.substr(res.find(" ")+1).substr(0,res.rfind(" "));
  } 
  return res;
}

bool WiktHandler::checkAttr(const Attributes &  	attrs, string key, string value ) {  
  XMLCh * _key = XMLString::transcode(key.c_str());
  bool res =_transcode(attrs.getValue(_key)).compare(value)==0 ;
  XMLString::release(&_key);
  return (res);
}

string WiktHandler::getAttrValue(const Attributes &  	attrs, string value) {
  XMLCh * _key =  XMLString::transcode(value.c_str());
  if (attrs.getValue(_key)==NULL) {
    XMLString::release(&_key);
    return "";
  }
  string res = _transcode(attrs.getValue(_key));
  XMLString::release(&_key);
  return   res;
}

void WiktHandler::startElement(const XMLCh *const    	 uri,
			       const XMLCh *const   	localname,
			       const XMLCh *const   	qname,
			       const Attributes &  	attrs) {  

  if(_transcode(qname).compare("page")==0) {
    nbPage++;
    if (nbPage%1000==0) {
      cerr << "nbPage : " << nbPage << endl;
    }
    tmpEntry = getAttrValue(attrs, "name");  
  } else if(_transcode(qname).compare("lang")==0  ) {
    //    cerr << tgtLang << " vs "<<getAttrValue(attrs, "name")<< endl;
    if (checkAttr(attrs, "name", srcLang)) {      
      srcStrings.insert(tmpEntry);
    } else if (checkAttr(attrs, "name", tgtLang) && tmpString.compare(":")!=0) {
      tgtStrings.insert(tmpEntry);
      //      cerr << "INSERT : " << tmpEntry << endl;
    }  
  } else if(_transcode(qname).compare("cat")==0) {
    tmpAttrs["cat_name"]= getAttrValue(attrs, "name");
  } else if(_transcode(qname).compare("subCat")==0) {
    tmpAttrs["subcat_name"]= getAttrValue(attrs, "name");
    tmpAttrs["subcat_lang"]= getAttrValue(attrs, "lang");
  }
  
}

void WiktHandler::characters(const XMLCh *const chars, const XMLSize_t length)  {
  tmpString = _transcode(chars);

}

void WiktHandler::endElement(const XMLCh *const uri,
			     const XMLCh *const localname,
			     const XMLCh *const qname) {
  if(_transcode(qname).compare("page")==0) {
    for (set<string>::iterator itTgt = tgtStrings.begin(); itTgt!=tgtStrings.end(); itTgt++) {
      for (set<string>::iterator itSrc = srcStrings.begin(); itSrc!=srcStrings.end(); itSrc++) {
	ofs << *itSrc << ";;;" << *itTgt << ";" << def <<";" << endl;	
      }
    }
    ofs.flush();
    tgtStrings.clear();
    tmpAttrs.clear();
    def.clear();
    srcStrings.clear();
    tmpEntry.clear();
    // frWikt
  }  else if(_transcode(qname).compare("subCat")==0) {
    if (tmpAttrs["subcat_name"].compare("f")==0 
	|| tmpAttrs["subcat_name"].compare("mf")==0
	|| tmpAttrs["subcat_name"].compare("la")==0 
	|| (tmpAttrs["subcat_name"].compare("m")==0
	    && tmpString.find("#")!=string::npos)
	|| tmpAttrs["subcat_name"].compare("fam")==0 
	|| tmpAttrs["subcat_name"].compare("terme")==0
	|| tmpAttrs["subcat_name"].compare("term")==0) {
      def+= " # " + tmpString;
    } else if (tmpAttrs["subcat_name"].compare("translation")==0 
	       && tmpAttrs["subcat_lang"].compare(tgtLang)==0 
	       && tmpString.compare(":")!=0
	       && tmpString!="") {
      tgtStrings.insert(tmpString);
    } else if (tmpAttrs["cat_name"].compare("translations")==0 
	       && tmpAttrs["subcat_name"].compare(tgtLang)==0 
	       && tmpString.find(":")==string::npos
	       && tmpString!="") {
      tgtStrings.insert(tmpString);
      // enwikt
    } else if (tmpAttrs["cat_name"].compare("translations")==0 
	       && (tmpAttrs["subcat_name"].compare("n")==0 
		   || tmpAttrs["subcat_name"].compare("m")==0 )
	       && tmpString.find("French:")!=string::npos) {
      srcStrings.insert(tmpString.substr(7));
    } else if (tmpAttrs["cat_name"].compare("translations")==0 
	       && tmpAttrs["subcat_lang"].compare(srcLang)==0 
	       && tmpString.find(":")==string::npos
	       && tmpString!="") {
      srcStrings.insert(tmpString);
    } 
  }  else if(_transcode(qname).compare("cat")==0) {
    //    def.clear();
  }
}
