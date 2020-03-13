#/bin/bash

# "$@" allows to pass any command-line arguments of this script to python tester

for testset in SDK-tests.txt geometry-tests.txt similarity-tests.txt similarity-solid-tests.txt
do
	python3 ../../tester/tester.py "$@" -e '(.*)=../\1' -f $testset 
done
