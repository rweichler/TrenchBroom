/*
 Copyright (C) 2010-2014 Kristian Duske
 
 This file is part of TrenchBroom.
 
 TrenchBroom is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 TrenchBroom is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with TrenchBroom. If not, see <http://www.gnu.org/licenses/>.
 */

#include "MapView2D.h"
#include "Logger.h"
#include "Model/CompareHits.h"
#include "Model/PointFile.h"
#include "Renderer/Compass2D.h"
#include "Renderer/GridRenderer.h"
#include "Renderer/MapRenderer.h"
#include "Renderer/RenderContext.h"
#include "Renderer/SelectionBoundsRenderer.h"
#include "View/ActionManager.h"
#include "View/Animation.h"
#include "View/CameraAnimation.h"
#include "View/CameraLinkHelper.h"
#include "View/CameraTool2D.h"
#include "View/ClipToolAdapter.h"
#include "View/CommandIds.h"
#include "View/CreateBrushToolAdapter2D.h"
#include "View/CreateEntityToolAdapter.h"
#include "View/FlashSelectionAnimation.h"
#include "View/GLContextManager.h"
#include "View/Grid.h"
#include "View/MapDocument.h"
#include "View/MapViewToolBox.h"
#include "View/MoveObjectsToolAdapter.h"
#include "View/ResizeBrushesToolAdapter.h"
#include "View/RotateObjectsToolAdapter.h"
#include "View/SelectionTool.h"
#include "View/VertexTool.h"
#include "View/VertexToolAdapter.h"
#include "View/wxUtils.h"

namespace TrenchBroom {
    namespace View {
        MapView2D::MapView2D(wxWindow* parent, Logger* logger, MapDocumentWPtr document, MapViewToolBox& toolBox, Renderer::MapRenderer& renderer, GLContextManager& contextManager, const ViewPlane viewPlane) :
        MapViewBase(parent, logger, document, toolBox, renderer, contextManager),
        m_camera(),
        m_clipToolAdapter(NULL),
        m_createBrushToolAdapter(NULL),
        m_createEntityToolAdapter(NULL),
        m_moveObjectsToolAdapter(NULL),
        m_resizeBrushesToolAdapter(NULL),
        m_rotateObjectsToolAdapter(NULL),
        m_vertexToolAdapter(NULL),
        m_cameraTool(NULL) {
            bindEvents();
            bindObservers();
            initializeCamera(viewPlane);
            initializeToolChain(toolBox);
            setCompass(new Renderer::Compass2D());
        }

        MapView2D::~MapView2D() {
            unbindObservers();
            destroyToolChain();
        }
        
        void MapView2D::initializeCamera(const ViewPlane viewPlane) {
            switch (viewPlane) {
                case MapView2D::ViewPlane_XY:
                    m_camera.setDirection(Vec3f::NegZ, Vec3f::PosY);
                    m_camera.moveTo(Vec3f(0.0f, 0.0f, 16384.0f));
                    break;
                case MapView2D::ViewPlane_XZ:
                    m_camera.setDirection(Vec3f::PosY, Vec3f::PosZ);
                    m_camera.moveTo(Vec3f(0.0f, -16384.0f, 0.0f));
                    break;
                case MapView2D::ViewPlane_YZ:
                    m_camera.setDirection(Vec3f::NegX, Vec3f::PosZ);
                    m_camera.moveTo(Vec3f(16384.0f, 0.0f, 0.0f));
                    break;
            }
            m_camera.setNearPlane(1.0f);
            m_camera.setFarPlane(32768.0f);
            
        }

        void MapView2D::initializeToolChain(MapViewToolBox& toolBox) {
            const Grid& grid = lock(m_document)->grid();
            m_clipToolAdapter = new ClipToolAdapter2D(toolBox.clipTool(), grid);
            m_createBrushToolAdapter = new CreateBrushToolAdapter2D(toolBox.createBrushTool(), m_document);
            m_createEntityToolAdapter = new CreateEntityToolAdapter2D(toolBox.createEntityTool());
            m_moveObjectsToolAdapter = new MoveObjectsToolAdapter2D(toolBox.moveObjectsTool());
            m_resizeBrushesToolAdapter = new ResizeBrushesToolAdapter2D(toolBox.resizeBrushesTool());
            m_rotateObjectsToolAdapter = new RotateObjectsToolAdapter2D(toolBox.rotateObjectsTool());
            m_vertexToolAdapter = new VertexToolAdapter2D(toolBox.vertexTool());
            m_cameraTool = new CameraTool2D(m_camera);
            
            addTool(m_cameraTool);
            addTool(m_moveObjectsToolAdapter);
            addTool(m_rotateObjectsToolAdapter);
            addTool(m_resizeBrushesToolAdapter);
            addTool(m_createBrushToolAdapter);
            addTool(m_clipToolAdapter);
            addTool(m_vertexToolAdapter);
            addTool(m_createEntityToolAdapter);
            addTool(toolBox.selectionTool());
        }

        void MapView2D::destroyToolChain() {
            delete m_cameraTool;
            delete m_vertexToolAdapter;
            delete m_resizeBrushesToolAdapter;
            delete m_rotateObjectsToolAdapter;
            delete m_moveObjectsToolAdapter;
            delete m_createEntityToolAdapter;
            delete m_createBrushToolAdapter;
            delete m_clipToolAdapter;
        }

        void MapView2D::bindObservers() {
            m_camera.cameraDidChangeNotifier.addObserver(this, &MapView2D::cameraDidChange);
        }
        
        void MapView2D::unbindObservers() {
            m_camera.cameraDidChangeNotifier.removeObserver(this, &MapView2D::cameraDidChange);
        }
        
        void MapView2D::cameraDidChange(const Renderer::Camera* camera) {
            Refresh();
        }

        void MapView2D::bindEvents() {
            /*
            Bind(wxEVT_KEY_DOWN, &MapView2D::OnKey, this);
            Bind(wxEVT_KEY_UP, &MapView2D::OnKey, this);
            */
            
            /*
            Bind(wxEVT_MENU, &MapView2D::OnPopupReparentBrushes,         this, CommandIds::MapViewPopupMenu::ReparentBrushes);
            Bind(wxEVT_MENU, &MapView2D::OnPopupMoveBrushesToWorld,      this, CommandIds::MapViewPopupMenu::MoveBrushesToWorld);
            Bind(wxEVT_MENU, &MapView2D::OnPopupCreatePointEntity,       this, CommandIds::MapViewPopupMenu::LowestPointEntityItem, CommandIds::MapViewPopupMenu::HighestPointEntityItem);
            Bind(wxEVT_MENU, &MapView2D::OnPopupCreateBrushEntity,       this, CommandIds::MapViewPopupMenu::LowestBrushEntityItem, CommandIds::MapViewPopupMenu::HighestBrushEntityItem);
            
            Bind(wxEVT_UPDATE_UI, &MapView2D::OnUpdatePopupMenuItem, this, CommandIds::MapViewPopupMenu::ReparentBrushes);
            Bind(wxEVT_UPDATE_UI, &MapView2D::OnUpdatePopupMenuItem, this, CommandIds::MapViewPopupMenu::MoveBrushesToWorld);
            Bind(wxEVT_UPDATE_UI, &MapView2D::OnUpdatePopupMenuItem, this, CommandIds::MapViewPopupMenu::LowestPointEntityItem, CommandIds::MapViewPopupMenu::HighestPointEntityItem);
            Bind(wxEVT_UPDATE_UI, &MapView2D::OnUpdatePopupMenuItem, this, CommandIds::MapViewPopupMenu::LowestBrushEntityItem, CommandIds::MapViewPopupMenu::HighestBrushEntityItem);
            */
        }

        PickRequest MapView2D::doGetPickRequest(const int x, const int y) const {
            return PickRequest(Ray3(m_camera.pickRay(x, y)), m_camera);
        }
        
        Model::PickResult MapView2D::doPick(const Ray3& pickRay) const {
            MapDocumentSPtr document = lock(m_document);
            const Model::EditorContext& editorContext = document->editorContext();
            const Math::Axis::Type axis = pickRay.direction.firstComponent();
            
            Model::PickResult pickResult = Model::PickResult::bySize(editorContext, axis);
            document->pick(pickRay, pickResult);
            
            return pickResult;
        }
        
        void MapView2D::doUpdateViewport(const int x, const int y, const int width, const int height) {
            m_camera.setViewport(Renderer::Camera::Viewport(x, y, width, height));
        }

        Vec3 MapView2D::doGetPasteObjectsDelta(const BBox3& bounds) const {
            // TODO: implement this
            return Vec3::Null;
        }
        
        void MapView2D::doCenterCameraOnSelection() {
            const MapDocumentSPtr document = lock(m_document);
            assert(!document->selectedNodes().empty());
            
            const BBox3& bounds = document->selectionBounds();
            moveCameraToPosition(bounds.center());
        }
        
        void MapView2D::doMoveCameraToPosition(const Vec3& position) {
            animateCamera(Vec3f(position), m_camera.direction(), m_camera.up());
        }
        
        void MapView2D::animateCamera(const Vec3f& position, const Vec3f& direction, const Vec3f& up, const wxLongLong duration) {
            const Vec3f actualPosition = position.dot(m_camera.up()) * m_camera.up() + position.dot(m_camera.right()) * m_camera.right() + m_camera.position().dot(m_camera.direction()) * m_camera.direction();
            CameraAnimation* animation = new CameraAnimation(m_camera, actualPosition, m_camera.direction(), m_camera.up(), duration);
            m_animationManager->runAnimation(animation, true);
        }
        
        void MapView2D::doMoveCameraToCurrentTracePoint() {
            MapDocumentSPtr document = lock(m_document);
            
            assert(document->isPointFileLoaded());
            Model::PointFile* pointFile = document->pointFile();
            assert(pointFile->hasNextPoint());
            
            const Vec3f position = pointFile->currentPoint();
            moveCameraToPosition(position);
        }

        Vec3 MapView2D::doGetMoveDirection(const Math::Direction direction) const {
            switch (direction) {
                case Math::Direction_Forward:
                    return m_camera.direction().firstAxis();
                case Math::Direction_Backward:
                    return -m_camera.direction().firstAxis();
                case Math::Direction_Left:
                    return -m_camera.right().firstAxis();
                case Math::Direction_Right:
                    return m_camera.right().firstAxis();
                case Math::Direction_Up:
                    return m_camera.up().firstAxis();
                case Math::Direction_Down:
                    return -m_camera.up().firstAxis();
                DEFAULT_SWITCH()
            }
        }

        Vec3 MapView2D::doComputePointEntityPosition(const BBox3& bounds) const {
            MapDocumentSPtr document = lock(m_document);
            const BBox3 referenceBounds = document->referenceBounds();
            const Ray3& pickRay = MapView2D::pickRay();
            
            const Vec3 toMin = referenceBounds.min - pickRay.origin;
            const Vec3 toMax = referenceBounds.max - pickRay.origin;
            const Vec3 anchor = toMin.dot(pickRay.direction) > toMax.dot(pickRay.direction) ? referenceBounds.min : referenceBounds.max;
            const Plane3 dragPlane(anchor, -pickRay.direction);
            
            const FloatType distance = dragPlane.intersectWithRay(pickRay);
            if (Math::isnan(distance))
                return Vec3::Null;
            
            const BBox3& worldBounds = document->worldBounds();
            const Vec3 hitPoint = pickRay.pointAtDistance(distance);
            
            const Grid& grid = document->grid();
            return grid.moveDeltaForBounds(dragPlane, bounds, worldBounds, pickRay, hitPoint);
        }

        ActionContext MapView2D::doGetActionContext() const {
            return ActionContext_Default;
        }
        
        wxAcceleratorTable MapView2D::doCreateAccelerationTable(ActionContext context) const {
            ActionManager& actionManager = ActionManager::instance();
            return actionManager.createViewAcceleratorTable(context, ActionView_Map2D);
        }
        
        bool MapView2D::doCancel() {
            return false;
        }
        
        Renderer::RenderContext MapView2D::doCreateRenderContext() {
            return Renderer::RenderContext(Renderer::RenderContext::RenderMode_2D, m_camera, fontManager(), shaderManager());
        }
        
        void MapView2D::doRenderGrid(Renderer::RenderContext& renderContext, Renderer::RenderBatch& renderBatch) {
            MapDocumentSPtr document = lock(m_document);
            renderBatch.addOneShot(new Renderer::GridRenderer(m_camera, document->worldBounds()));
        }

        void MapView2D::doRenderMap(Renderer::MapRenderer& renderer, Renderer::RenderContext& renderContext, Renderer::RenderBatch& renderBatch) {
            renderer.render(renderContext, renderBatch);

            MapDocumentSPtr document = lock(m_document);
            if (document->hasSelectedNodes()) {
                const BBox3& bounds = document->selectionBounds();
                Renderer::SelectionBoundsRenderer boundsRenderer(bounds);
                boundsRenderer.render(renderContext, renderBatch);
            }
        }
        
        void MapView2D::doRenderTools(MapViewToolBox& toolBox, Renderer::RenderContext& renderContext, Renderer::RenderBatch& renderBatch) {
            renderTools(renderContext, renderBatch);
        }
        
        void MapView2D::doRenderExtras(Renderer::RenderContext& renderContext, Renderer::RenderBatch& renderBatch) {}
        
        void MapView2D::doLinkCamera(CameraLinkHelper& helper) {
            helper.addCamera(&m_camera);
        }
    }
}