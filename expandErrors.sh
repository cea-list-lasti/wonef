#!/bin/sh

if [ $# -lt 2 ]; then
 echo "Usage : ./expandErrors.sh dir logfile"
 exit
fi

DATAHOME=/data.2/mouton/CEA/wn.fr/CEA/wn.fr/
DATAHOME="."

echo $1
echo $2
 
mkdir -p $DATAHOME/$1

DIR1=$DATAHOME/$1/Type1
mkdir -p $DIR1

DIR2=$DATAHOME/$1/Type2
mkdir -p $DIR2

DIR3=$DATAHOME/$1/Type3
mkdir -p $DIR3

DIR4=$DATAHOME/$1/Type4
mkdir -p $DIR4


awk -v "DIR1=$DIR1" -v "DIR2=$DIR2" -v "DIR3=$DIR3" -v "DIR4=$DIR4" '
	BEGIN{
		print "beginning " 
		text=""
		cnt1=0
		cnt2=0
		cnt12=0
		cnt3=0
		cnt4=0
		cnt34=0
	}

	$0~/Error Type /{
		
		inside=1; 
		type=$3 ; 
	} 


	$0=="------" {	
		inside=0 ;
		gsub("\"", "\\\"", text)
		text=text"\n ERRORTYPE : "
		cmd="echo \""text"\" > "
		switch (type) {
		case 1 : 
			cnt1++
			cmd = cmd DIR1"/"cnt1".txt" ;
			break;
		case 2 : 
			cnt2++
			cmd = cmd DIR2"/"cnt2".txt" ;
			break;
		case 3 :
			cnt3++
			cmd = cmd DIR3"/"cnt3".txt" ;
			break;
	  	case 4 :
			cnt4++
			cmd = cmd DIR4"/"cnt4".txt" ;
			break;
		}
		
		cmd | getline l
		close(cmd)
		text=""
	} 

	inside==1{
		text=text"\n"$0 
	}
	
' $2
