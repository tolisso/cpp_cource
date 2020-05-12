#include <iostream>
#include <algorithm>
#include <sstream>
#include "big_integer.h"

big_integer binary_func(big_integer a, big_integer b, std::function<uint32_t(uint32_t, uint32_t)> func) {
    a = a.signed_binary();
    b = b.signed_binary();
    if (a.size < b.size) {
        big_integer c = a;
        a = b;
        b = c;
    }
    size_t b_size = b.size;
    b = b.resized(a.size);
    uint32_t val;
    if ((b.num_arr[b_size - 1] >> 31) == 0) {
        val = 0;
    } else {
        val = UINT32_MAX;
    }
    for (size_t i = b_size; i < b.size; i++) {
        b.num_arr[i] = val;
    }
    big_integer c(a);
    for (size_t i = 0; i < a.size; i++) {
        c.num_arr[i] = func(a.num_arr[i], b.num_arr[i]);
    }
    return c.unsigned_binary();
}

big_integer big_integer::resized(size_t new_size) const {
    auto* new_arr = new uint32_t[new_size];
    if (new_size <= size) {
        for (size_t i = 0; i < new_size; i++) {
            new_arr[i] = num_arr[i];
        }
    } else {
        for (size_t i = 0; i < size; i++) {
            new_arr[i] = num_arr[i];
        }
        for (size_t i = size; i < new_size; i++) {
            new_arr[i] = 0;
        }
    }
    big_integer ans(0);
    delete[] ans.num_arr;
    ans.size = new_size;
    ans.num_arr = new_arr;
    ans.sign = sign;
    return ans;
}

big_integer big_integer::strip() {
    size_t new_size = size;
    while (new_size != 1 && num_arr[new_size - 1] == 0) {
        new_size--;
    }
    return this->resized(new_size);
}

/// modify bigint
void big_integer::copy_num_arr(uint32_t* arr, size_t size) {
    //std::cout << (num_arr != nullptr) << ' ';
    delete[] num_arr;
    num_arr = new uint32_t[size];
    for (size_t i = 0; i < size; i++) {
        num_arr[i] = arr[i];
    }
    this->size = size;
}



bool bigger_by_mod(big_integer a, big_integer b) {
    if (a.size < b.size) {
        return true;
    } else if (a.size > b.size) {
        return false;
    } else {
        for (size_t i = a.size - 1; i != 0; i--) {
            if (a.num_arr[i] != b.num_arr[i]) {
                return a.num_arr[i] <= b.num_arr[i];
            }
        }
        return a.num_arr[0] <= b.num_arr[0];
    }
}

big_integer big_integer::add_big_integer(big_integer const& val) const {
    big_integer a(0);
    big_integer b(0);
    if (this->size < val.size) {
        a = val;
        b = *this;
    } else {
        a = *this;
        b = val;
    }
    if (a.sign != b.sign) {
        return a.sub_big_integer(b.negated());
    }
    b = b.resized(a.size);

    /// a & b have the same size
    auto ans = a;
    uint32_t remainder = 0;
    for (size_t i = 0; i < a.size; i++) {
        ans.num_arr[i] = (a.num_arr[i] + b.num_arr[i] + remainder);
        remainder = (static_cast<uint64_t>(a.num_arr[i]) + b.num_arr[i] + remainder) >> 32;
    }
    if (remainder != 0) {
        ans = ans.resized(ans.size + 1);
        ans.num_arr[ans.size - 1] = remainder;
    }
    return ans;
}

big_integer big_integer::sub_big_integer(big_integer const& val) const {
    big_integer a(0);
    big_integer b(0);
    if (bigger_by_mod(*this, val)) {
        a = val.negated();
        b = this->negated();
    } else {
        a = *this;
        b = val;
    }
    if (a.sign != b.sign) {
        big_integer negated_b = b.negated();
        return a.add_big_integer(negated_b);
    }
    b = b.resized(a.size);

    /// a & b have the same size
    auto ans = a;
    uint32_t remainder = 0;
    for (size_t i = 0; i < a.size; i++) {
        ans.num_arr[i] = a.num_arr[i] - b.num_arr[i] - remainder;
        if (a.num_arr[i] >= b.num_arr[i] + remainder) {
            remainder = 0;
        } else {
            remainder = 1;
        }
    }
    return ans.strip();
}

big_integer big_integer::div_int(uint32_t divisor) const {
    big_integer ans(*this);
    uint64_t remainder = 0;
    for (size_t i = size - 1; true; i--) {
        remainder <<= 32;
        remainder += num_arr[i];
        ans.num_arr[i] = remainder / divisor;
        remainder %= divisor;
        if (i == 0) {
            break;
        }
    }
    return ans.strip();
}

uint32_t big_integer::mod_int(uint32_t divisor) const {
    big_integer ans(*this);
    uint64_t remainder = 0;
    for (size_t i = size - 1; true; i--) {
        remainder <<= 32;
        remainder += num_arr[i];
        remainder %= divisor;
        if (i == 0) {
            break;
        }
    }
    return remainder;
}

big_integer big_integer::mul_int(uint32_t val) const {
    if (val == 0) {
        return 0;
    }
    big_integer ans(*this);
    uint64_t remainder = 0;
    for (size_t i = 0; i < size; i++) {
        uint64_t cur_num = (static_cast<uint64_t>(num_arr[i]) * val + remainder);
        ans.num_arr[i] = static_cast<uint32_t>(cur_num);
        remainder = (cur_num >> 32);
    }
    while (remainder != 0) {
        ans = ans.resized(ans.size + 1);
        ans.num_arr[ans.size - 1] = remainder;
        remainder >>= 32;
    }
    return ans;
}

big_integer big_integer::mul_big_integer(big_integer const& val) const {
    big_integer a = *this;
    big_integer b = val;
    if (a < 0) {
        a = a.negated();
    }
    if (b < 0) {
        b = b.negated();
    }
    if (bigger_by_mod(a, b)) {
        std::swap(a, b);
    }
    big_integer ans;
    ans = ans.resized(a.size + b.size + 1);
    for (size_t i = 0; i < b.size; i++) {
        uint32_t mult = b.num_arr[i];
        big_integer c = a.mul_int(mult);
        uint64_t remainder = 0;
        for (size_t j = 0; j < c.size; j++) {
            uint64_t new_val = remainder + ans.num_arr[i + j] + c.num_arr[j];
            remainder = new_val >> 32;
            ans.num_arr[i + j] = new_val;
        }
        for (size_t j = c.size; remainder != 0; j++) {
            uint64_t new_val = remainder + ans.num_arr[i + j];
            remainder = new_val >> 32;
            ans.num_arr[i + j] = new_val;
        }
    }
    ans.sign = this->sign != val.sign;
    return ans.strip();
}

big_integer big_integer::div_big_integer(big_integer const& val) const {
    big_integer x(*this);
    big_integer y(val);
    uint64_t f = (static_cast<uint64_t >(1) << 32) / (static_cast<uint64_t >(y.num_arr[y.size - 1]) + 1);
    big_integer r = x.mul_big_integer(f).abs();
    big_integer d = y.mul_big_integer(f).abs();
    if (r < 0) {
        r = r.negated();
    }
    if (d < 0) {
        d = d.negated();
    }
    if (x.size < y.size) {
        big_integer ans(0);
        return ans;
    }
//    if (x.size == y.size) {
//        return x.div_big_integer_slow(y);
//    }
    if (y.size == 1) {
        big_integer ans = x.div_int(y.num_arr[0]);
        ans.sign = x.sign != y.sign;
        return ans;
    }
    big_integer q(0);
    size_t q_size = x.size - y.size + 2;
    q = q.resized(q_size);
    size_t n = r.size + 1;
    r = r.resized(n);
    size_t m = d.size;

    for (size_t k = n - m - 1; ; k--) {
        big_integer r3_bi = r.get_pref_not_leading(m + k, 3).resized(3);
        big_integer d2_bi = d.get_pref_not_leading(m - 1, 2).resized(2);
        /// if your compilator hasn't got __int128 please use gmp mpz_t, logics still the same,
        /// because r3, d2 not more than 2^96
        __int128 r3 = 0;
        __int128 d2 = 0;
        r3 += r3_bi.num_arr[2];
        r3 <<= 32;
        r3 += r3_bi.num_arr[1];
        r3 <<= 32;
        r3 += r3_bi.num_arr[0];
        d2 += d2_bi.num_arr[1];
        d2 <<= 32;
        d2 += d2_bi.num_arr[0];
//        mpz_t r3;
//        mpz_t d2;
//        mpz_t result;
//        mpz_init(r3);
//        mpz_init(d2);
//        mpz_init(result);
//        mpz_add_ui(r3, r3, r3_bi.num_arr[2]);
//        mpz_mul_2exp(r3, r3, 32);
//        mpz_add_ui(r3, r3, r3_bi.num_arr[1]);
//        mpz_mul_2exp(r3, r3, 32);
//        mpz_add_ui(r3, r3, r3_bi.num_arr[0]);
//
//        mpz_add_ui(d2, d2, d2_bi.num_arr[1]);
//        mpz_mul_2exp(d2, d2, 32);
//        mpz_add_ui(d2, d2, d2_bi.num_arr[0]);
//        mpz_div(result, r3, d2);
//        uint32_t ans = mpz_get_ui(result);

        uint32_t qt = r3 / d2;
        big_integer dq = d.mul_int(qt).strip();
        if (dq > r.get_pref_not_leading(m + k, m + 1).strip()) {
            qt--;
            dq = d.mul_int(qt);
        }
//        std::cout << "qt, dq and r_pref" << std::endl;
//        std::cout << qt << std::endl;
        q.num_arr[k] = qt;
//        (dq).print_num_arr();
//        r.get_pref_not_leading(m + k, m + 1).print_num_arr();
//        std::cout << std::endl;


        big_integer r_sub = r.get_pref_not_leading(m + k, m + 1).strip() - dq;
        r_sub = r_sub.resized(m + 1);

        dq = dq.resized(m + 1);
//        std::cout << "r_sub: ";
//        r_sub.print_num_arr();
//        std::cout << r_sub.size << std::endl;
        for (size_t i = m + k - 1; ; i--) {
//            std::cout << i << ":i   i - k: " << i - k << " sizes: " << r.size << ' ' << r_sub.size <<  std::endl;
//            std::cout << r.num_arr[i] << ' ' << r_sub.num_arr[i - k] << " - r : r_sub" << std::endl;
            r.num_arr[i] = r_sub.num_arr[i - k];
            if (i == m + k - r_sub.size || i - k == 0) {
                break;
            }
        }
        if (k == 0) {
            break;
        }
    }
    q.sign = x.sign != y.sign;
    return q.strip();
}


big_integer big_integer::get_pref_not_leading(size_t from ,size_t pref_length) const {
    big_integer ans(0);
    size_t length = pref_length;
    if (length > from + 1) {
        length = from + 1;
    }
    ans = ans.resized(length);

    for (size_t i = 0; i < length; i++) {
        if (i > from) {
            break;
        }
        if (from >= i) {
            ans.num_arr[length - 1 - i] = num_arr[from - i];
        } else {
            ans.num_arr[length - 1 - i] = 0;
        }
    }
    return ans;
}

big_integer big_integer::get_pref(size_t from ,size_t pref_length) const {
    big_integer ans(0);
    ans = ans.resized(pref_length);

    for (size_t i = 0; i < pref_length; i++) {
        if (from >= i) {
            ans.num_arr[pref_length - 1 - i] = num_arr[from - i];
        } else {
            ans.num_arr[pref_length - 1 - i] = 0;
        }
    }
    return ans;
}

big_integer big_integer::div_big_integer_slow(big_integer const& val) const {
    big_integer l_val(0);
    big_integer r_val = val;
    big_integer l(0);
    big_integer r(1);
    while (bigger_by_mod(r_val, *this)) {
        r = r.mul_int(10);
        r_val = r_val.mul_int(10);
    }
    big_integer m = 0;
    big_integer m_val = 0;
    while (!(r == l + 1)) {
        m = (l + r).div_int(2);
        if ((l + r).mod_int(2)) {
            m_val = (l_val + r_val - val).div_int(2);
        } else {
            m_val = (l_val + r_val).div_int(2);
        }
        if (bigger_by_mod(m_val, *this)) {
            l = m;
            l_val = m_val;
        } else {
            r = m;
            r_val = m_val;
        }
    }
    l.sign = sign != val.sign;
    return l;
}

big_integer big_integer::signed_binary() const {
    big_integer ans(*this);
    if (!sign) {
        if ((num_arr[size - 1] >> 31) != 0) {
            ans = ans.resized(size + 1);
        }
        return ans;
    }
    ans.sign = false;
    big_integer zero(0);
    if (ans == zero) {
        return -*this;
    }
    if ((num_arr[size - 1] >> 31) != 0) {
        ans = ans.resized(size + 1);
        ans.num_arr[size] = UINT32_MAX;
    }
    for (size_t i = 0; i < size; i++) {
        ans.num_arr[i] ^= UINT32_MAX;
    }
    return ans + 1;
}
big_integer big_integer::unsigned_binary() const {
    if (sign) {
        throw "no sign expected";
    }
    big_integer ans(*this);
    ans.sign = (num_arr[size - 1] >> 31);

    if (ans.sign) {
        ans = ans + 1;
        for (size_t i = 0; i < size; i++) {
            ans.num_arr[i] ^= UINT32_MAX;
        }
    }
    return ans.strip();
}

big_integer::~big_integer() {
    delete[] num_arr;
}
big_integer::big_integer() {
    sign = false;
    num_arr = new uint32_t[1];
    num_arr[0] = 0;
    size = 1;
}
big_integer::big_integer(int val) {
    if (val == INT32_MIN) {
        sign = true;
        num_arr = new uint32_t[1];
        size = 1;
        num_arr[0] = static_cast<uint32_t >(INT32_MAX) + 1;
    } else {
        if (val >= 0) {
            sign = false;
        } else {
            sign = true;
            val = -val;
        }
        num_arr = new uint32_t[1];
        size = 1;
        num_arr[0] = val;
    }
}
big_integer::big_integer(big_integer const& bi) {
    if (this != &bi) {
        num_arr = new uint32_t[bi.size];
        for (size_t i = 0; i < bi.size; i++) {
            num_arr[i] = bi.num_arr[i];
        }
        size = bi.size;
        sign = bi.sign;
    }
}
big_integer::big_integer(std::string const &str) {
    num_arr = new uint32_t[1];
    std::string str_number = str;
    if (str_number.empty() && str_number == "-") {
        throw "empty string exception";
    }
    if (str_number[0] == '-') {
        str_number = str_number.substr(1, str_number.length() - 1);
        sign = true;
    } else {
        sign = false;
    }
    big_integer ans(0);
    for (size_t i = 0; i != str_number.length(); i++) {
        big_integer number = big_integer(str_number[i] - '0');
        ans = ans.mul_int(10).add_big_integer(number);
    }
    copy_num_arr(ans.num_arr, ans.size);
}
big_integer operator+(big_integer const& a, big_integer const& b) {
    return b.add_big_integer(a);
}
big_integer& big_integer::operator+=(big_integer const& other) {
    return *this = *this + other;
}
big_integer operator-(big_integer const& a, big_integer const& b) {
    return a.sub_big_integer(b);
}
big_integer& big_integer::operator-=(big_integer const& other) {
    return *this = *this - other;
}
big_integer operator*(big_integer const& a, big_integer const& b) {
    return a.mul_big_integer(b);
}
big_integer& big_integer::operator*=(big_integer const& other) {
    return *this = *this * other;
}
big_integer operator/(big_integer const& a, big_integer const& b) {
    return a.div_big_integer(b);
}
big_integer& big_integer::operator/=(big_integer const& other) {
    return *this = *this / other;
}
big_integer operator%(big_integer const& a, big_integer const& b) {
    big_integer c = a;
    big_integer d = b;
    if (d < 0) {
        d = d.negated();
    }
    big_integer mod = d * c.div_big_integer(d);
    return c - mod;
}
big_integer& big_integer::operator%=(big_integer const& other) {
    return *this = *this % other;
}
big_integer operator&(big_integer const& a, big_integer const& b) {
    return binary_func(a, b, [](uint32_t a, uint32_t b)->uint32_t {return a & b;});
}
big_integer& big_integer::operator&=(big_integer const& other) {
    return *this = *this & other;
}
big_integer operator|(big_integer const& a, big_integer const& b) {
    return binary_func(a, b, [](uint32_t a, uint32_t b)->uint32_t {return a | b;});
}
big_integer& big_integer::operator|=(big_integer const& other) {
    return *this = *this | other;
}
big_integer operator^(big_integer const& a, big_integer const& b) {
    return binary_func(a, b, [](uint32_t a, uint32_t b)->uint32_t {return a ^ b;});
}
big_integer& big_integer::operator^=(big_integer const& other) {
    return *this = *this ^ other;
}
big_integer big_integer::operator~() const {
    return binary_func(*this, 0, [](uint32_t a, uint32_t b)->uint32_t {return ~a;});
}
big_integer operator>>(big_integer const& a, int shift) {
    size_t discharge = shift / 32;
    if (a >= 0) {

        big_integer ans = a.get_pref(a.size - 1, a.size - discharge);
        return ans.div_int(static_cast<uint32_t > (1) << (shift % 32));
    } else {
        big_integer ans = a.div_int(static_cast<uint32_t > (1) << (shift % 32));
        ans = ans.signed_binary().get_pref(a.size - 1, a.size - discharge);
        size_t ans_size = ans.size;
        ans = ans.resized(a.size);
        for (size_t i = ans_size; i < a.size; i++) {
            ans.num_arr[i] = UINT32_MAX;
        }
        return ans.unsigned_binary() - 1;
    }
}
big_integer& big_integer::operator>>=(int shift) {
    return *this = (*this >> shift);
}
big_integer operator<<(big_integer const& a, int shift) {
    size_t discharge = shift / 32;
    big_integer ans = a.get_pref(a.size - 1, a.size + discharge);
    ans.sign = a.sign;
    return ans.mul_int(static_cast<uint32_t > (1) << (shift % 32));
}
big_integer& big_integer::operator<<=(int shift) {
    return *this = (*this << shift);
}
bool big_integer::isZeroes(big_integer const& other) const {
    return bigger_by_mod(*this, other) && bigger_by_mod(other, *this) &&
           bigger_by_mod(0, other) && bigger_by_mod(other, 0);
}
bool operator==(big_integer const& a, big_integer const& b) {
    if (a.isZeroes(b)) {
        return true;
    }
    return bigger_by_mod(a, b) && bigger_by_mod(b, a) && a.sign == b.sign;
}
bool operator!=(big_integer const& a, big_integer const& b) {
    return !(a == b); 
}
bool operator<=(big_integer const& a, big_integer const& b) {
    if (a.isZeroes(b)) {
        return true;
    }
    if (a.sign == true && b.sign == false) {
        return true;
    } else if (a.sign == false && b.sign == true) {
        return false;
    } else if (!a.sign) {
        return bigger_by_mod(a, b);
    } else if (a.sign) {
        return bigger_by_mod(b, a);
    }
}
bool operator>(big_integer const& a, big_integer const& b) {
    return ! (a <= b);
}
bool operator>=(big_integer const& a, big_integer const& b) {
    return (a > b) || (a == b);
}
bool operator<(big_integer const& a, big_integer const& b) {
    return !(a >= b);
}
big_integer big_integer::operator+() const{
    return *this;
}
big_integer big_integer::operator-() const {
    return this->negated();
}
big_integer& big_integer::operator=(big_integer const& other) {
//        std::cout << "=" << std::endl;
    if (this != &other) {
        copy_num_arr(other.num_arr, other.size);
        sign = other.sign;
    }
    return *this;
}
big_integer big_integer::negated() const {
    big_integer ans(*this);
    ans.sign ^= 1;
    return ans;
}
void big_integer::print_num_arr() {
    if (sign) {
        std::cout << '-';
    }
    for (size_t i = size - 1; ; i--) {
        std::cout << num_arr[i] << ' ';
        if (i == 0) {
            break;
        }
    }
    std::cout << std::endl;
}

std::string to_string(big_integer const& a) {
    std::stringstream ss;
    for (big_integer b = a; b.size != 1 || b.num_arr[0] != 0; b = b.div_int(10)) {
        ss << b.mod_int(10);
    }
    if (a.sign && a != 0) {
        ss << '-';
    } else if (a == 0) {
        ss << 0;
    }
    std::string ans;
    ss >> ans;
    std::reverse(ans.begin(), ans.end());
    return ans;
}

big_integer big_integer::abs() const {
    return this->sign ? negated() : big_integer(*this);
}

std::ostream& operator<<(std::ostream& s, big_integer const& a) {
    return s << to_string(a);
}