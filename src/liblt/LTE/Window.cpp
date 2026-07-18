#include "Window.h"

#include "Array.h"
#include "Keyboard.h"
#include "Matrix.h"
#include "Mouse.h"
#include "Pointer.h"
#include "Renderer.h"
#include "String.h"
#include "Texture2D.h"
#include "Viewport.h"

#include "SFML/Graphics.hpp"

namespace {
  Vector<Window>& GetStack() {
    static Vector<Window> stack;
    return stack;
  }

  void ProcessMouseEvent(sf::Mouse::Button button, bool pressed) {
    switch (button) {
      case sf::Mouse::Left:
        Mouse_SetPressed(MouseButton_Left, pressed); break;
      case sf::Mouse::Right:
        Mouse_SetPressed(MouseButton_Right, pressed); break;
      case sf::Mouse::Middle:
        Mouse_SetPressed(MouseButton_Middle, pressed); break;
      case sf::Mouse::XButton1:
        Mouse_SetPressed(MouseButton_X1, pressed); break;
      case sf::Mouse::XButton2:
        Mouse_SetPressed(MouseButton_X2, pressed); break;
      default: break;
    }
  }

  struct WindowImpl : public WindowT {
    sf::RenderWindow impl;
    String title;
    Viewport viewport;
    V2U size;
    uint bpp;
    bool captureMouse;
    bool hasFocus;

    WindowImpl(
        String const& title,
        V2U const& size,
        bool border,
        bool fullscreen) :
      title(title),
      size(size),
      bpp(32),
      captureMouse(false),
      hasFocus(true)
    {
      viewport = Viewport_Create(0, size, 1, true);
      sf::ContextSettings glSettings;
      glSettings.majorVersion = 3;
      glSettings.minorVersion = 3;
      // Request a 3.3 context (shaders use #version 330). Leave attributeFlags
      // at 0: setting the explicit Core bit makes SFML 2.6 + this Mesa driver
      // crash with a GLX MakeCurrent / oldCtxInfo assertion at first context
      // switch. The engine's draw path is made core-compatible via the VBO
      // fallbacks in Renderer.cpp (DrawQuad / DrawQuadOutline / DrawVertices),
      // so a core 3.3+ context works without client-side vertex arrays.
      glSettings.attributeFlags = 0;
      glSettings.depthBits = 24;
      glSettings.stencilBits = 8;
      impl.create(
        sf::VideoMode(size.x, size.y, bpp),
        title,
        fullscreen
          ? sf::Style::Fullscreen 
          : border
            ? sf::Style::Default
            : sf::Style::None,
        glSettings);
      impl.setMouseCursorVisible(false);
      impl.setView(sf::View(sf::FloatRect(0, 0, size.x, size.y)));
      viewport->size = size;

      // sf::Vector2i p = sf::Mouse::getPosition(impl);
      // sf::Mouse::setPosition(p, impl);
    }

    void Close() override {
      impl.close();
    }

    void Display() override {
      impl.display();
    }

    void* GetImplData() override {
      return &impl;
    }

    V2U GetSize() const override {
      return size;
    }

    bool HasFocus() const override {
      return hasFocus;
    }

    bool IsOpen() const override {
      return impl.isOpen();
    }

    void SetCaptureMouse(bool captureMouse) override {
      this->captureMouse = captureMouse;
    }

    void SetCursorVisible(bool visible) override {
      impl.setMouseCursorVisible(visible);
    }

    void SetFullscreen() override {
      impl.create(sf::VideoMode(size.x, size.y, bpp), title, sf::Style::Fullscreen);
      viewport->size.x = (float)impl.getSize().x;
      viewport->size.y = (float)impl.getSize().y;
      impl.setMouseCursorVisible(false);
    }

    void SetIcon(Texture2D const& icon) override {
      Array<uchar> buf(icon->GetMemory());
      icon->GetData(buf.data());
      impl.setIcon(
        icon->GetWidth(),
        icon->GetHeight(),
        (sf::Uint8 const*)buf.data());
    }

    void SetPosition(V2I const& p) override {
      impl.setPosition(sf::Vector2i(p.x, p.y));
    }

    void SetSync(bool sync) override {
      impl.setVerticalSyncEnabled(sync);
    }

    void Update() override {
      sf::Event e;
      while (impl.pollEvent(e)) {
        if (e.type == sf::Event::Resized) {
          float w = (float)e.size.width;
          float h = (float)e.size.height;
          impl.setView(sf::View(sf::FloatRect(0, 0, w, h)));
          size.x = e.size.width;
          size.y = e.size.height;
          viewport->size = V2(w, h);
        }

        else if (e.type == sf::Event::KeyPressed) {
          if (e.key.code != sf::Keyboard::Unknown)
            Keyboard_AddDown((int)e.key.code);
        }

        else if (e.type == sf::Event::MouseButtonPressed)
          ProcessMouseEvent(e.mouseButton.button, true);

        else if (e.type == sf::Event::MouseButtonReleased)
          ProcessMouseEvent(e.mouseButton.button, false);

        else if (e.type == sf::Event::MouseMoved) {
          V2I p(e.mouseMove.x, e.mouseMove.y);

          if (captureMouse) {
            const V2I borderSize = 1;
            V2I s = (V2I)size - borderSize;
            if (p.x < borderSize.x ||
                p.y < borderSize.y ||
                p.x > s.x ||
                p.y > s.y)
            {
              p = Clamp(p, borderSize, s);
              Mouse_SetPos(p);
            }
          }
          Mouse_UpdatePos(p);  
        }

        else if (e.type == sf::Event::MouseWheelMoved && hasFocus) {
          /* TODO : Improve precision on Windows. */
          Mouse_SetScrollDelta((float)e.mouseWheel.delta);
        }

        else if (e.type == sf::Event::GainedFocus)
          hasFocus = true;

        else if (e.type == sf::Event::LostFocus)
          hasFocus = false;

        else if (e.type == sf::Event::TextEntered) {
          if (e.text.unicode >= 32 && e.text.unicode <= 126)
            Keyboard_AddText((char)e.text.unicode);
        }
      }
    }
  };
}

Window Window_Create(
  String const& title,
  V2U const& size,
  bool border,
  bool fullscreen)
{
  return new WindowImpl(title, size, border, fullscreen);
}

Window Window_Get() {
  return GetStack().size() ? GetStack().back() : nullptr;
}

void Window_Pop() {
  Viewport_Pop();
  GetStack().pop();
}

void Window_Push(Window const& window) {
  GetStack().push(window);
  Viewport_Push(((WindowImpl*)window.t)->viewport);
}
