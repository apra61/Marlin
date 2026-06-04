#!/usr/bin/env bash
# Publikuje ten projekt do https://github.com/apra61/HA
# Wymaganie: najpierw utwórz PUSTE repozytorium „HA” na GitHubie (bez README).

set -euo pipefail
cd "$(dirname "$0")/.."

if ! git remote get-url origin &>/dev/null; then
  git remote add origin "https://github.com/apra61/HA.git"
fi

echo "Sprawdzam dostęp do apra61/HA..."
if ! git ls-remote origin &>/dev/null; then
  echo ""
  echo "Repozytorium https://github.com/apra61/HA nie istnieje lub brak dostępu."
  echo "1. Zaloguj się na GitHub → New repository → nazwa: HA → owner: apra61"
  echo "2. Bez inicjalizacji (bez README, bez .gitignore)"
  echo "3. Uruchom ponownie: ./scripts/publish-to-apra61-ha.sh"
  exit 1
fi

git push -u origin main
echo "Gotowe: https://github.com/apra61/HA"
