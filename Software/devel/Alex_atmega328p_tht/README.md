# Glücksrad-Logik

Preisverteilung
---------------

- Kategorie 1: 300
- Kategorie 2: 150
- Kategorie 3: 150
- Kategorie 4: 15
- Kategorie 5: 5

Input-Buttons
-------------

Button 1: 'the big Button'  - löst das Glücksrad aus
Button 2: Freigabebutton - entsperrt Button 1
Button 3: -

Logik
-----

Startup:
    - Starten der Demo-Animation(en)

Button 1:
    - Flag zur Blockierung von Button 1 setzen
    - zufällige Auswahl des Zielfeldes
    - ggf. neuwürfeln falls Preisklasse leer
    - Dekrementieren der ausgewählten Preisklasse
    - Starten der Rotationsanimation zum Zielfeldes

Button 2:
    - entfernen der Flag zur Blockierung von Button 1