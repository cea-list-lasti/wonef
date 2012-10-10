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


from xml.etree.ElementTree import ElementTree, Element
import sys

pos = sys.argv[1]

to30 = get_mapping(pos)
GT = ElementTree(file="GT_%ss.xml" % pos)

for synset in GT.findall("SYNSET"):
  if len(to30[synset.get('id')]) != 1:
    print(synset.get('id'))
  synset.set('id', ' '.join(to30[synset.get('id')]))

GT.write("GT_%ss30.xml" % pos, encoding="UTF-8")
