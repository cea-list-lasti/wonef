#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import time
from xml.etree.ElementTree import ElementTree, Element, dump

start = time.time()
toannotate = ElementTree().parse(sys.argv[1])
jaws_complet = ElementTree().parse(sys.argv[2])
end = time.time()
 
print("parsed in {} seconds!".format(end-start))

for jawssynset in jaws_complet.findall("SYNSET"):
  synsetid = jawssynset.get("id")
  goldsynset = toannotate.find("SYNSET[@id='%s']" % jawssynset.get("id"))
  if goldsynset is not None:
    jawsoriginals = [cs.get("original") for cs in jawssynset.findall("CANDIDATES")]
    goldoriginals = [cs.get("original") for cs in goldsynset.findall("CANDIDATES")]
    if jawsoriginals != goldoriginals:
      print(synsetid)
      print("  ", goldoriginals, jawsoriginals)
      # We first want to add the NEW candidates and translations 
      for original in set(jawsoriginals) - set(goldoriginals):
        print("  adding original %s" % original)
        goldsynset.append(Element("CANDIDATES", attrib = {'original': original}))
      # We then want to remove the OLD candidates and translations
      for original in set(goldoriginals) - set(jawsoriginals):
        print("  removing original %s" % original)
        goldsynset.remove(goldsynset.find("CANDIDATES[@original=\"%s\"]" % original))

      print("  ----")
      
      # Finally, we want to add the new translations
      for goldoriginal in goldsynset.findall("CANDIDATES"):
        jawsoriginalcandidates = [c.text for c in jawssynset.findall("CANDIDATES[@original=\"%s\"]/CANDIDATE" % goldoriginal.get('original'))]
        goldscores = {c.text: c.get('score') for c in jawssynset.findall("CANDIDATES[@original=\"%s\"]/CANDIDATE" % goldoriginal.get('original'))}
        goldoriginalcandidates = [c.text for c in goldoriginal.findall("CANDIDATE")]
        print("  ", goldoriginal.get('original'), " --> ", goldoriginalcandidates, jawsoriginalcandidates)
        for candidate in set(jawsoriginalcandidates) - set(goldoriginalcandidates):
          print("  add candidate %s in original %s"
              % (candidate, goldoriginal.get('original')))
          neworiginal = Element("CANDIDATE", score=goldscores[candidate])
          neworiginal.text = candidate
          goldoriginal.append(neworiginal)
        # also remove outdated translations (if any)!
        assert set(goldoriginalcandidates) - set(jawsoriginalcandidates) == set() 

ElementTree(toannotate).write("GT_nouns30missing.xml", encoding="UTF-8", xml_declaration=True)
