#!/bin/bash

echo ""
echo ":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::"
echo ":::::::::::::::::::::::::::::::::::"
echo "::::::::::::"
echo "::"
echo ""
echo ""
echo ""
echo "           Data Acquisition System Controller ... " 
echo ""
echo ""
echo "::"
echo "::"
echo "::"
echo ""


comment=${1}
### Define directories and files. 
#
basedir=${PWD}

daqdir=${basedir}/wave_source_codes/bin/wavedump

#rawdir=${basedir}/../data/raw
#rawdir=/run/media/assy2/E525/data/raw
#data save dir after HDD add 
#rawdir=/home/assy2/Work/E525/data/raw
rawdir=${basedir}/data

runlog=${rawdir}/run_log.x

#configration file directry
config_file_dir=${basedir}/wave_source_codes/conf

#if [[ ${cardtype} -eq 0 ]]; then 
#  card_file=${carddir}/WaveDumpConfig_QST_c1.txt
#elif [[ ${cardtype} -eq 1 ]]; then 
##  card_file=${carddir}/WaveDumpConfig_QST_c2.txt
#fi 


### Ask whether the user really starts DAQ or not. 
#
val="none"
while [[ $val != "y" ]] && [[ $val != "n" ]] ; do 
  read -p "  Would you like to start a new run [ y / n ] : " val
  
  if [[ $val == "n" ]] ; then
    echo ""
    echo "  ---> OK, exit from the DAQ control system."
    echo ""
    echo ""
    echo ":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::"
    echo "                                  Waiting for the new run ..."
    echo ":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::"
    echo ""
    echo ""
    exit
  fi 
done

# 1. run type (physics, calibration, or test) 
#
echo ""
runtype="null"
val="none"
DAQ_mode="3"
while [[ $val != "1" ]] && [[ $val != "2" ]] && [[ $val != "3" ]] ; do
  read -p "  DAQ run type [ (1) physics, (2) calibration, (3) test ] : " val

  if [[ $val == "1" ]] ; then
    runtype="physics"
  fi
  if [[ $val == "2" ]] ; then
    runtype="calibration"
  fi
  if [[ $val == "3" ]] ; then
    runtype="test"
  fi
	DAQ_mode=${val}
done

### Get the run number and make the output directory. 
#
#get run number
nline=`wc -l ${runlog} | awk '{print $1}'`
nline=$(( ${nline} - 2 ))
last_run=`sed -n ${nline}p ${runlog} | awk '{print $1}' | sed s/run//`  

#last_run=`tail -n 1 ${runlog} | awk '{print $1}' | sed s/run//`  
next_run=$(( 10#${last_run} + 1 ))
next_run=`printf "%04d" ${next_run}`
next_run=`echo "run"${next_run}`
outputdir=${rawdir}/${next_run}
output_file=${outputdir}/${next_run}

judge=`find ${rawdir} -name ${next_run}`
if [[ ${judge} != ${rawdir}"/"${next_run} ]] ; then

	if [[ $DAQ_mode == "1" ]] || [[ $DAQ_mode == "2" ]] ; then
	  mkdir ${outputdir}
  fi
	if [[ $DAQ_mode == "3" ]] ; then
	  mkdir data/test
  fi
else
  echo ""
  echo "  There already exists that run. Please try again."
  echo ""
  echo "${next_run} | null null | null |"                     >> ${runlog}
  echo "         | no comment |"                              >> ${runlog}
  echo ""                                                     >> ${runlog}
  echo ""
  echo ""
  echo ":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::"
  echo "                                  Waiting for the new run ..."
  echo ":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::"
  echo ""
  exit
fi
#
trap 'rm -r ${outputdir} ; echo "" ; exit 1' 2


### Select the run information. 
#
#
# 2. date & time 
#
s_date_string=`date '+%Y%m%d'`
s_time_string=`date '+%H%M'`
#
#
#
#
# 3. 
#




### Execute DAQ progtram
#
echo ""
val="none"

if [[ $DAQ_mode == "1" ]] || [[ $DAQ_mode == "2" ]] ; then
while [[ $val != "y" ]] && [[ $val != "n" ]] ; do 
  read -p "  Will you really start a new run (${next_run})?  [ y / n ] : " val

  if [[ $val == "n" ]] ; then
    echo ""
    echo "  ---> OK, exit from the DAQ control system."
    echo ""
    echo ""
    echo ":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::"
    echo "                                  Waiting for the new run ..."
    echo ":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::"
    echo ""
    rm -r ${outputdir} 
    exit
  fi 
done
echo ""
fi
if [[ $DAQ_mode == "3" ]] ; then
while [[ $val != "y" ]] && [[ $val != "n" ]] ; do 
  read -p "  Will you really start a new run (test)?  [ y / n ] : " val

  if [[ $val == "n" ]] ; then
    echo ""
    echo "  ---> OK, exit from the DAQ control system."
    echo ""
    echo ""
    echo ":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::"
    echo "                                  Waiting for the new run ..."
    echo ":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::"
    echo ""
    rm -r ${test} 
    exit
  fi 
done
echo ""
fi

echo "${daqdir} ${next_run} ${DAQ_mode}" 
echo ""
echo "  Activating Flash-ADC module ..."
#execute daq
if [[ $DAQ_mode == "1" ]] || [[ $DAQ_mode == "2" ]] ; then
eval ${daqdir} ${next_run} ${DAQ_mode} 
echo ""
#Get End time
e_date_string=`date '+%Y%m%d'`
e_time_string=`date '+%H%M'`
echo "${next_run} | ${s_date_string} ${s_time_string} | ${runtype} |"           >> ${runlog}
echo "         |${e_date_string} ${e_time_string} | ${comment} |"                                              >> ${runlog}
echo ""                                                                     >> ${runlog}
cp -r ${config_file_dir} ${outputdir}  
fi
echo ""
if [[ $DAQ_mode == "3" ]] ; then
eval ${daqdir} test ${DAQ_mode} 
fi
echo ":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::"
echo "                              ${next_run} finished		   "
echo ":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::"
echo ""
echo ""
#Wavedump log mv run file
if [[ $DAQ_mode == "1" ]] || [[ $DAQ_mode == "2" ]] ; then
  mv log.x ${outputdir}  
fi


#cp *.txt ${rawdir}
#mv *.txt ${outputdir}  
### Run online analysis code
#
#exe=/home/assy2/Work/E525/analysis/online.display/online_display
#sleep 30
#${exe} ${outputdir} ${next_run} 



