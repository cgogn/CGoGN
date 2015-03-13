#include "surface_modelisation.h"

#include "schnapps.h"
#include "view.h"
#include "mapHandler.h"

#include "Algo/Modelisation/polyhedron.h"
#include "Algo/Modelisation/extrusion.h"
#include <QMessageBox>
#include <QMouseEvent>
#include <iostream>
#include "Topology/generic/cellmarker.h"

namespace CGoGN
{

namespace SCHNApps
{

Surface_Modelisation_Plugin::Surface_Modelisation_Plugin()
{
    collect = false;
}

bool Surface_Modelisation_Plugin::enable()
{
	//	magic line that init static variables of GenericMap in the plugins
		GenericMap::copyAllStatics(m_schnapps->getStaticPointers());

	m_dockTab = new Surface_Modelisation_DockTab(m_schnapps, this);
	m_schnapps->addPluginDockTab(this, m_dockTab, "Surface_Modelisation");

	connect(m_schnapps, SIGNAL(selectedMapChanged(MapHandlerGen*, MapHandlerGen*)), this, SLOT(selectedMapChanged(MapHandlerGen*, MapHandlerGen*)));
	connect(m_schnapps, SIGNAL(mapAdded(MapHandlerGen*)), this, SLOT(mapAdded(MapHandlerGen*)));
	connect(m_schnapps, SIGNAL(mapRemoved(MapHandlerGen*)), this, SLOT(mapRemoved(MapHandlerGen*)));

	foreach(MapHandlerGen* map, m_schnapps->getMapSet().values())
		mapAdded(map);

	m_dockTab->updateMapParameters();

    m_drawer = new Utils::Drawer();
    registerShader(m_drawer->getShader());

    mapNumber = 1;

	return true;
}

void Surface_Modelisation_Plugin::disable()
{
	disconnect(m_schnapps, SIGNAL(selectedMapChanged(MapHandlerGen*, MapHandlerGen*)), this, SLOT(selectedMapChanged(MapHandlerGen*, MapHandlerGen*)));
    disconnect(m_schnapps, SIGNAL(mapAdded(MapHandlerGen*)), this, SLOT(mapAdded(MapHandlerGen*)));
    disconnect(m_schnapps, SIGNAL(mapRemoved(MapHandlerGen*)), this, SLOT(mapRemoved(MapHandlerGen*)));

    delete m_drawer;
}

void Surface_Modelisation_Plugin::draw(View *view)
{
        if(collect)
        {
            m_drawer->newList(GL_COMPILE_AND_EXECUTE);
            m_drawer->pointSize(3.0f);
            m_drawer->color3f(1.0f, 1.0f, 1.0f);
            m_drawer->begin(GL_POINTS);
            for (unsigned int i = 0; i < collectedVertices.size(); i++)
                m_drawer->vertex(collectedVertices[i]);
            m_drawer->end();
            m_drawer->endList();
        }
}

void Surface_Modelisation_Plugin::drawMap(View* view, MapHandlerGen* map)
{

}

void Surface_Modelisation_Plugin::mousePress(View* view, QMouseEvent* event)
{
    if(collect && (event->button() == Qt::LeftButton))
    {
        qglviewer::Vec point(event->x(), event->y(), 0.5);
        qglviewer::Vec vertex = view->camera()->unprojectedCoordinatesOf(point);
        PFP2::VEC3 position(vertex.x, vertex.y, vertex.z);
        collectedVertices.push_back(position);
        view->updateGL();
    }

}

void Surface_Modelisation_Plugin::selectedMapChanged(MapHandlerGen *prev, MapHandlerGen *cur)
{
	m_dockTab->updateMapParameters();
	if (cur==NULL)
		m_dockTab->setDisabled(true);
	else
		m_dockTab->setDisabled(false);
}

void Surface_Modelisation_Plugin::mapAdded(MapHandlerGen* map)
{
	connect(map, SIGNAL(attributeAdded(unsigned int, const QString&)), this, SLOT(attributeAdded(unsigned int, const QString&)));
	connect(map, SIGNAL(cellSelectorAdded(unsigned int, const QString&)), this, SLOT(cellSelectorAdded(unsigned int, const QString&)));
	connect(map, SIGNAL(cellSelectorRemoved(unsigned int, const QString&)), this, SLOT(cellSelectorRemoved(unsigned int, const QString&)));
}

void Surface_Modelisation_Plugin::mapRemoved(MapHandlerGen* map)
{
	disconnect(map, SIGNAL(attributeAdded(unsigned int, const QString&)), this, SLOT(attributeAdded(unsigned int, const QString&)));
    disconnect(map, SIGNAL(cellSelectorAdded(unsigned int, const QString&)), this, SLOT(cellSelectorAdded(unsigned int, const QString&)));
    disconnect(map, SIGNAL(cellSelectorRemoved(unsigned int, const QString&)), this, SLOT(cellSelectorRemoved(unsigned int, const QString&)));
}

void Surface_Modelisation_Plugin::attributeAdded(unsigned int orbit, const QString& name)
{
	MapHandlerGen* map = static_cast<MapHandlerGen*>(QObject::sender());
	if(orbit == VERTEX && map->isSelectedMap())
		m_dockTab->addVertexAttribute(name);
}

void Surface_Modelisation_Plugin::cellSelectorAdded(unsigned int orbit, const QString& name)
{
	MapHandlerGen* map = static_cast<MapHandlerGen*>(QObject::sender());

	if(map->isSelectedMap())
	{
		switch(orbit)
		{
			case VERTEX :
				m_dockTab->addVertexSelector(name);
				break;
			case EDGE :
				m_dockTab->addEdgeSelector(name);
				break;
			case FACE :
				m_dockTab->addFaceSelector(name);
				break;
		}
	}
}

void Surface_Modelisation_Plugin::cellSelectorRemoved(unsigned int orbit, const QString& name)
{
	MapHandlerGen* map = static_cast<MapHandlerGen*>(QObject::sender());

	if(map->isSelectedMap())
	{
		switch(orbit)
		{
			case VERTEX :
				m_dockTab->removeVertexSelector(name);
				break;
			case EDGE :
				m_dockTab->removeEdgeSelector(name);
				break;
			case FACE :
				m_dockTab->removeFaceSelector(name);
				break;
		}
	}
}

void Surface_Modelisation_Plugin::changePositionAttribute(const QString& view, const QString& map, const QString& name)
{
	MapHandlerGen* m = m_schnapps->getMap(map);
	if(m)
	{
		MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(m);
		h_parameterSet[m].positionAttribute = mh->getAttribute<PFP2::VEC3, VERTEX>(name);
		if(m->isSelectedMap())
			m_dockTab->updateMapParameters();
	}
}

void Surface_Modelisation_Plugin::changeVertexSelector(const QString& map, const QString& name)
{
	MapHandlerGen* m = m_schnapps->getMap(map);
	if(m)
	{
		MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(m);
		h_parameterSet[m].vertexSelector = mh->getCellSelector<VERTEX>(name);
		if(m->isSelectedMap())
			m_dockTab->updateMapParameters();
	}
}

void Surface_Modelisation_Plugin::changeEdgeSelector(const QString& map, const QString& name)
{
	MapHandlerGen* m = m_schnapps->getMap(map);
	if(m)
	{
		MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(m);
		h_parameterSet[m].edgeSelector = mh->getCellSelector<EDGE>(name);
		if(m->isSelectedMap())
			m_dockTab->updateMapParameters();
	}
}

void Surface_Modelisation_Plugin::changeFaceSelector(const QString& map, const QString& name)
{
	MapHandlerGen* m = m_schnapps->getMap(map);
	if(m)
	{
		MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(m);
		h_parameterSet[m].faceSelector = mh->getCellSelector<FACE>(name);
		if(m->isSelectedMap())
			m_dockTab->updateMapParameters();
	}
}

/*-------------------------------------------------------------------------------------*/
/* --------------------------------- OPERATIONS ---------------------------------------*/
/*-------------------------------------------------------------------------------------*/

/* --------------------------- general operations -------------------------------*/

void Surface_Modelisation_Plugin::createEmptyMap()
{
    QString mapName = "map_" + QString::number(mapNumber);
    MapHandlerGen* mhg = m_schnapps->addMap(mapName, 2);
	if(mhg)
	{
		MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(mhg);

		// add vertex position attribute
		VertexAttribute<PFP2::VEC3, PFP2::MAP> position = mh->addAttribute<PFP2::VEC3, VERTEX>("position");
		// update corresponding VBO & emit attribute update signal
		mh->notifyAttributeModification(position);
        mapNumber++;
	}
}

void Surface_Modelisation_Plugin::createNewFace(MapHandlerGen* mhg)
{
    MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(mhg);
    PFP2::MAP* map = mh->getMap();
    if (h_parameterSet[mhg].positionAttribute.isValid())
    {
		VertexAttribute<PFP2::VEC3, PFP2::MAP>& position = h_parameterSet[mhg].positionAttribute;
        if (collectedVertices.size() >= 3)
        {
            Dart d = map->newFace((unsigned int)(collectedVertices.size()));

            int i = 0;
            Traversor2FV<PFP2::MAP> t(*map, d);
            for(Dart it = t.begin(); it != t.end(); it = t.next())
            {
                position[it] = collectedVertices[i];
                i++;
            }

            mh->notifyConnectivityModification();
			mh->notifyAttributeModification(position);
            collectedVertices.clear();
        }
        else QMessageBox::information(0, "Attention", "To create a face you need at least 3 vertices");
    }

}

void Surface_Modelisation_Plugin::addCube(MapHandlerGen *mhg)
{
	MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(mhg);
    PFP2::MAP* map = mh->getMap();
    if (h_parameterSet[mhg].positionAttribute.isValid())
    {
		VertexAttribute<PFP2::VEC3, PFP2::MAP>& position = h_parameterSet[mhg].positionAttribute;

        Algo::Surface::Modelisation::embedPrism<PFP2>(*map, position, 4, true, 0.7f, 0.7f, 1.0f);

        mh->notifyAttributeModification(position);
		mh->notifyConnectivityModification();
    }
}

void Surface_Modelisation_Plugin::mergeVolumes(MapHandlerGen* mhg)
{
    MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(mhg);
    PFP2::MAP* map = mh->getMap();

    const MapParameters& p = h_parameterSet[mhg];
    if(p.edgeSelector && !p.edgeSelector->getSelectedCells().empty() && (p.edgeSelector->getSelectedCells().size() == 2) &&
       p.faceSelector && !p.faceSelector->getSelectedCells().empty() && (p.faceSelector->getSelectedCells().size() == 2))
    {
		const std::vector<Edge>& selectedDarts = p.edgeSelector->getSelectedCells();
		const std::vector<Face>& selectedFaces = p.faceSelector->getSelectedCells();

        Dart d1 = selectedDarts[0];
        Dart d2 = selectedDarts[1];
        Dart f1 = selectedFaces[0];
        Dart f2 = selectedFaces[1];
        int i = 1, j = 1;
        bool isWellSelected = true;

        if (!(map->sameFace(d1,f1) && (map->sameFace(d2,f2))))
        {
            if (!(map->sameFace(d1,f2) && (map->sameFace(d2,f1))))
            {
                if ((map->sameFace(map->phi2(d1),f1)) || (map->sameFace(map->phi2(d1),f2)))
                    d1 = map->phi2(d1);

                if ((map->sameFace(map->phi2(d2),f1)) || (map->sameFace(map->phi2(d2),f2)))
                    d2 = map->phi2(d2);

                if (map->sameFace(d1,d2) ||
                    !((map->sameFace(d1,f1) && (map->sameFace(d2,f2))) ||
                      (map->sameFace(d1,f2) && (map->sameFace(d2,f1)))))
                    isWellSelected = false;
            }
        }

        if(isWellSelected)
        {
            Dart it = map->phi1(d1);
            do
            {
                it = map->phi1(it);
                i++;
            }
            while(it != d1);

            it = map->phi1(d2);
            do
            {
                it = map->phi1(it);
                j++;
            }
            while(it != d2);

            if(i == j)
            {
                map->mergeVolumes(d1, d2);
                mh->notifyConnectivityModification();
            }
            else QMessageBox::information(0, "Attention", "Selected faces should have the same number of edges");
        }
    }
}

void Surface_Modelisation_Plugin::splitSurface(MapHandlerGen* mhg)
{
    MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(mhg);
    PFP2::MAP* map = mh->getMap();

    MapParameters& p = h_parameterSet[mhg];
    if(p.edgeSelector && !p.edgeSelector->getSelectedCells().empty())
    {
		std::vector<Edge> selectedDarts = p.edgeSelector->getSelectedCells();
		std::vector<Dart> path;
		for (std::vector<Edge>::iterator it = selectedDarts.begin(); it != selectedDarts.end(); ++it)
			path.push_back((*it).dart);

        bool isPath = true;
        unsigned int i = 0;

        if (!(map->sameVertex(path[0], map->phi1(path[path.size()-1])) ||
              map->sameVertex(path[0], map->phi_1(path[path.size()-1]))))
        {
            if (map->sameVertex(map->phi2(path[0]), map->phi1(path[path.size()-1])) ||
                map->sameVertex(map->phi2(path[0]), map->phi_1(path[path.size()-1])))
                    path[0] = map->phi2(path[0]);
            else isPath = false;
        }

		CellMarker<PFP2::MAP, VERTEX> vm(*map);

        if (isPath)
        {
			for(std::vector<Dart>::iterator it = path.begin() ; it != path.end()-1; ++it)
            {
                if (isPath)
				{
					if(vm.isMarked(*it))
                        isPath = false;
					vm.mark(*it);

					std::vector<Dart>::iterator next;
                    next = it + 1 ;

                    if (!map->sameVertex(map->phi1(*it), *next))
                    {
                        if (map->sameVertex(map->phi1(*it), map->phi2(*next)))
                        {
                            path[i+1] = map->phi2(*next);
                        }
                        else isPath = false;
                    }
                    i++;
                }
                else break;
            }
        }

        if(isPath)
        {
            map->splitSurface(path);
            mh->notifyConnectivityModification();
            mh->notifyAttributeModification(p.positionAttribute);
        }
        else QMessageBox::information(0, "Attention", "To split surface you need to select edges in order without intersections and first edge should be connected with last one");
    }
}

void Surface_Modelisation_Plugin::extrudeRegion(MapHandlerGen *mhg)
{
    MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(mhg);
    PFP2::MAP* map = mh->getMap();

    MapParameters& p = h_parameterSet[mhg];
    if(p.faceSelector && !p.faceSelector->getSelectedCells().empty())
    {
		const std::vector<Face>& selectedDarts = p.faceSelector->getSelectedCells();
        Algo::Surface::Modelisation::extrudeRegion<PFP2>(*map, p.positionAttribute, selectedDarts[0], p.faceSelector->getMarker());

        mh->notifyConnectivityModification();
		mh->notifyAttributeModification(p.positionAttribute);
    }
}

/* ------------------- operations with vertices ----------------------------*/

void Surface_Modelisation_Plugin::splitVertex(MapHandlerGen *mhg)
{
    MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(mhg);
    PFP2::MAP* map = mh->getMap();

    MapParameters& p = h_parameterSet[mhg];
    if(p.edgeSelector && !p.edgeSelector->getSelectedCells().empty() && (p.edgeSelector->getSelectedCells().size() == 2))
    {
		const std::vector<Edge>& selectedEdges = p.edgeSelector->getSelectedCells();

        Dart d0, d1;
		unsigned int v00 = map->getEmbedding<VERTEX>(selectedEdges[0].dart);
		unsigned int v01 = map->getEmbedding<VERTEX>(map->phi2(selectedEdges[0].dart));
		unsigned int v10 = map->getEmbedding<VERTEX>(selectedEdges[1].dart);
		unsigned int v11 = map->getEmbedding<VERTEX>(map->phi2(selectedEdges[1].dart));

        if (v00 == v10)
        {
            d0 = selectedEdges[0];
            d1 = selectedEdges[1];
        }
        else if (v00 == v11)
        {
            d0 = selectedEdges[0];
            d1 = map->phi2(selectedEdges[1]);
        }
        else if (v01 == v10)
        {
            d0 = map->phi2(selectedEdges[0]);
            d1 = selectedEdges[1];
        }
        else if (v01 == v11)
        {
            d0 = map->phi2(selectedEdges[0]);
            d1 = map->phi2(selectedEdges[1]);
        }
        else
        {
            QMessageBox::information(0, "Attention", "Selected edges don't belong to the same vertex");
            return;
        }

        int n = 3;
        PFP2::VEC3 pos0 = p.positionAttribute[d0] + p.positionAttribute[map->phi1(d0)];
        Dart d = map->phi_1(d0);
        while( d != map->phi2(d1))
        {
            pos0 += p.positionAttribute[d];
            d = map->phi_1(map->phi2(d));
            n++;
        }
        pos0 += p.positionAttribute[d];
        pos0 = pos0/n;

        n = 3;
        PFP2::VEC3 pos1 = p.positionAttribute[d1] + p.positionAttribute[map->phi1(d1)];
        d = map->phi_1(d1);
        while( d != map->phi2(d0))
        {
            pos1 += p.positionAttribute[d];
            d = map->phi_1(map->phi2(d));
            n++;
        }
        pos1 += p.positionAttribute[d];
        pos1 = pos1/n;

        map->splitVertex(d0, d1);
        p.positionAttribute[d0] = pos0;
        p.positionAttribute[d1] = pos1;

        mh->notifyAttributeModification(p.positionAttribute);
        mh->notifyConnectivityModification();
    }
}

void Surface_Modelisation_Plugin::deleteVertex(MapHandlerGen *mhg)
{
    MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(mhg);
    PFP2::MAP* map = mh->getMap();

    const MapParameters& p = h_parameterSet[mhg];
    if(p.vertexSelector && !p.vertexSelector->getSelectedCells().empty())
    {
		const std::vector<Vertex>& darts = p.vertexSelector->getSelectedCells();
        map->deleteVertex(darts[0]);
        mh->notifyAttributeModification(p.positionAttribute);
        mh->notifyConnectivityModification();
    }
}

/* ------------------- operations with edges ----------------------------*/

void Surface_Modelisation_Plugin::cutEdge(MapHandlerGen *mhg)
{
    MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(mhg);
    PFP2::MAP* map = mh->getMap();

    MapParameters& p = h_parameterSet[mhg];
    if(p.edgeSelector && !p.edgeSelector->getSelectedCells().empty())
    {
		const std::vector<Edge>& edge = p.edgeSelector->getSelectedCells();
        map->cutEdge(edge[0]);

		p.positionAttribute[map->phi1(edge[0].dart)] = (p.positionAttribute[edge[0].dart] + p.positionAttribute[map->phi1(map->phi1(edge[0].dart))]) / 2;
        mh->notifyAttributeModification(p.positionAttribute);
        mh->notifyConnectivityModification();
    }
}

void Surface_Modelisation_Plugin::uncutEdge(MapHandlerGen *mhg)
{
    MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(mhg);
    PFP2::MAP* map = mh->getMap();

    const MapParameters& p = h_parameterSet[mhg];
    if(p.edgeSelector && !p.edgeSelector->getSelectedCells().empty())
    {
		const std::vector<Edge>& edge = p.edgeSelector->getSelectedCells();
        if (!map->uncutEdge(edge[0]))
            map->uncutEdge(map->phi2(edge[0]));
        mh->notifyAttributeModification(p.positionAttribute);
        mh->notifyConnectivityModification();
    }
}

void Surface_Modelisation_Plugin::collapseEdge(MapHandlerGen *mhg)
{
    MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(mhg);
    PFP2::MAP* map = mh->getMap();

    const MapParameters& p = h_parameterSet[mhg];
    if(p.edgeSelector && !p.edgeSelector->getSelectedCells().empty())
    {
		const std::vector<Edge>& edge = p.edgeSelector->getSelectedCells();
        map->collapseEdge(edge[0]);
        mh->notifyAttributeModification(p.positionAttribute);
        mh->notifyConnectivityModification();
    }
}

void Surface_Modelisation_Plugin::flipEdge(MapHandlerGen *mhg)
{
	MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(mhg);
	PFP2::MAP* map = mh->getMap();

	const MapParameters& p = h_parameterSet[mhg];
	if(p.edgeSelector && !p.edgeSelector->getSelectedCells().empty())
	{
		const std::vector<Edge>& edge = p.edgeSelector->getSelectedCells();
        map->flipEdge(edge[0]);
        mh->notifyConnectivityModification();
	}
}

void Surface_Modelisation_Plugin::flipBackEdge(MapHandlerGen *mhg)
{
    MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(mhg);
    PFP2::MAP* map = mh->getMap();

    const MapParameters& p = h_parameterSet[mhg];
    if(p.edgeSelector && !p.edgeSelector->getSelectedCells().empty())
    {
		const std::vector<Edge>& edge = p.edgeSelector->getSelectedCells();
        map->flipBackEdge(edge[0]);
        mh->notifyConnectivityModification();
    }
}

/* ------------------- operations with faces ----------------------------*/

void Surface_Modelisation_Plugin::splitFace(MapHandlerGen *mhg)
{
    MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(mhg);
    PFP2::MAP* map = mh->getMap();

    const MapParameters& p = h_parameterSet[mhg];
    if(p.vertexSelector && !p.vertexSelector->getSelectedCells().empty() && (p.vertexSelector->getSelectedCells().size() == 2))
    {
		const std::vector<Vertex>& selectedDarts = p.vertexSelector->getSelectedCells();
        bool sameFace = false;

        Dart d = selectedDarts[0];
        Dart it = d;
        Dart it2;
        do
        {
            Dart e = selectedDarts[1];
            it2 = e;
            do
            {
                if (map->sameFace(it,it2))
                {
                    sameFace = true;
                    break;
                }
                it2 = map->phi2(map->phi_1(it2));
            } while (it2 != e);
            if (sameFace) break;
            it = map->phi2(map->phi_1(it));
        } while (it != d);

        if (sameFace)
        {
            map->splitFace(it, it2);
            mh->notifyConnectivityModification();
        }
        else QMessageBox::information(0, "Attention", "Selected points don't belong to the same face");
    }
    else QMessageBox::information(0, "Attention", "To split a face select 2 points of the same face");
}

void Surface_Modelisation_Plugin::mergeFaces(MapHandlerGen *mhg)
{
    MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(mhg);
    PFP2::MAP* map = mh->getMap();

    const MapParameters& p = h_parameterSet[mhg];
    if(p.edgeSelector && !p.edgeSelector->getSelectedCells().empty())
    {
		const std::vector<Edge>& edge = p.edgeSelector->getSelectedCells();
        map->mergeFaces(edge[0]);
        mh->notifyConnectivityModification();
    }
}

void Surface_Modelisation_Plugin::deleteFace(MapHandlerGen *mhg)
{
    MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(mhg);
    PFP2::MAP* map = mh->getMap();

    const MapParameters& p = h_parameterSet[mhg];
    if(p.faceSelector && !p.faceSelector->getSelectedCells().empty())
    {
		const std::vector<Face>& dart = p.faceSelector->getSelectedCells();
        map->deleteFace(dart[0]);
        mh->notifyConnectivityModification();
    }
}

void Surface_Modelisation_Plugin::sewFaces(MapHandlerGen *mhg)
{
    MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(mhg);
    PFP2::MAP* map = mh->getMap();

    const MapParameters& p = h_parameterSet[mhg];
    if(p.edgeSelector && !p.edgeSelector->getSelectedCells().empty() && (p.edgeSelector->getSelectedCells().size() == 2))
    {
		const std::vector<Edge>& selectedDarts = p.edgeSelector->getSelectedCells();
        Dart d = selectedDarts[0];
        Dart e = selectedDarts[1];

        if (map->isBoundaryEdge(d) && map->isBoundaryEdge(e))
        {
            map->sewFaces(d,e);
            mh->notifyConnectivityModification();
        }
        else QMessageBox::information(0, "Attention", "One or two of selcted faces are already sewed with another face");
    }
    else QMessageBox::information(0, "Attention", "To sew faces select 2 edges which you want to connect");
}

void Surface_Modelisation_Plugin::unsewFaces(MapHandlerGen *mhg)
{
    MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(mhg);
    PFP2::MAP* map = mh->getMap();

    MapParameters& p = h_parameterSet[mhg];
    if(p.edgeSelector && !p.edgeSelector->getSelectedCells().empty())
    {
		const std::vector<Edge>& dart = p.edgeSelector->getSelectedCells();
        if (!map->isBoundaryEdge(dart[0]))
        {
            Dart d = dart[0];
            PFP2::VEC3 pos1 = p.positionAttribute[d];
            PFP2::VEC3 pos2 = p.positionAttribute[map->phi1(d)];
            Dart e = map->phi2(d);

            map->unsewFaces(dart[0]);

            p.positionAttribute[d] = pos1;
            p.positionAttribute[map->phi1(d)] = pos2;
            p.positionAttribute[e] = pos2;
            p.positionAttribute[map->phi1(e)] = pos1;

            mh->notifyConnectivityModification();
            mh->notifyAttributeModification(p.positionAttribute);
        }
        else QMessageBox::information(0, "Attention", "One or two of selcted faces are already unsewed");
    }
}

void Surface_Modelisation_Plugin::extrudeFace(MapHandlerGen *mhg)
{
    MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(mhg);
    PFP2::MAP* map = mh->getMap();

    MapParameters& p = h_parameterSet[mhg];
    if(p.faceSelector && !p.faceSelector->getSelectedCells().empty())
    {
		const std::vector<Face>& selectedDart = p.faceSelector->getSelectedCells();

        Dart d = selectedDart[0];
        PFP2::REAL dist = 0;
        int n = 0;
        Dart it = d;
        do
        {
            dist = dist + (p.positionAttribute[it] - p.positionAttribute[map->phi1(it)]).norm();
            it = map->phi1(it);
            n++;
        } while (it != d);
        dist = dist/n;

        Algo::Surface::Modelisation::extrudeFace<PFP2>(*map, p.positionAttribute, d, dist);
        mh->notifyConnectivityModification();
		mh->notifyAttributeModification(p.positionAttribute);
    }
}

void Surface_Modelisation_Plugin::pathExtrudeFace(MapHandlerGen *mhg)
{
    MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(mhg);
    PFP2::MAP* map = mh->getMap();

    MapParameters& p = h_parameterSet[mhg];
	VertexAttribute<PFP2::VEC3, PFP2::MAP>& position = p.positionAttribute;
    if(p.faceSelector && !p.faceSelector->getSelectedCells().empty())
    {
		const std::vector<Face>& selectedDart = p.faceSelector->getSelectedCells();
        Dart d = selectedDart[0];
        for (unsigned int i = 0; i < collectedVertices.size(); i++)
        {
            Dart it = d;
            PFP2::VEC3 center;
            int n = 0;
            do
            {
                center += position[it];
                it = map->phi1(it);
                n++;
            } while (it != d);
            center = center / n;

            std::vector<PFP2::VEC3> norms;
            do
            {
                norms.push_back((center - position[it]));
                it = map->phi1(it);
            } while (it != d);

            PFP2::REAL dist = 0;
            dist = (collectedVertices[i] - center).norm();
            d = Algo::Surface::Modelisation::extrudeFace<PFP2>(*map, position, d, dist);

            it = d;
            n = 0;
            do
            {
                position[it] = collectedVertices[i] - norms[n];
                it = map->phi1(it);
                n++;
            } while (it != d);
        }

        mh->notifyConnectivityModification();
		mh->notifyAttributeModification(position);
        collectedVertices.clear();
    }
}

#if CGOGN_QT_DESIRED_VERSION == 5
	Q_PLUGIN_METADATA(IID "CGoGN.SCHNapps.Plugin")
#else
	Q_EXPORT_PLUGIN2(Surface_Modelisation_Plugin, Surface_Modelisation_Plugin)
#endif

} // namespace SCHNApps

} // namespace CGoGN
