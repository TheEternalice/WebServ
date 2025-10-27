#!/bin/bash

# Lire le body
read -n $CONTENT_LENGTH POST_DATA

# Extraire la valeur après "message="
VALUE="${POST_DATA#message=}"

# Remplacer + par espace et %XX par caractères
# 1️⃣ + → espace
VALUE="${VALUE//+/ }"

# 2️⃣ Décodage URL (%XX)
decode() {
  local url_encoded="${1//+/ }"
  printf '%b' "${url_encoded//%/\\x}"
}
VALUE=$(decode "$VALUE")

# Écrire dans le fichier
SCRIPT_DIR=$(dirname "$0")
echo "$VALUE" >> "$SCRIPT_DIR/test.txt"

# Retour au client
echo "Content-Type: text/plain"
echo ""
echo "Message écrit !"
