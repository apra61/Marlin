# Analiza planu kont z plikow CSV

## Zakres danych

Analiza obejmuje dwa eksporty SAP:

- `SKA1_f398.csv` - dane kont KG na poziomie planu kont.
- `SKB1_535a.csv` - dane kont KG na poziomie jednostki gospodarczej.

Pliki sa rozdzielane separatorem `;`.

## Podsumowanie liczbowe

| Obszar | Wynik |
| --- | ---: |
| Plan kont w `SKA1` | `YCOA` |
| Jednostka gospodarcza w `SKB1` | `1600` |
| Wiersze / unikalne konta w `SKA1` | 2579 / 2579 |
| Wiersze / unikalne konta w `SKB1` | 1153 / 1153 |
| Duplikaty kont w `SKA1` | 0 |
| Duplikaty par `JG + Konto KG` w `SKB1` | 0 |
| Konta z `SKB1`, ktorych brak w `SKA1` | 0 |
| Konta z `SKA1` nierozszerzone do JG `1600` | 1426 |
| Konta aktywne, rozszerzone i bez blokady ksiegowania na obu poziomach | 1046 |

## Struktura kont

| Pole | Rozklad |
| --- | --- |
| `SKA1-Kto bil.` | `X`: 1730, puste: 849 |
| `SKA1-RodzKontKG` | `X`: 1730, `P`: 757, `S`: 77, `N`: 15 |

Najwieksze grupy `GrKt` w `SKA1`:

| Grupa | Liczba kont |
| --- | ---: |
| `FIN.` | 890 |
| `ERG.` | 770 |
| `SAKO` | 658 |
| `SECC` | 77 |
| `ANL.` | 73 |
| `ABST` | 61 |
| `MAT.` | 49 |
| `SASL` | 1 |

## Jakosc danych

| Kontrola | Wynik |
| --- | ---: |
| Konta `SKA1` z pustym opisem | 98 |
| Konta z `SKA1-Blok.ks. = X` | 357 |
| Konta z `SKA1-Blok.ks. = X`, ktore sa rozszerzone do `1600` | 57 |
| Konta z `SKB1-Blok.ks. = X` | 51 |

Przyklady kont bez opisu:

- `11003400`
- `11003420`
- `11003480`
- `11003481`
- `11003500`
- `11003520`
- `11003580`
- `11003581`
- `11003600`
- `11003620`

Przyklady kont z blokada ksiegowania w `SKA1`:

- `11001000`
- `11001010`
- `11001020`
- `11001030`
- `11001040`
- `11001045`
- `11001050`
- `11001060`
- `11001070`
- `11001080`

Przyklady kont z blokada ksiegowania w `SKB1`:

- `11009010`
- `11009030`
- `11009040`
- `11009060`
- `11009081`
- `11009110`
- `11009130`
- `11009140`
- `11009160`
- `11009210`

## Dane spolki `1600`

| Pole | Rozklad |
| --- | --- |
| `SKB1-Wsk. uzg.` | puste: 1032, `A`: 65, `D`: 34, `K`: 22 |
| `SKB1-Wal.` | `PLN`: 1134, `EUR`: 19 |

Najczestsze grupy statusu pol `SKB1-GrpStatPol`:

| Grupa statusu pol | Liczba kont |
| --- | ---: |
| `YB01` | 368 |
| `YB04` | 159 |
| `YB05` | 83 |
| `YB29` | 72 |
| `SECC` | 72 |
| `YB03` | 62 |
| `YB65` | 47 |
| `YB07` | 43 |
| `YB06` | 34 |
| `YB67` | 32 |

## Konta nierozszerzone do JG `1600`

Liczba kont z `SKA1`, ktore nie wystepuja w `SKB1` dla JG `1600`: 1426.

Rozklad wg `GrKt`:

| Grupa | Liczba kont |
| --- | ---: |
| `FIN.` | 804 |
| `SAKO` | 299 |
| `ERG.` | 294 |
| `MAT.` | 12 |
| `SECC` | 6 |
| `ABST` | 5 |
| `ANL.` | 5 |
| `SASL` | 1 |

Rozklad wg `Kto bil.`:

| `Kto bil.` | Liczba kont |
| --- | ---: |
| `X` | 1124 |
| puste | 302 |

## Wnioski

- Dane sa spojne technicznie: kazde konto z `SKB1` istnieje w `SKA1`.
- Nie znaleziono duplikatow kont w `SKA1` ani duplikatow par `JG + Konto KG` w `SKB1`.
- Najwieksza luka do weryfikacji biznesowej to 1426 kont z planu `YCOA`, ktore nie sa rozszerzone do jednostki `1600`.
- 98 kont nie ma opisu w eksporcie `SKA1`; warto je uzupelnic albo potwierdzic, ze sa nieuzywane.
- 57 kont z blokada ksiegowania na poziomie planu kont jest jednoczesnie rozszerzonych do `1600`; warto sprawdzic, czy to oczekiwany stan.
- Do jednoznacznej klasyfikacji aktywa/pasywa potrzebny jest eksport wersji sprawozdania finansowego, poniewaz `SKA1-Kto bil.` mowi tylko, czy konto jest bilansowe.
