# Analiza planu kont z plikow CSV - ponowna analiza

## Zakres danych

Analiza obejmuje nowe eksporty SAP:

- `SKA1_3195.csv` - dane kont KG na poziomie planu kont.
- `SKB1_1241.csv` - dane kont KG na poziomie jednostki gospodarczej.

Pliki sa rozdzielane separatorem `;`.

## Podsumowanie liczbowe

| Obszar | Wynik |
| --- | ---: |
| Plan kont w `SKA1` | `YCOA` |
| Jednostka gospodarcza w `SKB1` | `1600` |
| Wiersze / unikalne konta w `SKA1` | 1102 / 1102 |
| Wiersze / unikalne konta w `SKB1` | 1102 / 1102 |
| Duplikaty kont w `SKA1` | 0 |
| Duplikaty par `JG + Konto KG` w `SKB1` | 0 |
| Konta z `SKB1`, ktorych brak w `SKA1` | 0 |
| Konta z `SKA1` nierozszerzone do JG `1600` | 0 |
| Konta aktywne, rozszerzone i bez blokady ksiegowania na obu poziomach | 1046 |

## Struktura kont

| Pole | Rozklad |
| --- | --- |
| `SKA1-Kto bil.` | `X`: 557, puste: 545 |
| `SKA1-RodzKontKG` | `X`: 557, `P`: 467, `S`: 71, `N`: 7 |

Najwieksze grupy `GrKt` w `SKA1`:

| Grupa | Liczba kont |
| --- | ---: |
| `ERG.` | 474 |
| `SAKO` | 358 |
| `SECC` | 71 |
| `ANL.` | 68 |
| `ABST` | 56 |
| `FIN.` | 38 |
| `MAT.` | 37 |

## Jakosc danych

| Kontrola | Wynik |
| --- | ---: |
| Konta `SKA1` z pustym opisem | 0 |
| Konta z `SKA1-Blok.ks. = X` | 56 |
| Konta z `SKA1-Blok.ks. = X`, ktore sa rozszerzone do `1600` | 56 |
| Konta z `SKB1-Blok.ks. = X` | 0 |
| Konta z wartoscia `#NAZWA?` w polu `SKB1-Pd` | 9 |

Przyklady kont bez opisu:

- brak

Przyklady kont z blokada ksiegowania w `SKA1`:

- `11001000`
- `12101108`
- `12101208`
- `12101209`
- `12101908`
- `12400108`
- `12400208`
- `13111106`
- `13111206`
- `13111306`

Przyklady kont z blokada ksiegowania w `SKB1`:

- brak

## Dane spolki `1600`

| Pole | Rozklad |
| --- | --- |
| `SKB1-Wsk. uzg.` | puste: 981, `A`: 65, `D`: 34, `K`: 22 |
| `SKB1-Wal.` | `PLN`: 1093, `EUR`: 9 |
| `SKB1-Pd` | puste: 673, `-`: 305, `+`: 100, `#NAZWA?`: 9, `*`: 5, `>`: 5, `<`: 3, `P0`: 2 |

Najczestsze grupy statusu pol `SKB1-GrpStatPol`:

| Grupa statusu pol | Liczba kont |
| --- | ---: |
| `YB01` | 365 |
| `YB04` | 159 |
| `YB29` | 72 |
| `SECC` | 72 |
| `YB03` | 62 |
| `YB65` | 47 |
| `YB07` | 43 |
| `YB05` | 35 |
| `YB06` | 34 |
| `YB67` | 32 |

## Konta nierozszerzone do JG `1600`

Liczba kont z `SKA1`, ktore nie wystepuja w `SKB1` dla JG `1600`: 0.

Rozklad wg `GrKt`:

| Grupa | Liczba kont |
| --- | ---: |
| brak kont nierozszerzonych | 0 |

Rozklad wg `Kto bil.`:

| `Kto bil.` | Liczba kont |
| --- | ---: |
| brak kont nierozszerzonych | 0 |

## Wnioski

- Dane sa spojne technicznie: kazde konto z `SKB1` istnieje w `SKA1`.
- Nie znaleziono duplikatow kont w `SKA1` ani duplikatow par `JG + Konto KG` w `SKB1`.
- Nowy eksport jest kompletny wzgledem JG `1600`: wszystkie 1102 konta z `SKA1` maja odpowiadajacy rekord w `SKB1`.
- Nie ma pustych opisow kont w `SKA1`.
- 56 kont ma blokade ksiegowania na poziomie planu kont i wszystkie sa rozszerzone do `1600`; warto potwierdzic, czy to oczekiwany stan dla kont migracyjnych/pomocniczych.
- `SKB1` nie zawiera blokad ksiegowania, ale pole podatkowe `Pd` ma 9 wartosci `#NAZWA?`, co wyglada na blad eksportu albo interpretacji danych.
- Do jednoznacznej klasyfikacji aktywa/pasywa potrzebny jest eksport wersji sprawozdania finansowego, poniewaz `SKA1-Kto bil.` mowi tylko, czy konto jest bilansowe.
