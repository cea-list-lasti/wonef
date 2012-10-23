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

echo "Translating... $seqsspaces"
# It's really WOLF, not $WOLF
./translate$Poss $seqsspaces 2>&1 | tee logs/trans$Poss.$seqs | egrep "duration|note"
if [[ ${PIPESTATUS[0]} -ne 0 ]]; then echo "Translation failed, exiting."; exit 255; fi
gprof translate$Poss > profiled$Poss 2> /dev/null

echo -e "\n-- Evaluating with $REFERENCE... --"
./evalJAWS-WOLF $pos $seqs
gprof evalJAWS-WOLF > profiledEval$Poss.$seqs
if [[ ${PIPESTATUS[0]} -ne 0 ]]; then echo "Evaluation failed, exiting."; exit 255; fi

echo -e "\n                *** Normal ***"
tail -4 logs/eval.$pos.$seqs
echo -e "\n                *** Best ***"
tail -4 logs/eval.best.$pos.$seqs
echo -e "\n-- Evaluating with Gold... --"
echo -e "\n                *** Normal ***"
tail -4 logs/eval.gold.${pos}.$seqs
echo -e "\n                *** Best ***"
tail -4 logs/eval.gold.best.${pos}.$seqs

# Archive relevant files to our archive.

#echo -n "Finished! Archiving to $archivedir/${Poss}_${seqs}_$time.tar.bz2..."

#tmpsubdir="$Poss__${day}__${time}"
#tmppath=/tmp/$tmpsubdir
#archivedir=/home/pradet/archives/$day

#mkdir -p $tmppath
#mkdir -p $archivedir
#chmod -f o+w $archivedir

#cp logs/trans$Poss.$seqs $tmppath
#cp logs/eval$Poss.$seqs logs/eval${Poss}Best.$seqs logs/eval${Poss}G.$seqs logs/eval${Poss}GBest.$seqs $tmppath
#cp $WNDATA $WNBESTDATA $tmppath
#cp profiled$Poss $tmppath

#pushd /tmp > /dev/null
#  tar cjf $archivedir/$Poss__$time.tar.bz2 $tmpsubdir
#  rm -rf $tmpsubdir
#popd > /dev/null

#echo " done!"
