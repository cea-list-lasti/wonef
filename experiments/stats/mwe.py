#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
from xml.etree.ElementTree import ElementTree
from collections import defaultdict

targetmwe = defaultdict(set)
sourcemwe = defaultdict(set)
jaws = ElementTree(file = sys.argv[1])

for synset in ElementTree(jaws).findall("SYNSET"):
  for original in synset.findall("CANDIDATES"):
    origword = original.get("original")
    if "_" in origword:
      sourcemwe[origword].add(synset.get("id"))
    for c in synset.findall("CANDIDATES/CANDIDATE"):
      if "_" in c.text:
        targetmwe[c.text].add(synset.get("id"))

monosemous = len([e for e in sourcemwe if len(sourcemwe[e]) == 1])

print(monosemous / len(sourcemwe))
print(len(sourcemwe)/len(jaws.findall("SYNSET/CANDIDATES")))
print(len(targetmwe)/len(jaws.findall("SYNSET/CANDIDATES/CANDIDATE")))
