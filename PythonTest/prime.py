import pytest
import unittest

def is_prime(num):
    if num <= 1:
        return False
    for i in range(2, int(num**0.5) + 1):
        if num % i == 0:
            return False
    return True

def prime_numbers(limit):
    primes = []
    for num in range(2, limit + 1):
        if is_prime(num):
            primes.append(num)
    return primes

def test_is_prime():
    assert is_prime(2) == True
    assert is_prime(4) == False
    
class TestPrime(unittest.TestCase):

    def test_prime2(self):
        self.assertEqual(is_prime(2), True)

    def test_prime4(self):
        self.assertEqual(is_prime(4), False)

def fibonacci(limit):
    fib_sequence = [0, 1]
    while True:
        next_value = fib_sequence[-1] + fib_sequence[-2]
        if next_value > limit:
            break
        fib_sequence.append(next_value)
    return fib_sequence

# Example usage
limit = 50
print(f"Prime numbers up to {limit}: {prime_numbers(limit)}")

# Example usage
limit = 50
print(f"Fibonacci numbers up to {limit}: {fibonacci(limit)}")

if __name__ == "__main__":
    unittest.main(verbosity=2)


