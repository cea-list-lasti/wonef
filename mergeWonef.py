#!/usr/bin/env python2.7

from xml.etree.ElementTree import ElementTree, Element, SubElement
import sys
from glob import glob
from collections import defaultdict

def get_literals(synset, literals):
    for wonef_literal in synset.findall("SYNONYM/LITERAL"):
        if wonef_literal.text != "_EMPTY_":
            for lnote in wonef_literal.get("lnote").split(";"):
                literals[wonef_literal.text].append(lnote)


# First parse WOLF to create an empty WoNeF (without literals)
all_synsets = {}
print("parsing WOLF")
for synset in ElementTree(file = "wolf-1.0b.fixed.xml").findall("SYNSET"):
    literals = defaultdict(list)
    if "wolf" in sys.argv:
        get_literals(synset, literals)
    synset.remove(synset.find("SYNONYM"))
    all_synsets[synset.find("ID").text] = synset, literals

# Then retrieve relevant literals by parsing every PoS-specific WoNeF
mode = sys.argv[1]

for posfile in glob("*-{}.xml".format(mode)):
    print("parsing {}".format(posfile))
    for synset in ElementTree(file = posfile).findall("SYNSET"):
        try:
            final_synset, literals = all_synsets[synset.find("ID").text]
            get_literals(synset, literals)

        except KeyError as e:
            print("{} missing".format(synset.find("ID").text))

# Build synsets back : (empty synset, literals) -> complete synset
print("building synsets")
for synset, literals_notes in all_synsets.values():

    # Huge hack to insert element at the correct place (after ID)
    i = 0
    for e in list(synset):
        i += 1
        if e.tag == "ID": break

    literals_container = Element("SYNONYM")
    synset.insert(i, literals_container)

    if not literals_notes:
        empty_literal = Element("LITERAL")
        empty_literal.text = "_EMPTY_"
        literals_container.append(empty_literal)
    else:
        for literal in literals_notes:
            literal_node = SubElement(literals_container, "LITERAL")
            literal_node.text = literal
            literal_node.set("lnote", ";".join(literals_notes[literal]))

# Finally, sort synsets
print("sorting")
final = Element("WN")

for synset in sorted([synset for synset, literals in all_synsets.values()], key=lambda item: item.find("ID").text):
    final.append(synset)

print("writing")
# Write them to XML
name = "wonef-wolf-{}-complet.xml" if "wolf" in sys.argv else "wonef-{}-complet.xml"
ElementTree(final).write(name.format(mode), encoding="utf-8")
