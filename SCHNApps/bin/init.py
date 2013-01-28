importPlugin = schnapps.loadPlugin("ImportSurfacePlugin");
differentialPropertiesPlugin = schnapps.loadPlugin("DifferentialPropertiesPlugin");
renderPlugin = schnapps.loadPlugin("RenderPlugin");

hand = importPlugin.importFromFile("/home/kraemer/Media/Data/surface/midRes/handman_12k.off");

v = schnapps.getView("view_0");

schnapps.linkViewAndPlugin(v.getName(), renderPlugin.getName());
schnapps.linkViewAndMap(v.getName(), hand.getName());
