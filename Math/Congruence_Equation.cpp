#include <vector>
#include <cassert>
#include <cmath>
#include <functional>

// @param m `1 <= m`
// @return x mod m
constexpr long long safe_mod(long long x, long long m) {
    x %= m;
    if (x < 0) x += m;
    return x;
}

// @param b `1 <= b`
// @return pair(g, x) s.t. g = gcd(a, b), xa = g (mod b), 0 <= x < b/g
constexpr std::pair<long long, long long> inv_gcd(long long a, long long b) {
    a = safe_mod(a, b);
    if (a == 0) return {b, 0};

    // Contracts:
    // [1] s - m0 * a = 0 (mod b)
    // [2] t - m1 * a = 0 (mod b)
    // [3] s * |m1| + t * |m0| <= b
    long long s = b, t = a;
    long long m0 = 0, m1 = 1;

    while (t) {
        long long u = s / t;
        s -= t * u;
        m0 -= m1 * u;  // |m1 * u| <= |m1| * s <= b

        // [3]:
        // (s - t * u) * |m1| + t * |m0 - m1 * u|
        // <= s * |m1| - t * u * |m1| + t * (|m0| + |m1| * u)
        // = s * |m1| + t * |m0| <= b

        auto tmp = s;
        s = t;
        t = tmp;
        tmp = m0;
        m0 = m1;
        m1 = tmp;
    }
    // by [3]: |m0| <= b/g
    // by g != b: |m0| < b/g
    if (m0 < 0) m0 += b / s;
    return {s, m0};
}

constexpr long long solve_linear_congruence_equation(long long a, long long b, long long p) {
    a = safe_mod(a, p);
    b = safe_mod(b, p);
    std::pair<long long, long long> inv = inv_gcd(a, p);
    return safe_mod(inv.second * b, p);
}

long long substitution(std::vector<long long>& a, long long x) {
    long long result = 0;
    long long y = 1;
    for (long long& aa : a) {
        result += aa * y;
        y *= x;
    }
    return result;
}

std::vector<long long> differentiate(std::vector<long long>& a) {
    std::vector<long long> b;
    for (int i = 1; i < (int) a.size(); ++i) {
        b.emplace_back(i * a[i]);
    }
    return b;
}

std::vector<long long> solve_congruence_equation_with_prime_power(std::vector<long long>& a, long long p, long long n) {
    std::vector<long long> solutions;
    if (n == 1) {
        for (int i = 0; i < p; ++i) {
            if (substitution(a, i) % p == 0) {
                solutions.emplace_back(i);
            }
        }
        return solutions;
    }
    std::vector<long long> b = differentiate(a);
    std::vector<long long> pre_solutions = solve_congruence_equation_with_prime_power(a, p, n - 1);
    long long p1 = std::pow(p, n - 1);
    for (long long x0 : pre_solutions) {
        long long f_x0 = substitution(a, x0);
        long long c = f_x0 / p1;
        long long d = substitution(b, x0);
        long long y0 = solve_linear_congruence_equation(d, -c, p);
        solutions.emplace_back(safe_mod(x0 + p1 * y0, p1 * p));
    }
    return solutions;
}

// (rem, mod)
std::pair<long long, long long> crt(std::vector<long long>& r, std::vector<long long>& m) {
    assert(r.size() == m.size());
    int n = int(r.size());
    // Contracts: 0 <= r0 < m0
    long long r0 = 0, m0 = 1;
    for (int i = 0; i < n; i++) {
        assert(1 <= m[i]);
        long long r1 = safe_mod(r[i], m[i]), m1 = m[i];
        if (m0 < m1) {
            std::swap(r0, r1);
            std::swap(m0, m1);
        }
        if (m0 % m1 == 0) {
            if (r0 % m1 != r1) return {0, 0};
            continue;
        }
        // assume: m0 > m1, lcm(m0, m1) >= 2 * max(m0, m1)

        // (r0, m0), (r1, m1) -> (r2, m2 = lcm(m0, m1));
        // r2 % m0 = r0
        // r2 % m1 = r1
        // -> (r0 + x*m0) % m1 = r1
        // -> x*u0*g % (u1*g) = (r1 - r0) (u0*g = m0, u1*g = m1)
        // -> x = (r1 - r0) / g * inv(u0) (mod u1)

        // im = inv(u0) (mod u1) (0 <= im < u1)
        long long g, im;
        std::tie(g, im) = inv_gcd(m0, m1);

        long long u1 = (m1 / g);
        // |r1 - r0| < (m0 + m1) <= lcm(m0, m1)
        if ((r1 - r0) % g) return {0, 0};

        // u1 * u1 <= m1 * m1 / g / g <= m0 * m1 / g = lcm(m0, m1)
        long long x = (r1 - r0) / g % u1 * im % u1;

        // |r0| + |m0 * x|
        // < m0 + m0 * (u1 - 1)
        // = m0 + m0 * m1 / g - m0
        // = lcm(m0, m1)
        r0 += x * m0;
        m0 *= u1;  // -> lcm(m0, m1)
        if (r0 < 0) r0 += m0;
    }
    return {r0, m0};
}

std::vector<long long> solve(std::vector<long long>& a, long long m) {
    std::vector<std::vector<long long>> v;
    std::vector<long long> md;
    long long _m = m;
    for (int i = 2; i * i <= _m; ++i) {
        if (_m % i == 0) {
            int cnt = 0;
            long long pw = 1;
            while (_m % i == 0) {
                pw *= i;
                _m /= i;
                ++cnt;
            }
            v.emplace_back(solve_congruence_equation_with_prime_power(a, i, cnt));
            md.emplace_back(pw);
        }
    }
    if (_m != 1) {
        v.emplace_back(solve_congruence_equation_with_prime_power(a, _m, 1));
        md.emplace_back(_m);
    }
    int d = (int) v.size();
    std::vector<long long> r;
    std::vector<long long> solutions;

    std::function<void(int)> dfs = [&](int depth) {
        if (depth == d) {
            solutions.emplace_back(crt(r, md).first);
            return;
        }
        for (long long vv : v[depth]) {
            r.emplace_back(vv);
            dfs(depth + 1);
            r.pop_back();
        }
    };

    dfs(0);
    return solutions;
}