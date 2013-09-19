#!/usr/bin/env python2
#-*- coding: utf-8 -*-

"""
veriteTerrain.xml is an annotated gold standard, and the output is every synset
which has a valid translation (valide="1").
"""

import random, re, htmlentitydefs, sys
from xml.etree.ElementTree import ElementTree, Element, SubElement, tostring, fromstring

tree = ElementTree()
tree.parse("veriteTerrain.xml")

result = Element("VERITE")
for synset in tree.findall("SYNSET"):
	keep = False
	for cand in synset.findall("CANDIDATES/CANDIDATE"):
		if cand.get("valide") == "1": keep = True
	if keep:
		result.append(synset)

print tostring(result)
