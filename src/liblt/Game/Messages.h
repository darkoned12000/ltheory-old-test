#ifndef Game_Messages_h__
#define Game_Messages_h__

#include "Common.h"
#include "Game/Object.h"
#include "LTE/AutoClass.h"
#include "LTE/Color.h"

AutoClassEmpty(MessageBoost) MessageBoost() = default; };
AutoClassEmpty(MessageCruise) MessageCruise() = default; };
AutoClassEmpty(MessageFire) MessageFire() = default; };
AutoClassEmpty(MessageLaunch) MessageLaunch() = default; };
AutoClassEmpty(MessageReload) MessageReload() = default; };

AutoClass(MessageCollectStars,
  Color, totalColor,
  Position, totalPosition,
  uint, count)

  MessageCollectStars() :
    totalColor(0),
    totalPosition(0),
    count(0)
    {}

  Color GetColor() const {
    return totalColor / count;
  }

  Position GetPosition() const {
    return totalPosition / count;
  }
};

AutoClassEmpty(MessageEjected)
  MessageEjected() = default;
};

AutoClass(MessageGetColor,
  Color, color)
  MessageGetColor() = default;
};

AutoClass(MessageLink,
  Object, object)
  MessageLink() = default;
};

AutoClass(MessageStartUsing,
  Object, object,
  Object, target)
  MessageStartUsing() = default;
};

AutoClass(MessageStopUsing,
  Object, object)
  MessageStopUsing() = default;
};

AutoClass(MessageTargetPosition,
  Position, position)
  MessageTargetPosition() = default;
};

AutoClass(MessageTargetObject,
  Object, object)
  MessageTargetObject() = default;
};

AutoClass(MessageThrustAngular,
  V3, direction,
  float, amount)
  MessageThrustAngular() = default;
};

AutoClass(MessageThrustLinear,
  V3, direction)
  MessageThrustLinear() = default;
};

AutoClass(MessageUnlink,
  Object, object)
  MessageUnlink() = default;
};

AutoClass(MessageWaypoint,
  Object, location)
  MessageWaypoint() = default;
};

#endif
