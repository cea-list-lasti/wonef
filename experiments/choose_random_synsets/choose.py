#!/usr/bin/env python3
#-*- coding: utf-8 -*-

import random, re, html.entities, sys
from xml.etree.ElementTree import ElementTree, Element, SubElement, tostring, fromstring

tree = ElementTree().parse(sys.argv[1])
ids, chosen_list, ids_empty = [], [], set()
avoid = ElementTree().parse(sys.argv[3])

avoid_list = [synset.get("id") for synset in avoid.findall("SYNSET")]

# get all synset ids
for synset in tree.findall("SYNSET"):
    ids.append(synset.get("id"))
    if not synset.findall("CANDIDATES/CANDIDATE"):
        ids_empty.add(synset.get("id"))

print(1.0 * len(ids_empty) / len(ids))

# choose interesting ids
i = int(sys.argv[2])
while i > 0:
    chosen = random.choice(ids)
    if chosen not in avoid_list:
        ids.remove(chosen)
        chosen_list.append(chosen)
        if not chosen in ids_empty:
            i -= 1

print("chose %d synsets" % len(chosen_list))

# get all interesting ids and related info
result = Element("VERITE")
for synset in [s for s in tree.findall("SYNSET") if s.get("id") in chosen_list]:
    synsetinfo = SubElement(result, "SYNSET", id=synset.get("id"))
    original_def = SubElement(synsetinfo, "ORIGINALDEF")
    original_def.text = synset.find("ORIGINALDEF").text.strip()

    for candidates in synset.findall("CANDIDATES"):
        candidates_node = SubElement(synsetinfo, "CANDIDATES", original=candidates.get("original"))
        for cand in candidates.findall("CANDIDATE"):
            candidat = SubElement(candidates_node, "CANDIDATE", valide="0")
            candidat.text = cand.text

# dump !
ElementTree(result).write('300advs.xml', encoding='utf8')
