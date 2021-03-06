#!/bin/bash

set -o errexit

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

if [ "$1" != "noun" -a "$1" != "adj" -a "$1" != "verb" -a "$1" != "adv" ]; then
  echo "Usage: ./translateWN.sh pos --extract 1 2 4 --module 5 4 3"
  exit 255
fi

if [ `hostname` == "master0.alineos.net" ]; then
  echo "How do you *dare* launching this on master0?"
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
    -p|--prefer) mode='prefer' ;;
    -q|--quiet) quiet='quiet';;
    *) if [ "$mode" = "extract" ]; then
        extract="$extract$1"
        extractspaces="$extractspaces$1 "
       elif [ "$mode" = "module" ]; then
        module="$module$1"
        modulespaces="$modulespaces$1 "
       elif [ "$mode" = "prefer" ]; then
        prefer=$1
       fi ;;
  esac
  shift
done

if [ -n "$extractspaces" ]; then
  extractoptions="--extract $extractspaces"
fi

if [ -z "$quiet" ]; then
  grep_logs='Overall|note|duration'
else
  # unlikely to exist, right?
  grep_logs='afasdjseadgf'
fi

# Reusing our above example, seqspaces will be "1 2 3 4" and seqs will be
# "1234". Having $seqs is useful for filenames, while seqspaces is used to
# launch translateWN.
seqsspaces="$extractoptions --module $modulespaces --mode $prefer"
seqs="e${extract}.m${module}.$prefer"

# Store the time of launch for archives
day=`date +%Y_%B_%d`
time=`date +%H_%M_%S`

# Never, ever display logs or data from last time.
mkdir -p logs data
rm -f logs/* data/*

echo "Translating $pos $seqsspaces"
./build/translate$Poss $seqsspaces 2>&1 | tee logs/trans$Poss.$seqs | egrep $grep_logs
if [[ ${PIPESTATUS[0]} -ne 0 ]]; then echo "Translation failed, exiting."; exit 255; fi
gprof build/translate$Poss > profiles/profiled.create.$pos.$seqs 2> /dev/null

echo "Evaluating..."
./build/evalJAWS-WOLF $prefer $pos $seqs
if [[ ${PIPESTATUS[0]} -ne 0 ]]; then echo "Evaluation failed, exiting."; exit 255; fi
gprof build/evalJAWS-WOLF 2>/dev/null > profiles/profiled.eval.$pos.$seqs

echo "Done!"
