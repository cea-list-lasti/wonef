#---------------------------------------#
# Extraction de la partie additionnelle #
#---------------------------------------#

cp addPart.xq addPart.xq.tmp
QUERY='addPart.xq.tmp'
ADDPART='data2/JAWS.xml'

input=$1 # JAWS dont on veut évaluer la partie additionnelle (situé dans data2/)
module=$2 # module ayant engendré la partie additionnelle que l'on veut évaluer
pos=$3 # nature du JAWS que l'on veut évaluer (N pour nom, V pour verbe)

# /!\ à antislasher tous les "."
# exemple :
#./evalAddpart.sh data\.fr\.nouns\.best\.wolf\.noun\.Noen1 simsyn1\.1 N
# ou
#./evalAddpart.sh data\.fr\.verbs1 vote-lastchance1\.1 V

echo "Extracting $module from data2/$input ..."

# Adaptation de la requête
sed -i "s/INPUT/$input/" $QUERY
sed -i "s/MODULE/$module/g" $QUERY

java -cp /data/text/bin/basex.jar org.basex.BaseX < $QUERY &> $ADDPART

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

WOLF='/home/qp230782/Projets/wolf/wolf-0.1.4.xml'
EWN='/home/qp230782/ressources/wn_fr.ewn '
POLYSEMOUSINDEX='/home/baguenierj/Projets/index.polysemous.verb'
BCSMODE=4
BCSFILE='/home/qp230782/projets/5000_bc.xml'

if [ "$pos" = "N" ]
then
./evalJAWS-WOLF $POLYSEMOUSINDEX $WOLF $ADDPART wolf $BCSMODE $BCSFILE &> logs/evalNouns_$module
elif [ "$pos" = "V" ]
then
./evalVerbsJAWS-WOLF $POLYSEMOUSINDEX $EWN $ADDPART ewn $BCSMODE $BCSFILE &> logs/evalVerbsEWN_$module
else
echo "Precise the POS to evaluate"
fi
