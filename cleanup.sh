pwd=$(pwd)
cd $ARCHIVEVAR
make clean
rm -f logs/*.*
rm -f description_all.txt
cd $pwd