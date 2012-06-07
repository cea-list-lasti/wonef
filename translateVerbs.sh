#!/bin/bash

WOLF='/home/qp230782/projets/wolf/wolf-0.1.4.xml'
EWN='/home/qp230782/ressources/wn_fr.ewn '
POLYSEMOUSINDEX='/home/baguenierj/Projets/index.polysemous.verb' #2.0
#POLYSEMOUSINDEX='/home/baguenierj/Projets/VERB.POLYSEMOUS.IDX' #1.5
BCSMODE=4
BCSFILE='/home/qp230782/projets/5000_bc.xml'
MAPFILE='/data/text/jeanne/mappings-upc-2007/mapping-20-15/wn20-15.verb'

seqsspaces=$*
seqs=${seqsspaces// /}

echo "Translating... $seqsspaces"
./translateVerbs $seqsspaces &> logs/transVerbs$seqs #2.0
#./translateVerbs EWN $seqsspaces &> logs/transVerbsEWN$seqs #1.5

#valgrind --show-reachable=yes ./translateVerbs $seqsspaces
#gdb --args ./translateVerbs $seqsspaces

# The produced file needs some fixes before evaluation
WNDATA="data2/data.fr.verbs$seqs" #2.0
#WNDATA="data2/data.fr.verbs.ewn$seqs" #1.5
echo "Fixing WNDATA..."
sed -i 's/&/&amp;/g' $WNDATA
sed -i -r 's/[<]([^A-Z/])/\&lt;\1/g'  $WNDATA
sed -i -r 's/([^A-Z "])[>]/\&gt;\1/g'  $WNDATA

echo "Evaluating..."
#./evalVerbsJAWS-WOLF $POLYSEMOUSINDEX $WOLF $WNDATA wolf $BCSMODE $BCSFILE &> logs/evalVerbs$seqs #Wolf
./evalVerbsJAWS-WOLF $POLYSEMOUSINDEX $EWN $WNDATA ewn $BCSMODE $BCSFILE $MAPFILE &> logs/evalVerbsEWN$seqs #EWN
