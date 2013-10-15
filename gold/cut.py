#!/usr/bin/env python3
#-*- coding: utf-8 -*-

from xml.etree.ElementTree import ElementTree, Element

for filename in ['GT_adv30']:
    nb_test, nb_dev = 0, 0.01
    test, dev = Element("VERITE"), Element("VERITE")
    ratio = 0
    ground_truth = ElementTree(file = "{}.xml".format(filename))
    for synset in ElementTree(ground_truth).findall("SYNSET"):
        if nb_dev == 0 or nb_test/nb_dev <= 9:
            test.append(synset)
            nb_test += len(synset.findall("CANDIDATES/CANDIDATE"))
        else:
            dev.append(synset)
            nb_dev += len(synset.findall("CANDIDATES/CANDIDATE"))

    ElementTree(test).write("{}.test.xml".format(filename), encoding="utf-8")
    ElementTree(dev).write("{}.dev.xml".format(filename), encoding="utf-8")


        

