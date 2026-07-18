#include "ResourceMap.h"
#include "Map.h"
#include "OS.h"

#include "Debug.h"

namespace {
  struct ResourceMapImpl : public ResourceMapT {
    Map<String, String> pathMap;

    void AddDirectory(String const& path, String const& alias) override {
      Vector<String> elements = OS_ListDir(path);
      for (size_t i = 0; i < elements.size(); ++i) {
        String const& element = elements[i];
        if (element == "." || element == "..")
          continue;

        String fullPath = path + element;
        if (OS_IsDir(fullPath))
          AddDirectory(fullPath + "/", alias + element + "/");
        else if (OS_IsFile(fullPath))
          AddFile(fullPath, alias + element);
      }
    }

    void AddFile(String const& path, String const& alias) override {
      pathMap[alias] = path;
    }

    bool Exists(String const& path) const override {
      return pathMap.contains(path);
    }

    String Get(String const& path) const override {
      String const* result = pathMap.get(path);
      if (!result)
        dbg | "Bad lookup : " | path | endl;
      return result ? *result : "";
    }
  };
}

ResourceMap ResourceMap_Create() {
  return new ResourceMapImpl;
}
