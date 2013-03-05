importPlugin = schnapps.loadPlugin("ImportSurface");
renderPlugin = schnapps.loadPlugin("Render");
renderVectorPlugin = schnapps.loadPlugin("RenderVector");
renderTopoSurfacePlugin = schnapps.loadPlugin("RenderTopoSurfaceD");
differentialPropertiesPlugin = schnapps.loadPlugin("DifferentialProperties");
subdivisionPlugin = schnapps.loadPlugin("SubdivideSurface");
surfaceDeformationPlugin = schnapps.loadPlugin("SurfaceDeformation");

obj = importPlugin.importFromFile("/home/kraemer/Media/Data/surface/lowRes/iphi_good_9k.off");

v = schnapps.getView("view_0");

schnapps.linkViewAndPlugin(v.getName(), renderPlugin.getName());
schnapps.linkViewAndPlugin(v.getName(), renderVectorPlugin.getName());
schnapps.linkViewAndPlugin(v.getName(), renderTopoSurfacePlugin.getName());
schnapps.linkViewAndPlugin(v.getName(), surfaceDeformationPlugin.getName());

schnapps.linkViewAndMap(v.getName(), obj.getName());

differentialPropertiesPlugin.computeNormal(obj.getName());
differentialPropertiesPlugin.computeCurvature(obj.getName());
