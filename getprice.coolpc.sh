#!/bin/bash
curl -vs http://coolpc.com.tw/evaluate.php|& iconv --from big5 --to utf8 -c |sed '/OPTION/!d;s/[^>]*>//;/OPTION value/d;/OPTGROUP/d;'
