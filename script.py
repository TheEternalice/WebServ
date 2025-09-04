#!/usr/bin/env python3
# script.py

# Indiquer le type MIME pour la réponse HTTP
print("Content-Type: text/html")
print()  # ligne vide obligatoire entre headers et body

# Corps de la réponse HTML
print("<!DOCTYPE html>")
print("<html lang='fr'>")
print("<head>")
print("    <meta charset='UTF-8'>")
print("    <title>Script Python Test</title>")
print("</head>")
print("<body>")
print("    <h1>Bonjour depuis le script Python !</h1>")
print("    <p>Ceci est une page générée dynamiquement.</p>")
print("</body>")
print("</html>")
