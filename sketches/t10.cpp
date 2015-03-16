#include <unordered_map>
#include <string>
using namespace std;

#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))

typedef unordered_map<string, void *> NestedMap;
typedef unordered_map<string, void *> *PtrToNestedMap;

int main() {
  unordered_map<string, NestedMap> m1;
  unordered_map<string, PtrToNestedMap> m2;
  unordered_map<string, NestedMap*> m3;
}
