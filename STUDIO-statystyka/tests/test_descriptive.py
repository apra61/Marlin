from __future__ import annotations

import math
import sys
import unittest
from pathlib import Path


PROJECT_ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(PROJECT_ROOT / "src"))

from studio_statystyka import summarize  # noqa: E402


class DescriptiveStatsTest(unittest.TestCase):
    def test_summarizes_odd_count_values(self) -> None:
        stats = summarize([1, 2, 3, 4, 5])

        self.assertEqual(stats.count, 5)
        self.assertEqual(stats.minimum, 1)
        self.assertEqual(stats.maximum, 5)
        self.assertEqual(stats.mean, 3)
        self.assertEqual(stats.median, 3)
        self.assertEqual(stats.variance, 2)
        self.assertTrue(math.isclose(stats.standard_deviation, math.sqrt(2)))

    def test_summarizes_even_count_values(self) -> None:
        stats = summarize([10, 20, 30, 40])

        self.assertEqual(stats.count, 4)
        self.assertEqual(stats.mean, 25)
        self.assertEqual(stats.median, 25)
        self.assertEqual(stats.variance, 125)

    def test_rejects_empty_input(self) -> None:
        with self.assertRaisesRegex(ValueError, "At least one numeric value"):
            summarize([])


if __name__ == "__main__":
    unittest.main()
