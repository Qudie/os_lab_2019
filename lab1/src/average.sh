#!/bin/bash
summ=0
count=0
for var in $(cat $1)
do
(( count += 1 ))
(( summ += var))
done
echo "Количество элементов: $count"
echo "Сумма чисел: $summ"
let "r = (($summ/$count |bc -l))"
echo "Среднее арифметическое:"
echo $r