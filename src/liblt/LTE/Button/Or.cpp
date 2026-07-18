#include "../Buttons.h"

#include "LTE/AutoClass.h"
#include "LTE/Pool.h"
#include "LTE/StdMath.h"

namespace LTE {
  namespace {
    AutoClassDerived(ButtonOr, ButtonT,
      Button, a,
      Button, b)
      DERIVED_TYPE_EX(ButtonOr)
      POOLED_TYPE

      ButtonOr() = default;

      bool Down() const override {
        return a->Down() || b->Down();
      }

      float DownTime() const override {
        return Min(a->DownTime(), b->DownTime());
      }

      bool Pressed() const override {
        return a->Pressed() || b->Pressed();
      }

      bool Released() const override {
        return a->Released() || b->Released();
      }

      String ToString() const override {
        return a->ToString() + ", " + b->ToString();
      }
    };

    DERIVED_IMPLEMENT(ButtonOr)
  }

  Button Button_Or(Button const& a, Button const& b) {
    return new ButtonOr(a, b);
  }
}
