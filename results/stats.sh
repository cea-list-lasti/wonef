#!/bin/bash

echo "Source: `grep "Source literal" $1 | wc -l`"
echo "Merge : `grep "Merge literals" $1 | wc -l`"
echo "New   : `grep "New literal" $1 | wc -l`"
echo "Empty : `grep "Empty literal" $1 | wc -l`"
echo ""
echo "Nb pairs:  `grep "Nb pairs" $1`"
