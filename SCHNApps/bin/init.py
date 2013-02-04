importPlugin = schnapps.loadPlugin("ImportSurface");
renderPlugin = schnapps.loadPlugin("Render");
renderVectorPlugin = schnapps.loadPlugin("RenderVector");
differentialPropertiesPlugin = schnapps.loadPlugin("DifferentialProperties");
subdivisionPlugin = schnapps.loadPlugin("SubdivideSurface");

#obj = importPlugin.importFromFile("/home/untereiner/Developments/meshes/egea.off");

v = schnapps.getView("view_0");

schnapps.linkViewAndPlugin(v.getName(), renderPlugin.getName());
schnapps.linkViewAndPlugin(v.getName(), renderVectorPlugin.getName());
#schnapps.linkViewAndMap(v.getName(), obj.getName());

#differentialPropertiesPlugin.computeNormal(obj.getName());
#differentialPropertiesPlugin.computeCurvature(obj.getName());
