#!/bin/bash
PostData=`curl -sv http://coolpc.com.tw/3d.php|& grep -o "id=[0-9]\+ "|tr '\n' ','|sed 's/id=//g;s/ ,/,/g;s/^/N=/;s/,$//;s/$/\&S=1/;'`

curl -sv --data "$PostData" "http://coolpc.com.tw/3dm.php" 2>/dev/null | iconv --from big5 --to utf8|sed 's/GTX /GTX/g'|grep -o "<td colspan=2 class=w>.*$"|grep kbd|sed 's/<[^>]*>/ /g;s/[^ a-zA-Z0-9$-]*//g;s/^ //;s/^\([^ ]* [^ ]* \)[^$]*\$/\1$/;s/[ ]\+/ /g;'|column -t

#curl -sv http://coolpc.com.tw/3d.php|& iconv --from big5 --to utf8| sed -n 's/^[^>]\+>\([^<]\+\)<.*$/\1/p'
