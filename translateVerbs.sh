#!/bin/bash

WOLF='/home/qp230782/projets/wolf/wolf-0.1.4.xml'
EWN='/home/qp230782/ressources/wn_fr.ewn.utf8'
GOLD='/data/text/jeanne/Gold/VT_verbes.xml'
POLYSEMOUSINDEX='/home/baguenierj/Projets/index.polysemous.verb'
#BCSMODE=4
#BCSFILE='/home/qp230782/projets/5000_bc.xml'

seqsspaces=$*
seqs=${seqsspaces// /}

echo "Translating... $seqsspaces"
./translateVerbs Noen $seqsspaces 2>&1 | tee logs/transVerbs$seqs | egrep "duration|note"
gprof translateVerbs > profiledVerb 2> /dev/null

#valgrind --show-reachable=yes ./translateVerbs $seqsspaces
#gdb --args ./translateVerbs $seqsspaces

# The produced file needs some fixes before evaluation
WNDATA="data2/data.fr.verbs.Noen$seqs"
WNBESTDATA="data2/data.fr.verbs.best.Noen$seqs"

echo -e "\n-- Evaluating with EWN... --"
./evalJAWS-WOLF verb $POLYSEMOUSINDEX $EWN $WNDATA ewn &> logs/evalVerbs$seqs
echo -e "\n                *** Normal ***"
tail -27 logs/evalVerbs$seqs
./evalJAWS-WOLF verb $POLYSEMOUSINDEX $EWN $WNBESTDATA ewn &> logs/evalVerbsBest$seqs
echo -e "\n                *** Best ***"
tail -27 logs/evalVerbsBest$seqs

echo -e "\n-- Evaluating with Gold... --"
./evalJAWS-WOLF verb $POLYSEMOUSINDEX $GOLD $WNDATA gold &> logs/evalVerbsG$seqs
echo -e "\n                *** Normal ***"
tail -27 logs/evalVerbsG$seqs
./evalJAWS-WOLF verb $POLYSEMOUSINDEX $GOLD $WNBESTDATA gold &> logs/evalVerbsGBest$seqs
echo -e "\n                *** Best ***"
tail -27 logs/evalVerbsGBest$seqs
