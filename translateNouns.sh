#!/bin/bash

# Usage
# The parameters specify which iterations should be run. For example, with
# ./translateNouns.sh 1 2 3 4, iterations 1 2 3 4 will be launched.
#
# Once finished, interesting files are:
#  * data2/data.fr.nouns.wolf.nouns.Noen1234 (it's JAWS!)
#  * logs/trans1234
#  * logs/eval1234 (end of file contains results)

# Reusing our above example, seqspaces will be "1 2 3 4" and seqs will be
# "1234". Having $seqs is useful for filenames, while seqspaces is used to
# launch translateWN.
seqsspaces=$*
seqs=${seqsspaces// /}


WOLF='/home/qp230782/Projets/wolf/wolf-0.1.4.xml'
# This is simply index.noun without the monosemous nouns
POLYSEMOUSINDEX='/home/qp230782/Projets/index.polysemous.noun'
# OLDSCHOOL (means I don't care about BCSFILE?)
#BCSMODE=4
#BCSFILE='/home/qp230782/Projets/5000_bc.xml'
GOLD='/data/text/jeanne/Gold/VT_noms.xml'

echo "Translating... $seqsspaces"
# It's really WOLF, not $WOLF
./translateWN WOLF Noen $seqsspaces 2>&1 | tee logs/transNouns$seqs | egrep "duration|note"
gprof translateWN > profiledNoun 2> /dev/null

# The produced file needs some fixes before evaluation
WNDATA="data2/data.fr.nouns.wolf.Noen$seqs"
WNBESTDATA="data2/data.fr.nouns.best.wolf.Noen$seqs"

echo -e "\n-- Evaluating with Wolf... --"
./evalJAWS-WOLF noun $POLYSEMOUSINDEX $WOLF $WNDATA wolf &> logs/evalNouns$seqs
echo -e "\n                *** Normal ***"
tail -27 logs/evalNouns$seqs
./evalJAWS-WOLF noun $POLYSEMOUSINDEX $WOLF $WNBESTDATA wolf &> logs/evalNounsBest$seqs
echo -e "\n                *** Best ***"
tail -27 logs/evalNounsBest$seqs


echo -e "\n-- Evaluating with Gold... --"
./evalJAWS-WOLF noun $POLYSEMOUSINDEX $GOLD $WNDATA gold &> logs/evalNounsG$seqs
echo -e "\n                *** Normal ***"
tail -27 logs/evalNounsG$seqs
./evalJAWS-WOLF noun $POLYSEMOUSINDEX $GOLD $WNBESTDATA gold &> logs/evalNounsGBest$seqs
echo -e "\n                *** Best ***"
tail -27 logs/evalNounsGBest$seqs
