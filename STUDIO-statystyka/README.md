# STUDIO-statystyka

Starter Python project for descriptive statistics.

## Features

- Calculate count, minimum, maximum, mean, median, population variance, and standard deviation.
- Run calculations from a small command-line interface.
- Validate behavior with standard-library unit tests.

## Quick start

Run the CLI without installing the package:

```bash
cd STUDIO-statystyka
PYTHONPATH=src python3 -m studio_statystyka 1 2 3 4 5
```

Run tests:

```bash
python3 -m unittest discover -s tests
```

## Project layout

```text
STUDIO-statystyka/
|-- pyproject.toml
|-- README.md
|-- src/
|   `-- studio_statystyka/
|       |-- __init__.py
|       |-- __main__.py
|       `-- descriptive.py
`-- tests/
    `-- test_descriptive.py
```
