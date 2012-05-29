#!/bin/bash

WOLF='/home/qp230782/projets/wolf/wolf-0.1.4.xml'
POLYSEMOUSINDEX='/home/baguenierj/Projets/index.polysemous.verb'
BCSMODE=4
BCSFILE='/home/qp230782/projets/5000_bc.xml'

seqsspaces=$*
seqs=${seqsspaces// /}

echo "Translating... $seqsspaces"
./translateVerbs $seqsspaces &> logs/transVerbs$seqs

# The produced file needs some fixes before evaluation
WNDATA="data2/data.fr.verbs$seqs"
echo "Fixing WNDATA..."
sed -i 's/&/&amp;/g' $WNDATA
sed -i -r 's/[<]([^A-Z/])/\&lt;\1/g'  $WNDATA
sed -i -r 's/([^A-Z "])[>]/\&gt;\1/g'  $WNDATA

echo "Evaluating..."
./evalVerbsJAWS-WOLF $POLYSEMOUSINDEX $WOLF $WNDATA wolf $BCSMODE $BCSFILE &> logs/evalVerbs$seqs