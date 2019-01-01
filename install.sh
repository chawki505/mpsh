#! /bin/bash

echo "Compilation des fichier source"
make
echo "netoyage"
make clean

# set PATH so it includes user's private bin directories
export PATH="$HOME/bin:$HOME/.local/bin:$PATH"

echo $PATH

mkdir -p $HOME/bin
mkdir -p $HOME/.local/bin

echo "ajout de l'executable dans le dossier bin de `echo $USER` "
cp mpsh /$HOME/bin/

echo "FIN"

mpsh