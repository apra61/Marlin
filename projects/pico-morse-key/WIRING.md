# Schemat polaczen

Schemat dotyczy wersji z dwoma przetwornikami CS1237: jeden obsluguje belke
kropki, drugi belke kreski.

```text
BELKA KROPKI / DOT                    RASPBERRY PI PICO
Mostek tensometryczny
EXC+ / E+  <-- zasilanie mostka CS1237
EXC- / E-  <-- masa mostka CS1237
SIG+ / A+  --> CS1237 DOT AINP
SIG- / A-  --> CS1237 DOT AINN

CS1237 DOT
VCC  -------------------------------> 3V3 albo zasilanie zgodne z modulem
GND  -------------------------------> GND
DRDY/DOUT --------------------------> GP2
SCLK <------------------------------- GP3


BELKA KRESKI / DASH
Mostek tensometryczny
EXC+ / E+  <-- zasilanie mostka CS1237
EXC- / E-  <-- masa mostka CS1237
SIG+ / A+  --> CS1237 DASH AINP
SIG- / A-  --> CS1237 DASH AINN

CS1237 DASH
VCC  -------------------------------> 3V3 albo zasilanie zgodne z modulem
GND  -------------------------------> GND
DRDY/DOUT --------------------------> GP4
SCLK <------------------------------- GP5


WYJSCIA
GP16 -------------------------------> optoizolator / tranzystor open collector
                                      -> wejscie KEY radia
GP15 -------------------------------> buzzer/piezo albo wzmacniacz sidetone
GP25 -------------------------------> LED statusu Pico
GND  -------------------------------> wspolna masa Pico i obu modulow CS1237
```

## Uwagi praktyczne

- Jeden CS1237 mierzy jeden mostek tensometryczny, dlatego projekt uzywa dwoch
  ukladow albo dwoch niezaleznych kanalow w module zgodnym z CS1237.
- Jesli modul CS1237 jest zasilany z 5 V, dopasuj `DRDY/DOUT` i `SCLK` do
  logiki 3,3 V Raspberry Pi Pico.
- Nie podlaczaj wejscia KEY radia bezposrednio do GPIO. `GP16` powinien sterowac
  optoizolatorem, tranzystorem NPN albo MOSFET-em w ukladzie open collector.
- Masa modulow CS1237 musi byc wspolna z masa Pico. Strone radia najlepiej
  odseparowac optycznie.
