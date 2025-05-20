#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>   // std::gcd (C++17), либо <cstdlib> и писать свою gcd

// Функция для нахождения НОД двух 64-битных чисел (C++17)
long long gcd_ll(long long a, long long b) {
    a = (a < 0 ? -a : a);
    b = (b < 0 ? -b : b);
    while (b != 0) {
        long long r = a % b;
        a = b;
        b = r;
    }
    return a;
}

// Структура для хранения полинома: coefficients[i] соответствует коэффициенту при x^i
struct Polynomial {
    // Коэффициенты в порядке возрастания степеней: c0 + c1*x + c2*x^2 + ...
    std::vector<long long> coefficients;
};

// Получить производную полинома p' (коэффициенты при x^k становятся (k+1)*coeff[k+1]).
Polynomial derivative(const Polynomial &p) {
    Polynomial dp;
    if (p.coefficients.size() <= 1) {
        // Константный полином -> производная = 0
        dp.coefficients.clear();
        return dp;
    }
    dp.coefficients.resize(p.coefficients.size() - 1);
    for (size_t i = 1; i < p.coefficients.size(); i++) {
        dp.coefficients[i - 1] = p.coefficients[i] * (long long)i;
    }
    return dp;
}

// Сложение двух полиномов (p1 + p2).
Polynomial polyAdd(const Polynomial &p1, const Polynomial &p2) {
    Polynomial res;
    size_t n = std::max(p1.coefficients.size(), p2.coefficients.size());
    res.coefficients.resize(n, 0LL);
    for (size_t i = 0; i < n; i++) {
        long long c1 = 0, c2 = 0;
        if (i < p1.coefficients.size()) c1 = p1.coefficients[i];
        if (i < p2.coefficients.size()) c2 = p2.coefficients[i];
        res.coefficients[i] = c1 + c2;
    }
    return res;
}

// Вычитание полиномов (p1 - p2).
Polynomial polySub(const Polynomial &p1, const Polynomial &p2) {
    Polynomial res;
    size_t n = std::max(p1.coefficients.size(), p2.coefficients.size());
    res.coefficients.resize(n, 0LL);
    for (size_t i = 0; i < n; i++) {
        long long c1 = 0, c2 = 0;
        if (i < p1.coefficients.size()) c1 = p1.coefficients[i];
        if (i < p2.coefficients.size()) c2 = p2.coefficients[i];
        res.coefficients[i] = c1 - c2;
    }
    return res;
}

// Умножение полинома p на константу k.
Polynomial polyMulConst(const Polynomial &p, long long k) {
    Polynomial res;
    res.coefficients.resize(p.coefficients.size());
    for (size_t i = 0; i < p.coefficients.size(); i++) {
        // Осторожно с переполнением, в демо-версии считаем, что не вылезем
        res.coefficients[i] = p.coefficients[i] * k;
    }
    return res;
}

// Умножение полинома p на x (то есть "сдвиг степеней": x^i -> x^(i+1)).
Polynomial polyMulX(const Polynomial &p) {
    Polynomial res;
    res.coefficients.resize(p.coefficients.size() + 1);
    // сдвигаем на 1
    for (size_t i = 0; i < p.coefficients.size(); i++) {
        res.coefficients[i+1] = p.coefficients[i];
    }
    res.coefficients[0] = 0; // хотя по умолчанию и так 0
    return res;
}

// Вычисление значения полинома p(1/b) как дроби (num/den).
// Возвращаем (num, den) несокращёнными.
std::pair<long long, long long> evaluateAtOneOverB(const Polynomial &p, long long b) {
    // P(1/b) = c0 + c1*(1/b) + c2*(1/b^2) + ...
    // Считаем всё через дроби. Будем накапливать суммарную дробь.
    // Для удобства храним текущую сумму тоже как (num, den).
    long long num = 0;  // числитель
    long long den = 1;  // знаменатель
    // Прибавляем каждый член ci * (1/b)^i
    //   (1/b)^i = 1 / b^i
    //   => term = ci / b^i
    // Сложение дробей: (n1/d1) + (n2/d2) = (n1*d2 + n2*d1)/(d1*d2)
    // Тут n2 = ci, d2 = b^i.
    for (size_t i = 0; i < p.coefficients.size(); i++) {
        long long ci = p.coefficients[i];
        if (ci == 0) continue;
        
        // term = ci / (b^i)
        // Складываем term с (num/den).
        // num/den + ci/(b^i) = (num * b^i + ci * den) / (den * b^i)
        
        // Может быть b^i довольно большим, но b<=10 и i<=10 => b^i <= 10^10,
        // теоретически влезает в 64-бит (10^10 < 2^31). С запасом помещается.
        long long powerBi = 1;
        for (size_t j = 0; j < i; j++) {
            powerBi *= b;
        }
        
        long long newNum = num * powerBi + ci * den;
        long long newDen = den * powerBi;
        // Сохраняем (newNum, newDen) как текущую сумму (пока не сокращаем, сократим в конце).
        num = newNum;
        den = newDen;
    }
    // возвращаем (num, den)
    return {num, den};
}

int main() {
    // Читаем a и b
    int a, b;
    std::cin >> a >> b;

    // 1) Если b = 1 -> выводим infinity
    if (b == 1) {
        std::cout << "infinity\n";
        return 0;
    }

    // 2) Строим полином P_a(x) для S(a, x) = P_a(x) / (1-x)^(a+1)
    //    Рекуррентно:
    //    P_0(x) = x
    //    P_a(x) = x * [ (1 - x)*P_{a-1}'(x) + a * P_{a-1}(x) ].
    
    // Начинаем с P_0(x)
    Polynomial Pprev;
    Pprev.coefficients = {0, 1}; // это 0 + 1*x = x

    // последовательно строим P_k(x) для k=1..a
    for (int k = 1; k <= a; k++) {
        // derivative
        Polynomial dPprev = derivative(Pprev);
        // (1 - x)*P_{a-1}'(x) = P_{a-1}'(x) - x*P_{a-1}'(x)
        // Считаем отдельно:
        Polynomial part1 = dPprev;           // P_{a-1}'(x)
        Polynomial part2 = polyMulX(dPprev); // x * P_{a-1}'(x)
        Polynomial part1_sub = polySub(part1, part2); // (1 - x)*P'_{a-1}(x)
        // a * P_{a-1}(x)
        Polynomial part3 = polyMulConst(Pprev, k);
        // суммируем: (1 - x)*P'_{a-1}(x) + a P_{a-1}(x)
        Polynomial inside = polyAdd(part1_sub, part3);
        // умножаем всё на x (т.е. сдвиг полинома)
        Polynomial Pcur = polyMulX(inside);

        // Pcur -- это наш P_k(x)
        Pprev = Pcur; // дальше пойдёт как P_{k-1} на следующем шаге
    }

    // P_a(x) находится в Pprev
    Polynomial P_a = Pprev;

    // 3) S(a, x) = P_a(x) / (1-x)^(a+1).
    //   При x=1/b -> знаменатель = (1 - 1/b)^(a+1) = ((b-1)/b)^(a+1).
    //   = (b-1)^(a+1) / b^(a+1).
    //   Итого:
    //   S(a, 1/b) = P_a(1/b) / ((b-1)/b)^(a+1)
    //              = P_a(1/b) * ( b^(a+1) / (b-1)^(a+1) ).

    // Вычисляем P_a(1/b) = num/den
    auto fracP = evaluateAtOneOverB(P_a, b); 
    long long numP = fracP.first;   // числитель
    long long denP = fracP.second;  // знаменатель

    // Умножаем на b^(a+1) / (b-1)^(a+1).
    // Запишем это двумя этапами:
    //  numFinal / denFinal = (numP / denP) * (b^(a+1) / (b-1)^(a+1)).
    //  => numFinal = numP * b^(a+1)
    //     denFinal = denP * (b-1)^(a+1)

    // Считаем b^(a+1) и (b-1)^(a+1)
    long long powB = 1, powBm1 = 1;
    for (int i = 0; i < a+1; i++) {
        powB   *= b;
        powBm1 *= (b - 1);
    }

    long long numFinal = numP * powB;
    long long denFinal = denP * powBm1;

    // Сократим дробь (numFinal / denFinal)
    long long g = gcd_ll(numFinal, denFinal);
    numFinal /= g;
    denFinal /= g;

    // Если знаменатель отрицательный, перенесём знак в числитель
    if (denFinal < 0) {
        denFinal = -denFinal;
        numFinal = -numFinal;
    }

    // Выводим ответ: p/q
    std::cout << numFinal << "/" << denFinal << "\n";

    return 0;
}