#include <Stk.h>
#include "player2.h"
#include "mu_types.h"
#include "gmock/gmock.h"  // Brings in Google Mock.

class MockPlayer : public mu::Player2 {
 public:
  MOCK_METHOD0(transport, mu::Transport *());
  MOCK_METHOD1(set_transport, void(mu::Transport *transport));
  MOCK_METHOD0(start, void());
  MOCK_METHOD0(stop, void());
};
