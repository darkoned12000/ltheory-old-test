#include "../Buttons.h"

#include "LTE/AutoClass.h"
#include "LTE/Keyboard.h"
#include "LTE/Pool.h"

namespace LTE {
  namespace {
    AutoClassDerived(ButtonKey, ButtonT,
      Key, key)
      DERIVED_TYPE_EX(ButtonKey)
      POOLED_TYPE

      ButtonKey() = default;

      bool Down() const override {
        return Keyboard_Down(key);
      }

      float DownTime() const override {
        NOT_IMPLEMENTED
        return 0;
      }

      bool Pressed() const override {
        return Keyboard_Pressed(key);
      }

      bool Released() const override {
        return Keyboard_Released(key);
      }

      String ToString() const override {
        return Key_Name(key);
      }
    };

    DERIVED_IMPLEMENT(ButtonKey)
  }

  Button Button_Key(Key key) {
    return new ButtonKey(key);
  }
}
