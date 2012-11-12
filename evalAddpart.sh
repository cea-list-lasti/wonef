#!/bin/bash
#---------------------------------------#
# Extraction de la partie additionnelle #
#---------------------------------------#

# exemple :
#./evalAddpart.sh noun e135.m43.precision multiplesource
# ou
#./evalAddpart.sh adj e1m12.coverage vote-lastchance1.1
# ou
#./evalAddpart.sh verb e12345m12345.fscore vote-lastchance1.1

pos=$1 # nature du JAWS que l'on veut évaluer (N pour nom, V pour verbe, A pour adjectif)
conf=$2 # configuration ayant servi à créer ce JAWS
module=$3 # module ayant engendré la partie additionnelle que l'on veut évaluer

echo "Extracting $module from $conf..."

# Adaptation de la requête
function extract() {
  inputjaws=$1
  outputjaws=$2

  # Extraction de la partie additionnelle
  echo "From $inputjaws to $outputjaws"
  cp addPart.xq addPart.xq.tmp
  QUERY='addPart.xq.tmp'
  sed -i "s,INPUT,$PWD/$inputjaws," $QUERY
  sed -i "s/MODULE/$module/g" $QUERY
  java -cp /home/pradet/bin/basex.jar org.basex.BaseX < $QUERY &> $outputjaws
  rm $QUERY

  # Mise en forme du fichier de sortie (la partie additionnelle de JAWS)
  sed -i 's/^> //' $outputjaws
  sed -i '/^[A-Z].*$/d' $outputjaws
  sed -i '/^$/d' $outputjaws
  sed -i "1i\<JAWS pos=\"$pos\" module=\"$module\">" $outputjaws
  echo '</JAWS>' >> $outputjaws
}

# JAWS dont on veut évaluer la partie additionnelle (situé dans data/)
input="data/jaws.$pos.$conf.xml"
addpart="data/jaws.$pos.$conf.$module.xml"
extract $input $addpart

inputbest="data/jaws.best.$pos.$conf.xml"
addpartbest="data/jaws.best.$pos.$conf.$module.xml"
extract $inputbest $addpartbest

#---------------------------------------#
# Evaluation de la partie additionnelle #
#---------------------------------------#

echo "Evaluating additional part ..."

./evalJAWS-WOLF $pos $conf.$module

echo -e "\n-- Evaluating with Wolf 1.0... --"
echo -e "\n                *** Normal ***"
tail -3 logs/eval.wolfone.${pos}.$conf.$module
echo -e "\n                *** Best ***"
tail -3 logs/eval.wolfone.best.${pos}.$conf.$module
echo -e "\n-- Evaluating with Gold... --"
echo -e "\n                *** Normal ***"
tail -3 logs/eval.gold.${pos}.$conf.$module
echo -e "\n                *** Best ***"
tail -3 logs/eval.gold.best.${pos}.$conf.$module
