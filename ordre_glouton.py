#!/usr/bin/python2

import re
import subprocess
from operator import itemgetter
import sys

pos = 'Verbs' # or Nouns
module_names = {
    'Verbs': ['extract', 'vote-lastchance', 'simsyn', 'hyperypo', 'hyperhypo', 'hyperhypo', 'hyperhypo', 'hyperhypo', 'hyperhypo'],
    'Nouns': []
    }

modules = list(range(1, 9)) # 1..8
bestsofar = ''

def parseEval(current_try, processed):
    filename = "logs/eval%sEWN_%s" % (pos, processed)
    with open(filename) as f:
        for l in f.readlines():
            if l.startswith("cntPolysemous%sProcessedInJaws : " % pos):
                count = int(re.search("\d+", l).group(), 10)
            elif l.startswith("Generous Precision"):
                print l
                precision = float(re.search("[\d.]+", l).group())

    print count, precision
    return count, precision

prev = (0,0)

while True: 
   scores = []

   for i in modules:
    current_try = bestsofar + str(i)
    subprocess.call(['./translate%s.sh' % pos, " ".join([str(x) for x in current_try])])
    processed = "%s%d.%d" % (module_names[pos][i], i, len(current_try))
    best = '.best' if len(current_try) > 4 else ''
    print " ".join(['./evalAddpart.sh', 'data.fr.%s%s%s' % (pos.lower(), current_try, best), processed, pos[0]])
    subprocess.call(['./evalAddpart.sh', 'data.fr.%s%s%s' % (pos.lower(), current_try, best), processed, pos[0]])
    score = parseEval(current_try, processed)
    scores.append(score)
    print

   print scores
   best = scores.index(max(scores)) # precision, then count
   bestsofar += str(best)
   print "BESTSOFAR is %s" % bestsofar

   if prev >= score:
     break

   prev = score


