#pragma once

#include "SplitView.h"
#include <Urho3D/Container/Pair.h>

namespace Urho3D
{

class TabBar;
class TabButton;

enum DockLocation
{
    DL_LEFT,
    DL_RIGHT,
    DL_TOP,
    DL_BOTTOM,
    DL_COUNT
};

static_assert(DL_COUNT == 4, "Just no");

class DockView : public UIElement
{
    URHO3D_OBJECT(DockView, UIElement);

public:
    DockView(Context* context);

    static void RegisterObject(Context* context);

    void SetDefaultSplitStyle();
    void SetDefaultTabBarStyle();
    //void SetSideOffset();
    void SetPriority(DockLocation first, DockLocation second, DockLocation third, DockLocation forth);

    void AddDock(DockLocation location, const String& title, UIElement* content);

    /// Get central element.
    UIElement* GetCentralElement() const { return centralElement_; }

    /// Find best dock location for given screen position.
    UIElement* FindBestLocation(const IntVector2& position);

private:
    void RelocateDock(TabButton* dockTitle, UIElement* newPanel, const IntVector2& hintPosition);
    void UpdateDockSplits();
    void HandleTabSelected(StringHash eventType, VariantMap& eventData);

private:
    /// Split views (unordered).
    SharedPtr<SplitView> splitElements_[DL_COUNT];
    /// Dock containers (unordered).
    SharedPtr<UIElement> containerElements_[DL_COUNT];
    /// Center element.
    SharedPtr<UIElement> centralElement_;
    /// Tab bars (unordered);
    TabBar* tabBars_[DL_COUNT] = { nullptr, nullptr, nullptr, nullptr };

    /// Dock locations order.
    DockLocation locations_[DL_COUNT] = { DL_RIGHT, DL_BOTTOM, DL_LEFT, DL_TOP };
    /// Docks (ordered).
    UIElement* dockContainers_[DL_COUNT] = { nullptr, nullptr, nullptr, nullptr };
    /// Split offsets (ordered).
    int offsets_[DL_COUNT] = { 200, 200, 200, 200 };
};

}
