#!/bin/bash

msg="digraph d0{"
FORMAT='m\1 [label="\4\\n\2 per \3s\\nHP \5\\nEXP \6\\nEXPjob \7", color='

c=0

echo $msg

read line 
echo $line | sed 's/map \([^$]*\)/ map [shape=box,color=black, label="\1"]/;'

while read line; do     
  exp=`echo "$c ${line}" | sed 's/\([0-9]\+\) [^ ]* p=\([0-9]*\) r=\([0-9.]*\) name="\([^"]*\)" hp=\([0-9]*\) xp=\([0-9]*\) xpjob=\([0-9]*\)/\6/'`
  echo -n "$c ${line}" | sed 's/\([0-9]\+\) [^ ]* p=\([0-9]*\) r=\([0-9.]*\) name="\([^"]*\)" hp=\([0-9]*\) xp=\([0-9]*\) xpjob=\([0-9]*\)/'"$FORMAT"'/'
  if (( $exp > 3000 ))
  then
    echo "limegreen,shape=box,style=filled]"
  else
    echo "red,shape=plaintext]"
  fi

  echo "map -> m$c [weight=$exp]"

  c=`expr $c + 1`
done < /dev/stdin

echo "}"


