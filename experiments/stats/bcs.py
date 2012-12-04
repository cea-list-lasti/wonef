bcs = {}

print("init")
for l in open("bcs"):
  synset, importance = l.split()
  bcs[synset] = int(importance)

def isbcs(synsetid):
  return synsetid in bcs
