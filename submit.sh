#!/bin/bash

if [ $# != 1 ]; then
     echo "You must provide a data list: \"isobric2018_picoDst.list\" OR \"isobric2018_MuDst.list\" OR \"AuAu14p5_2019.list\" OR \"test_MuDst.list\" OR \"test_picoDst.list\""
	 echo "For instance, submit job using: \"./submit.sh isobric2018_MuDst.list\""
	 exit 0
fi

dir=$PWD
dataList=$1

echo $dir
echo $dataList

# if one runs this macro in home directory, one needs to build a gpfs directory to store the produced miniTrees
# NOTE, do NOT dump miniTrees into your home directory !!!
gpfsDir="/gpfs01/star/pwg/syang/run21/OOTrigger/triggerAnalyzer"
echo $gpfsDir

if [ ! -d submitjob ]; then
     mkdir -p $gpfsDir/submitjob
     ln -s $gpfsDir/submitjob .
fi

cd submitjob

mkdir -p submitReport submitScript submitList submitErrInfo rootfiles

rm -rf submitReport/*
rm -rf submitScript/*
rm -rf submitList/*
rm -rf submitErrInfo/*

ln -fs $dir/triggerAnalyzer_scheduler.xml ./

star-submit-template -template triggerAnalyzer_scheduler.xml -entities dir=$dir,dataList=$dataList

#for list in `ls $dir/picoDataList/sublist`
#do
#   echo $list
#   star-submit-template -template Run14_AuAu200_miniDst.xml -entities dir=$dir,list=$list
#done
