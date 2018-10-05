#!/bin/bash

raw_data=`curl -vs http://coolpc.com.tw/evaluate.php|& iconv --from big5 --to utf8 -c |sed '/OPTION/!d;s/[^>]*>//;/OPTION value/d;/OPTGROUP/d;'`

if [ $1 == 1 ]; then
	echo "$raw_data" | sed -n '{s/^\([^\$]*\), \(.*\)$/\2 , \1/; ;s/^\(\$[0-9]\+.\)\?\$\([0-9]\+\)[^,]*,/\$\2 ,/;p}'| sed  '/^[^\$]/d;/^\$1 ,/d; s/^\$//' | sort -h
else
	echo "$raw_data"
fi
