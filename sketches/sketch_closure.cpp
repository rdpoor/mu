#include <functional>

struct SICPStream;

typedef std::function<SICPStream *()> Deferred;

#define DEFER(form) ([=]() -> SICPStream* { return (form); })
#define FORCE(deferred_form) ((deferred_form)())

#define PAIR(value, rest) (new SICPStream(value, DEFER(rest)))
#define FIRST(stream) (stream->first_)
#define REST(stream) (FORCE(stream->rest_))

struct SICPStream {
public:
  SICPStream(int first, Deferred rest) : first_(first), rest_(rest) {}

  int first_;
  Deferred rest_;
};

SICPStream *integers(int starting) {
  return PAIR(starting, integers(starting + 1));
}

// ================================================================
    
int main() {
  SICPStream *s = integers(1);
  
  for (int i=5; i>=0; --i) {
    printf("%p, %d\n", s, FIRST(s));
    SICPStream *s1 = REST(s);
    delete s;
    s = s1;
  }
  
  return 0;
}
