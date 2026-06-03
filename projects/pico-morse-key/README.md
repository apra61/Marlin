# Elektroniczny klucz Morse'a na Raspberry Pi Pico

Ten projekt opisuje dwupadowy klucz Morse'a, w ktorym klasyczne styki zostaly
zastapione dwiema belkami tensometrycznymi. Raspberry Pi Pico odczytuje nacisk
z dwoch torow analogowych, filtruje sygnal, wykrywa kropke i kreske z histereza
oraz generuje wyjscie CW w trybie iambic B.

## Zalozenia

- Pady: dwie niezalezne belki tensometryczne, lewa dla kropek i prawa dla kresek.
- Kontroler: Raspberry Pi Pico / RP2040.
- Wyjscie: izolowane wyjscie kluczujace do wejscia CW nadajnika, lokalny sidetone
  PWM i dioda statusu.
- Tempo domyslne: 20 WPM. Kropka trwa `1200 / WPM`, kreska `3 * kropka`,
  przerwa miedzy elementami `1 * kropka`.

## Elektronika

Pico nie mierzy bezposrednio mostka tensometrycznego, bo typowy mostek daje
sygnal rzedu pojedynczych miliwoltow. Kazda belka wymaga toru kondycjonowania:

1. Tensometr w mostku pelnym albo polmostku z precyzyjnymi rezystorami
   dopelniajacymi.
2. Wzmacniacz instrumentalny z zasilaniem 3,3 V, np. INA333, AD8426, INA125
   albo gotowy modul dajacy wyjscie analogowe.
3. Polaryzacja wyjscia wzmacniacza w okolice 1,65 V przy braku nacisku.
4. Zakres wyjscia ograniczony do 0-3,3 V. Wejsc ADC Pico nie wolno przekraczac.

Moduly HX711 sa wygodne do wag, ale w kluczu Morse'a sa graniczne: 10 SPS jest
zbyt wolne, a 80 SPS dziala tylko przy niskich predkosciach i wymaga osobnego
sterownika cyfrowego. Ten firmware zaklada szybszy tor analogowy do ADC Pico.

## Polaczenia

| Funkcja | Pin Pico | Uwagi |
| --- | --- | --- |
| Belka kropki, wyjscie wzmacniacza | GP26 / ADC0 | 0-3,3 V, spoczynek ok. 1,65 V |
| Belka kreski, wyjscie wzmacniacza | GP27 / ADC1 | 0-3,3 V, spoczynek ok. 1,65 V |
| Wyjscie kluczujace CW | GP16 | Steruje optoizolatorem albo tranzystorem open collector |
| Sidetone PWM | GP15 | Do buzzera/piezo przez prosty filtr lub wzmacniacz |
| LED statusu | GP25 | Wbudowana dioda Pico |
| Masa analogowa | GND | Wspolna masa z torami wzmacniaczy |

Nie podlaczaj wejscia CW radia bezposrednio do GPIO. Uzyj optoizolatora
albo tranzystora/MOSFET-a w ukladzie otwartego kolektora i sprawdz polaryzacje
oraz napiecie na gniezdzie klucza w dokumentacji nadajnika.

## Mechanika belek

- Belki pracuja jako wsporniki jednostronnie zamocowane.
- Tensometry najlepiej przykleic blisko mocowania, gdzie odksztalcenie jest
  najwieksze i powtarzalne.
- Dobrze sprawdza sie regulowany ogranicznik ugiecia oraz sruba do ustawienia
  sily progowej.
- Obie belki powinny miec podobna sztywnosc, ale progi mozna skorygowac w kodzie.

## Firmware

Pliki:

- `src/main.cpp` - odczyt ADC, filtracja, kalibracja, histereza i wyjscia Pico.
- `src/morse_keyer.hpp` oraz `src/morse_keyer.cpp` - przenosny rdzen klucza
  iambic, bez zaleznosci od Pico SDK.
- `test/test_morse_keyer.cpp` - testy hostowe algorytmu.

Po starcie trzymaj obie belki puszczone. Firmware czeka chwile, mierzy poziomy
zerowe, a potem sledzi dryft zera tylko wtedy, gdy dany pad nie jest aktywny.

Najwazniejsze stale w `src/main.cpp`:

- `kWpm` - predkosc nadawania.
- `kPressThresholdCounts` - prog aktywacji nacisku w licznikach ADC.
- `kReleaseThresholdCounts` - prog zwolnienia; mniejszy od progu aktywacji,
  zeby zapewnic histereze.
- `direction` w konstruktorze `AnalogPaddle` - ustaw `1` albo `-1`, jesli nacisk
  na danej belce zmniejsza zamiast zwiekszac odczyt wzgledem zera.

## Budowanie i testowanie

Test logiki bez Pico SDK:

```sh
cd projects/pico-morse-key
make test
```

Budowanie firmware dla Pico:

```sh
cd projects/pico-morse-key
export PICO_SDK_PATH=/sciezka/do/pico-sdk
cmake -B build/pico -S .
cmake --build build/pico
```

Wgranie: przytrzymaj `BOOTSEL`, podlacz Pico przez USB i skopiuj
`build/pico/pico_morse_key.uf2` na dysk `RPI-RP2`.

## Uruchomienie

1. Podlacz oba tory tensometryczne i sprawdz multimetrem, ze wyjscia wzmacniaczy
   sa w zakresie 0-3,3 V.
2. Wlacz Pico bez dotykania belek.
3. Otworz port USB-serial. Co sekunde firmware wypisuje surowe odczyty,
   odchylenie od zera i stan aktywny dla obu padow.
4. Nacisnij belke kropki i kreski. Jesli odchylenie ma znak ujemny, zmien
   `direction` z `1` na `-1` dla odpowiedniego toru.
5. Dobierz progi aktywacji i zwolnienia tak, aby nacisk byl pewny, ale bez
   przypadkowego kluczowania od drgan mechanicznych.
