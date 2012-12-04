#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
from xml.etree.ElementTree import ElementTree
from collections import defaultdict

binarise = sys.argv[1] == "binarise"
first = ElementTree().parse(sys.argv[2])
second = ElementTree().parse(sys.argv[3])

def get_scores(annotated):
  max_score = 0
  scores = defaultdict(lambda:defaultdict(set))

  for synset in annotated.findall("SYNSET"):
    synset_candidates = synset.findall("CANDIDATES/CANDIDATE")
    if any(c.get("score") != "0" for c in synset_candidates):
      for c in synset.findall("CANDIDATES/CANDIDATE"):
        score = int(c.get("score"))
        if binarise: score = 1 if score == 1 else 0
        max_score = score if score > max_score else max_score
        scores[synset.get("id")][score].add(c.text)
  return max_score, scores

max_first_score, firstscores = get_scores(first)
max_second_score, secondscores = get_scores(second)

max_score = max(max_first_score, max_second_score)

table = defaultdict(lambda: defaultdict(int))

for synset in firstscores:
  if not synset in secondscores: continue

  first, second = defaultdict(int), defaultdict(int)
  for i in range(max_score+1):
    first[i] = firstscores[synset][i]
    second[i] = secondscores[synset][i]


  for i in range(max_score+1):
    for j in range(max_score+1):
      table[i][j] += len(first[i] & second[j])

for i in range(max_score+1):
  for j in range(max_score+1):
    print("{:>5}".format(table[i][j]), end=" ")
  print()
  
