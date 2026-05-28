import argparse

import pytest

from csv_to_calc import (
    CsvToCalcError,
    build_parser,
    count_rows_to_write,
    main,
    read_csv,
    validate_args,
)


def _namespace(csv_file, target_file, **overrides):
    values = {
        "csv_file": str(csv_file),
        "target_file": str(target_file),
        "sheet": None,
        "start_row": 1,
        "start_col": 1,
        "delimiter": ";",
        "encoding": "utf-8",
        "has_header": False,
        "dry_run": False,
    }
    values.update(overrides)
    return argparse.Namespace(**values)


def test_read_csv_reads_rows_with_default_semicolon_delimiter(tmp_path):
    csv_path = tmp_path / "dane.csv"
    csv_path.write_text("imie;nazwisko\nJan;Kowalski\n", encoding="utf-8")

    assert read_csv(csv_path) == [
        ["imie", "nazwisko"],
        ["Jan", "Kowalski"],
    ]


@pytest.mark.parametrize("option", ["--start-row", "--start-col"])
def test_parser_rejects_non_positive_start_positions(option, capsys):
    parser = build_parser()

    with pytest.raises(SystemExit) as excinfo:
        parser.parse_args(["dane.csv", "arkusz.xlsx", option, "0"])

    assert excinfo.value.code == 2
    assert "musi być większe od 0" in capsys.readouterr().err


def test_validate_args_rejects_missing_files(tmp_path):
    target_path = tmp_path / "arkusz.xlsx"
    target_path.write_bytes(b"placeholder")

    with pytest.raises(CsvToCalcError) as excinfo:
        validate_args(_namespace(tmp_path / "brak.csv", target_path))

    assert excinfo.value.exit_code == 2
    assert "Nie znaleziono pliku CSV" in str(excinfo.value)


def test_validate_args_rejects_bad_source_format(tmp_path):
    csv_path = tmp_path / "dane.txt"
    target_path = tmp_path / "arkusz.xlsx"
    csv_path.write_text("a,b\n", encoding="utf-8")
    target_path.write_bytes(b"placeholder")

    with pytest.raises(CsvToCalcError) as excinfo:
        validate_args(_namespace(csv_path, target_path))

    assert excinfo.value.exit_code == 2
    assert "rozszerzenie .csv" in str(excinfo.value)


def test_validate_args_rejects_bad_target_format(tmp_path):
    csv_path = tmp_path / "dane.csv"
    target_path = tmp_path / "arkusz.txt"
    csv_path.write_text("a,b\n", encoding="utf-8")
    target_path.write_text("", encoding="utf-8")

    with pytest.raises(CsvToCalcError) as excinfo:
        validate_args(_namespace(csv_path, target_path))

    assert excinfo.value.exit_code == 2
    assert "rozszerzenie .ods albo .xlsx" in str(excinfo.value)


def test_main_reports_friendly_error_for_invalid_xlsx(tmp_path, capsys):
    csv_path = tmp_path / "dane.csv"
    target_path = tmp_path / "arkusz.xlsx"
    csv_path.write_text("a,b\n", encoding="utf-8")
    target_path.write_text("to nie jest xlsx", encoding="utf-8")

    exit_code = main([str(csv_path), str(target_path)])

    assert exit_code == 1
    stderr = capsys.readouterr().err
    assert "Błąd: Plik .xlsx ma nieprawidłowy format" in stderr
    assert "Traceback" not in stderr


@pytest.mark.parametrize(
    ("rows", "has_header", "expected_count"),
    [
        ([["h1", "h2"], ["a", "b"], ["c", "d"]], False, 3),
        ([["h1", "h2"], ["a", "b"], ["c", "d"]], True, 2),
        ([], True, 0),
    ],
)
def test_count_rows_to_write_with_and_without_header(rows, has_header, expected_count):
    assert count_rows_to_write(rows, has_header) == expected_count
