#!/usr/bin/env python
# -*- coding: utf-8 -*-

from xml.etree.ElementTree import ElementTree
import sys
import glob

wolf, wonef = set(), set()

def parse_mapping(mapping_directory):
    mapping = {}
    for mapping_file in glob.glob("{}/*".format(mapping_directory)):
        for l in open(mapping_file):
            two, three, *p = l.split()
            mapping[two] = three

    return mapping

def parse_id(synset_id):
    synset_id = synset_id.replace('ENG20', 'eng-20', 1)
    wn_lang, wn_version, numeric_id, pos = synset_id.split('-')
    return numeric_id, pos

def parse_debvisdic(f, needs_mapping, wanted_pos):
    mapping = {}
    if needs_mapping:
        mapping = parse_mapping("../mapping-20-30")
    wordnet = set()
    xml = ElementTree(file = f)
    for synset in xml.findall("SYNSET"):
        synset_id, pos = parse_id(synset.find("ID").text)
        if synset.find("POS").text != wanted_pos: continue

        for l in synset.findall("SYNONYM/LITERAL"):
            literal = l.text
            if literal != "_EMPTY_":
                wordnet.add("eng-30-{}-{}:{}".format(mapping.get(synset_id, synset_id), pos, literal))

    return wordnet

pos_name = {
    "n": "Nouns",
    "v": "Verbs",
    "a": "Adjectives",
    "b": "Adverbs"}

for wanted_pos in ["n", "v", "a", "b"]:
    print(pos_name[wanted_pos])
    print("*" * len(pos_name[wanted_pos]))

    for wolf_name, needs_mapping in [("0.1.5.format", True), ("1.0b", False)]:
        wolf = parse_debvisdic("wolf-{}.xml".format(wolf_name), needs_mapping, wanted_pos)
        print("WOLF {}".format(wolf_name))

        for wonef_name in ["precision", "fscore", "coverage"]:
            wonef = parse_debvisdic("wonef-{}-0.1.xml".format(wonef_name), False, wanted_pos)
            inboth = wonef & wolf

            print(wonef_name)
            print("  Précision    : {:.2%}".format(len(inboth)/len(wonef)))
            print("  Rappel       : {:.2%}".format(len(inboth)/len(wolf)))
            print("  Ajouts WoNeF : {}".format(len(wonef - wolf)))
        print()
