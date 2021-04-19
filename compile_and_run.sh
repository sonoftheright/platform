#!/bin/bash

FLAG=$1
PRODFLAG=""
if [ '--production' = "${FLAG}" ]; then
  PRODFLAG="--production"
fi

sh ./compile.sh $PRODFLAG
sh ./package.sh

case "$OSTYPE" in

  cygwin*)
    cd ./run;
    ./example.exe
  ;;

  darwin*)
    if [ '--production' = "${FLAG}" ]; then
      cd ./build;
      open Example.app;
      exit 0;
    fi

    cd ./run;
    ./example;

  ;;
esac
