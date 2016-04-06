#!/bin/bash
curl -sv https://www.isunfar.com.tw/ecdiy/getitem.ashx|& tr '"' '\n'|sed 's/＄/$/g'|grep '\$'

