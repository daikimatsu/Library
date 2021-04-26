#include <vector>
template<typename T>
class Kitamasa {
    int k;
    std::vector<T> a, d;
public:
    Kitamasa(std::vector<T> _a, std::vector<T> _d, int _k) :a(_a), d(_d), k(_k) {}
private:
    std::vector<T> Calc1(std::vector<T> x) {
        std::vector<T> res(k);
        for (int i = 0; i < k; ++i) {
            res[i] = x[k - 1] * d[i] + (i > 0 ? x[i - 1] : 0);
        }
        return res;
    }
    std::vector<T> Calc2(std::vector<T> x) {
        std::vector<T> res(k), y = x;
        for (int i = 0; i < k; ++i) {
            for (int j = 0; j < k; ++j) {
                res[j] += y[j] * x[i];
            }
            y = Calc1(y);
        }
        return res;
    }
public:
    T Solve(long long n) {
        if (n < k) {
            return a[n];
        }
        std::vector<T> f = Solve2(n);
        T res = 0;
        for (int i = 0; i < k; ++i) {
            res += f[i] * a[i];
        }
        return res;
    }
private:
    std::vector<T> Solve2(long long n) {
        if (n == k) {
            return d;
        } else if (n % 2 == 0 && n / 2 >= k) {
            return Calc2(Solve2(n / 2));
        } else {
            return Calc1(Solve2(n - 1));
        }
    }
};

/*
 * @see
 */