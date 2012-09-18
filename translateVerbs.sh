#!/bin/bash

# Usage
# The parameters specify which iterations should be run. For example, with
# ./translateVerbs.sh 1 2 3 4, iterations 1 2 3 4 will be launched.
#
# Once finished, interesting files are:
#  * data2/data.fr.verbs.wolf.nouns.Noen1234 (it's JAWS!)
#  * logs/transVerbs1234
#  * various evaluation files (end of file contains results):
#    * logs/evalVerbs1234      # WOLF / normal
#    * logs/evalVerbsG1234     # WOLF / best
#    * logs/evalVerbsBest1234  # gold / normal
#    * logs/evalVerbsGBest1234 # gold / best

WOLF='/home/qp230782/projets/wolf/wolf-0.1.4.xml'
EWN='/home/qp230782/ressources/wn_fr.ewn.utf8'
GOLD='/data/text/jeanne/Gold/VT_verbes.xml'
POLYSEMOUSINDEX='/home/baguenierj/Projets/index.polysemous.verb'
#BCSMODE=4
#BCSFILE='/home/qp230782/projets/5000_bc.xml'

seqsspaces=$*
seqs=${seqsspaces// /}

# Store the time of launch for archives
day=`date +%Y_%B_%d`
time=`date +%H_%M_%S`

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

# Archive relevant files to our archive.
echo -e "Finished! Archiving..."
tmpsubdir="Verbs__${day}__${time}"
tmppath=/tmp/$tmpsubdir
archivedir=/data/text/quentin/archives/$day/

echo $tmpsubdir $tmppath $archivedir
mkdir -p $tmppath
mkdir -p $archivedir

cp logs/transVerbs$seqs $tmppath
cp logs/evalVerbs$seqs logs/evalVerbsBest$seqs logs/evalVerbsG$seqs logs/evalVerbsGBest$seqs $tmppath
cp $WNDATA $WNBESTDATA $tmppath

pushd /tmp
  tar cjf $archivedir/Verbs__$time.tar.bz2 $tmpsubdir
  rm -rf $tmpsubdir
popd
