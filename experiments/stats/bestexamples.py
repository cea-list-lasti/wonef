#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
from xml.etree.ElementTree import ElementTree, Element
from collections import defaultdict, OrderedDict

jaws = ElementTree(file = sys.argv[1])
candidatestree = Element("candidates")
candidates_for_examples = {}

for synset in ElementTree(jaws).findall("SYNSET"):
  if len(synset.findall("CANDIDATES/CANDIDATE")) > 1:
    score_max = 0
    #meroholo = [c.text for c in synset.findall("MERO")] + [c.text for c in synset.findall("HOLO")]
    #instances = [i.get("translation") for i in synset.findall("INSTANCES")]
    #if set(meroholo) & set(instances):
    #  continue

    for instance in synset.findall("INSTANCES/INSTANCE"):
      if instance.get("processed") == sys.argv[2]:
        score = float(instance.get("score"))
        score_max = score if score > score_max else score
      elif instance.get("processed") == "levenshtein":
        score_max = 0
        break
    if score_max > 0:
      candidates_for_examples[score_max] = synset

for score, synset in OrderedDict(sorted(candidates_for_examples.items(), key=lambda t: t[0])).items():
  candidatestree.append(synset)


ElementTree(candidatestree).write('output.xml', encoding="utf-8")
