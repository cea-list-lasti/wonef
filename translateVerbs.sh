#!/bin/bash

WOLF='/home/qp230782/projets/wolf/wolf-0.1.4.xml'
EWN='/home/qp230782/ressources/wn_fr.ewn '
POLYSEMOUSINDEX='/home/baguenierj/Projets/index.polysemous.verb'
BCSMODE=4
BCSFILE='/home/qp230782/projets/5000_bc.xml'

seqsspaces=$*
seqs=${seqsspaces// /}

echo "Translating... $seqsspaces"
./translateVerbs $seqsspaces &> logs/transVerbs$seqs

#valgrind --show-reachable=yes ./translateVerbs $seqsspaces
#gdb --args ./translateVerbs $seqsspaces

# The produced file needs some fixes before evaluation
WNDATA="data2/data.fr.verbs$seqs"
WNBESTDATA="data2/data.fr.verbs.best$seqs"
echo "Fixing WNDATA..."
sed -i 's/&/&amp;/g' $WNDATA
sed -i 's/&/&amp;/g' $WNBESTDATA
sed -i -r 's/[<]([^A-Z/])/\&lt;\1/g'  $WNDATA
sed -i -r 's/[<]([^A-Z/])/\&lt;\1/g'  $WNBESTDATA
sed -i -r 's/([^A-Z /"])[>]/\&gt;\1/g'  $WNDATA
sed -i -r 's/([^A-Z /"])[>]/\&gt;\1/g'  $WNBESTDATA

echo "Evaluating..."
#./evalVerbsJAWS-WOLF $POLYSEMOUSINDEX $WOLF $WNDATA wolf $BCSMODE $BCSFILE &> logs/evalVerbs$seqs #Wolf
./evalVerbsJAWS-WOLF $POLYSEMOUSINDEX $EWN $WNDATA ewn $BCSMODE $BCSFILE &> logs/evalVerbsEWN$seqs #EWN
./evalVerbsJAWS-WOLF $POLYSEMOUSINDEX $EWN $WNBESTDATA ewn $BCSMODE $BCSFILE &> logs/evalVerbsBestEWN$seqs
