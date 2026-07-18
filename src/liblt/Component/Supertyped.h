#ifndef Component_Supertyped_h__
#define Component_Supertyped_h__

#include "Common.h"
#include "Game/Item.h"
#include "LTE/AutoClass.h"
#include "UI/Icon.h"

AutoClass(ComponentSupertyped,
  Item, type)
  ComponentSupertyped() = default;
};

AutoComponent(Supertyped)
  void SetSupertype(Item const& type) override {
    Supertyped.type = type;

    BaseT::SetSupertype(type);
  }

  Icon GetIcon() const override {
    return Supertyped.type->GetIcon();
  }

  ItemT* GetSupertype() const override {
    return Supertyped.type.t;
  }
};

#endif
