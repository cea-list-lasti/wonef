#!/usr/bin/env python
# -*- coding: utf-8 -*-

wolf, wonef = set(), set()

for i in open('wolf10bpairs'): wolf.add(i.strip().split(':')[1])
for i in open('wonefpairs'): wonef.add(i.strip().split(':')[1])

inboth = len(wonef & wolf)

wonefspecific = len(wonef - wolf)
wolfspecific = len(wolf - wonef)

print(wonefspecific)

merge = wonef | wolf
print(len(merge))
print(len(wolf))

print(100 * len(merge) / len(wolf))
print(100 * len(merge) / len(wonef))
print("{:.2%}".format(len(wolf)/len(merge)))
print("{:.2%}".format(len(wonef)/len(merge)))

#for s in (merge):
#  print(s)
