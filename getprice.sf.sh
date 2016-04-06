#!/bin/bash
curl -sv "https://www.isunfar.com.tw/ashx/prodlist_pt.ashx?bc_no=$1&ept=$2" |& sed 's/pc_cname/####/g'|grep -o "pname[^#]*####"|cut -d "," -f 1,2,3,4| sed 's/":/ /;s/,//;s/\.0//g;s/^pname "//;s/""/\t/;s/$/"/;s/,"mem_price.":0//g;s/prod_price"://;s/,"mem_price.":/ /g;s/"$//'
#curl -vs https://www.sinya.com.tw/diy/|& sed '/OPTION/!d;s/[^>]*>//;/OPTION value/d;/OPTGROUP/d;'
