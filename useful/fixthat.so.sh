#!/bin/bash

yum -y install $(yum provides `ldd "$1" |& grep not|& cut -d ' ' -f 1`|grep "i686\|x86_64"|cut -d ' ' -f 1)

