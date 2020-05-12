#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#include <cstddef>
#include <gmp.h>
#include <iosfwd>
#include <functional>

struct big_integer
{
    big_integer();
    big_integer(big_integer const& other);
    big_integer(int a);
    explicit big_integer(std::string const& str);
    ~big_integer();

    big_integer& operator=(big_integer const& other);

    big_integer& operator+=(big_integer const& rhs);
    big_integer& operator-=(big_integer const& rhs);
    big_integer& operator*=(big_integer const& rhs);
    big_integer& operator/=(big_integer const& rhs);
    big_integer& operator%=(big_integer const& rhs);

    big_integer& operator&=(big_integer const& rhs);
    big_integer& operator|=(big_integer const& rhs);
    big_integer& operator^=(big_integer const& rhs);

    big_integer& operator<<=(int rhs);
    big_integer& operator>>=(int rhs);

    big_integer operator+() const;
    big_integer operator-() const;
    big_integer operator~() const;

//    big_integer& operator++();
//    big_integer operator++(int);
//
//    big_integer& operator--();
//    big_integer operator--(int);

    friend bool operator==(big_integer const& a, big_integer const& b);
    friend bool operator!=(big_integer const& a, big_integer const& b);
    friend bool operator<(big_integer const& a, big_integer const& b);
    friend bool operator>(big_integer const& a, big_integer const& b);
    friend bool operator<=(big_integer const& a, big_integer const& b);
    friend bool operator>=(big_integer const& a, big_integer const& b);

    friend std::string to_string(big_integer const& a);
    void print_num_arr();
    friend bool bigger_by_mod(big_integer a, big_integer b);

    friend big_integer operator+(big_integer const& a, big_integer const& b);
    friend big_integer operator-(big_integer const& a, big_integer const& b);
    friend big_integer operator*(big_integer const& a, big_integer const& b);
    friend big_integer operator/(big_integer const& a, big_integer const& b);
    friend big_integer operator%(big_integer const& a, big_integer const& b);

    friend big_integer operator&(big_integer const& a, big_integer const& b);
    friend big_integer operator|(big_integer const& a, big_integer const& b);
    friend big_integer operator^(big_integer const& a, big_integer const& b);

    friend big_integer operator<<(big_integer const& a, int b);
    friend big_integer operator>>(big_integer const& a, int b);

private:
    uint32_t* num_arr;
    size_t size;

    friend big_integer binary_func(big_integer a, big_integer b, std::function<uint32_t(uint32_t, uint32_t)> func);

    bool sign;
    bool isZeroes(big_integer const& other) const;
    big_integer resized(size_t new_size) const;
    big_integer strip();
    void copy_num_arr(uint32_t* arr, size_t size);
    big_integer get_pref_not_leading(size_t from ,size_t pref_length) const;
    big_integer add_big_integer(big_integer const& val) const;
    big_integer sub_big_integer(big_integer const& val) const;
    big_integer div_int(uint32_t divisor) const;
    uint32_t mod_int(uint32_t divisor) const;
    big_integer mul_int(uint32_t val) const;
    big_integer mul_big_integer(big_integer const& val) const;
    big_integer div_big_integer(big_integer const& val) const;
    big_integer get_pref(size_t from ,size_t pref_length) const;
    big_integer div_big_integer_slow(big_integer const& val) const;
    big_integer signed_binary() const;
    big_integer unsigned_binary() const;
    big_integer negated() const;
    big_integer abs() const;

};



bool operator==(big_integer const& a, big_integer const& b);
bool operator!=(big_integer const& a, big_integer const& b);
bool operator<(big_integer const& a, big_integer const& b);
bool operator>(big_integer const& a, big_integer const& b);
bool operator<=(big_integer const& a, big_integer const& b);
bool operator>=(big_integer const& a, big_integer const& b);

std::string to_string(big_integer const& a);
std::ostream& operator<<(std::ostream& s, big_integer const& a);

#endif // BIG_INTEGER_H
