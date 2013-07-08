#!/usr/bin/env python2
# -*- coding: utf-8 -*-

# Converts wolf-0.1.4 from its xml format with WordNet 2.0 IDs
# to the DebVisDic format with WordNet 3.0 IDs

from lxml import etree
from lxml.etree import ElementTree, Element
from collections import defaultdict
import sys


mapping = {}
mapping_entries = (line.strip().split(" ") for line in open("mapping-20-30/wn20-30.noun", "r"))
for mapping_entry in mapping_entries:
  mapping['ENG20-'+mapping_entry[0]+'-n'] = 'eng-30-'+mapping_entry[1]+'-n'
mapping_entries = (line.strip().split(" ") for line in open("mapping-20-30/wn20-30.verb", "r"))
for mapping_entry in mapping_entries:
  mapping['ENG20-'+mapping_entry[0]+'-v'] = 'eng-30-'+mapping_entry[1]+'-v'
mapping_entries = (line.strip().split(" ") for line in open("mapping-20-30/wn20-30.adj", "r"))
for mapping_entry in mapping_entries:
  mapping['ENG20-'+mapping_entry[0]+'-a'] = 'eng-30-'+mapping_entry[1]+'-a'
mapping_entries = (line.strip().split(" ") for line in open("mapping-20-30/wn20-30.adv", "r"))
for mapping_entry in mapping_entries:
  mapping['ENG20-'+mapping_entry[0]+'-b'] = 'eng-30-'+mapping_entry[1]+'-b'

wordnet20 = etree.fromstring(sys.stdin.read())
wordnet30 = Element("WN")

for synset in wordnet20.findall("SYNSET"):

  for ilr in synset.findall("ILR"):
    if ilr[1].text != None:
      ilr_id = ilr[1].text[6:]
      if 'ENG20-'+ilr_id in mapping:
        ilr_id = mapping['ENG20-'+ilr_id]
      else:
        ilr_id = 'ENG20-'+ilr_id
      ilr.attrib["type"] = ilr[0].text
      ilr.remove(ilr[1])
      ilr.remove(ilr[0])
      ilr.text = ilr_id

  synset_id = synset.find("ID").text[6:17]
  if 'ENG20-'+synset_id in mapping:
    synset_id = mapping['ENG20-'+synset_id]
    synset.find("ID").text = synset_id
    synonym = synset.find("SYNONYM")
    literals = synonym.findall("LITERAL")

  wordnet30.append(synset)

print(etree.tostring(wordnet30,encoding="UTF-8",
        xml_declaration=True,
        pretty_print=True,
        doctype='<!DOCTYPE WN SYSTEM "debvisdic-strict.dtd">'))
