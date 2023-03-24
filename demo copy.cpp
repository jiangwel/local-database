#include <bits/stdc++.h>
using namespace std;

class Big {
  public:
  Big() {
    shared_ptr<Point> p1 = make_shared<Point>();
    dir_.push_back(p1);
  }
  class Point {
    vector<int> list_;
    public:
    Point() {
      list_.push_back(0);
      list_.push_back(1);
    }
    vector<int>& Get() {
      return list_;
    }
  };

  void RedistributeBucket(shared_ptr<Point> a) {
    a->Get().pop_back();
    cout << a->Get().size() << endl;
  }
  
  void Insert(){
    shared_ptr<Point> point;
    point = dir_[0];
    RedistributeBucket(point);
    cout << point->Get().size() << endl;
  }
  private:
  vector<shared_ptr<Point>> dir_;

};
int main() {
  double a = 1.00001;
  //打印a的整数部分 
  

  printf("%f", a);
  
  printf("%.f", a);
}