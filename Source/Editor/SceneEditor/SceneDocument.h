#pragma once

#include "../Document.h"
#include "../Action.h"
#include <QAction>
#include <QSet>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/Scene.h>

namespace Urho3D
{

class Input;

}

namespace Urho3DEditor
{

class MainWindow;
class SceneOverlay;
class Urho3DWidget;

/// Scene camera.
class SceneCamera
{
public:
    /// Construct.
    SceneCamera(Urho3D::Context* context);
    /// Get camera.
    Urho3D::Camera& GetCamera() const { return *camera_; }

    /// Set grab mouse.
    void SetGrabMouse(bool grab);
    /// Move camera.
    void Move(const Urho3D::Vector3& movement, const Urho3D::Vector3& rotation);

private:
    /// Input subsystem.
    Urho3D::Input* input_;
    /// Camera node.
    Urho3D::Node cameraNode_;
    /// Camera component.
    Urho3D::Camera* camera_;
    /// Camera angles.
    Urho3D::Vector3 angles_;
};

/// Scene document.
class SceneDocument : public Document, public Urho3D::Object
{
    Q_OBJECT
    URHO3D_OBJECT(SceneDocument, Urho3D::Object);

public:
    /// Pick mode.
    enum PickMode
    {
        PickGeometries,
        PickLights,
        PickZones,
        PickRigidbodies
    };
    /// Hot key mode.
    enum HotKeyMode
    {
        HotKeyStandard,
        HotKeyBlender
    };
    /// Set of nodes.
    using NodeSet = QSet<Urho3D::Node*>;
    /// Set of components.
    using ComponentSet = QSet<Urho3D::Component*>;

public:
    /// Construct.
    SceneDocument(MainWindow& mainWindow);
    /// Get scene.
    Urho3D::Scene& GetScene() const { return *scene_; }

    /// Add overlay.
    void AddOverlay(SceneOverlay* overlay);
    /// Remove overlay.
    void RemoveOverlay(SceneOverlay* overlay);

    /// Add action.
    void AddAction(const ActionGroup& actionGroup);
    /// Undo action.
    void UndoAction();
    /// Redo action.
    void RedoAction();

    /// Get current camera.
    Urho3D::Camera& GetCurrentCamera();

    /// Set selection.
    virtual void SetSelection(const NodeSet& selectedNodes, const ComponentSet& selectedComponents);
    /// Get selected nodes.
    const NodeSet& GetSelectedNodes() const { return selectedNodes_; }
    /// Get selected components.
    const ComponentSet& GetSelectedComponents() const { return selectedComponents_; }
    /// Get selected nodes and components.
    const NodeSet& GetSelectedNodesAndComponents() const { return selectedNodesCombined_; }

    /// Return title of the document.
    virtual QString GetTitle() override { return GetRawTitle(); }
    /// Return whether the document can be saved.
    virtual bool CanBeSaved() override { return true; }
    /// Return whether the document widget should be visible when the document is active.
    virtual bool IsPageWidgetVisible() override { return false; }
    /// Return whether the Urho3D widget should be visible when the document is active.
    virtual bool IsUrho3DWidgetVisible() override { return true; }
    /// Get name filters for open and save dialogs.
    virtual QString GetNameFilters() override;

signals:
    /// Signals that selection has been changed.
    void selectionChanged();
    /// Signals that node transforms has been changed.
    void nodeTransformChanged(const Urho3D::Node& node);

private:
    /// Handle update.
    virtual void HandleUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    /// Handle mouse button up/down.
    virtual void HandleMouseButton(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    /// Handle post-render update.
    virtual void HandlePostRenderUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

protected:
    /// Handle current document changed.
    virtual void HandleCurrentPageChanged(Document* document) override;
    /// Load the document from file.
    virtual bool DoLoad(const QString& fileName) override;

protected:
    /// Get camera ray.
    Urho3D::Ray GetCameraRay(const Urho3D::IntVector2& position) const;
    /// Check whether to draw debug geometry for node.
    virtual bool ShallDrawNodeDebug(Urho3D::Node* node);
    /// Draw node debug geometry.
    virtual void DrawNodeDebug(Urho3D::Node* node, Urho3D::DebugRenderer* debug, bool drawNode = true);
    /// Draw debug geometry.
    virtual void DrawDebugGeometry();
    /// Draw debug components.
    virtual void DrawDebugComponents();
    /// Perform ray cast.
    virtual void PerformRaycast(bool mouseClick);
    /// Gather selected nodes.
    void GatherSelectedNodes();

protected:
    /// Widget.
    Urho3DWidget* widget_;
    /// Overlays.
    QList<SceneOverlay*> overlays_;

    /// Camera.
    SceneCamera camera_;
    /// Scene.
    Urho3D::SharedPtr<Urho3D::Scene> scene_;
    /// Viewport.
    Urho3D::SharedPtr<Urho3D::Viewport> viewport_;

    /// Actions.
    QVector<ActionGroup> actions_;

    /// Selected nodes.
    NodeSet selectedNodes_;
    /// Selected components.
    ComponentSet selectedComponents_;
    /// Selected nodes and components.
    NodeSet selectedNodesCombined_;

};

}
