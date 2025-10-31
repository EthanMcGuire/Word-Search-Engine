#pragma once

#ifndef EVENT_H
#define EVENT_H

/*
class Button;
class MessageBoxContainer;
class Widget;
class Menu;
class Scrollbar;
*/

struct Event
{   
    virtual ~Event() = default;
};

/*
struct EventButtonPressed : public Event
{
    Button *button;

    EventButtonPressed(Button *button) : button(button) {};
    virtual ~EventButtonPressed() = default;
};

struct EventMessageBoxClosed : public Event
{
    MessageBoxContainer *messageBox;

    EventMessageBoxClosed(MessageBoxContainer *messageBox) : messageBox(messageBox) {};
    virtual ~EventMessageBoxClosed() = default;
};

struct EventWidgetGainedFocus : public Event
{
    Widget *widget;

    EventWidgetGainedFocus(Widget *widget) : widget(widget) {};
    virtual ~EventWidgetGainedFocus() = default;
};

struct EventWidgetDestroyed : public Event
{
    Widget *widget;

    EventWidgetDestroyed(Widget *widget) : widget(widget) {};
    virtual ~EventWidgetDestroyed() = default;
};

struct EventMenuDestroyed : public Event
{
    Menu *menu;

    EventMenuDestroyed(Menu *menu) : menu(menu) {};
    virtual ~EventMenuDestroyed() = default;
};

struct EventScrollbarMoved : public Event
{
    Scrollbar *scrollbar;

    EventScrollbarMoved(Scrollbar *scrollbar) : scrollbar(scrollbar) {};
    virtual ~EventScrollbarMoved() = default;
};
*/

struct EventCameraMoved : public Event
{
    int cameraX, cameraY;

    EventCameraMoved(int cameraX, int cameraY) : cameraX(cameraX), cameraY(cameraY) {};
    virtual ~EventCameraMoved() = default;
};

#endif