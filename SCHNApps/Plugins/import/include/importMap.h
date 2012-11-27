#ifndef _IMPORTMAP_H_
#define _IMPORTMAP_H_


#include "visualPlugin.h"

#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"

#include "Geometry/vector_gen.h"
#include "Geometry/matrix.h"

#include "Algo/Import/import.h"
#include "Algo/Export/export.h"

#include "Algo/Render/GL2/mapRender.h"
#include "Algo/Render/GL2/topoRender.h"

#include "Utils/Shaders/shaderPhong.h"
#include "Utils/Shaders/shaderFlat.h"
#include "Utils/Shaders/shaderSimpleColor.h"
#include "Utils/Shaders/shaderVectorPerVertex.h"
#include "Utils/pointSprite.h"
#include "Utils/text3d.h"
#include "Utils/vboRender.h"

#include "Utils/Qt/qtInputs.h"

#include "Algo/Geometry/boundingbox.h"
#include "Algo/Geometry/normal.h"



using namespace CGoGN ;

struct PFP: public PFP_STANDARD
{
	// definition of the map
	typedef EmbeddedMap2 MAP ;
};


class ImportMap : public VisualPlugin{
	Q_OBJECT
	Q_INTERFACES(Plugin)
public:
	ImportMap();
	~ImportMap();

	void cb_updateMatrix(View* view){}

	void cb_redraw(Scene* scene){}

	void cb_initGL(Scene* scene){}



	bool activate();
	void disable();

protected:
	bool importMap(QString filename);

protected slots:
	void saveMap();

};


#endif
