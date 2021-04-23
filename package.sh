#!/bin/bash
# THIS WILL WORK AS LONG AS WE DON'T HAVE ANY dylibs!!!
# SEE HERE FOR DETAILS: https://stackoverflow.com/questions/1596945/building-osx-app-bundle

rm -rf ./build/Example.*

# HERE'S WHERE WE BUILD THE ACTUAL MAC APP BUNDLE:
case "$OSTYPE" in
  darwin*)
    mkdir ./build/Example.app
    mkdir ./build/Example.app/Contents
    mkdir ./build/Example.app/Contents/Resources
    mkdir ./build/Example.app/Contents/MacOS
    mkdir ./build/Example.app/Contents/MacOS/run
    mkdir ./build/Example.app/Contents/MacOS/run/assets

    cp ./build/mac_app_files/Example.icns ./build/Example.app/Contents/Resources/
    cp ./build/mac_app_files/Info.plist ./build/Example.app/Contents/
    cp ./build/mac_app_files/run.sh ./build/Example.app/Contents/MacOS/
    cp ./run/example ./build/Example.app/Contents/MacOS/run/

    cp -r ./run/assets/* ./build/Example.app/Contents/MacOS/run/assets/

    chmod -R a+x ./build/Example.app
esac
