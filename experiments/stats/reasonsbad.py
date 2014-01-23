#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
from xml.etree.ElementTree import ElementTree
from collections import defaultdict

jaws = ElementTree(file = sys.argv[1])
reasons = defaultdict(int)

for synset in ElementTree(jaws).findall("SYNSET"):
  reasons[synset.get("issue")] += 1

print(reasons, "{:.2f} ".format((reasons['notenough'] + reasons['toomuch'])/ sum(reasons.values())))
