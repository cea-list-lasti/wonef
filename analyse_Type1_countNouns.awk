#!/bin/awk

BEGIN{
	FS="[' ]"
	sum=0
}




{
	
	cmd = "grep -le \"^"$6"$\" /data/CEA/data/FrameNetLUI/externals/noms.list | wc -l "
	cmd | getline l 
	close(cmd) 
	if (l==0) {
		if ($6~/s$/) {
			pattern=substr($6, 0, length($6)-1)
		}
		cmd = "grep -le \"^"pattern"$\" /data/CEA/data/FrameNetLUI/externals/noms.list | wc -l "
		cmd | getline l 
		close(cmd) 
	}
	cmd2="echo "sum"+"l" | bc -l" 
	cmd2 | getline sum 
	close(cmd2) 
	
	
	if (l==0) {
		print "not noun : "$6	
		cmd3="grep -e \"^"$6"$\" /data/CEA/data/FrameNetLUI/externals/noms.list" 
		cmd3 | getline test
		close(cmd3)
	} else {
		print "considered as noun : " $6
		cmd3="grep -e \"^"pattern"$\" /data/CEA/data/FrameNetLUI/externals/noms.list" 
		cmd3 | getline test
		close(cmd3)
	}
} 


END{
	print "Nb of as-noun : "sum
} 



#~/CEA/WN.FR/Fast_KNN_Search.mpi/type1.log

