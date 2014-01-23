#!/bin/sh
 
 
if [ $# -lt 2 ]; then
   echo "Usage : ./howmuch.sh logdir type"
   exit
fi

DATADIR=/data.2/CEA/wn.fr/CEA/wn.fr

grep -lre "ERRORTYPE : [A-Z]" $DATADIR/$1/$2 | wc -l


MT1=`grep -lre "ERRORTYPE : MT1" $DATADIR/$1/$2 | wc -l`
echo MT1 : $MT1

MT2=`grep -lre "ERRORTYPE : MT2" $DATADIR/$1/$2 | wc -l`
echo MT2 : $MT2

echo ------------------

MP1=`grep -lre "ERRORTYPE : MP1" $DATADIR/$1/$2 | wc -l`
echo MP1 : $MP1

MP2=`grep -lre "ERRORTYPE : MP2" $DATADIR/$1/$2 | wc -l`
echo MP2 : $MP2

echo ------------------

D1=`grep -lre "ERRORTYPE : D1" $DATADIR/$1/$2 | wc -l`
echo D1 : $D1

D2=`grep -lre "ERRORTYPE : D2" $DATADIR/$1/$2 | wc -l`
echo D2 : $D2

D3=`grep -lre "ERRORTYPE : D3" $DATADIR/$1/$2 | wc -l`
echo D3 : $D3

D4=`grep -lre "ERRORTYPE : D4" $DATADIR/$1/$2 | wc -l`
echo D4 : $D4

D5=`grep -lre "ERRORTYPE : D5" $DATADIR/$1/$2 | wc -l`
echo D5 : $D5

echo ------------------

W=`grep -lre "ERRORTYPE : W" $DATADIR/$1/$2 | wc -l`
echo W : $W




