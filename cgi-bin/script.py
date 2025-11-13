#!/usr/bin/env python3
# -*- coding: utf-8 -*-

print("Content-Type: text/html; charset=utf-8\n")

poem = """Maître Corbeau, sur un arbre perché,
Tenait en son bec un fromage.
Maître Renard, par l’odeur alléché,
Lui tint à peu près ce langage :
« Hé ! bonjour, Monsieur du Corbeau.
Que vous êtes joli ! que vous me semblez beau !
Sans mentir, si votre ramage
Se rapporte à votre plumage,
Vous êtes le Phénix des hôtes de ces bois. »
À ces mots le Corbeau ne se sent pas de joie ;
Et pour montrer sa belle voix,
Il ouvre un large bec, laisse tomber sa proie.
Le Renard s’en saisit, et dit : « Mon bon Monsieur,
Apprenez que tout flatteur
Vit aux dépens de celui qui l’écoute :
Cette leçon vaut bien un fromage, sans doute. »
Le Corbeau, honteux et confus,
Jura, mais un peu tard, qu’on ne l’y prendrait plus.
"""

html = f"""<!DOCTYPE html>
<html lang="fr">
<head>
<meta charset="utf-8" />
<title>Le Corbeau et le Renard — La Fontaine</title>
<meta name="viewport" content="width=device-width, initial-scale=1" />
<style>
  :root {{
    --bg: #0f1115;
    --card: #161a22;
    --text: #e8ecf1;
    --muted: #aab3bf;
  }}
  body {{
    margin: 0;
    font-family: ui-serif, Georgia, "Times New Roman", serif;
    background: linear-gradient(180deg, #0e1116, #0b0e12 30%, #0f1115);
    color: var(--text);
    line-height: 1.6;
  }}
  .wrap {{
    max-width: 720px;
    margin: 7vh auto;
    padding: 24px;
  }}
  .card {{
    background: radial-gradient(1200px 400px at 10% -20%, rgba(80,130,255,.10), transparent 60%),
                radial-gradient(1000px 450px at 110% 20%, rgba(0,180,140,.10), transparent 60%),
                var(--card);
    border: 1px solid rgba(255,255,255,.06);
    border-radius: 18px;
    padding: 28px 26px;
    box-shadow: 0 20px 40px rgba(0,0,0,.35), inset 0 1px 0 rgba(255,255,255,.04);
  }}
  h1 {{
    font-size: 1.85rem;
    margin: 0 0 8px;
    letter-spacing: .2px;
  }}
  .meta {{
    color: var(--muted);
    font-size: .95rem;
    margin-bottom: 18px;
  }}
  .poem {{
    font-size: 1.1rem;
    white-space: pre-line; /* garde les retours à la ligne du texte */
  }}
</style>
</head>
<body>
  <div class="wrap">
    <div class="card">
      <h1>Le Corbeau et le Renard</h1>
      <div class="meta">Jean de La Fontaine — Fables (Livre I, fable 2)</div>
      <div class="poem">{poem}</div>
    </div>
  </div>
</body>
</html>
"""

print(html)
