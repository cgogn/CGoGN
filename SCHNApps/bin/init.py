importPlugin = schnapps.loadPlugin("ImportSurface");
differentialPropertiesPlugin = schnapps.loadPlugin("DifferentialProperties");
renderPlugin = schnapps.loadPlugin("Render");
renderVectorPlugin = schnapps.loadPlugin("RenderVector");

obj = importPlugin.importFromFile("/home/kraemer/Media/Data/surface/midRes/bimba_75k.off");

v = schnapps.getView("view_0");

schnapps.linkViewAndPlugin(v.getName(), renderPlugin.getName());
schnapps.linkViewAndPlugin(v.getName(), renderVectorPlugin.getName());
schnapps.linkViewAndMap(v.getName(), obj.getName());

differentialPropertiesPlugin.computeNormal(obj.getName());
differentialPropertiesPlugin.computeCurvature(obj.getName());
