#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
This scripts simply lists all annotations who are not consistent in a given
file. That is, if multiple english word from a WordNet synset, and some of
those world give the same french translation, we have to give the same label to
those french translations.
"""

from xml.etree.ElementTree import ElementTree
import sys

verite = ElementTree().parse(sys.argv[1])

for synset in verite.findall("SYNSET"):
  candidats = [(candidat.text, candidat.get("valide")) for candidat
      in synset.findall("CANDIDATES/CANDIDATE")]
  valide_dict = {}
  sevalide_dict = {}

  for word, score in candidats:
    score = int(score)
    word_cut, score_cut = word, score

    if word.startswith("se_"):
      word_cut = word[3:]
    elif word.startswith("s'"):
      word_cut = word[2:]

    if word_cut != word:
      if score == 1: score_cut = 4
      elif score == 4: score_cut = 1

    is_pronominal = word != word_cut

    #print("%s %s %s %d %d" % (synset.get("id"), word, word_cut, score, score_cut))

    if word_cut in valide_dict:
      reference_score = valide_dict[word_cut]
      if reference_score != score_cut:
        if is_pronominal and reference_score == 2 or sevalide_dict["s"+word_cut] == 0 and score == 2:
         pass
        elif (reference_score == 4 and score_cut == 1) or (reference_score == 1 and score_cut == 4):
          print("omg, %s - %s COULD be inconsistent ! (%d != %d)" % (synset.get("id"), word, score_cut, reference_score))
        else:
          print("omg, %s - %s is not consistent ! (%d != %d)" % (synset.get("id"), word, score_cut, reference_score))
    else:
      valide_dict[word_cut] = score_cut
      sevalide_dict["s"+word_cut] = score
