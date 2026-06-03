# Elektroniczny klucz Morse'a na Raspberry Pi Pico

Ten projekt opisuje dwupadowy klucz Morse'a, w ktorym klasyczne styki zostaly
zastapione dwiema belkami tensometrycznymi. Raspberry Pi Pico odczytuje nacisk
z dwoch przetwornikow CS1237, filtruje sygnal, wykrywa kropke i kreske z
histereza oraz generuje wyjscie CW w trybie iambic B.

## Zalozenia

- Pady: dwie niezalezne belki tensometryczne, lewa dla kropek i prawa dla kresek.
- Kontroler: Raspberry Pi Pico / RP2040.
- Wyjscie: izolowane wyjscie kluczujace do wejscia CW nadajnika, lokalny sidetone
  PWM i dioda statusu.
- Tempo domyslne: 20 WPM. Kropka trwa `1200 / WPM`, kreska `3 * kropka`,
  przerwa miedzy elementami `1 * kropka`.

## Elektronika

Pico nie mierzy bezposrednio mostka tensometrycznego, bo typowy mostek daje
sygnal rzedu pojedynczych miliwoltow. Kazda belka ma wlasny przetwornik CS1237:

1. Tensometr w mostku pelnym albo polmostku z precyzyjnymi rezystorami
   dopelniajacymi.
2. Modul CS1237 z wejsciem roznicowym `AINP/AINN`, wzmacniaczem PGA i interfejsem
   dwupinowym `SCLK` + `DRDY/DOUT`.
3. Zasilanie CS1237 zgodne z modulem. Jesli modul pracuje na 5 V, dopasuj poziomy
   logiczne do 3,3 V GPIO Pico.
4. W firmware ustawiono `Channel A`, `PGA 128` i `640 Hz`, bo domyslne 10 Hz jest
   zbyt wolne dla wygodnego kluczowania.

Jeden CS1237 obsluguje jedna belke. Dla dwoch padow potrzebne sa dwa uklady
CS1237 albo dwa niezalezne kanaly w zgodnym module wielokanalowym.

## Polaczenia

| Funkcja | Pin Pico | Uwagi |
| --- | --- | --- |
| CS1237 kropki, `DRDY/DOUT` | GP2 | Wejscie z podciagnieciem Pico |
| CS1237 kropki, `SCLK` | GP3 | Wyjscie zegara, stan spoczynkowy niski |
| CS1237 kreski, `DRDY/DOUT` | GP4 | Wejscie z podciagnieciem Pico |
| CS1237 kreski, `SCLK` | GP5 | Wyjscie zegara, stan spoczynkowy niski |
| Wyjscie kluczujace CW | GP16 | Steruje optoizolatorem albo tranzystorem open collector |
| Sidetone PWM | GP15 | Do buzzera/piezo przez prosty filtr lub wzmacniacz |
| LED statusu | GP25 | Wbudowana dioda Pico |
| Masa | GND | Wspolna masa Pico i obu modulow CS1237 |

Szczegolowy schemat tekstowy jest w `WIRING.md`.

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

- `src/cs1237.hpp` oraz `src/cs1237.cpp` - sterownik dwupinowego interfejsu
  CS1237, konfiguracja rejestru i konwersja probek 24-bitowych.
- `src/main.cpp` - odczyt CS1237, filtracja, kalibracja, histereza i wyjscia Pico.
- `src/morse_keyer.hpp` oraz `src/morse_keyer.cpp` - przenosny rdzen klucza
  iambic, bez zaleznosci od Pico SDK.
- `test/test_morse_keyer.cpp` - testy hostowe algorytmu i konwersji ramek CS1237.

Po starcie trzymaj obie belki puszczone. Firmware czeka chwile, mierzy poziomy
zerowe, a potem sledzi dryft zera tylko wtedy, gdy dany pad nie jest aktywny.

Najwazniejsze stale w `src/main.cpp`:

- `kWpm` - predkosc nadawania.
- `kCs1237Config` - konfiguracja CS1237: kanal A, PGA 128, 640 Hz.
- `kPressThresholdCounts` - prog aktywacji nacisku w licznikach CS1237.
- `kReleaseThresholdCounts` - prog zwolnienia; mniejszy od progu aktywacji,
  zeby zapewnic histereze.
- `direction` w konstruktorze `Cs1237Paddle` - ustaw `1` albo `-1`, jesli nacisk
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

1. Podlacz oba mostki tensometryczne do dwoch modulow CS1237 i sprawdz zasilanie
   oraz poziomy logiczne `DRDY/DOUT`.
2. Wlacz Pico bez dotykania belek.
3. Otworz port USB-serial. Co sekunde firmware wypisuje surowe odczyty,
   odchylenie od zera i stan aktywny dla obu padow.
4. Nacisnij belke kropki i kreski. Jesli odchylenie ma znak ujemny, zmien
   `direction` z `1` na `-1` dla odpowiedniego toru.
5. Dobierz progi aktywacji i zwolnienia tak, aby nacisk byl pewny, ale bez
   przypadkowego kluczowania od drgan mechanicznych.
