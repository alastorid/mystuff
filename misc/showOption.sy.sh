#!/bin/bash
curl -vs https://www.sinya.com.tw/diy/|& grep -o "option([0-9]\+,[^)]*)"|sed 's/^[^0-9]*\([^,]*\),.*/\1/'|sort -h
