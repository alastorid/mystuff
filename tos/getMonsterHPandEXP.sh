#!/bin/bash

mid=57983
msg=""
if [ -n "$1" ]
then
  mid=`echo "$1" | grep -o "[0-9]*"|sed -n '1,1 p'`  
  msg="$1"  
else
  exit
fi 

info=`curl -sv http://www.tosbase.com/database/monsters/$mid/|&grep -o "<td colspan=\"2\">[0-9,]*</td>\|<td>[0-9,]\+</td>"|sed -n '2,4 s/<[^>]*>\|,//pg'|tr '\n' ' '`

msg="$msg "`echo $info | sed 's/\([0-9]\+\) \([0-9]\+\) \([0-9]\+\)/hp=\1 xp=\2 xpjob=\3/'`

echo $msg

