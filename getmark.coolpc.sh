#!/bin/bash
PostData=`curl -sv http://coolpc.com.tw/3d.php|& grep -o "id=[0-9]\+ "|tr '\n' ','|sed 's/id=//g;s/ ,/,/g;s/^/N=/;s/,$//'`

curl -sv --data "$PostData" "http://coolpc.com.tw/3dm.php" 2>/dev/null | iconv --from big5 --to utf8| grep -o "title=.*$"|sed 's/<[^>]*>/ /g;s/^title=[^ ]* //;s/ [^ >]*>//;s/[ ]\+/ /g;/0 0 0 0 0 0/d'


#curl -sv http://coolpc.com.tw/3d.php|& iconv --from big5 --to utf8| sed -n 's/^[^>]\+>\([^<]\+\)<.*$/\1/p'
