// Implementation for Problem 066 - any_ptr
#ifndef SRC_HPP
#define SRC_HPP

#include <stdexcept>
#include <initializer_list>

namespace sjtu {

class any_ptr {
 private:
  struct control_block_base {
    int refcnt;
    control_block_base() : refcnt(1) {}
    virtual ~control_block_base() {}
    virtual void *get_void_ptr() = 0;
  };

  template <class T>
  struct control_block_impl : control_block_base {
    T *ptr;
    explicit control_block_impl(T *p) : ptr(p) {}
    ~control_block_impl() override { delete ptr; }
    void *get_void_ptr() override { return static_cast<void *>(ptr); }
  };

  control_block_base *cb = nullptr;

  void add_ref() {
    if (cb) ++cb->refcnt;
  }

  void release() {
    if (cb) {
      if (--cb->refcnt == 0) {
        delete cb;
      }
      cb = nullptr;
    }
  }

 public:
  // 默认构造：空指针
  any_ptr() = default;

  // 拷贝构造：浅拷贝，共享同一块内存
  any_ptr(const any_ptr &other) : cb(other.cb) { add_ref(); }

  // 从裸指针构造：接管生命周期
  template <class T>
  explicit any_ptr(T *ptr) : cb(nullptr) {
    if (ptr) cb = new control_block_impl<T>(ptr);
  }

  // 析构：仅最后一个持有者释放
  ~any_ptr() { release(); }

  // 拷贝赋值：浅拷贝
  any_ptr &operator=(const any_ptr &other) {
    if (this == &other) return *this;
    // 增加新对象引用计数再释放旧对象，避免自赋值边界问题
    if (other.cb) other.cb->refcnt++;
    release();
    cb = other.cb;
    return *this;
  }

  // 从裸指针赋值：释放旧对象，接管新指针
  template <class T>
  any_ptr &operator=(T *ptr) {
    release();
    cb = nullptr;
    if (ptr) cb = new control_block_impl<T>(ptr);
    return *this;
  }

  // 取出引用（非常量）
  template <class T>
  T &unwrap() {
    if (!cb) throw std::bad_cast();
    auto *impl = dynamic_cast<control_block_impl<T> *>(cb);
    if (!impl || !impl->ptr) throw std::bad_cast();
    return *(impl->ptr);
  }

  // 取出引用（常量）
  template <class T>
  const T &unwrap() const {
    if (!cb) throw std::bad_cast();
    auto *impl = dynamic_cast<const control_block_impl<T> *>(cb);
    if (!impl || !impl->ptr) throw std::bad_cast();
    return *(impl->ptr);
  }
};

// 由指定值构造 any_ptr
template <class T>
any_ptr make_any_ptr(const T &t) {
  return any_ptr(new T(t));
}

// 可变参数构造：使用列表初始化以支持容器构造
template <class T, class... Args>
any_ptr make_any_ptr(Args... args) {
  return any_ptr(new T{args...});
}

// initializer_list 构造（匹配如 vector, map 等）
template <class T, class U>
any_ptr make_any_ptr(std::initializer_list<U> init) {
  return any_ptr(new T{init});
}

}  // namespace sjtu

#endif  // SRC_HPP
