
touch pathname.txt
printf $1>>pathname.txt
unzip archive.zip -d $1
cd $1/archive
make 
make display/display
make master/master
make cmd_shell/cmd_shell
make watchdog/watchdog
make motor1/motor1
make motor2/motor2
