# Glücksrad

## Hardware
Die gesamte Steuerung des Glücksrades erfolgt über zwei Knöpfe:

- Bumper     : extern am Glücksrad anzuschließender goldener Riesenknopf
- Mode-Button: kleiner, roter Knopf auf der Rückseite des Glücksrades

## Start
Beim Einschalten des Glücksrades wird automatisch der letzte Modus und die gespeicherte (Rest-)Preisliste geladen.
Anschließend wird der geladene Modus durch 1-sekündiges Aufleuchten aller LEDs dargestellt.

| Farbe | Modus      | Folgestatus                          |
| ----- | ---------- | ------------------------------------ |
| grün  | Standard   | Demo-Animation                       |
| gelb  | Countdown  | Demo- oder Preise-Leer-Animation     |
| rot   | Ladefehler | Demo-Animation (wechsel zu Standard) |

<img src="../../Bilder/Diagramme/start.png"      width="400">

## Drehen & Modi
Befindet sich das Glücksrad in der Demo-Animation kann durch Drücken auf den Bumper die Drehanimation ausgelöst werden.
Sobald das Glücksrad auf einem zufälligen Feld zur Ruhe gekommen ist, startet die Preis-Animationen für die entsprechende Farbe.
Nur durch Betätigen des Mode-Button kann das Glücksrad zurück in die Demo-Animation wechseln.

<img src="../../Bilder/Diagramme/drehen.png"     width="800">


## Preise
Es gibt insgesamt fünf verschiedene Preiskategorien mit unterschiedlicher Wertigkeit.
Die Wahrscheinlichkeiten einen Preis zu erhalten ist nur abhängig von der Anzahl der hinterlegten bzw. verbleibenden Preise.


### Keplertag 2021
Beim Keplertag sind nur vier der fünf möglichen Preiskategorien belegt.

| Preis          | Anzahl der LEDs | Keplertag 2021 |
| -------------- | --------------- | -------------- |
| rubin   (rot)  |  2 von 20 (10%) |    36          |
| saphir  (blau) |  4 von 20 (20%) |    75          |
| silber  (weiß) |  7 von 20 (35%) |   750          |
| gold    (gelb) |  7 von 20 (35%) |   750          |

### Frühlingsfest 2018 & 2019

| Preis          | Anzahl der LEDs | Frühlingsfest 2019 | Frühlingsfest 2018 |
| -------------- | --------------- | ------------------ | ------------------ |
| gold    (gelb) |  1 von 20 (5%)  |     9              |     5              |
| silber  (weiß) |  1 von 20 (5%)  |    11              |    15              |
| rubin   (rot)  |  4 von 20 (20%) |    38              |   150              |
| smaragd (grün) |  4 von 20 (20%) |    70              |   150              |
| saphir  (blau) | 10 von 20 (50%) |   170              |   300              |

Beispielsweise war die initiale Wahrscheinlichkeit beim Frühlingsfest 2018 einen goldenen Preis zu gewinnen ca. 0.8%.

    5 / (5 + 15 + 150 + 150 + 300) ~ 0.0080645 ~ 0.8%

## Einstellungsmenü

Das Glücksrad hat ein (verstecktes) Einstellungsmenü.
Es kann nur durch 3-sekündiges Drücken des Mode-Button in der Demo- oder der Preise-Leer-Animation erreicht werden.

Der ausgewählte Menüpunkt kann durch eine kurze Betätigung des Mode-Buttons gewechselt werden:

| Menü | Bedeutung                           |
| ---- | ----------------------------------- |
| blau | Einstellungsmenü abbrechen          |
| grün | Wechsel in den Standardmodus        |
| gelb | Wechsel in den Countdown-Modus      |
| rot  | Reset der internen Preisliste & Wechsel in den Countdown-Modus |

Durch ein 3-sekündiges Drücken des Mode-Button wird der aktuelle Menüpunkt ausgelöst.
Danach verlässt das Glücksrad das Einstellungsmenü automatisch.

<img src="../../Bilder/Diagramme/menue.png"     width="800">
