#!/bin/sh
ps cax | grep JLkGDB.sh > /dev/null
if [ $? -eq 0 ]; then
  echo "Process is running."
  pkill -9 JLkGDB.sh 
else
  echo "Process is not running."
fi

#!/bin/sh
ps cax | grep JLinkGDBServer > /dev/null
if [ $? -eq 0 ]; then
  echo "Process is running."
  pkill -9 JLinkGDBServer
else
  echo "Process is not running."
fi
