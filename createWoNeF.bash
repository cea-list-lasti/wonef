#!/bin/bash

# This script is a wrapper around translateWN.sh *when translateWN.sh already works*!
# The goal is to create WoNeF in one fell swoop.

mkdir -p building
rm -f building/*

# High-precision

./translateWN.sh noun --prefer precision --module 3 4
cp data/jaws.best.noun.e.m34.precision.deb.xml building/noun-precision.xml
./translateWN.sh adj --prefer precision --module 2
cp data/jaws.best.adj.e.m2.precision.deb.xml building/adj-precision.xml
./translateWN.sh verb --prefer precision --module 2
cp data/jaws.best.verb.e.m2.precision.deb.xml building/verb-precision.xml
./translateWN.sh adv --prefer precision --module 2
cp data/jaws.best.adv.e.m2.precision.deb.xml building/adv-precision.xml

# F-score

./translateWN.sh noun --prefer fscore --module 3 4 8
cp data/jaws.best.noun.e.m348.fscore.deb.xml building/noun-fscore.xml
./translateWN.sh adj --prefer fscore --module 2
cp data/jaws.best.adj.e.m2.fscore.deb.xml building/adj-fscore.xml
./translateWN.sh verb --prefer fscore --module 2
cp data/jaws.best.verb.e.m2.fscore.deb.xml building/verb-fscore.xml
./translateWN.sh adv --prefer fscore --module 2
cp data/jaws.best.adv.e.m2.fscore.deb.xml building/adv-fscore.xml

# Coverage

./translateWN.sh noun --prefer coverage --module 3 4 8 1 2 6 7
cp data/jaws.best.noun.e.m3481267.coverage.deb.xml building/noun-coverage.xml
./translateWN.sh adj --prefer coverage --module 2
cp data/jaws.best.adj.e.m2.coverage.deb.xml building/adj-coverage.xml
./translateWN.sh verb --prefer coverage --module 2
cp data/jaws.best.verb.e.m2.coverage.deb.xml building/verb-coverage.xml
./translateWN.sh adv --prefer coverage --module 2
cp data/jaws.best.adv.e.m2.coverage.deb.xml building/adv-coverage.xml
