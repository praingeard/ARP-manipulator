pwd=$(pwd)
if [ -z "$1" ]; then 
    ARCHIVEVAR=./archive
else 
    ARCHIVEVAR=$1
fi

unzip -u -d /tmp/archive archive.zip
rm -rf $ARCHIVEVAR
mkdir $ARCHIVEVAR
mv  -v /tmp/archive/archive/* $ARCHIVEVAR
cd $ARCHIVEVAR
make 
make display/display
make master/master
make cmd_shell/cmd_shell
make watchdog/watchdog
make motor1/motor1
make motor2/motor2
cd $pwd