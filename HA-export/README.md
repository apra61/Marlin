# HA — gospodarka wodą (ESPHome + Home Assistant)

Repozytorium konfiguracji ogrodu: **2× Wemos D1 Mini** (pomiar) + pakiety HA.

| Urządzenie | Plik | Encje |
|------------|------|--------|
| **Node_pomiar** | [esphome/node_pomiar.yaml](esphome/node_pomiar.yaml) | `Podziemny poziom`, `Oczko poziom`, `Podziemny MAX` |
| **Node_taras** | [esphome/node_taras.yaml](esphome/node_taras.yaml) | `Taras poziom`, `Taras MAX` |

Sterowanie przekaźnikami (pompa 1, filtr, światło) — planowany **Node_sterowanie** (osobna konfiguracja ESPHome).

## Struktura

```text
HA/
├── esphome/              # firmware Wemos
├── homeassistant/
│   └── packages/         # pakiety do configuration.yaml
└── README.md
```

## ESPHome

```bash
pip install esphome
cd esphome
cp secrets.yaml.example secrets.yaml
# WiFi, MQTT, api_encryption_key (32 B base64)

esphome run node_pomiar.yaml
esphome run node_taras.yaml
```

Kalibracja: `substitutions` → `*_distance_full_m` / `*_distance_empty_m` w YAML.

## Home Assistant

W `configuration.yaml` (w swojej instalacji HA):

```yaml
homeassistant:
  packages:
    ogrod_pomiary: !include <ścieżka-do-repo>/homeassistant/packages/ogrod_pomiary.yaml
```

Jeśli to repo jest sklonowane jako `/config/custom/HA`:

```yaml
homeassistant:
  packages:
    ogrod_pomiary: !include custom/HA/homeassistant/packages/ogrod_pomiary.yaml
```

## Repozytorium GitHub: `apra61/HA`

**URL docelowy:** https://github.com/apra61/HA

Repozytorium musi istnieć na GitHubie (puste, bez README). Następnie:

```bash
cd HA   # katalog tego projektu
./scripts/publish-to-apra61-ha.sh
```

Ręcznie:

```bash
git remote add origin https://github.com/apra61/HA.git
git push -u origin main
```

> Środowisko Cloud Agent nie może utworzyć repozytorium za Ciebie — jeśli `git push` zwraca „Repository not found”, utwórz `apra61/HA` w przeglądarce i uruchom skrypt ponownie.
