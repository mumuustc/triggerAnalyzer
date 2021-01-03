#!/bin/bash

if [ ! -d test ];then
   mkdir test 
fi

rm -f test/runtestLog
rm -f test/test.root
root4star -b -l <<EOF >& test/runtestLog
.O2
.x doEvent.C(1000,"test.list","test/test.root",1) 
#.x doEvent.C(-1,"test.list","test/test.root",1) 
.q
EOF
