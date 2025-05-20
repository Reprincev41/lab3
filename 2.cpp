#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include <map>
#include <numeric>

using namespace std;

vector<int> generateSmallPrimes(int limit = 500) {
    vector<bool> isPrime(limit + 1, true);
    isPrime[0] = isPrime[1] = false;
    for (int p = 2; p * p <= limit; p++) {
        if (isPrime[p]) {
            for (int i = p * p; i <= limit; i += p) {
                isPrime[i] = false;
            }
        }
    }
    vector<int> primes;
    for (int p = 2; p <= limit; p++) {
        if (isPrime[p]) primes.push_back(p);
    }
    return primes;
}

long long powMod(long long a, long long b, long long m) {
    long long res = 1;
    a %= m;
    while (b > 0) {
        if (b % 2 == 1) res = (res * a) % m;
        a = (a * a) % m;
        b >>= 1;
    }
    return res;
}

vector<pair<long long, int>> factorize(long long n, const vector<int>& primes) {
    vector<pair<long long, int>> factors;
    for (int p : primes) {
        if ((long long)p * p > n) break;
        if (n % p == 0) {
            int cnt = 0;
            while (n % p == 0) {
                n /= p;
                cnt++;
            }
            factors.emplace_back(p, cnt);
        }
    }
    if (n > 1) factors.emplace_back(n, 1);
    return factors;
}

long long powWithLimit(long long q, int a, long long limit) {
    long long result = 1;
    for (int i = 0; i < a; ++i) {
        if (result > limit / q) return -1;
        result *= q;
    }
    return result;
}

bool isPrimeMiller(long long n, const vector<pair<long long, int>>& factors, int t, mt19937& gen) {
    if (n < 2) return false;
    if (n == 2 || n == 3) return true;
    if (n % 2 == 0) return false;

    uniform_int_distribution<long long> dis(2, n - 2);
    vector<long long> a_list;
    for (int i = 0; i < t; i++) {
        long long a = dis(gen);
        a_list.push_back(a);
        if (powMod(a, n - 1, n) != 1) return false;
    }

    for (auto [q, _] : factors) {
        bool found = false;
        for (long long a : a_list) {
            if (powMod(a, (n - 1) / q, n) != 1) {
                found = true;
                break;
            }
        }
        if (!found) return false; 
    }

    return true;
}



long long generateMillerCandidate(int bitSize, mt19937& gen, const vector<int>& primes, int t, int& k) {
    const long long max_m = 1LL << (bitSize - 1);
    uniform_int_distribution<int> a_i(1, 3);

    while (true) {
        long long m = 1;
        vector<int> shuffledPrimes = primes;
        shuffle(shuffledPrimes.begin(), shuffledPrimes.end(), gen);

        for (int prime : shuffledPrimes) {
            int a = a_i(gen);
            long long term = powWithLimit(prime, a, max_m);
            
            if (term == -1) continue;
            if (m > max_m / term) break;
            
            m *= term;
            if (m >= max_m) {
                m /= term;
                break;
            }
        }

        if (m < 2) continue;

        long long n = 2 * m + 1;
        vector<pair<long long, int>> factors = factorize(n - 1, primes);
        
        if (isPrimeMiller(n, factors, t, gen)) {
            return n;
        } else {
            k++;
        }
    }
}



bool isMillerRabin(long long n, int t, mt19937& gen) {
    if (n < 2) return false;
    if (n == 2 || n == 3) return true;
    if (n % 2 == 0) return false;

    long long d = n - 1;
    int s = 0;
    while (d % 2 == 0) {
        d /= 2;
        s++;
    }

    uniform_int_distribution<long long> dis(2, n - 2);

    for (int i = 0; i < t; i++) {
        long long a = dis(gen);
        long long x = powMod(a, d, n);
        if (x == 1 || x == n - 1) continue;

        bool composite = true;
        for (int j = 0; j < s - 1; j++) {
            x = powMod(x, 2, n);
            if (x == n - 1) {
                composite = false;
                break;
            }
        }

        if (composite) return false;
    }

    return true;
}

bool isPrimePocklington(long long n, const vector<pair<long long, int>>& factorsF, int t, mt19937& gen) {
    if (n < 2) return false;
    if (n == 2 || n == 3) return true;
    if (n % 2 == 0) return false;

    uniform_int_distribution<long long> dis(2, n - 2);
    vector<long long> a_list;
    for (int i = 0; i < t; i++) {
        long long a = dis(gen);
        a_list.push_back(a);
        if (powMod(a, n - 1, n) != 1) return false;
    }

    for (long long a : a_list) {
        bool found = false;
        for (const auto& [q, _] : factorsF)  {
            if (powMod(a, (n - 1) / q, n) == 1) {
                found = true;
                break;
            }
        } //внешний по аи внтурений qj
        if (!found) return true;
    }

    return false;

}

long long generatePocklingtonCandidate(int bitSize, mt19937& gen, const vector<int>& primes, int t, int& k) {
    const int halfBitSize = bitSize / 2;
    const long long maxF = 1LL << (halfBitSize + 1);
    const long long maxR = 1LL << (halfBitSize);

    uniform_int_distribution<int> a_i(1, 3);
    uniform_int_distribution<long long> rDist(2, maxR);

    while (true) {
        long long F = 1;
        vector<int> shuffledPrimes = primes;
        shuffle(shuffledPrimes.begin(), shuffledPrimes.end(), gen);

        for (int prime : shuffledPrimes) {
            int a = a_i(gen);
            long long term = powWithLimit(prime, a, maxF);
            if (term == -1) continue;
            if (F > maxF / term) break;
            F *= term;
        }

        if (F < 2 || F > maxF) continue;

        long long R;
        do {
            R = rDist(gen) * 2;
        } while (R >= maxR);

        long long n = R * F + 1;
        if (n > (1LL << bitSize)) continue;

        vector<pair<long long, int>> factorsF = factorize(F, primes);
        k++;
        if (isPrimePocklington(n, factorsF, t, gen) && isMillerRabin(n, 5, gen)) {
            return n;
        }
    }
}

long long generateGOSTCandidate(long long q, int t, mt19937& gen, int maxAttempts = 10000) {
    uniform_real_distribution<double> xi_dist(0.0, 1.0);
    int attempts = 0;
    while (attempts < maxAttempts) {
        attempts++;
        long long t1 = 1LL << (t - 1);
        double xi = xi_dist(gen);
        long long term1 = t1 / q;
        long long term2 = static_cast<long long>(t1 * xi) / q;
        long long N = ceil(term1+ term2);
        if (N % 2 != 0) N += 1;

        long long u = 0;
        while (true) {
            long long p = (N + u) * q + 1;
            if (p > (1LL << t)) break;

            if (powMod(2, p-1, p) == 1 && powMod(2, N+u, p) != 1) {
                if (isMillerRabin(p, 5, gen)) {
                    return p;
                }
            }
            u += 2;
        }
    }
    return -1;
}

void printTable(const vector<vector<string>>& data, const vector<string>& headers) {
    const int cols = headers.size();
    vector<int> widths(cols, 0);
    
    for (int i = 0; i < cols; i++) {
        widths[i] = headers[i].length();
        for (auto& row : data) {
            if (row[i].length() > widths[i]) {
                widths[i] = row[i].length();
            }
        }
    }

    auto printLine = [&]() {
        cout << "+";
        for (int w : widths) cout << setw(w + 2) << setfill('-') << "" << "+";
        cout << setfill(' ') << endl;
    };

    printLine();
    cout << "|";
    for (int i = 0; i < cols; i++) {
        cout << " " << setw(widths[i]) << headers[i] << " |";
    }
    cout << endl;
    printLine();

    for (auto& row : data) {
        cout << "|";
        for (int i = 0; i < cols; i++) {
            cout << " " << setw(widths[i]) << row[i] << " |";
        }
        cout << endl;
    }
    printLine();
}

double calculateMillerError(const vector<pair<long long, int>>& factors, int t) {
    double prob = 1.0;
    for (const auto& [q, _] : factors) {
        prob *= (1.0 / q );
    }
    return pow(prob, t);
}

double calculatePocklingtonError(const vector<pair<long long, int>>& factorsF, int t) {
    double prob = 1.0;
    for (const auto& [q, _] : factorsF) {
        prob *= (1.0 - 1.0 / q);
    }
    return pow(1.0 - prob, t);
}



int main() {
    const vector<int> primes = generateSmallPrimes();
    random_device rd;
    mt19937 gen(rd());
    const int bitSize = 10;
    const int testsCount = 10;

    cout << "Тест Миллера:\n";
    vector<vector<string>> millerData;
    for (int i = 1; i <= testsCount; i++) {
        int k = 0;
        long long candidate = generateMillerCandidate(bitSize, gen, primes, 5, k);
        auto factors = factorize(candidate - 1, primes);
        bool isPrime = isMillerRabin(candidate, 1, gen);
        
        ostringstream error_stream;
        error_stream << fixed << setprecision(5) << calculateMillerError(factors, 1);
        
        millerData.push_back({
            to_string(i),
            to_string(candidate),
            isPrime ? "+" : "-",
            to_string(k),
            error_stream.str()
        });
    }
    printTable(millerData, {"№", "Число", "Простое", "k", "Вероятность ошибки"});

    cout << "\nТест Поклинттона:\n";
    vector<vector<string>> pocklingtonData;
    for (int i = 1; i <= testsCount; i++) {
        int k_p = 0;
        long long candidate = generatePocklingtonCandidate(bitSize, gen, primes, 10, k_p);
        auto factorsF = factorize((candidate - 1)/2, primes); // Примерный факторинг F
        bool isPrime = isMillerRabin(candidate, 10, gen);

        ostringstream error_stream;
        error_stream << fixed << setprecision(5) << calculatePocklingtonError(factorsF, 9);

        pocklingtonData.push_back({
            to_string(i),
            to_string(candidate),
            isPrime ? "+" : "-",
            to_string(k_p),
            error_stream.str()
        });
    }
    printTable(pocklingtonData, {"№", "Число", "Простое ", "k", "Вероятность ошибки"});
    

    cout << "\nТест ГОСТ Р 34.10-94:\n";
    vector<vector<string>> gostData;
    for (int i = 1; i <= testsCount; i++) {
        int halfBitSize = bitSize / 2;
        int k_q = 0;
        long long q = generateMillerCandidate(halfBitSize, gen, primes, 5, k_q);
        if (q == -1) continue;

        long long p = generateGOSTCandidate(q, bitSize, gen);
        bool isPrime = (p != -1) && isMillerRabin(p, 5, gen);
        
        gostData.push_back({
            to_string(i),
            (p != -1) ? to_string(p) : "N/A",
            isPrime ? "+" : "-",
        });
    }
    printTable(gostData, {"№", "Число", "Простое"});
    
    return 0;
}