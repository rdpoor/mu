/*
 * Test a rule: a MuStream is always a tree.  Deleting the root deletes the
 * entire tree.  A clone() method makes a complete copy of the three.
 */

#include <stdio.h>
#include <vector>

// ================================================================
// MuBuffer only exists to see how an included class is created and
// deleted.
class MuBuffer {
public:
  MuBuffer() {
    printf("%p MuBuffer()\n", this);
  }
  ~MuBuffer() {
    printf("%p ~MuBuffer()\n", this);
  }
};

// ================================================================
// MuTree is the abstract base class for all MuTree-like classes.
class MuTree {
public:
  MuTree() {}
  virtual ~MuTree() {}          // delete
  virtual MuTree *clone() = 0;  // make a deep copy of the entire tree
};

// ================================================================
// T1 features a value slot and is a "leaf" on the tree (no inputs).
class T1 : public MuTree {
public:
  T1() : value_(0) {
    printf("%p T1()\n", this);
  }
  ~T1() {
    printf("%p ~T1()\n", this);
  }

  T1 *clone() {
    // printf("T1<%p>.clone()\n", this);
    T1 *c = new T1();
    c->set_value(value());
    return c;
  }

  double value() { return value_; };
  void set_value(double value) { value_ = value; }
private:
  MuBuffer buffer_;
  double value_;
};

// ================================================================
// T2 features a source slot.  The source must be deleted (in a delete
// operation) or copied (in a clone operation).
class T2 : public MuTree {
public:
  T2() : source_(NULL) {
    printf("%p T2()\n", this);
  }
  ~T2() {
    if (source_ != NULL) delete source_;
    printf("%p ~T2()\n", this);
  }

  // Can I/should I use the standard copy constructor instead?
  T2 *clone() {
    // printf("T2<%p>.clone()\n", this);
    T2 *c = new T2();
    if (source_ != NULL) c->set_source(source_->clone());
    return c;
  }

  MuTree *source() const { return source_; }
  void set_source(MuTree *source) { source_ = source; }
private:
  MuBuffer buffer_;
  MuTree *source_;
};

// ================================================================
// T3 features multiple inputs.  Make sure they all get deleted and
// copied.
typedef std::vector<MuTree *> MuTreeVector;

class T3 : public MuTree {
public:
  T3() {
    printf("%p T3()\n", this);
  }
  ~T3() {
    for (int i=sources_.size()-1; i>=0; --i) { 
      MuTree *t = sources_.at(i);
      if (t != NULL) { delete t; }
    }
    printf("%p ~T3()\n", this);
  }
  T3 *clone() {
    // printf("T3<%p>.clone()\n", this);
    T3 *c = new T3();
    for (int i=sources_.size()-1; i>=0; --i) {
      MuTree *t = sources_.at(i);
      // printf("cloning sources_[%d] = %p\n", i, t);
      c->add_source((t == NULL) ? t : t->clone());
    }
    return c;
  }
  void add_source(MuTree *source) { sources_.push_back(source); }
  const MuTreeVector &sources() const { return sources_; }
private:
  MuTreeVector sources_;
};

// ================================================================
void do_test() {

  // Create a tree with a stack-allocated T2.  The T2 dtor should
  // be called when the do_test() method exits.
  printf("=== stack allocated T2\n");
  T1 *t1a = new T1();
  t1a->set_value(11);
  T2 t2a;
  t2a.set_source(t1a);

  // Create a tree witha heap-allocated T2.  The T2 dtor must be
  // explicitly called.
  printf("=== heap allocated T2\n");
  T1 *t1b = new T1();
  t1b->set_value(22);
  T2 *t2b = new T2();
  t2b->set_source(t1b);

  // Test cloning on t2a
  printf("=== clone t2a\n");
  T2 *t2d = t2a.clone();
  printf("t2d->source() = %p\n", t2d->source());
  T1 *t1d = (T1 *)t2d->source();
  printf("t1d->value() = %f\n", t1d->value());

  // Test cloning on t2b
  printf("=== clone t2b\n");
  T2 *t2c = t2b->clone();
  printf("t2c->source() = %p\n", t2c->source());
  T1 *t1c = (T1 *)t2c->source();
  printf("t1c->value() = %f\n", t1c->value());

  delete t2d;

  // Test cloning and deleting on T3
  printf("=== setup t3e\n");
  T1 *t1e = new T1();
  t1e->set_value(33);
  T2 *t2e = new T2();
  T3 *t3e = new T3();
  t3e->add_source(t1e);
  t3e->add_source(t2e);

  printf("=== clone t3e\n");
  T3 *t3f = t3e->clone();

  printf("=== delete t3e\n");
  delete t3e;

  printf("=== inspect t3f\n");
  printf("t3f count = %ld\n", t3f->sources().size());
  for (int i = t3f->sources().size()-1; i>=0; --i) {
    printf("  t3f[%i] = %p\n", i, t3f->sources().at(i));
  }

  printf("=== delete t3f\n");
  delete t3f;

  printf("=== exiting block\n");
}

int main() {
  do_test();
  return 0;
}
