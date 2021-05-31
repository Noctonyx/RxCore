#pragma once

#include <string>
#include <string>

#include "SDL_mouse.h"
#include "SDL_video.h"

//struct GLFWwindow;
//struct GLFWcursor;

namespace ecs {
    class World;
}

namespace RxCore
{

    enum class ECursorStandard : uint32_t
    {
        Arrow,
        IBeam,
        //Crosshair,
        Hand,
        ResizeX,
        ResizeY
    };

    class Window
    {
    public:
        Window(uint32_t width, uint32_t height, const std::string & title);

        ~Window();

        Window(const Window & other) = delete;

        Window(Window && other) noexcept = delete;

        Window & operator=(const Window & other) = delete;

        Window & operator=(Window && other) noexcept = delete;

        void Update();

        [[nodiscard]] uint32_t getWidth() const;

        [[nodiscard]] uint32_t getHeight() const;

        //[[nodiscard]] float getAspectRatio() const;

        //[[nodiscard]] bool shouldClose() const;

        //void setMouseVisible(bool visible);
        void setRelativeMouseMode(bool mode);

        [[nodiscard]] bool getMouseVisible() const;

        void setTitle(std::string & title);

        [[nodiscard]] SDL_Window * GetWindow() const;

        void setCursor(ECursorStandard standard);
        void hideCursor(bool hidden);
        void setCursorPosition(int32_t x, int32_t y);

        //void doResize(uint32_t width, uint32_t height);
        //void doChar(uint32_t codepoint);
        //void doKey(int32_t key, int32_t action, int32_t mods);
        //void mousePosition(float x_pos, float y_pos, int32_t mods);
        //void buttonAction(int32_t button, bool pressed, int32_t mods);
        //void scroll(float y_scroll, int32_t mods);

        //void setWorld(ecs::World * world);

    private:
        //bool m_GLFWInit = false;
        SDL_Window * m_Window = nullptr;
        bool m_IsMouseVisible = true;
        //ecs::World* world_ = nullptr;
        float cursorX;
        float cursorY;
        //SDL_Cursor* cursor_ = nullptr;
        //ECursorStandard cursorStandard_{};

        SDL_Cursor* cursorArrow;
        SDL_Cursor* cursorIBeam;
        SDL_Cursor* cursorResizeY;
        SDL_Cursor* cursorResizeX;
        SDL_Cursor* cursorHand;

        bool hidden_ = false;
    };
} // namespace RXCore
