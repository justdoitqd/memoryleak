#/usr/bin/bash
while [ 1 ]; do 
echo `date` "  |  " `cat /proc/$1/status|egrep "VmSize|VmData"`
 sleep 1
 done
