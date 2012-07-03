#!/bin/bash

WOLF='/home/qp230782/projets/wolf/wolf-0.1.4.xml'
EWN='/home/qp230782/ressources/wn_fr.ewn '
POLYSEMOUSINDEX='/home/baguenierj/Projets/index.polysemous.verb'
BCSMODE=4
BCSFILE='/home/qp230782/projets/5000_bc.xml'

seqsspaces=$*
seqs=${seqsspaces// /}

echo "Translating... $seqsspaces"
./translateVerbs Noen $seqsspaces 2>&1 | tee logs/transVerbs$seqs | grep duration

#valgrind --show-reachable=yes ./translateVerbs $seqsspaces
#gdb --args ./translateVerbs $seqsspaces

# The produced file needs some fixes before evaluation
WNDATA="data2/data.fr.verbs.Noen$seqs"
WNBESTDATA="data2/data.fr.verbs.best.Noen$seqs"

echo -e "\n-- Evaluating... --"
./evalJAWS-WOLF verb $POLYSEMOUSINDEX $EWN $WNDATA ewn $BCSMODE $BCSFILE &> logs/evalVerbs$seqs
echo -e "\n                *** Normal ***"
tail -12 logs/evalVerbs$seqs
./evalJAWS-WOLF verb $POLYSEMOUSINDEX $EWN $WNBESTDATA ewn $BCSMODE $BCSFILE &> logs/evalVerbsBest$seqs
echo -e "\n                *** Best ***"
tail -12 logs/evalVerbsBest$seqs
