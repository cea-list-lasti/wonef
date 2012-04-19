#!/bin/awk


BEGIN {
	FS="[ :]"
#	print "hyper : "hyper
	print "hypo : " hypo
#	hypo="voiture"
#	hyper="vÃ©hicule"
}
	
$0~/ , / {
	sum = 0
	sum2 = 0
	for (i=4;  i<NF; i+=3) { 
		cmd= "grep \" "$i":\" ../data/bestcooc/"hypo".50"rel".txt | wc -l "
#		print cmd
		cmd | getline l
		#print l 		
		close(cmd)
		if (l > 0 ) {
			print $i " : " $(i+1)
		}
		cmd2 = "echo "l"+"sum" | bc -l "
		cmd2 | getline sum
		close(cmd2)
		cmd3 = "echo "$(i+1)"+"sum2" | bc -l "
		cmd3 | getline sum2
		close(cmd3)
	}
	print sum 
	print sum2
}
