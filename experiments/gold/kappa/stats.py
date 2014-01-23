#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
from xml.etree.ElementTree import ElementTree
from collections import defaultdict



def cand_syns(gt):
  candidates, synsets = 0, 0
  for synset in gt.findall("SYNSET"):
    synset_candidates = synset.findall("CANDIDATES/CANDIDATE")
    if any(c.get("score") != "0" for c in synset_candidates):
    #if len(synset_candidates) > 0:
    #if True:
      candidates += len(synset_candidates)
      synsets += 1

  return candidates, synsets
  
cand, syn = 0, 0

for n in sys.argv[1:]:
  gt = ElementTree(file=n)
  ncandidates, nsynsets = cand_syns(gt)
  cand += ncandidates
  syn += nsynsets


print(cand, syn, cand / syn)
