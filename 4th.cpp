#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>

// Читаем n, m, затем n чисел.
// Строим dp[i] = макс. разница (текущий_игрок - другой_игрок),
//                если сейчас брать с позиции i (0-based).
// dp[n] = 0 (когда элементов нет).
// dp[i] = max_{k=1..m, i+k <= n} ( sum(i..i+k-1) - dp[i+k] ).
// В конце dp[0] > 0 -> первый игрок выиграл, иначе второй.

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, m;
    std::cin >> n >> m;
    std::vector<long long> arr(n);
    for(int i = 0; i < n; i++){
        std::cin >> arr[i];
    }

    // Префиксные суммы, prefixSum[i] = сумма arr[0..i-1], prefixSum[0] = 0
    std::vector<long long> prefixSum(n+1, 0LL);
    for(int i = 0; i < n; i++){
        prefixSum[i+1] = prefixSum[i] + arr[i];
    }

    // dp[i] - макс. разница счёта, если сейчас ходить с i-го элемента
    std::vector<long long> dp(n+1, 0LL);

    // "снизу вверх"
    for(int i = n-1; i >= 0; i--){
        long long best = std::numeric_limits<long long>::lowest();
        // перебираем, сколько взять (от 1 до m)
        for(int k = 1; k <= m; k++){
            if(i + k > n) break;
            long long sumTaken = prefixSum[i + k] - prefixSum[i]; // сумма arr[i..i+k-1]
            long long candidate = sumTaken - dp[i + k];
            if(candidate > best) {
                best = candidate;
            }
        }
        dp[i] = best;
    }

    // dp[0] > 0 -> первый игрок (Павел) выиграл, иначе второй (Вика).
    if(dp[0] > 0) std::cout << 1 << "\n";
    else std::cout << 0 << "\n";

    return 0;
}