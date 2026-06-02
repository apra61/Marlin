"""Descriptive statistics helpers."""

from __future__ import annotations

from dataclasses import dataclass
from math import sqrt
from typing import Iterable


@dataclass(frozen=True)
class DescriptiveStats:
    """Summary statistics for a non-empty numeric sample."""

    count: int
    minimum: float
    maximum: float
    mean: float
    median: float
    variance: float
    standard_deviation: float


def summarize(values: Iterable[float]) -> DescriptiveStats:
    """Return descriptive statistics using population variance."""

    numbers = _normalize(values)
    average = sum(numbers) / len(numbers)
    variance = sum((number - average) ** 2 for number in numbers) / len(numbers)

    return DescriptiveStats(
        count=len(numbers),
        minimum=min(numbers),
        maximum=max(numbers),
        mean=average,
        median=_median(numbers),
        variance=variance,
        standard_deviation=sqrt(variance),
    )


def _normalize(values: Iterable[float]) -> list[float]:
    numbers = [float(value) for value in values]
    if not numbers:
        raise ValueError("At least one numeric value is required.")
    return numbers


def _median(values: list[float]) -> float:
    sorted_values = sorted(values)
    middle = len(sorted_values) // 2

    if len(sorted_values) % 2:
        return sorted_values[middle]

    return (sorted_values[middle - 1] + sorted_values[middle]) / 2
