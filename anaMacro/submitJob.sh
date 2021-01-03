#!/bin/bash

initDir=$PWD
gpfsDir="/gpfs01/star/pwg/syang/run21/OOTrigger/triggerAnalyzer/anaMacro"

if [ ! -d condor_project ]; then
     mkdir -p $gpfsDir/condor_project
	 ln -s $gpfsDir/condor_project .
fi

logDir="condor_project/log"
outputDir="condor_project/output"

mkdir -p $logDir $outputDir
rm -rf $logDir/* $outputDir/*

condorJobCfg="condor_project/condorJob.con"

cat > $condorJobCfg <<- _EOF_
Universe     = vanilla
Notification = never
GetEnv       = True
InitialDir   = $initDir
Requirements = (CPU_Type != "crs") && (CPU_Experiment == "star")
+Experiment  = "star"
+Job_Type    = "cas"
Executable   = analysis
_EOF_

ifile=0
for inputFile in `cat datalist4Job`
do
     echo $inputFile

     cat >> $condorJobCfg <<- _EOF_
     
Arguments      = $inputFile $outputDir/$ifile
Output         = $logDir/condorJob_${ifile}.out
Error          = $logDir/condorJob_${ifile}.err
Log            = $logDir/condorJob_${ifile}.olog
Queue
_EOF_
     
     let "ifile+=1";
done

condor_submit $condorJobCfg
