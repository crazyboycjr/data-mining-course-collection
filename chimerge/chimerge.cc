#include <algorithm>
#include <array>
#include <cstdio>
#include <cstring>
#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <numeric>
#include <iostream>

const int kMaxInterval = 6;

class DataItem {
 public:
  std::array<double, 4> attr_;
  std::string class_name_;

  DataItem() = default;
  ~DataItem() = default;

  DataItem(const DataItem &item)
    : attr_(item.attr_), class_name_(item.class_name_) {}

  DataItem(DataItem &&item)
      : attr_(std::move(item.attr_)), class_name_(std::move(item.class_name_)) {}
  DataItem &operator=(const DataItem &item) = default;
};

std::vector<DataItem> data_item;
int next_class_key = 0;
std::unordered_map<std::string, int> class_key_dict;

const double inf = 1e30;

class Interval {
 public:
  double lef_, rig_;
  std::vector<double> count_;

  Interval() = default;
  ~Interval() = default;

  Interval(Interval &&i)
    : lef_(i.lef_), rig_(i.rig_), count_(std::move(i.count_)) {}
  Interval &operator=(const Interval &i) = default;

  void Merge(const Interval &i) {
    rig_ = i.rig_;
    for (size_t j = 0; j < count_.size(); j++)
      count_[j] += i.count_[j];
  }
  inline std::string DebugStr() {
    std::stringstream ss;
    ss << "(" << lef_ << "," << rig_ << "): [";
    for (size_t i = 0; i + 1 < count_.size(); i++)
      ss << count_[i] << ", ";
    ss << count_[count_.size() - 1] << "]";
    return ss.str();
  }
};

const std::string kInputFile = "./iris.data";

void ReadData() {
  FILE *fp = fopen(kInputFile.c_str(), "r");
  double attr1, attr2, attr3, attr4;
  char class_buf[100];
  while (~fscanf(fp, "%lf,%lf,%lf,%lf,%s\n", &attr1, &attr2, &attr3, &attr4, class_buf)) {
    DataItem item;
    memcpy(item.attr_.begin(), &attr4, sizeof(attr1) * 4);
    std::reverse(item.attr_.begin(), item.attr_.end());
    item.class_name_ = std::string(class_buf);
    if (class_key_dict.find(item.class_name_) == class_key_dict.end()) {
      class_key_dict[item.class_name_] = next_class_key++;
    }
#ifdef DEBUG
    for (int i = 0; i < 4; i++) printf("%.3f,", item.attr_[i]);
    printf("%s\n", item.class_name_.c_str());
#endif  // DEBUG
    data_item.emplace_back(std::move(item));
  }
  fclose(fp);
}

double ChiSquare(const auto &xs, const auto &ys) {
  double R[2], C[xs.size()], N, E[2][xs.size()];
  // TODO(cjr) use std::reduce to parallel, refer to mxnet ReduceSum
  R[0] = std::accumulate(xs.begin(), xs.end(), 0);
  R[1] = std::accumulate(ys.begin(), ys.end(), 0);
  N = R[0] + R[1];
  for (size_t i = 0; i < xs.size(); i++)
    C[i] = xs[i] + ys[i];
  for (size_t i = 0; i < xs.size(); i++) {
    E[0][i] = R[0] * C[i] / N;
    E[1][i] = R[1] * C[i] / N;
  }
  double ret = 0;
  for (size_t i = 0; i < xs.size(); i++) {
    if (E[0][i] > 0)
      ret += (xs[i] - E[0][i]) * (xs[i] - E[0][i]) / E[0][i];
    if (E[1][i] > 0)
    ret += (ys[i] - E[1][i]) * (ys[i] - E[1][i]) / E[1][i];
  }
  return ret;
}

void ChiMerge(int attr_num) {
  std::vector<Interval> interval_vec;
  std::map<double, std::vector<DataItem>> attr_map;  // sorted by key
  for (const auto &item : data_item) {
    attr_map[item.attr_[attr_num]].emplace_back(item);
  }
  for (const auto &kv : attr_map) {
    Interval interval;
    interval.lef_ = kv.first;
    interval.count_ = std::vector<double>(next_class_key);
    for (const auto &di : kv.second)
      interval.count_[class_key_dict[di.class_name_]]++;
    interval_vec.emplace_back(std::move(interval));
  }
  for (size_t i = 0; i < interval_vec.size(); i++) {
    if (i + 1 == interval_vec.size())
      interval_vec[i].rig_ = inf;
    else
      interval_vec[i].rig_ = interval_vec[i + 1].lef_;
  }
  while (interval_vec.size() > kMaxInterval) {
#ifdef DEBUG
    std::stringstream ss;
    for (size_t i = 0; i < interval_vec.size(); i++)
      ss << interval_vec[i].DebugStr() << "\n";
    //ss << "[";
    //for (size_t i = 0; i + 1 < interval_vec.size(); i++) {
    //  ss << interval_vec[i].lef_ << ", ";
    //}
    //ss << interval_vec.rbegin()->lef_;
    //ss << "]";
    printf("%s\n", ss.str().c_str());
#endif  // DEBUG
    double min_chi2 = inf;
    int idx;
    for (size_t i = 0; i + 1 < interval_vec.size(); i++) {
      double chi2 = ChiSquare(interval_vec[i].count_, interval_vec[i + 1].count_);
      if (min_chi2 > chi2) {
        min_chi2 = chi2;
        idx = i;
      }
    }
    interval_vec[idx].Merge(interval_vec[idx + 1]);
    interval_vec.erase(interval_vec.begin() + idx + 1);
#ifdef DEBUG
    printf("min_chi2 = %f\n", min_chi2);
#endif
  }

  for (size_t i = 0; i < interval_vec.size(); i++)
    printf("%s\n", interval_vec[i].DebugStr().c_str());
  printf("\n");
}

int main() {
  ReadData();
  for (int i = 0; i < 4; i++) {
    ChiMerge(i);
  }
  return 0;
}