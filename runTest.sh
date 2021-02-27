#!/bin/bash

if [ $# != 1 ]; then
     echo "You must provide the input data list: \"test_picoDst.list\" OR \"test_MuDst.list\""
	 echo "For instance, run test using: \"./runTest.sh test_MuDst.list\""
	 exit 0
fi

testList=$1

if [ ! -d test ];then
   mkdir test 
fi

rm -f test/runtestLog
root4star -b -l <<EOF >& test/runtestLog
.O2
.x doEvent.C(1000,"$testList","test/test.root",1) 
#.x doEvent.C(-1,"$testList","test/test.root",1) 
.q
EOF
