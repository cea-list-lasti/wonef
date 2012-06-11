#---------------------------------------#
# Extraction de la partie additionnelle #
#---------------------------------------#

QUERY='addPart.xq'
ADDPART='data2/JAWS.xml'

input=$1 # JAWS dont on veut évaluer la partie additionnelle
module=$2 # module ayant engendré la partie additionnelle que l'on veut évaluer

echo "Extracting $module from data2/$input ..."

# Adaptation de la requête
sed -i "s/INPUT/$input/" $QUERY
sed -i "s/MODULE/$module/" $QUERY

java -cp /data/text/bin/basex.jar org.basex.BaseX < $QUERY &> $ADDPART

# Réinitialisation de la requête
sed -i "s/$input/INPUT/" $QUERY
sed -i "s/$module/MODULE/" $QUERY

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

EWN='/home/qp230782/ressources/wn_fr.ewn '
POLYSEMOUSINDEX='/home/baguenierj/Projets/index.polysemous.verb'
BCSMODE=4
BCSFILE='/home/qp230782/projets/5000_bc.xml'

./evalVerbsJAWS-WOLF $POLYSEMOUSINDEX $EWN $ADDPART ewn $BCSMODE $BCSFILE &> logs/evalVerbsEWN_$module
