import random
import math
from tabulate import tabulate

def generate_small_primes(limit=500):
    sieve = [True] * (limit + 1)
    sieve[0] = sieve[1] = False
    for p in range(2, int(math.sqrt(limit)) + 1):
        if sieve[p]:
            for i in range(p*p, limit+1, p):
                sieve[i] = False
    return [p for p, is_prime in enumerate(sieve) if is_prime]

def pow_mod(a, b, m):
    return pow(a, b, m)

def factorize(n, primes):
    factors = []
    for p in primes:
        if p*p > n:
            break
        if n % p == 0:
            cnt = 0
            while n % p == 0:
                n //= p
                cnt += 1
            factors.append((p, cnt))
    if n > 1:
        factors.append((n, 1))
    return factors

def pow_with_limit(q, a, limit):
    result = 1
    for _ in range(a):
        if result > limit // q:
            return -1
        result *= q
    return result

def is_prime_miller(n, factors, t, gen):
    if n < 2:
        return False
    if n in (2, 3):
        return True
    if n % 2 == 0:
        return False

    a_list = [random.randint(2, n-2) for _ in range(t)]
    for a in a_list:
        if pow_mod(a, n-1, n) != 1:
            return False

    for q, _ in factors:
        found = False
        for a in a_list:
            if pow_mod(a, (n-1)//q, n) != 1:
                found = True
                break
        if not found:
            return False
    return True

def generate_miller_candidate(bit_size, primes, t, max_attempts=100000):
    max_m = 1 << (bit_size - 1)
    gen = random.Random()
    
    for _ in range(max_attempts):
        m = 1
        shuffled_primes = random.sample(primes, len(primes))
        
        for p in shuffled_primes:
            a = random.randint(1, 3)
            term = pow_with_limit(p, a, max_m)
            if term == -1:
                continue
            if m > max_m // term:
                break
            m *= term
            if m >= max_m:
                m //= term
                break
        
        if m < 2:
            continue
        
        n = 2 * m + 1
        factors = factorize(n-1, primes)
        
        if is_prime_miller(n, factors, t, gen):
            return n
    return -1

def is_miller_rabin(n, t):
    if n < 2:
        return False
    if n in (2, 3):
        return True
    if n % 2 == 0:
        return False

    d = n - 1
    s = 0
    while d % 2 == 0:
        d //= 2
        s += 1

    for _ in range(t):
        a = random.randint(2, n-2)
        x = pow_mod(a, d, n)
        if x == 1 or x == n-1:
            continue
        
        composite = True
        for _ in range(s-1):
            x = pow_mod(x, 2, n)
            if x == n-1:
                composite = False
                break
        if composite:
            return False
    return True

def is_prime_pocklington(n, factors_f, t):
    if n < 2:
        return False
    if n in (2, 3):
        return True
    if n % 2 == 0:
        return False

    a_list = [random.randint(2, n-2) for _ in range(t)]
    for a in a_list:
        if pow_mod(a, n-1, n) != 1:
            return False

    for a in a_list:
        found = False
        for q, _ in factors_f:
            if pow_mod(a, (n-1)//q, n) == 1:
                found = True
                break
        if not found:
            return True
    return False

def generate_pocklington_candidate(bit_size, primes, t, max_attempts=100000):
    half_bit_size = bit_size // 2
    max_f = 1 << (half_bit_size + 1)
    max_r = 1 << half_bit_size
    gen = random.Random()

    for _ in range(max_attempts):
        F = 1
        shuffled_primes = random.sample(primes, len(primes))
        
        for p in shuffled_primes:
            a = random.randint(1, 3)
            term = pow_with_limit(p, a, max_f)
            if term == -1:
                continue
            if F > max_f // term:
                break
            F *= term
        
        if F < 2 or F > max_f:
            continue
        
        R = random.randint(1, max_r//2) * 2
        n = R * F + 1
        
        if n > (1 << bit_size):
            continue
        
        factors_f = factorize(F, primes)
        if is_prime_pocklington(n, factors_f, t) and is_miller_rabin(n, 5):
            return n
    return -1

def generate_gost_candidate(q, t, max_attempts=10000):
    for _ in range(max_attempts):
        t1 = 1 << (t-1)
        xi = random.random()
        term1 = t1 // q
        term2 = int(t1 * xi) // q
        N = (term1 + term2 + 1) // 2 * 2
        
        u = 0
        while True:
            p = (N + u) * q + 1
            if p > (1 << t):
                break
            if (pow(2, p-1, p) == 1 and 
                pow(2, N+u, p) != 1 and 
                is_miller_rabin(p, 5)):
                return p
            u += 2
    return -1

def print_table(data, headers):
    print(tabulate(data, headers=headers, tablefmt="grid", stralign="center"))

def calculate_miller_error(factors, t):
    prob = 1.0
    for q, _ in factors:
        prob *= 1.0 / q
    return (prob) ** t

def calculate_pocklington_error(factors_f, t):
    prob = 1.0
    for q, _ in factors_f:
        prob *= (1.0 - 1.0 / q)
    return (1.0 - prob) ** t

def main():
    primes = generate_small_primes()
    bit_size = 16
    tests_count = 10

    print("Тест Миллера:")
    miller_data = []
    for i in range(1, tests_count+1):
        candidate = generate_miller_candidate(bit_size, primes, 5)
        if candidate == -1:
            continue
        factors = factorize(candidate-1, primes)
        is_prime = is_miller_rabin(candidate, 5)
        error = calculate_miller_error(factors, 1)
        miller_data.append([
            str(i),
            str(candidate),
            "+" if is_prime else "-",
            "0",
            f"{error:.5f}"
        ])
    print_table(miller_data, ["№", "Число", "Простое", "k", "Вероятность ошибки"])

    print("\nТест Поклингтона:")
    pocklington_data = []
    for i in range(1, tests_count+1):
        candidate = generate_pocklington_candidate(bit_size, primes, 10)
        if candidate == -1:
            continue
        factors_f = factorize((candidate-1)//2, primes)
        is_prime = is_miller_rabin(candidate, 10)
        error = calculate_pocklington_error(factors_f, 9)
        pocklington_data.append([
            str(i),
            str(candidate),
            "+" if is_prime else "-",
            "0",
            f"{error:.5f}"
        ])
    print_table(pocklington_data, ["№", "Число", "Простое ", "k", "Вероятность ошибки"])

    print("\nТест ГОСТ Р 34.10-94:")
    gost_data = []
    for i in range(1, tests_count+1):
        half_bit_size = bit_size // 2
        q = generate_miller_candidate(half_bit_size, primes, 5)
        if q == -1:
            continue
        p = generate_gost_candidate(q, bit_size)
        is_prime = p != -1 and is_miller_rabin(p, 5)
        gost_data.append([
            str(i),
            str(p) if p != -1 else "N/A",
            "+" if is_prime else "-",
        ])
    print_table(gost_data, ["№", "Число", "Простое"])

if __name__ == "__main__":
    main()
