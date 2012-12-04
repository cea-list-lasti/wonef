#!/usr/bin/env python
# -*- coding: utf-8 -*-

wolf, wonef = set(), set()

for i in open('wolfpairs'): wolf.add(tuple(i.strip().split(':')))
for i in open('wonefpairs'): wonef.add(tuple(i.strip().split(':')))

inboth = len(wonef & wolf)

wonefspecific = len(wonef - wolf)
wolfspecific = len(wolf - wonef)

#print(wonefspecific)

merge = wonef | wolf
#print(len(merge))
#print(len(wolf))

#print(100 * len(merge) / len(wolf))
#print(100 * len(merge) / len(wonef))

for s in (merge):
  print(s)
