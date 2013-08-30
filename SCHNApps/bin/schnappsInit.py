v0 = schnapps.getView("view_0");

importPlugin = schnapps.enablePlugin("Surface_Import");
surfaceRender = schnapps.enablePlugin("Surface_Render");

v0.linkPlugin(surfaceRender.getName());

handman = importPlugin.importFromFile("/home/kraemer/Media/Data/surface/midRes/handman_34k.off");

v0.linkMap(handman.getName());

handman.createVBO("position");

surfaceRender.changePositionVBO(v0.getName(), handman.getName(), "position");
surfaceRender.changeRenderEdges(v0.getName(), handman.getName(), True);
