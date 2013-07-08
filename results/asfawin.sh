#!/bin/bash

echo "WOLF 0.1.4"
asfawon wolf-0.1.4/wolf-0.1.4-wn30.xml  | grep "Nb "
echo

echo "WOLF 1.0b"
asfawon wolf-1.0b/wolf-1.0b.withManValNO.xml
echo

echo "WoNeF precision"
asfawon wonef-0.1/wonef-precision-0.1.xml
echo

echo "WoNeF F-score"
asfawon wonef-0.1/wonef-fscore-0.1.xml
echo

echo "WOLF 0.1.4 + WoNeF precision"
echo " All"
asfawon wolf-0.1.4/wolf-0.1.4-wn30.xml wonef-0.1/wonef-precision-0.1.xml -o output-0p.xml > output-0p.log
./stats.sh output-0p.log
echo " n"
asfawon -p n wolf-0.1.4/wolf-0.1.4-wn30.xml wonef-0.1/wonef-precision-0.1.xml -o output-0p-n.xml > output-0p-n.log
./stats.sh output-0p-n.log
echo " v"
asfawon -p v wolf-0.1.4/wolf-0.1.4-wn30.xml wonef-0.1/wonef-precision-0.1.xml -o output-0p-v.xml > output-0p-v.log
./stats.sh output-0p-v.log
echo " a"
asfawon -p a wolf-0.1.4/wolf-0.1.4-wn30.xml wonef-0.1/wonef-precision-0.1.xml -o output-0p-a.xml > output-0p-a.log
./stats.sh output-0p-a.log
echo " b"
asfawon -p b wolf-0.1.4/wolf-0.1.4-wn30.xml wonef-0.1/wonef-precision-0.1.xml -o output-0p-b.xml > output-0p-b.log
./stats.sh output-0p-b.log
echo

echo "WOLF 0.1.4 + WoNeF F-score"
echo " All"
asfawon wolf-0.1.4/wolf-0.1.4-wn30.xml wonef-0.1/wonef-fscore-0.1.xml -o output-0f.xml > output-0f.log
./stats.sh output-0f.log
echo " n"
asfawon -p n wolf-0.1.4/wolf-0.1.4-wn30.xml wonef-0.1/wonef-fscore-0.1.xml -o output-0f-n.xml > output-0f-n.log
./stats.sh output-0f-n.log
echo " v"
asfawon -p v wolf-0.1.4/wolf-0.1.4-wn30.xml wonef-0.1/wonef-fscore-0.1.xml -o output-0f-v.xml > output-0f-v.log
./stats.sh output-0f-v.log
echo " a"
asfawon -p a wolf-0.1.4/wolf-0.1.4-wn30.xml wonef-0.1/wonef-fscore-0.1.xml -o output-0f-a.xml > output-0f-a.log
./stats.sh output-0f-a.log
echo " b"
asfawon -p b wolf-0.1.4/wolf-0.1.4-wn30.xml wonef-0.1/wonef-fscore-0.1.xml -o output-0f-b.xml > output-0f-b.log
./stats.sh output-0f-b.log
echo


echo "WOLF 1.0b + WoNeF precision"
echo " All"
asfawon wolf-1.0b/wolf-1.0b.withManValNO.xml wonef-0.1/wonef-precision-0.1.xml -o output-p.xml > output-p.log
./stats.sh output-p.log
echo " n"
asfawon -p n wolf-1.0b/wolf-1.0b.withManValNO.xml wonef-0.1/wonef-precision-0.1.xml -o output-p-n.xml > output-p-n.log
./stats.sh output-p-n.log
echo " v"
asfawon -p v wolf-1.0b/wolf-1.0b.withManValNO.xml wonef-0.1/wonef-precision-0.1.xml -o output-p-v.xml > output-p-v.log
./stats.sh output-p-v.log
echo " a"
asfawon -p a wolf-1.0b/wolf-1.0b.withManValNO.xml wonef-0.1/wonef-precision-0.1.xml -o output-p-a.xml > output-p-a.log
./stats.sh output-p-a.log
echo " b"
asfawon -p b wolf-1.0b/wolf-1.0b.withManValNO.xml wonef-0.1/wonef-precision-0.1.xml -o output-p-b.xml > output-p-b.log
./stats.sh output-p-b.log
echo

echo "WOLF 1.0b + WoNeF F-score"
echo " All"
asfawon wolf-1.0b/wolf-1.0b.withManValNO.xml wonef-0.1/wonef-fscore-0.1.xml -o output-f.xml > output-f.log
./stats.sh output-f.log
echo " n"
asfawon -p n wolf-1.0b/wolf-1.0b.withManValNO.xml wonef-0.1/wonef-fscore-0.1.xml -o output-f-n.xml > output-f-n.log
./stats.sh output-f-n.log
echo " v"
asfawon -p v wolf-1.0b/wolf-1.0b.withManValNO.xml wonef-0.1/wonef-fscore-0.1.xml -o output-f-v.xml > output-f-v.log
./stats.sh output-f-v.log
echo " a"
asfawon -p a wolf-1.0b/wolf-1.0b.withManValNO.xml wonef-0.1/wonef-fscore-0.1.xml -o output-f-a.xml > output-f-a.log
./stats.sh output-f-a.log
echo " b"
asfawon -p b wolf-1.0b/wolf-1.0b.withManValNO.xml wonef-0.1/wonef-fscore-0.1.xml -o output-f-b.xml > output-f-b.log
./stats.sh output-f-b.log
echo

