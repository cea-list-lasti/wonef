#!/bin/sh


if [ $# -lt 3 ]; then
  echo "Usage : ./editRandom.sh logdir errorType processingType"
  exit
fi



DATAHOME=/data/CEA/data/wn.fr
DATAHOME=/data2/CEA/wn.fr
DATAHOME=/data.2/CEA/wn.fr/CEA/wn.fr

max=`ls -l $DATAHOME/$1/$2 | wc -l `
echo "nbFiles :" $max
rand=$(( ${max}-1 ))
file=$DATAHOME/$1/$2/$(( 1+(`od -An -N2 -i /dev/urandom` ) %($rand) )).txt

echo "grep -l "$3 $file 
if [[ -n $3 ]]
then 
while [[ ! `grep -l $3 $file` ]]
do
	echo "retrying after : " $file
	file=$DATAHOME/$1/$2/$(( 1+(`od -An -N2 -i /dev/urandom` ) %($(($max-1))) )).txt
	
done
fi 

echo "opening : "  $file
vi $file

sh ~/CEA/WN.FR/Fast_KNN_Search.mpi/wntransfertmodules/howMuch.sh $1 $2
