#!/bin/bash

# Usage
# The parameters specify which iterations should be run. For example, with
# ./translateAdjs.sh 1 2 3 4, iterations 1 2 3 4 will be launched.
#
# Once finished, interesting files are:
#  * data2/data.fr.adjs.wolf.nouns.Noen1234 (it's JAWS!)
#  * logs/transAdjs1234
#  * various evaluation files (end of file contains results):
#    * logs/evalAdjs1234      # WOLF / normal
#    * logs/evalAdjsG1234     # WOLF / best
#    * logs/evalAdjsBest1234  # gold / normal
#    * logs/evalAdjsGBest1234 # gold / best

# Reusing our above example, seqspaces will be "1 2 3 4" and seqs will be
# "1234". Having $seqs is useful for filenames, while seqspaces is used to
# launch translateWN.
seqsspaces=$*
seqs=${seqsspaces// /}

# Store the time of launch for archives
day=`date +%Y_%B_%d`
time=`date +%H_%M_%S`

WOLF='/home/qp230782/Projets/wolf/wolf-0.1.4.xml'
# This is simply index.noun without the monosemous nouns
POLYSEMOUSINDEX='/home/baguenierj/Projets/index.polysemous.adj'
# OLDSCHOOL (means I don't care about BCSFILE?)
#BCSMODE=4
#BCSFILE='/home/qp230782/Projets/5000_bc.xml'
GOLD='/data/text/jeanne/Gold/VT_adjectifs.xml'

echo "Translating... $seqsspaces"
./translateAdjs Noen $seqsspaces 2>&1 | tee logs/transAdjs$seqs | egrep "duration|note"
gprof translateAdjs > profiledAdj 2> /dev/null

# The produced file needs some fixes before evaluation
WNDATA="data2/data.fr.adjs.Noen$seqs"
WNBESTDATA="data2/data.fr.adjs.best.Noen$seqs"

echo -e "\n-- Evaluating with Wolf... --"
./evalJAWS-WOLF adj $POLYSEMOUSINDEX $WOLF $WNDATA wolf &> logs/evalAdjs$seqs
echo -e "\n                *** Normal ***"
tail -27 logs/evalAdjs$seqs
./evalJAWS-WOLF adj $POLYSEMOUSINDEX $WOLF $WNBESTDATA wolf &> logs/evalAdjsBest$seqs
echo -e "\n                *** Best ***"
tail -27 logs/evalAdjsBest$seqs

echo -e "\n-- Evaluating with Gold... --"
./evalJAWS-WOLF adj $POLYSEMOUSINDEX $GOLD $WNDATA gold &> logs/evalAdjsG$seqs
echo -e "\n                *** Normal ***"
tail -27 logs/evalAdjsG$seqs
./evalJAWS-WOLF adj $POLYSEMOUSINDEX $GOLD $WNBESTDATA gold &> logs/evalAdjsGBest$seqs
echo -e "\n                *** Best ***"
tail -27 logs/evalAdjsGBest$seqs

# Archive relevant files to our archive.
tmpsubdir="Adjs__${day}__${time}"
tmppath=/tmp/$tmpsubdir
archivedir=/data/text/quentin/archives/$day

mkdir -p $tmppath
mkdir -p $archivedir
chmod -f o+w $archivedir

echo -n "Finished! Archiving to $archivedir/Adjs__$time.tar.bz2..."

cp logs/transAdjs$seqs $tmppath
cp logs/evalAdjs$seqs logs/evalAdjsBest$seqs logs/evalAdjsG$seqs logs/evalAdjsGBest$seqs $tmppath
cp $WNDATA $WNBESTDATA $tmppath
cp profiledAdj $tmppath

pushd /tmp > /dev/null
tar cjf $archivedir/Adjs__$time.tar.bz2 $tmpsubdir
  rm -rf $tmpsubdir
popd > /dev/null

echo " done!"
