#! /bin/bash

echo "compilation des fichier source"
make
echo "netoyage"
make clean

# set PATH so it includes user's private bin directories
PATH="$HOME/bin:$HOME/.local/bin:$PATH"

mkdir -p $HOME/bin
mkdir -p $HOME/.local/bin

echo "ajout de l'executable dans le dossier bin de `echo $USER` "
cp -i mpsh /home/$USER/bin/



