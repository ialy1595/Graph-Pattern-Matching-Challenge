SECONDS=0
./main/program ../data/lcc_$1.igraph ../query/lcc_$1_$2.igraph ../candidate_set/lcc_$1_$2.cs
duration=$SECONDS
echo "$(($duration)) seconds elapsed."