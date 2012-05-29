#!/bin/bash

WOLF='/home/qp230782/projets/wolf/wolf-0.1.4.xml'
POLYSEMOUSINDEX='/home/qp230782/projets/index.polysemous.noun'
BCSMODE=4
BCSFILE='/home/qp230782/projets/5000_bc.xml'

seqsspaces=$*
seqs=${seqsspaces// /}

echo "Translating... $seqsspaces"
# It's really WOLF, not $WOLF
./translateWN WOLF noun Noen $seqsspaces &> logs/trans$seqs

# The produced file needs some fixes before evaluation
WNDATA="data2/data.fr.nouns.wolf.noun.Noen$seqs"
echo "Fixing WNDATA..."
sed -i 's/&/&amp;/g' $WNDATA
sed -i -r 's/[<]([^A-Z/])/\&lt;\1/g'  $WNDATA
sed -i -r 's/([^A-Z "])[>]/\&gt;\1/g'  $WNDATA

echo "Evaluating..."
./evalJAWS-WOLF $POLYSEMOUSINDEX $WOLF $WNDATA wolf $BCSMODE $BCSFILE &> logs/eval$seqs

