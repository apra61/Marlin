# Gospodarka wodą — ESPHome (2× Wemos D1 Mini)

Firmware pomiaru poziomu zgodnie z planem **2× Wemos D1 Mini**:

| Urządzenie | Plik | Encje |
|------------|------|--------|
| **Node_pomiar** | [esphome/node_pomiar.yaml](esphome/node_pomiar.yaml) | `Podziemny poziom`, `Oczko poziom`, `Podziemny MAX` |
| **Node_taras** | [esphome/node_taras.yaml](esphome/node_taras.yaml) | `Taras poziom`, `Taras MAX` |

Sterowanie przekaźnikami (pompa 1, filtr, światło) — **osobny** `Node_sterowanie` (poza tym katalogiem).

## Montaż elektryczny

| Wemos | Pin | Funkcja |
|-------|-----|---------|
| Node_pomiar | D1 / D2 | Ultradźwięk podziemny (Trigger / Echo) |
| Node_pomiar | D5 / D6 | Ultradźwięk oczko |
| Node_pomiar | D7 | Pływak MAX podziemny |
| Node_taras | D1 / D2 | Ultradźwięk taras |
| Node_taras | D7 | Pływak MAX taras |

- Sondy **5 V**; Wemos GPIO **3,3 V** — na linii **Echo** zalecany dzielnik (np. 1 kΩ / 2 kΩ) lub level shifter.
- Kondensator **100 µF** przy zasilaniu każdej sondy.
- Długi kabel (> ~10 m): ekranowana skrętka lub sonda **RS485** (wymaga innej konfiguracji).

## Instalacja ESPHome

```bash
pip install esphome
cd garden-water/esphome
cp secrets.yaml.example secrets.yaml
# Uzupełnij WiFi, MQTT, API key

esphome compile node_pomiar.yaml
esphome upload node_pomiar.yaml   # pierwszy raz: --device /dev/ttyUSB0

esphome compile node_taras.yaml
esphome upload node_taras.yaml
```

Kalibracja: w plikach YAML zmień `substitutions` `*_distance_full_m` i `*_distance_empty_m` po pomiarze odległości sondy → lustro wody przy ~0% i ~100% pojemności użytkowej.

## Home Assistant

Pakiet [homeassistant/packages/ogrod_pomiary.yaml](homeassistant/packages/ogrod_pomiary.yaml):

- Szablon alarmu **MAX** (podziemny lub taras)
- Szacunek łącznej pojemności deszczówki (L)
- Automatyzacje: **MAX → wyłącz `switch.node_sterowanie_pompa_1`**

Po MQTT discovery sprawdź rzeczywiste `entity_id` w **Ustawienia → Urządzenia** i popraw nazwy w automatyzacjach, jeśli różnią się od domyślnych.

```yaml
# configuration.yaml (fragment)
homeassistant:
  packages:
    ogrod_pomiary: !include garden-water/homeassistant/packages/ogrod_pomiary.yaml
```

## MQTT (topic_prefix)

| Node | Prefix |
|------|--------|
| Node_pomiar | `ogrod/node_pomiar` |
| Node_taras | `ogrod/node_taras` |

Discovery: `homeassistant/...` (standard ESPHome + MQTT).
