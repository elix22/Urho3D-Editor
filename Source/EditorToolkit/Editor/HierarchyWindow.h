#pragma once

#include "Selection.h"
#include "../GenericUI/GenericUI.h"
#include <Urho3D/Core/Object.h>
#include <Urho3D/Container/HashMap.h>

namespace Urho3D
{

class AbstractUI;
class Scene;
class Node;

class HierarchyWindow : public Object
{
    URHO3D_OBJECT(HierarchyWindow, Object);

public:
    HierarchyWindow(Context* context);
    void Initialize(AbstractUI* ui);
    void SetScene(Scene* scene);
    void SetSelection(Selection* selection);
    Selection::ObjectSet GetSelectedObjects();

private:
    GenericHierarchyListItem* FindItem(Object* object);
    /// Subtract right set from left one.
    void Subtract(const Selection::ObjectSet& lhs, const Selection::ObjectSet& rhs, Selection::ObjectSet& result) const;
    /// Gather selection from hierarchy list.
    void GatherHierarchyListSelections(Selection::ObjectSet& result) const;
    void AddNode(Node* node);

    // @name Editor and UI Events
    // @{

    void HandleListSelectionChanged(StringHash eventType, VariantMap& eventData);
    void HandleEditorSelectionChanged(StringHash eventType, VariantMap& eventData);

    // @}

    // @name Scene Events
    // @{

    void HandleNodeAdded(StringHash eventType, VariantMap& eventData);
    void HandleNodeRemoved(StringHash eventType, VariantMap& eventData);
    void HandleComponentAdded(StringHash eventType, VariantMap& eventData);
    void HandleComponentRemoved(StringHash eventType, VariantMap& eventData);
    void HandleNodeNameChanged(StringHash eventType, VariantMap& eventData);
    void HandleNodeEnabledChanged(StringHash eventType, VariantMap& eventData);
    void HandleComponentEnabledChanged(StringHash eventType, VariantMap& eventData);

    // @}

    // @name UI Events
    // @{

    void HandleUIElementNameChanged(StringHash eventType, VariantMap& eventData);
    void HandleUIElementVisibilityChanged(StringHash eventType, VariantMap& eventData);
    void HandleUIElementAttributeChanged(StringHash eventType, VariantMap& eventData);
    void HandleUIElementAdded(StringHash eventType, VariantMap& eventData);
    void HandleUIElementRemoved(StringHash eventType, VariantMap& eventData);

    // @}

private:
    SharedPtr<GenericDialog> dialog_;
    GenericHierarchyList* hierarchyList_ = nullptr;
    SharedPtr<Scene> scene_;
    SharedPtr<Selection> selection_;
    HashMap<WeakPtr<Object>, WeakPtr<GenericHierarchyListItem>> objectsToItems_;

    bool suppressEditorSelectionChanges_ = false;
};

}
