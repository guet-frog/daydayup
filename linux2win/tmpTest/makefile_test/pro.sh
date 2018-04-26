#!/bin/sh

file_name_head="clouder"

#touch ${file_name_head}1.h

#for file in $(ls)

for loop in 2 3 4 5
do
    touch ${file_name_head}${loop}.h
done
