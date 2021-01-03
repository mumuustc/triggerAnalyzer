#!/bin/bash

dir=$PWD
echo $dir

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

ln -fs $dir/Run19_AuAu14p5_trigger.xml ./

star-submit-template -template Run19_AuAu14p5_trigger.xml -entities dir=$dir

#for list in `ls $dir/picoDataList/sublist`
#do
#   echo $list
#   star-submit-template -template Run14_AuAu200_miniDst.xml -entities dir=$dir,list=$list
#done
