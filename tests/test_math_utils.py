import pytest

from cpkit import H, P, base2, dig, division, modpow, pcnt, plist


@pytest.mark.parametrize(
    ("function", "args", "expected"),
    [
        (base2, (10,), "1010"),
        (base2, (0,), "0"),
        (H, (3, 2), 6),
        (P, (5, 2), 20),
        (modpow, (2, 10, 1_000), 24),
        (division, (10, 2, 1_000_000_007), 5),
        (dig, (-100,), 3),
        (dig, (0,), 1),
        (plist, (0,), []),
        (plist, (1,), []),
        (plist, (2,), [2]),
        (plist, (10,), [2, 3, 5, 7]),
        (pcnt, (10,), 4),
    ],
)
def test_public_math_helper(function, args, expected):
    assert function(*args) == expected
