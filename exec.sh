#!/bin/bash

s=1606
for i in `seq 1 5`;
do
	./eternity $((s+i)) 
done
