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
POLYSEMOUSINDEX='/home/baguenierj/Projets/index.polysemous.adj'
# OLDSCHOOL (means I don't care about BCSFILE?)
#BCSMODE=4
#BCSFILE='/home/qp230782/Projets/5000_bc.xml'
GOLD='/data/text/jeanne/Gold/VT_adjectifs.xml'

echo "Translating... $seqsspaces"
./translateAdjs Noen $seqsspaces 2>&1 | tee logs/transAdjs$seqs | grep "duration "
gprof translateAdjs > profiledAdj 2> /dev/null

# The produced file needs some fixes before evaluation
WNDATA="data2/data.fr.adjs.Noen$seqs"
WNBESTDATA="data2/data.fr.adjs.best.Noen$seqs"

echo -e "\n-- Evaluating with Wolf... --"
./evalJAWS-WOLF adj $POLYSEMOUSINDEX $WOLF $WNDATA wolf &> logs/evalAdjs$seqs
echo -e "\n                *** Normal ***"
tail -25 logs/evalAdjs$seqs
./evalJAWS-WOLF adj $POLYSEMOUSINDEX $WOLF $WNBESTDATA wolf &> logs/evalAdjsBest$seqs
echo -e "\n                *** Best ***"
tail -25 logs/evalAdjsBest$seqs

echo -e "\n-- Evaluating with Gold... --"
./evalJAWS-WOLF adj $POLYSEMOUSINDEX $GOLD $WNDATA gold &> logs/evalAdjsG$seqs
echo -e "\n                *** Normal ***"
tail -25 logs/evalAdjsG$seqs
./evalJAWS-WOLF adj $POLYSEMOUSINDEX $GOLD $WNBESTDATA gold &> logs/evalAdjsGBest$seqs
echo -e "\n                *** Best ***"
tail -25 logs/evalAdjsGBest$seqs
