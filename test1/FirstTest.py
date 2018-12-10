import unittest


class FirstTest(unittest.TestCase):

    def test_a(self):
        self.assertEqual(0, 0)


if __name__ == "__main__":
    unittest.main()
