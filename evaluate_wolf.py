#!/usr/bin/env python3
#-*- coding: utf-8 -*-

from xml.etree.ElementTree import Element, ElementTree
import sys, glob

wolf_xml = ElementTree(file = sys.argv[1])

mappings = {}
for mapping_file in glob.glob("experiments/mapping-20-30/*"):
    with open(mapping_file) as mapping:
        for map_line in mapping:
            source, target, n, *rest = map_line.split()
            mappings[source] = target

bcs_list = []
with open('bcs.txt') as bcsfile:
    for bcs in bcsfile:
        bcs_list.append(bcs.strip())
        
if 'jaws' in sys.argv:
    goldfiles = ['gold/GT_nouns30.test.xml']
else:
    goldfiles = ['gold/GT_nouns30.test.xml', 'gold/GT_verbs30.test.xml', 'gold/GT_adjs30.test.xml']


gold = {}
for pos_gold in goldfiles:
    gold_xml = ElementTree(file = pos_gold)
    for synset in gold_xml.findall("SYNSET"):
        synset_id = synset.get("id")
        validated = [c.text for c in synset.findall("CANDIDATES/CANDIDATE") if c.get('score') == '1']
        if 'nomwe' in sys.argv:
            validated = list(filter(lambda c: not '_' in c, validated))
        gold[synset_id] = validated

wolf = {}
nb_candidates_total = 0
for synset in wolf_xml.findall("SYNSET"):
    if 'jaws' in sys.argv:
        synset_20_id = synset.find("ID").text[6:14]
        if synset_20_id in mappings:
            synset_id = mappings[synset_20_id]
        else:
            continue
    else:
        synset_id = synset.find("ID").text[7:15]

    if 'bcs' in sys.argv and not synset_id in bcs_list:
        continue

    candidates = [c.text for c in synset.findall("SYNONYM/LITERAL") if c.text != '_EMPTY_']
    if 'nomwe' in sys.argv:
        candidates = list(filter(lambda c: not '_' in c, candidates))
    nb_candidates_total += len(candidates)
    wolf[synset_id] = candidates

precision, rappel, n = 0, 0, 0
nbTermsOk, nbInJawsSynsetInGt, nbTermsInGtAndAJawsSynset = 0, 0, 0
for synset_id in gold:
    if not synset_id in wolf:
        continue

    in_gold = set(gold[synset_id])
    in_wolf = set(wolf[synset_id])

    if len(in_gold) == 0:
        continue
    if len(in_wolf) == 0:
        continue

    try:
        nbTermsOk += len(in_gold & in_wolf)
        nbInJawsSynsetInGt += len(in_wolf)
        nbTermsInGtAndAJawsSynset += len(in_gold)
    except:
        pass

precision_globale = 100*nbTermsOk/nbInJawsSynsetInGt
rappel_global = 100*nbTermsOk/nbTermsInGtAndAJawsSynset
f1 = 2 * precision_globale * rappel_global / (precision_globale + rappel_global)

print("{} {} {}".format(nbTermsOk, nbInJawsSynsetInGt, nbTermsInGtAndAJawsSynset))
print("P/R/F1 {} {:.2f} {:.2f} {:.2f}".format(nb_candidates_total, precision_globale, rappel_global, f1))
