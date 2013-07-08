#!/bin/bash


sed -e 's,<SYNONYM/>,<SYNONYM><LITERAL/></SYNONYM>,' -e 's,</TYPE>\([^<]*\)</ILR>,</TYPE><VAL>\1</VAL></ILR>,g' -e 's,<LITERAL>\([^<]*\)<SENSE>\([^<]*\)</SENSE></LITERAL>,<LITERAL lnote="\2">\1</LITERAL>,g' -e 's,<SUMO>\([^<]*\)<TYPE>\([^<]*\)</TYPE></SUMO>,<SUMO type="\2">\1</SUMO>,g' -e 's,\(<SYNSET>\)\(.*</SYNONYM>\)\(<ILR.*\)\(<DEF>\),\1\3\2\4,g' -e 's,\(</ILR>\)\(<ID>.*</DOMAIN>\)\(<SUMO.*\)\(</SYNSET>\),\1\3\2\4,'g -e 's,\(<SYNSET>\)\(<ID>.*</DOMAIN>\)\(<SUMO.*\)\(</SYNSET>\),\1\3\2\4,'g -e 's,\(<POS>.*</POS>\)\(.*\)\(</SYNSET>\),\2\1\3,' $* | ./convert2030.py | xmllint --valid --format -
