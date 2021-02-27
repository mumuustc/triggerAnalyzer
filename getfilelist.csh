#!/bin/csh

rm -rf isobric2018_picoDst.list 
get_file_list.pl -keys 'path,filename' -cond 'storage!=hpss,production=P20ic,trgsetupname=production_isobar_2018,filetype=daq_reco_picoDst,tpx=1,sanity=1' -limit 0 -distinct -delim '/' > isobric2018_picoDst.list

rm -rf isobric2018_MuDst.list 
get_file_list.pl -keys 'path,filename' -cond 'storage!=hpss,production=P20ic,trgsetupname=production_isobar_2018,filetype=daq_reco_MuDst,tpx=1,sanity=1' -limit 0 -distinct -delim '/' > isobric2018_MuDst.list

#rm -rf auau14p5_2019_picoDst.list 
#get_file_list.pl -keys 'path,filename' -cond 'storage=hpss,production=P19ic,trgsetupname=production_14p5GeV_2019,filetype=daq_reco_picoDst,tpx=1,sanity=1' -limit 0 -distinct -delim '/' > auau14p5_2019_picoDst.list
#
#rm -rf auau14p5_2019_MuDst.list 
#get_file_list.pl -keys 'path,filename' -cond 'storage=hpss,production=P19ic,trgsetupname=production_14p5GeV_2019,filetype=daq_reco_MuDst,tpx=1,sanity=1' -limit 0 -distinct -delim '/' > auau14p5_2019_MuDst.list

#rm -rf pico.list 
#get_file_list.pl -keys 'path,filename' -cond 'storage!=hpss,filename~st_physics,production=P11id,library=SL19c,trgsetupname=AuAu200_production_2011,filetype=daq_reco_picoDst,tpx=1,sanity=1' -limit 0 -distinct -delim '/' > pico.list

# in range use
#rm -rf micro.list 
#get_file_list.pl -keys 'path,filename' -cond 'storage=hpss,filetype=daq_reco_MuDst,filename~st_mtd,production=P15ie,trgsetupname=AuAu_200_production_high_2014,runnumber[]15166000-15166999,tpx=1,sanity=1' -limit 0 -distinct -delim '/' >& micro.list

## outside range use
#get_file_list.pl -keys 'path,filename' -cond 'storage=hpss,filetype=daq_reco_MuDst,filename~st_mtd,production=P15ie,trgsetupname=AuAu_200_production_high_2014,runnumber][15166023-15166028,tpx=1,sanity=1' -limit 0 -distinct -delim '/' >& micro.list

## or logic 
#get_file_list.pl -keys 'path,filename' -cond 'storage=hpss,filetype=daq_reco_MuDst,filename~st_mtd,production=P15ie,trgsetupname=AuAu_200_production_high_2014||AuAu_200_production_mid_2014||AuAu_200_production_low_2014,runnumber[]15166001-15166060,tpx=1,sanity=1' -limit 0 -distinct -delim '/' >& micro.list

##day select
#get_file_list.pl -keys 'path,filename' -cond 'storage=local,filetype=daq_reco_MuDst,filename~st_physics,trgsetupname=production_pp201long3_2015||production_pp200long2_2015||production_pp200long_2015||production_pp200trans_2015,daynumber~41,tpx=1,sanity=1' -limit 0 -distinct -delim '/' >& micro.list

##output daynumber
#get_file_list.pl -keys 'daynumber' -cond 'storage=local,filetype=daq_reco_MuDst,filename~st_physics,trgsetupname=production_pp201long3_2015||production_pp200long2_2015||production_pp200long_2015||production_pp200trans_2015,tpx=1,sanity=1' -limit 0 -distinct -delim '/' >& micro.list

## day select 
#get_file_list.pl -keys 'path,filename' -cond 'storage=hpss,filetype=daq_reco_MuDst,filename~st_mtd,production=P15ie,trgsetupname=AuAu_200_production_high_2014||AuAu_200_production_mid_2014||AuAu_200_production_low_2014,daynumber~151' -limit 0 -distinct -delim '/' >& micro.list
