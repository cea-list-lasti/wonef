#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from lxml.etree import ElementTree, Element
from collections import defaultdict
import sys

if len(sys.argv) < 3:
    print("Give a WoNeF xml file!")
    exit()

golds = [
    ('n', 'GT_nouns30.xml'),
    ('v', 'GT_verbs30.xml'),
    ('a', 'GT_adjs30.xml')]
validated = defaultdict(list)

for pos, g in golds:
    gold_xml = ElementTree(file=g)
    for synset in gold_xml.findall("SYNSET"):
        correct_candidates = [c.text
                              for c in synset.findall("CANDIDATES/CANDIDATE")
                              if c.get("score") == "1"]
        if correct_candidates:
            synset_id = "{}-{}".format(synset.get("id"), pos)
            validated[synset_id].extend(correct_candidates)

wonef = ElementTree(file=sys.argv[1])
wonef_gold = Element("WN")

for synset in wonef.findall("SYNSET"):
    synset_id = synset.find("ID").text[7:17]
    if synset_id in validated:
        synonym = synset.find("SYNONYM")
        literals = synonym.findall("LITERAL")

        translations = set(validated[synset_id])
        existing = set([l.text for l in literals if l.text != "__EMPTY__"])

        # nothing exists, remove the empty literal
        if not literals:
            synonym.remove(literals)

        # add missing literals
        for t in translations - existing:
            literal = Element("LITERAL")
            literal.text = t
            synonym.append(literal)

        # put the lnote for every literal
        for l in literals:
            if l.text in validated[synset_id]:
                newlnote = ";".join([l.get("lnote")] + ["wonef-gold"])
                l.set("lnote", newlnote)
    wonef_gold.append(synset)

ElementTree(wonef_gold).write(sys.argv[2],
                              encoding="UTF-8", xml_declaration=True)
