#!/bin/bash
curl -vs http://coolpc.com.tw/evaluate.php|& iconv --from big5 --to utf8|sed '/OPTION/!d;s/[^>]*>//'
