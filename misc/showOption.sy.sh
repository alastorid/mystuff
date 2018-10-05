#!/bin/bash
curl -vs https://www.sinya.com.tw/diy/|& sed -n '/data-id="\([0-9]\+\)">/ {s/.*data-id="\([0-9]\+\)">/\1/;p;}'|sort -h
