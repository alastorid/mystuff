#!/bin/bash
curl -vs https://www.isunfar.com.tw/product/ecdiy.aspx |& grep -o 'data-ps="[^"]*"'|sed 's/.*"\([^"]*\)"/\1/'

