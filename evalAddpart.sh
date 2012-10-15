#!/bin/bash
#---------------------------------------#
# Extraction de la partie additionnelle #
#---------------------------------------#

cp addPart.xq addPart.xq.tmp
QUERY='addPart.xq.tmp'
ADDPART='data2/JAWS.xml'

input=$1 # JAWS dont on veut évaluer la partie additionnelle (situé dans data2/)
module=$2 # module ayant engendré la partie additionnelle que l'on veut évaluer
pos=$3 # nature du JAWS que l'on veut évaluer (N pour nom, V pour verbe)

# exemple :
#./evalAddpart.sh data2/data.fr.nouns.best.wolf.noun.Noen1 simsyn1.1 N
# ou
#./evalAddpart.sh data2/data.fr.adjs.Noen1 vote-lastchance1.1 A
# ou
#./evalAddpart.sh data2/data.fr.verbs.Noen1 vote-lastchance1.1 V

echo "Extracting $module from $input ..."

# Adaptation de la requête
sed -i "s,INPUT,$PWD/$input," $QUERY
sed -i "s/MODULE/$module/g" $QUERY

java -cp /home/pradet/bin/basex.jar org.basex.BaseX < $QUERY &> $ADDPART

rm $QUERY

# Mise en forme du fichier de sortie (la partie additionnelle de JAWS)
sed -i 's/^> //' $ADDPART
sed -i '/^[A-Z].*$/d' $ADDPART
sed -i '/^$/d' $ADDPART
sed -i "1i\<JAWS pos=\"verb\" module=\"$module\">" $ADDPART
echo '</JAWS>' >> $ADDPART

#---------------------------------------#
# Evaluation de la partie additionnelle #
#---------------------------------------#

echo "Evaluating additional part ..."

DATAPATH="/home/pradet/data"
WOLF="$DATAPATH/opendata/wolf/wolf-0.1.4.xml"
EWN="$DATAPATH/opendata/ewn/wn_fr.ewn.utf8"
GOLDV="$DATAPATH/Gold/GT_verbs.xml"
GOLDA="$DATAPATH/Gold/GT_adjs.xml"
GOLDN="$DATAPATH/Gold/GT_nouns.xml"
POLYSEMOUSINDEXN="$DATAPATH/opendata/polysemous/WordNet-2.0/index.polysemous.noun"
POLYSEMOUSINDEXV="$DATAPATH/opendata/polysemous/WordNet-2.0/index.polysemous.verb"
POLYSEMOUSINDEXA="$DATAPATH/opendata/polysemous/WordNet-2.0/index.polysemous.adj"
BCSMODE=4
BCSFILE="$DATAPATH/opendata/5000_bc.xml"

if [ "$pos" = "N" ]
then
  ./evalJAWS-WOLF noun $POLYSEMOUSINDEXN $WOLF $ADDPART WOLF $BCSMODE $BCSFILE &> logs/evalNouns_$module
  tail -27 logs/evalNouns_$module
  echo -e "\n-- Evaluating with Gold... --"
  ./evalJAWS-WOLF noun $POLYSEMOUSINDEXN $GOLDN $ADDPART GOLD &> logs/evalNounsG_$module
  tail -27 logs/evalNounsG_$module
elif [ "$pos" = "A" ]
then
  ./evalJAWS-WOLF adj $POLYSEMOUSINDEXA $WOLF $ADDPART WOLF $BCSMODE $BCSFILE &> logs/evalAdjs_$module
  tail -27 logs/evalAdjs_$module
  echo -e "\n-- Evaluating with Gold... --"
  ./evalJAWS-WOLF adj $POLYSEMOUSINDEXA $GOLDA $ADDPART GOLD &> logs/evalAdjsG_$module
  tail -27 logs/evalAdjsG_$module
elif [ "$pos" = "V" ]
then
  ./evalJAWS-WOLF verb $POLYSEMOUSINDEXV $EWN $ADDPART EWN $BCSMODE $BCSFILE &> logs/evalVerbs_$module
  tail -27 logs/evalVerbs_$module
  echo -e "\n-- Evaluating with Gold... --"
  ./evalJAWS-WOLF verb $POLYSEMOUSINDEXV $GOLDV $ADDPART GOLD &> logs/evalVerbsG_$module
  tail -27 logs/evalVerbsG_$module
else
  echo "Precise the POS to evaluate"
fi
