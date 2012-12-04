#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
from xml.etree.ElementTree import ElementTree, Element
from collections import defaultdict

jaws = ElementTree(file = sys.argv[1])
bads = Element("root")
alle = 0

for synset in ElementTree(jaws).findall("SYNSET"):
  if synset.findall("CANDIDATES/CANDIDATE"):
    alle += 1
    if not [c for c in synset.findall("CANDIDATES/CANDIDATE") if c.get("score") != "0"]:
      bads.append(synset)
      print(synset.get("id"))


print("{} unvalid, {} of total".format(len(bads), len(bads)/alle))
ElementTree(bads).write('output.xml', encoding="utf-8")
