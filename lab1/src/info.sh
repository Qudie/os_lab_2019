#!/bin/bash
echo "The current directory is:"
pwd
echo "PATH is:"
echo $PATH
echo "The date and time:"
D=$(date  +%Y-%m-%d)
T=$(date +%H:%M:%S)
echo  "$D" "$T"