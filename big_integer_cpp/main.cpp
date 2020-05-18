#include <iostream>
#include "big_integer.cpp"

using namespace std;

//void test (string a, string b) {
//    big_integer first(a);
//    big_integer second(b);
//    first.div_big_integer(second).print_num_arr();
//    first.div_big_integer_slow(second).print_num_arr();
//    if (first.div_big_integer(second) == first.div_big_integer_slow(second)) {
//    if (first + second == second + first) {
//        cout << "YES" << endl;
//        return;
//    }
//    cout << "NO: " << a << " / " << b << endl;
//}

int main() {
    big_integer a("-1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000");
    big_integer b( "1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000");

    cout << a + b;
//    a = b + c;
//    a = b + c;
//    a = b;
//    a = b;
//    for (int i = 0; i < 1000; i++) {
//        string a;
//        string b;
//        for (int j = 0; j < rand() % 1000 + 1; j++) {
//            a += ('1' + rand() % 9);
//        }
//        for (int j = 0; j < rand() % 500 + 1; j++) {
//            b += ('1' + rand() % 9);
//        }
//        test(a, b);
//    }
}