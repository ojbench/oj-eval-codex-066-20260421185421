#include "src.hpp"

#include <bits/stdc++.h>
using namespace std;
using namespace sjtu;

// For OJ, echo simple interactions to validate functionality.
int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  // Read a mode to choose behavior; default basic test
  int mode = 0;
  if (!(cin >> mode)) mode = 0;

  if (mode == 0) {
    any_ptr a = make_any_ptr(1);
    any_ptr b = a;
    a.unwrap<int>() = 42;
    cout << b.unwrap<int>() << "\n"; // should print 42
    b = new string;
    b.unwrap<string>() = string("hello");
    cout << b.unwrap<string>() << "\n"; // hello
    try {
      (void)a.unwrap<string>();
    } catch (const exception &e) {
      cout << "bad cast" << "\n";
    }
  } else if (mode == 1) {
    any_ptr v = make_any_ptr<vector<int>>(1, 2, 3);
    cout << v.unwrap<vector<int>>().size() << "\n";
  } else if (mode == 2) {
    any_ptr m = make_any_ptr<map<int,int>>({{1,2},{3,4}});
    cout << m.unwrap<map<int,int>>().size() << "\n";
  }
  return 0;
}

