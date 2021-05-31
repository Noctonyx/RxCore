#include "Window.hpp"

#include "DirectXMath.h"
//#include "ini.h"
#include "Log.h"
#include "SDL.h"
#include "SDL_vulkan.h"

namespace RxCore
{
#if 0
    void callbackKey(GLFWwindow * window,
                     int32_t key,
                     int32_t scancode,
                     int32_t action,
                     int32_t mods)
    {
        auto wnd = static_cast<Window *>(glfwGetWindowUserPointer(window));
        wnd->doKey(key, action, mods);
    }

    void callbackChar(GLFWwindow * window, uint32_t codepoint)
    {
        auto wnd = static_cast<Window *>(glfwGetWindowUserPointer(window));
        wnd->doChar(codepoint);
    }

    void callbackCursorPos(GLFWwindow * window, double xpos, double ypos)
    {
        auto wnd = static_cast<Window *>(glfwGetWindowUserPointer(window));
        int32_t mods = 0;

        if (glfwGetKey(window, static_cast<int>(EKey::ShiftLeft)) == GLFW_PRESS) {
            mods |= GLFW_MOD_SHIFT;
        }
        if (glfwGetKey(window, static_cast<int>(EKey::ShiftRight)) == GLFW_PRESS) {
            mods |= GLFW_MOD_SHIFT;
        }
        if (glfwGetKey(window, static_cast<int>(EKey::ControlLeft)) == GLFW_PRESS) {
            mods |= GLFW_MOD_CONTROL;
        }
        if (glfwGetKey(window, static_cast<int>(EKey::ControlRight)) == GLFW_PRESS) {
            mods |= GLFW_MOD_CONTROL;
        }
        if (glfwGetKey(window, static_cast<int>(EKey::AltLeft)) == GLFW_PRESS) {
            mods |= GLFW_MOD_ALT;
        }
        if (glfwGetKey(window, static_cast<int>(EKey::AltRight)) == GLFW_PRESS) {
            mods |= GLFW_MOD_ALT;
        }

        wnd->mousePosition(static_cast<float>(xpos), static_cast<float>(ypos), mods);
    }

    void callbackMouseButton(GLFWwindow * window, int32_t button, int32_t action, int32_t mods)
    {
        auto wnd = static_cast<Window *>(glfwGetWindowUserPointer(window));

        //spdlog::info("Mouse Button = {}, pressed = {}", button, action);
        wnd->buttonAction(button, action == GLFW_PRESS, mods);
    }

    void callbackScroll(GLFWwindow * window, double /*xoffset*/, double y_offset)
    {
        auto wnd = static_cast<Window *>(glfwGetWindowUserPointer(window));

        int32_t mods = 0;

        if (glfwGetKey(window, static_cast<int>(EKey::ShiftLeft)) == GLFW_PRESS) {
            mods |= GLFW_MOD_SHIFT;
        }
        if (glfwGetKey(window, static_cast<int>(EKey::ShiftRight)) == GLFW_PRESS) {
            mods |= GLFW_MOD_SHIFT;
        }
        if (glfwGetKey(window, static_cast<int>(EKey::ControlLeft)) == GLFW_PRESS) {
            mods |= GLFW_MOD_CONTROL;
        }
        if (glfwGetKey(window, static_cast<int>(EKey::ControlRight)) == GLFW_PRESS) {
            mods |= GLFW_MOD_CONTROL;
        }
        if (glfwGetKey(window, static_cast<int>(EKey::AltLeft)) == GLFW_PRESS) {
            mods |= GLFW_MOD_ALT;
        }
        if (glfwGetKey(window, static_cast<int>(EKey::AltRight)) == GLFW_PRESS) {
            mods |= GLFW_MOD_ALT;
        }

        wnd->scroll(static_cast<float>(y_offset), mods);
    }

    void FrameBufferResizeCallback(GLFWwindow * window, int width, int height)
    {
        Window * w = static_cast<Window *>(glfwGetWindowUserPointer(window));
        w->doResize(width, height);
    }
#endif

    Window::Window(uint32_t width, uint32_t height, const std::string & title)
    {
        //SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
        m_Window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                    width, height,
                                    SDL_WINDOW_VULKAN | SDL_WINDOW_ALLOW_HIGHDPI |
                                    SDL_WINDOW_RESIZABLE);


        cursorArrow = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
        cursorResizeX = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
        cursorResizeY = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
        cursorIBeam = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
        //cursorEggtimer = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAIT);
        cursorHand = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);

#if 0
        if (!m_GLFWInit) {
            glfwInit();
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            m_GLFWInit = true;
        }

        mINI::INIFile file("config.ini");
        mINI::INIStructure ini;

        file.read(ini);
        auto w = ini.get("window").get("width");

        m_Window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(m_Window, this);

        glfwSetFramebufferSizeCallback(m_Window, &FrameBufferResizeCallback);

        glfwSetCursorPosCallback(GetWindow(), callbackCursorPos);
        glfwSetMouseButtonCallback(GetWindow(), callbackMouseButton);
        glfwSetScrollCallback(GetWindow(), callbackScroll);

        glfwSetKeyCallback(GetWindow(), callbackKey);
        glfwSetCharCallback(GetWindow(), callbackChar);
#endif
    }

    Window::~Window()
    {
        SDL_DestroyWindow(m_Window);

        SDL_FreeCursor(cursorArrow);
        SDL_FreeCursor(cursorHand);
        SDL_FreeCursor(cursorResizeX);
        SDL_FreeCursor(cursorResizeY);
        SDL_FreeCursor(cursorIBeam);
    }

    void Window::Update()
    {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            /* handle your event here */
            switch (event.type) {
            case SDL_KEYDOWN:
                break;
            case SDL_KEYUP:
                break;
            case SDL_QUIT:
                break;
            }
        }
    }

    uint32_t Window::getWidth() const
    {
        int height, width;
        SDL_Vulkan_GetDrawableSize(m_Window, &width, &height);
        return width;
    }

    uint32_t Window::getHeight() const
    {
        int height, width;
        SDL_Vulkan_GetDrawableSize(m_Window, &width, &height);
        return height;
    }

    void Window::setRelativeMouseMode(bool mode)
    {
        SDL_SetRelativeMouseMode(mode ? SDL_TRUE : SDL_FALSE);
    }
#if 0
    float Window::getAspectRatio() const
    {
        int height, width;
        glfwGetFramebufferSize(m_Window, &width, &height);
        return float(width) / float(height);
    }
#endif
#if 0
    bool Window::shouldClose() const
    {
        return glfwWindowShouldClose(m_Window);
    }
#endif
#if 0
    void Window::setMouseVisible(bool visible)
    {
        m_IsMouseVisible = visible;
        if (visible) {
            glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } else {
            glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }
#endif
    bool Window::getMouseVisible() const
    {
        return m_IsMouseVisible;
    }

    void Window::setTitle(std::string & title)
    {
        SDL_SetWindowTitle(m_Window, title.c_str());
    }

    SDL_Window * Window::GetWindow() const
    {
        return m_Window;
    }

    void Window::setCursor(ECursorStandard standard)
    {
        switch (standard) {
        case ECursorStandard::Arrow:
            SDL_SetCursor(cursorArrow);
            break;
        case ECursorStandard::IBeam:
            SDL_SetCursor(cursorIBeam);
            break;
        case ECursorStandard::ResizeX:
            SDL_SetCursor(cursorResizeX);
            break;
        case ECursorStandard::ResizeY:
            SDL_SetCursor(cursorResizeY);
            break;
        case ECursorStandard::Hand:
            SDL_SetCursor(cursorHand);
            break;
        }
    }
#if 0
    void Window::doResize(uint32_t width, uint32_t height)
    {
        //onResize.Broadcast(width, height);
        if (world_) {
            world_->getStream<WindowResize>()->add<WindowResize>({width, height});
            world_->setSingleton<WindowDetails>({this, width, height});
        }
    }

    void Window::doChar(uint32_t codepoint)
    {
        //onChar.Broadcast(static_cast<char>(codepoint));

        if (world_) {
            auto s = world_->getStream<KeyboardChar>();
            s->add<KeyboardChar>({static_cast<char>(codepoint)});
        }
    }

    void Window::doKey(int32_t key, int32_t action, int32_t mods)
    {
        //  onKey.Broadcast(static_cast<EKey>(key), static_cast<EInputAction>(action),
        //                  static_cast<EInputMod>(mods));

        if (world_) {
            auto s = world_->getStream<KeyboardKey>();

            s->add<KeyboardKey>({
                static_cast<EKey>(key), static_cast<EInputAction>(action),
                static_cast<EInputMod>(mods)
            });
        }
    }
#endif
#if 0
    void Window::setCursor(ECursorStandard standard)
    {
        //auto window = Locator::Window::get();
        if (standard == cursorStandard_) {
            return;
        }

        cursor_ = glfwCreateStandardCursor(static_cast<int32_t>(standard));
        glfwSetCursor(GetWindow(), cursor_);
        cursorStandard_ = standard;
    }
#endif
    void Window::hideCursor(bool hidden)
    {
        if (hidden_ == hidden) {
            return;
        }
        SDL_ShowCursor(hidden ? SDL_DISABLE : SDL_ENABLE);

        if (!hidden) {
            setCursorPosition(static_cast<int32_t>(cursorX), static_cast<int32_t>(cursorY));
        }
        hidden_ = hidden;
    }

    void Window::setCursorPosition(int32_t x, int32_t y)
    {
        cursorX = static_cast<float>(x);
        cursorY = static_cast<float>(y);

        SDL_WarpMouseInWindow(m_Window, x, y);
    }
} // namespace RXCore
