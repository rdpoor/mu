/*
 * A generalized resource pool: Once an object is allocated, rather
 * than freeing it, it is saved in a vector.  The next time an object
 * is allocated, it's fetched from the vector rather than creating a
 * new one.
 */

#include <vector>
#include <mutex>

namespace mu {

  // ================================================================
  // MuPool

  template <class T> class MuPool {
  public:
    MuPool() : created_(0), called_(0) {}
    ~MuPool() {}

    T *allocate() {
      called_ += 1;
      T *item;
      mutex_.lock();
      if (v_.size() == 0) {
        created_ += 1;
        item = new T;
      } else {
        item = v_.back();
        v_.pop_back();
      }
      mutex_.unlock();
      return item;
    }

    void deallocate(T *item) {
      mutex_.lock();
      v_.push_back(item);
      mutex_.unlock();
    }
    
    int size() {
      return v_.size();
    }

    int created() {
      return created_;
    }

    int called() {
      return called_;
    }

  protected:
    std::vector<T *> v_;
    std::mutex mutex_;
    int created_;
    int called_;
  };
  
}

