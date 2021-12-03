#!/bin/sh 
pwd=$(pwd)
more README.txt

cd $ARCHIVEVAR

cat master/description_master.txt cmd_shell/description_cmd_shell.txt motor1/description_motor1.txt display/description_display. logarp/description_logarp.txt watchdog/description_watchdog.txt> description_all.txt
more description_all.txt
rm -f description_all.txt
cd $pwd