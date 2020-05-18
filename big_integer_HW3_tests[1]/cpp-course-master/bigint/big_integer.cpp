#include <iostream>
#include <algorithm>
#include <sstream>
#include <assert.h>
#include "big_integer.h"

big_integer binary_func(big_integer a, big_integer b, std::function<uint32_t(uint32_t, uint32_t)> func) {
    a = a.signed_binary();
    b = b.signed_binary();
    if (a.num_arr.size_ < b.num_arr.size_) {
        big_integer c = a;
        a = b;
        b = c;
    }
    size_t b_size = b.num_arr.size_;
    b = b.resized(a.num_arr.size_);
    uint32_t val;
    if ((b.num_arr[b_size - 1] >> 31) == 0) {
        val = 0;
    } else {
        val = UINT32_MAX;
    }
    for (size_t i = b_size; i < b.num_arr.size_; i++) {
        b.num_arr.set(i, val);
    }
    big_integer c(a);
    for (size_t i = 0; i < a.num_arr.size_; i++) {
        c.num_arr.set(i, func(a.num_arr[i], b.num_arr[i]));
    }
    return c.unsigned_binary();
}

big_integer big_integer::resized(size_t new_size) const {
    array_ new_arr(new_size);
    if (new_size <= num_arr.size_) {
        for (size_t i = 0; i < new_size; i++) {
            new_arr[i] = num_arr[i];
        }
    } else {
        for (size_t i = 0; i < num_arr.size_; i++) {
            new_arr[i] = num_arr[i];
        }
        for (size_t i = num_arr.size_; i < new_size; i++) {
            new_arr[i] = 0;
        }
    }
    big_integer ans(0);
    ans.num_arr = new_arr;
    ans.sign = sign;
    return ans;
}

big_integer big_integer::strip() {
    size_t new_size = num_arr.size_;
    while (new_size != 1 && num_arr[new_size - 1] == 0) {
        new_size--;
    }
    if (num_arr.size_ > 4 && new_size > 4) {
        num_arr.size_ = new_size;
        return *this;
    } else {
        return resized(new_size);
    }
}

/// modify bigint
//void big_integer::copy_num_arr(uint32_t* arr, size_t size) {
//    //std::cout << (num_arr != nullptr) << ' '; .set(i,
//    num_arr = new uint32_t[size];
//    for (size_t i = 0; i < size; i++) {
//        num_arr[i] = arr[i];
//    }
//    this->size = size;
//}



bool bigger_by_mod(big_integer const& a, big_integer const& b) {
    if (a.num_arr.size_ < b.num_arr.size_) {
        return true;
    } else if (a.num_arr.size_ > b.num_arr.size_) {
        return false;
    } else {
        for (size_t i = a.num_arr.size_ - 1; i != 0; i--) {
            if (a.num_arr[i] != b.num_arr[i]) {
                return a.num_arr[i] <= b.num_arr[i];
            }
        }
        return a.num_arr[0] <= b.num_arr[0];
    }
}

big_integer big_integer::add_big_integer(big_integer const& val) const {
    if (this->sign != val.sign) {
        return sub_big_integer(val.negated());
    }
    big_integer a;
    big_integer b;
    if (this->num_arr.size_ < val.num_arr.size_) {
        a = val;
        b = *this;
    } else {
        a = *this;
        b = val;
    }

    b = b.resized(a.num_arr.size_);

    /// a & b have the same num_arr.size_
    big_integer ans;
    ans.unique_eq(a);
    uint32_t remainder = 0;
    for (size_t i = 0; i < a.num_arr.size_; i++) {
        ans.num_arr[i] = (a.num_arr[i] + b.num_arr[i] + remainder);
        remainder = (static_cast<uint64_t>(a.num_arr[i]) + b.num_arr[i] + remainder) >> 32;
    }
    if (remainder != 0) {
        ans = ans.resized(ans.num_arr.size_ + 1);
        ans.num_arr[ans.num_arr.size_ - 1] = remainder;
    }
    return ans;
}

big_integer big_integer::sub_big_integer(big_integer const& val) const {
    if (sign != val.sign) {
        return add_big_integer(val.negated());
    }
    big_integer a(0);
    big_integer b(0);
    if (bigger_by_mod(*this, val)) {
        a = val.negated();
        b = this->negated();
    } else {
        a = *this;
        b = val;
    }

    b = b.resized(a.num_arr.size_);

    /// a & b have the same num_arr.size_
    big_integer ans;
    ans.unique_eq(a);
    uint32_t remainder = 0;
    for (size_t i = 0; i < a.num_arr.size_; i++) {
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
    ans.num_arr.make_unique();
    uint64_t remainder = 0;
    for (size_t i = num_arr.size_ - 1; true; i--) {
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
    for (size_t i = num_arr.size_ - 1; true; i--) {
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
    big_integer ans;
    ans.unique_eq(*this);
    uint64_t remainder = 0;
    for (size_t i = 0; i < num_arr.size_; i++) {
        uint64_t cur_num = (static_cast<uint64_t>(num_arr[i]) * val + remainder);
        ans.num_arr[i] = cur_num;
        remainder = (cur_num >> 32);
    }
    if (remainder != 0) {
        ans = ans.resized(ans.num_arr.size_ + 1);
        ans.num_arr[ans.num_arr.size_ - 1] = remainder;
        remainder >>= 32;
    }
    return ans;
}

big_integer big_integer::mul_big_integer(big_integer const& val) const {
    if (val.num_arr.size_ == 1) {
        big_integer ans(mul_int(val.num_arr[0]));
        ans.sign = sign ^ val.sign;
        return ans;
    }
    if (num_arr.size_ == 1) {
        big_integer ans(val.mul_int(num_arr[0]));
        ans.sign = sign ^ val.sign;
        return ans;
    }
    big_integer a;
    big_integer b;
    if (bigger_by_mod(*this, val)) {
        a = val.abs();
        b = this->abs();
    } else {
        a = this->abs();
        b = val.abs();
    }
    big_integer ans;
    ans = ans.resized(a.num_arr.size_ + b.num_arr.size_ + 1);
    for (size_t i = 0; i < b.num_arr.size_; i++) {
        uint32_t mult = b.num_arr[i];
        big_integer c = a.mul_int(mult);
        uint64_t remainder = 0;
        for (size_t j = 0; j < c.num_arr.size_; j++) {
            uint64_t new_val = remainder + ans.num_arr[i + j] + c.num_arr[j];
            remainder = new_val >> 32;
            ans.num_arr[i + j] = new_val;
        }
        for (size_t j = c.num_arr.size_; remainder != 0; j++) {
            uint64_t new_val = remainder + ans.num_arr[i + j];
            remainder = new_val >> 32;
            ans.num_arr[i + j] = new_val;
        }
    }
    ans.sign = this->sign != val.sign;
    return ans.strip();
}

big_integer big_integer::div_big_integer(big_integer const& val) const {
    if (this->num_arr.size_ < val.num_arr.size_) {
        return 0;
    }
    uint64_t f = (1ll << 32) / (static_cast<uint64_t >(val.num_arr[val.num_arr.size_ - 1]) + 1);
    big_integer r = this->mul_big_integer(f).abs();
    big_integer d = val.mul_big_integer(f).abs();
//    if (this->num_arr.size_ == val.num_arr.size_) {
//        return this->div_big_integer_slow(y);
//    }
    if (val.num_arr.size_ == 1) {
        big_integer ans = this->div_int(val.num_arr[0]);
        ans.sign = this->sign != val.sign;
        return ans;
    }
    big_integer q(0);
    size_t q_size = this->num_arr.size_ - val.num_arr.size_ + 2;
    q = q.resized(q_size);
    size_t n = r.num_arr.size_ + 1;
    r = r.resized(n);
    size_t m = d.num_arr.size_;

    for (size_t k = n - m - 1; ; k--) {
        /// if your compilator hasn't got __int128 please use gmp mpz_t, logics still the same,
        /// because r3, d2 not more than 2^96
        __int128 r3 = 0;
        __int128 d2 = 0;
        if (m + k >= 0) {
            r3 += r.num_arr[m + k];
        }
        r3 <<= 32;
        if (m + k - 1 >= 0) {
            r3 += r.num_arr[m + k - 1];
        }
        r3 <<= 32;
        if (m + k - 2 >= 0) {
            r3 += r.num_arr[m + k - 2];
        }
        if (m - 1 >= 0) {
            d2 += d.num_arr[m - 1];
        }
        d2 <<= 32;
        if (m - 2 >= 0) {
            d2 += d.num_arr[m - 2];
        }
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
        big_integer dq = d.mul_int(qt);
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


        big_integer r_sub;
        r_sub.unique_eq((r.get_pref_not_leading(m + k, m + 1).strip() - dq).resized(m + 1));

//        std::cout << "r_sub: ";
//        r_sub.print_num_arr();
//        std::cout << r_sub.num_arr.size_ << std::endl;
        for (size_t i = m + k - 1; ; i--) {
//            std::cout << i << ":i   i - k: " << i - k << " sizes: " << r.num_arr.size_ << ' ' << r_sub.num_arr.size_ <<  std::endl;
//            std::cout << r.num_arr[i] << ' ' << r_sub.num_arr[i - k] << " - r : r_sub" << std::endl;
            r.num_arr[i] = r_sub.num_arr[i - k];
            if (i == m + k - r_sub.num_arr.size_ || i - k == 0) {
                break;
            }
        }
        if (k == 0) {
            break;
        }
    }
    q.sign = this->sign != val.sign;
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
        if ((num_arr[num_arr.size_ - 1] >> 31) != 0) {
            ans = ans.resized(num_arr.size_ + 1);
        }
        return ans;
    }
    ans.sign = false;
    big_integer zero(0);
    if (ans == zero) {
        return -*this;
    }
    if ((num_arr[num_arr.size_ - 1] >> 31) != 0) {
        ans = ans.resized(num_arr.size_ + 1);
        ans.num_arr.set(num_arr.size_, UINT32_MAX);
    }
    for (size_t i = 0; i < num_arr.size_; i++) {
        ans.num_arr.set(i, ans.num_arr[i] ^ UINT32_MAX);
    }
    return ans + 1;
}
big_integer big_integer::unsigned_binary() const {
    assert(!sign);
    big_integer ans(*this);
    ans.sign = (num_arr[num_arr.size_ - 1] >> 31);

    if (ans.sign) {
        ans = ans + 1;
        for (size_t i = 0; i < num_arr.size_; i++) {
            ans.num_arr.set(i, ans.num_arr[i] ^ UINT32_MAX);
        }
    }
    return ans.strip();
}

big_integer::big_integer() : num_arr(1) {
    sign = false;
    num_arr[0] = 0;
    num_arr.size_ = 1;
}

void big_integer::unique_eq(big_integer const & other) {
    num_arr.equal(other.num_arr);
    sign = other.sign;
}

big_integer::big_integer(int val) : num_arr(1){
    if (val == INT32_MIN) {
        num_arr[0] = static_cast<uint32_t >(INT32_MAX) + 1;
        sign = true;
        num_arr.size_ = 1;
    } else {
        if (val >= 0) {
            sign = false;
        } else {
            sign = true;
            val = -val;
        }
        num_arr.size_ = 1;
        num_arr[0] = val;
    }
}

big_integer::big_integer(big_integer const& bi) : num_arr(bi.num_arr) {
    sign = bi.sign;
}

big_integer::big_integer(std::string const &str) : num_arr(1) {
    if (!check_string(str)) {
        throw std::runtime_error("bad string exception");
    }
    std::string str_number = str;
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
    num_arr = ans.num_arr;
}

bool big_integer::check_string(std::string const& str) {
    if (str.empty()) {
        return false;
    }
    size_t i = 0;
    if (str[i] == '-') {
        i++;
    }
    if (i == str.length()) {
        return false;
    }
    for (; i < str.length(); i++) {
        if (str[i] > '9' || str[i] < '0') {
            return false;
        }
    }
    return true;
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

        big_integer ans = a.get_pref(a.num_arr.size_ - 1, a.num_arr.size_ - discharge);
        return ans.div_int(static_cast<uint32_t > (1) << (shift % 32));
    } else {
        big_integer ans = a.div_int(static_cast<uint32_t > (1) << (shift % 32));
        ans = ans.signed_binary().get_pref(a.num_arr.size_ - 1, a.num_arr.size_ - discharge);
        size_t ans_size = ans.num_arr.size_;
        ans = ans.resized(a.num_arr.size_);
        for (size_t i = ans_size; i < a.num_arr.size_; i++) {
            ans.num_arr.set(i, UINT32_MAX);
        }
        return ans.unsigned_binary() - 1;
    }
}
big_integer& big_integer::operator>>=(int shift) {
    return *this = (*this >> shift);
}
big_integer operator<<(big_integer const& a, int shift) {
    size_t discharge = shift / 32;
    big_integer ans = a.get_pref(a.num_arr.size_ - 1, a.num_arr.size_ + discharge);
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
    num_arr = other.num_arr;
    sign = other.sign;
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
    for (size_t i = num_arr.size_ - 1; ; i--) {
        std::cout << num_arr[i] << ' ';
        if (i == 0) {
            break;
        }
    }
    std::cout << std::endl;
}

std::string to_string(big_integer const& a) {
    std::stringstream ss;
    for (big_integer b = a; b.num_arr.size_ != 1 || b.num_arr[0] != 0; b = b.div_int(10)) {
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

array_::array_(size_t size) {
    if (size > 4) {
        big.arr = new uint32_t[size];
        big.refs = new uint32_t(1);
    }
    size_ = size;
}

array_::array_(array_ const& other) {
    if (other.size_ > 4) {
        big.arr = other.big.arr;
        big.refs = other.big.refs;
        (*big.refs)++;
    } else {
        small.arr.equal(other.small.arr, other.size_);
    }
    size_ = other.size_;
}

array_& array_::operator=(array_ const& other) {
    if (size_ > 4) {
        if (--(*big.refs) == 0) {
            delete[] big.arr;
            delete big.refs;
        }
    }
    if (other.size_ > 4) {
        big.arr = other.big.arr;
        big.refs = other.big.refs;
        (*other.big.refs)++;
    } else {
        small.arr.equal(other.small.arr, other.size_);
    }
    size_ = other.size_;
    return *this;
}

array_::~array_() {
    if (size_ > 4) {
        if (--(*big.refs) == 0) {
            delete[] big.arr;
            delete big.refs;
        }
    }
}

uint32_t& array_::operator[](size_t i) {
    if (size_ <= 4) {
        return small.arr[i];
    }
    return big.arr[i];
}

uint32_t const& array_::operator[](size_t i) const {
    if (size_ <= 4) {
        return small.arr[i];
    }
    return big.arr[i];
}

void array_::set(size_t i, uint32_t val) {
    if (size_ <= 4) {
        small.arr[i] = val;
    } else {
        if (*big.refs != 1) {
//        std::cout << *refs << std::endl;
            (*big.refs)--;
            auto new_arr = new uint32_t[size_];

            for (size_t i = 0; i < size_; i++) {
                new_arr[i] = big.arr[i];
            }
            big.refs = new uint32_t(1);
            big.arr = new_arr;
        }
        big.arr[i] = val;
    }
}

void array_::make_unique() {
    if (size_ > 4 && *big.refs != 1) {
//        std::cout << *refs << std::endl;
        (*big.refs)--;
        auto new_arr = new uint32_t[size_];

        for (size_t i = 0; i < size_; i++) {
            new_arr[i] = big.arr[i];
        }
        big.refs = new uint32_t(1);
        big.arr = new_arr;
    }
}

void array_::equal(array_ const & other) {
    size_ = other.size_;
    if (size_ > 4) {
        big.arr = new uint32_t[other.size_];
        for (size_t i = 0; i < size_; i++) {
            big.arr[i] = other.big.arr[i];
        }
        big.refs = new uint32_t(1);
    } else {
        small.arr = other.small.arr;
    }
}


uint32_t& small_arr::operator[](size_t i) {
    if (i == 0) {
        return _0;
    } else if (i == 1) {
        return _1;
    } else if (i == 2) {
        return _2;
    }
    return _3;
}

uint32_t const& small_arr::operator[](size_t i) const {
    if (i == 0) {
        return _0;
    } else if (i == 1) {
        return _1;
    } else if (i == 2) {
        return _2;
    }
    return _3;
}
inline void small_arr::equal(small_arr const & other, size_t const&size) {
    switch(size) {
        case 4:
            _3 = other._3;
        case 3:
            _2 = other._2;
        case 2:
            _1 = other._1;
        case 1:
            _0 = other._0;
    }
}