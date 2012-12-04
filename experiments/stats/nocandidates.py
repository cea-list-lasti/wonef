#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
from xml.etree.ElementTree import ElementTree, Element
from collections import defaultdict
import bcs

jaws = ElementTree(file = sys.argv[1])
bads = Element("root")
bcsnotr, notr = 0, 0
alls, allbcs = 0, 0

for synset in ElementTree(jaws).findall("SYNSET"):
  alls += 1
  if bcs.isbcs(synset.get("id")):
    allbcs += 1
  if len(synset.findall("CANDIDATES/CANDIDATE")) == 0:
    bads.append(synset)
    notr += 1
    if bcs.isbcs(synset.get("id")):
      bcsnotr += 1
    
print("{} synsets with no translation: {:.3f} of all synsets, {:.3f} of all BCS synsets)".format(notr, notr/alls, bcsnotr/allbcs if allbcs > 0 else 0))
ElementTree(bads).write('output.xml', encoding="utf-8")
