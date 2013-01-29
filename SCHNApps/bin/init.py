importPlugin = schnapps.loadPlugin("ImportSurface");
differentialPropertiesPlugin = schnapps.loadPlugin("DifferentialProperties");
renderPlugin = schnapps.loadPlugin("Render");

hand = importPlugin.importFromFile("/home/kraemer/Media/Data/surface/midRes/handman_12k.off");

v = schnapps.getView("view_0");

schnapps.linkViewAndPlugin(v.getName(), renderPlugin.getName());
schnapps.linkViewAndMap(v.getName(), hand.getName());
