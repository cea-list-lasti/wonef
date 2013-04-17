#!/usr/bin/env python

import sys
from xml.etree.ElementTree import ElementTree, Element

pos = sys.argv[1]
conf = sys.argv[2]
module = sys.argv[3]

result = Element("WN")

for synset in ElementTree(file = "data/jaws.best.{}.{}.deb.xml".format(pos, conf)).findall("SYNSET"):
    for synonym in synset.findall("SYNONYM"):
        for literal in synonym.findall("LITERAL"):
            if literal.text == '_EMPTY_' or not module in literal.get("lnote"):
                synonym.remove(literal)
    result.append(synset)

ElementTree(result).write("data/jaws.{}.{}.{}.deb.xml".format(pos, conf, module), encoding="utf-8")
