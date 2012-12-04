#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
from collections import defaultdict

index = open(sys.argv[1])
data = open(sys.argv[2])

synsets = defaultdict(int)

for l in index:
  if l.startswith("  "):
    continue

  splits = l.split()
  nbsynsets = int(splits[2])
  for s in splits[-nbsynsets:]:
    synsets[s] += 1

polysemous, alls = 0, 0

for s in synsets:
  if synsets[s] > 1:
    polysemous += 1
  alls += 1

print("{:2f} {} {}".format(polysemous/alls, polysemous, alls))
