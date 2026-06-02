"""Command-line interface for STUDIO-statystyka."""

from __future__ import annotations

import argparse
import json
from dataclasses import asdict
from typing import Sequence

from .descriptive import summarize


def main(argv: Sequence[str] | None = None) -> int:
    parser = argparse.ArgumentParser(
        prog="studio-statystyka",
        description="Calculate descriptive statistics for numeric values.",
    )
    parser.add_argument("values", nargs="+", type=float, help="Numeric values to summarize.")
    args = parser.parse_args(argv)

    print(json.dumps(asdict(summarize(args.values)), indent=2, sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
