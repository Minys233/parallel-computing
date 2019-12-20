#! /bin/bash
python Random_matrix.py "$1" "$1"
echo "Single process"
for ((i=0; i<5; i++)); do
	./reduction "$1" "$1" -t -d | grep -Eo '[[:digit:]]+\.?[[:digit:]]*'
	sleep 0.5
done
echo "Multi process"
for ((i=0; i<5; i++)); do
        mpirun reduction-multi "$1" "$1" -t -d | grep -Eo '[[:digit:]]+\.?[[:digit:]]*'
	sleep 0.5
done
./validation
