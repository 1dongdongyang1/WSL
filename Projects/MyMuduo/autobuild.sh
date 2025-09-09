#!/bin/bash

set -e

# 如果没有build目录，创建该目录
if [ ! -d `pwd`/build ]; then
    mkdir `pwd`/build
fi

rm -rf `pwd`/build/*

cd `pwd`/build &&
    cmake .. &&
    make

# 回到顶层根目录
cd ..

# 把头文件拷贝到 /usr/local/include/mymuduo 
# so库拷贝到    /usr/local/lib
if [ ! -d /usr/local/include/mymuduo ]; then
    mkdir /usr/local/include/mymuduo
fi

for header in `ls *.h`
do
    cp $header /usr/local/include/mymuduo
done

cp `pwd`/lib/libmymuduo.so /usr/local/lib

ldconfig