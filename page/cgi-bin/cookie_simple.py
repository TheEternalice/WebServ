#!/usr/bin/env python3
import os
import sys
import json

COOKIE_FILE = "/tmp/server_cookies.json"

def load_cookies():
    try:
        with open(COOKIE_FILE, 'r') as f:
            return json.load(f)
    except (FileNotFoundError, json.JSONDecodeError):
        return {}

def save_cookies(cookies):
    with open(COOKIE_FILE, 'w') as f:
        json.dump(cookies, f)

print("Content-Type: text/plain")
print()

method = os.environ.get('REQUEST_METHOD', 'GET')
cookie_header = os.environ.get('HTTP_COOKIE', '')

print(f"METHODE: {method}")
print(f"HTTP_COOKIE: {cookie_header}")

# Charge les cookies côté serveur
cookies = load_cookies()

if method == 'POST':
    content_length = int(os.environ.get('CONTENT_LENGTH', 0))
    body = ""
    if content_length > 0:
        body = sys.stdin.read(content_length)
    
    print(f"BODY RECU: '{body}'")
    
    if body:
        # Sauvegarde le cookie côté serveur
        cookies['monCookie'] = body
        save_cookies(cookies)
        
        # Envoie le cookie au navigateur
        print(f"Set-Cookie: monCookie={body}; Path=/; Max-Age=604800")
        print("✅ Cookie sauvegardé côté serveur")
    else:
        # Supprime le cookie
        if 'monCookie' in cookies:
            del cookies['monCookie']
            save_cookies(cookies)
            print("Set-Cookie: monCookie=; Path=/; Max-Age=0")
            print("🗑️ Cookie supprimé côté serveur")
        else:
            print("ℹ️ Cookie déjà absent côté serveur")
    
    print(f"\nCOOKIES SERVEUR APRÈS POST:")
    if cookies:
        for name, value in cookies.items():
            print(f"  {name} = {value}")
    else:
        print("  Aucun cookie")
    
    print(f"\nCOOKIES NAVIGATEUR:")
    if cookie_header:
        print(f"  {cookie_header}")
    else:
        print("  Aucun cookie")
    
    exit()

# Synchronise les cookies côté serveur avec ceux du navigateur
if cookie_header:
    browser_cookies = {}
    for cookie_pair in cookie_header.split(';'):
        if '=' in cookie_pair:
            name, value = cookie_pair.strip().split('=', 1)
            browser_cookies[name] = value
    
    # Met à jour le cookie côté serveur avec celui du navigateur
    for name, value in browser_cookies.items():
        if name == 'monCookie':
            cookies[name] = value
    
    save_cookies(cookies)

print("COOKIES SERVEUR:")
if cookies:
    for name, value in cookies.items():
        print(f"  {name} = {value}")
else:
    print("  Aucun cookie")

print("\nCOOKIES NAVIGATEUR:")
if cookie_header:
    print(f"  {cookie_header}")
else:
    print("  Aucun cookie")