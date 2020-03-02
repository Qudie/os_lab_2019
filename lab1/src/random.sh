#!/bin/bash
MAXCOUNT=150
count=1
echo "Записаны 150 случайных чисел"
echo "-----------------"
while [ "$count" -le $MAXCOUNT ]   
do
  number=$RANDOM
  echo $number >> numbers.txt
  let "count += 1"  
done
echo "-----------------"