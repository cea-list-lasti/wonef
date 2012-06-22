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
BCSMODE=4
BCSFILE='/home/qp230782/Projets/5000_bc.xml'

echo "Translating... $seqsspaces"
# It's really WOLF, not $WOLF
./translateWN WOLF noun Noen $seqsspaces &> logs/trans$seqs

# The produced file needs some fixes before evaluation
WNDATA="data2/data.fr.nouns.wolf.noun.Noen$seqs"
echo "Fixing WNDATA..."
sed -i 's/&/&amp;/g' $WNDATA
sed -i -r 's/[<]([^A-Z/])/\&lt;\1/g'  $WNDATA
sed -i -r 's/([^A-Z /"])[>]/\&gt;\1/g'  $WNDATA

echo "Evaluating..."
./evalJAWS-WOLF $POLYSEMOUSINDEX $WOLF $WNDATA wolf $BCSMODE $BCSFILE &> logs/eval$seqs

