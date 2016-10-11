#!/bin/bash

mid=1841

if [ -n "$1" ] 
then
  mid=$1
fi
msg=`echo "map $mid"`

info=`curl -sv "http://www.tosbase.com/game/world-map/$mid/" |& grep -o "Level:....[^<]*<br/>\|<h1>[^<]*</h1>\|<a href=\"database/monsters/[0-9][^<]*</a></span></div><div>[^<]*</div><div>[^<]*</div>"|sed '1,2 {N;s/\n/ /;};3,$ s/<a href="database\/monsters\/\([0-9]\+\)[^>]*>\([^<]*\)<\/a><\/span><\/div><div>Population: \([0-9]*\)<\/div><div>Respawn: \([0-9.]*\)s<\/div>/id=\1_p=\3_r=\4_name="\2"/' |tr '\n' '\0' | xargs -0 -n1 -P0 ./getMonsterHPandEXP.sh | tr '_' ' ' |sort|tr '\n' '#'`

echo "$msg $info"|sed 's/<[^>]*>//g'|tr '#' '\n'|sed '${/^$/d;};'

