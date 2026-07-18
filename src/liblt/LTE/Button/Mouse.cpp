#include "../Buttons.h"

#include "LTE/AutoClass.h"
#include "LTE/Mouse.h"
#include "LTE/Pool.h"

namespace LTE {
  namespace {
    AutoClassDerived(ButtonMouse, ButtonT,
      MouseButton, button)
      DERIVED_TYPE_EX(ButtonMouse)
      POOLED_TYPE

      ButtonMouse() = default;

      bool Down() const override {
        return Mouse_Down(button);
      }

      float DownTime() const override {
        return Mouse_GetDownTime(button);
      }

      bool Pressed() const override {
        return Mouse_Pressed(button);
      }

      bool Released() const override {
        return Mouse_Released(button);
      }

      String ToString() const override {
        return Stringize() | "Mouse " | MouseButton_String[button];
      }
    };

    DERIVED_IMPLEMENT(ButtonMouse)
  }

  Button Button_Mouse(MouseButton button) {
    return new ButtonMouse(button);
  }
}
