
Glücksrad
-----------

**Status:**

- STATE_STARTING
    - während des Startens
    - keine besondere Auswirkungen

- STATE_DEMO
    - standard Animation des Glücksrades

    - **btnBumper**:
        - startet die Drehanimation
        - state -> STATE_ROTATING
        
    - **btnMode**
        - resettet nach 5-Sekündigem gedrückt-halten die in der eeprom gespeicherten Preisanzahlen
        - während den 5 Sekunden leuchten die LEDs von grün bis hin zu rot auf
        - state -> STATE_RESET_PRICES

- STATE_ROTATING
    - Dreh-Animation
    - nach beendigung: state -> STATE_ROTATE_FINISHED
    - wenn keine Preise vorhanden: state -> STATE_PRICES_EMPTY

- STATE_ROTATE_FINISHED
    - lässt ausgewählte LED schnell blinken, bis btnMode gedrückt wird

    - **btnMode**:
        - wenn Preise vorhanden: state -> STATE_DEMO
        - sonst state -> STATE_PRICES_EMPTY

- STATE_RESET_PRICES
    - leds grün -> rot animation
    - nach 5 Sekunden:
        - resettet die Preisanzahlen in der eeprom
        - state -> STATE_PRICES_RESETTED

- STATE_PRICES_RESETTED
    - lässt alle leds für 3 Sekunden lang rot blinken

- STATE_PRICES_EMPTY
    - blinkt durchgängig in den Glücksrad-Farben
