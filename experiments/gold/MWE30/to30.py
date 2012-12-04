#!/usr/bin/python
# -*- coding: utf-8 -*-

from collections import defaultdict

def get_mapping(pos):
  to30 = defaultdict(list)

  for line in open('mapping-20-30/wn20-30.%s' % pos):
    splits = line.split()
    source, dest = splits[0], splits[1]
    if not dest in to30[source]:
      to30[source].append(dest)

  return to30

import re

def get_definitions(pos):
  defs = {}
  with open("data.{}".format(pos)) as datapos:
    for l in datapos:
      if l.startswith("  "): continue
      synsetid, definition = re.search(r'(\d{8}) .+ \| (.+)$', l).groups()
      defs[synsetid] = definition.strip()
  return defs

from xml.etree.ElementTree import ElementTree, Element
import sys

pos = sys.argv[1]
infile = sys.argv[2]
outfile = sys.argv[3]

to30 = get_mapping(pos)
defs = get_definitions(pos)
GT = ElementTree(file=infile)

for synset in GT.findall("SYNSET"):
  synsetid = synset.get('id')
  if len(to30[synsetid]) != 1:
    print(len(to30[synsetid]), synsetid)
  synset.set('id', ' '.join(to30[synsetid]))
  synset.find('ORIGINALDEF').text = ""

GT.write(outfile, encoding="UTF-8")
