#!/bin/bash

# Usage
# The parameters specify which extraction and module iterations should be run.
# For example, with # ./translateWN.sh -e 1 2 3 -m {1..4}, extraction 1 to 3
# and modules 1 to 4 will be launched.
#
# Once finished, interesting files are:
#  * data2/data.fr.$POS.wolf.Noen.e123m12 (it's JAWS!)
#  * logs/transNouns.e123m12
#  * various evaluation files (end of file contains results):
#    * logs/evalNouns.e123m12      # WOLF / normal
#    * logs/evalNounsG.e123m12     # WOLF / best
#    * logs/evalNounsBest.e123m12  # gold / normal
#    * logs/evalNounsGBest.e123m12 # gold / best

DATAPATH="/home/pradet/data"

if [ "$1" != "noun" -a "$1" != "adj" -a "$1" != "verb" ]; then
  echo "Usage: ./translateWN.sh pos --extract 1 2 4 --module 5 4 3"
  exit 255
fi

# Parse the part-of-speech
pos=$1 # noun
poss="${1}s" #nouns
Poss=`echo $poss | sed 's/./\U&/'` #Nouns
shift

if [ "$pos" = "verb" ]; then
  REFERENCE='EWN'
else
  REFERENCE='WOLF'
fi

# Parses the remaining options
mode='module'

until [ -z "$1" ]
do
  case $1 in
    -e|--extract) mode='extract' ;;
    -m|--module) mode='module' ;;
    *) if [ "$mode" = "extract" ]; then
        extract="$extract$1"
        extractspaces="$extractspaces$1 "
       elif [ "$mode" = "module" ]; then
        module="$module$1"
        modulespaces="$modulespaces$1 "
       fi ;;
  esac
  shift
done

if [ -n "$extractspaces" ]; then
  extractoptions="--extract $extractspaces"
fi

# Reusing our above example, seqspaces will be "1 2 3 4" and seqs will be
# "1234". Having $seqs is useful for filenames, while seqspaces is used to
# launch translateWN.
seqsspaces="$extractoptions --module $modulespaces"
seqs="e${extract}.m${module}"

# Store the time of launch for archives
day=`date +%Y_%B_%d`
time=`date +%H_%M_%S`

WOLF="$DATAPATH/opendata/wolf/wolf-0.1.4.xml"
EWN="$DATAPATH/opendata/ewn/wn_fr.ewn.utf8"
# This is simply index.noun without the monosemous nouns
POLYSEMOUSINDEX="$DATAPATH/opendata/polysemous/WordNet-3.0/index.polysemous.$pos"
GOLD="$DATAPATH/Gold30/GT_${poss}30.xml"

echo "Translating... $seqsspaces"
# It's really WOLF, not $WOLF
./translate$Poss $seqsspaces 2>&1 | tee logs/trans$Poss.$seqs | egrep "duration|note"
if [[ ${PIPESTATUS[0]} -ne 0 ]]; then echo "Translation failed, exiting."; exit 255; fi
gprof translate$Poss > profiled$Poss 2> /dev/null

# The produced files
WNDATA="data2/data.fr.$poss.$seqs"
WNBESTDATA="data2/data.fr.$poss.best.$seqs"


echo -e "\n-- Evaluating with $REFERENCE... --"

./evalJAWS-WOLF $pos $POLYSEMOUSINDEX ${!REFERENCE} $WNDATA $REFERENCE &> logs/eval$Poss.$seqs
if [[ ${PIPESTATUS[0]} -ne 0 ]]; then echo "Evaluation failed, exiting."; exit 255; fi
echo -e "\n                *** Normal ***"
tail -29 logs/eval$Poss.$seqs

./evalJAWS-WOLF $pos $POLYSEMOUSINDEX ${!REFERENCE} $WNBESTDATA $REFERENCE &> logs/eval${Poss}Best.$seqs
if [[ ${PIPESTATUS[0]} -ne 0 ]]; then echo "Best evaluation failed, exiting."; exit 255; fi
echo -e "\n                *** Best ***"
tail -29 logs/eval${Poss}Best.$seqs

echo -e "\n-- Evaluating with Gold... --"
./evalJAWS-WOLF $pos $POLYSEMOUSINDEX $GOLD $WNDATA GOLD &> logs/eval${Poss}G.$seqs
if [[ ${PIPESTATUS[0]} -ne 0 ]]; then echo "Gold evaluation failed, exiting."; exit 255; fi
echo -e "\n                *** Normal ***"
tail -29 logs/eval${Poss}G.$seqs

./evalJAWS-WOLF $pos $POLYSEMOUSINDEX $GOLD $WNBESTDATA GOLD &> logs/eval${Poss}GBest.$seqs
if [[ ${PIPESTATUS[0]} -ne 0 ]]; then echo "Best Gold evaluation, exiting."; exit 255; fi
echo -e "\n                *** Best ***"
tail -29 logs/eval${Poss}GBest.$seqs

# Archive relevant files to our archive.
tmpsubdir="$Poss__${day}__${time}"
tmppath=/tmp/$tmpsubdir
archivedir=/home/pradet/archives/$day

mkdir -p $tmppath
mkdir -p $archivedir
chmod -f o+w $archivedir

echo -n "Finished! Archiving to $archivedir/${Poss}_${seqs}_$time.tar.bz2..."

cp logs/trans$Poss.$seqs $tmppath
cp logs/eval$Poss.$seqs logs/eval${Poss}Best.$seqs logs/eval${Poss}G.$seqs logs/eval${Poss}GBest.$seqs $tmppath
cp $WNDATA $WNBESTDATA $tmppath
cp profiled$Poss $tmppath

pushd /tmp > /dev/null
  tar cjf $archivedir/$Poss__$time.tar.bz2 $tmpsubdir
  rm -rf $tmpsubdir
popd > /dev/null

echo " done!"
