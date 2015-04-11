#!/bin/bash

if [ -z $1 ]; then
    echo "Please specify a game title"
    exit
fi

# Source files
sed -i "s/GAME_TITLE/$1/g" src/*

# Android files
cd pkg/android/
sed -i "s/GAME_TITLE/$1/g" build.xml
sed -i "s/GAME_TITLE/$1/g" AndroidManifest.xml
sed -i "s/GAME_TITLE/$1/g" src/org/GAME_TITLE/app/GAME_TITLE.java
sed -i "s/GAME_TITLE/$1/g" .project
sed -i "s/GAME_TITLE/$1/g" res/values/strings.xml
sed -i "s/GAME_TITLE/$1/g" res/layout/main.xml
sed -i "s/GAME_TITLE/$1/g" README.android
mv src/org/GAME_TITLE/app/GAME_TITLE.java src/org/GAME_TITLE/app/$1.java
mv src/org/GAME_TITLE src/org/$1
git add src/org/$1

