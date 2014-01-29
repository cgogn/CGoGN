/*
Copyright 2013 Doxygraph project

This Source Code Form is subject to the terms of the Mozilla Public License,
version 2.0. If a copy of the MPL was not distributed with this file, You can
obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "cgraph.h"
#include "gvplugin.h"
#include "gvc.h"
#include <emscripten/bind.h>
#include <string>
#include <cstdlib>
#include <cstdio>

typedef struct gvlayout_engine_s gvlayout_engine_t;
typedef struct gvtextlayout_engine_s gvtextlayout_engine_t;
typedef struct gvevent_key_binding_s gvevent_key_binding_t;
#include "gvcint.h"

extern gvplugin_library_t gvplugin_dot_layout_LTX_library;
extern gvplugin_library_t gvplugin_neato_layout_LTX_library;
extern gvplugin_library_t gvplugin_core_LTX_library;

std::string wrap_gvcVersion(GVC_t *gvc)
{
  return gvcVersion(gvc);
}

std::string wrap_gvcBuildDate(GVC_t *gvc)
{
  return gvcBuildDate(gvc);
}

GVC_t *wrap_gvContext()
{
  GVC_t *gvc = gvContext();
  if (! gvc) return 0;
  gvAddLibrary(gvc, &gvplugin_dot_layout_LTX_library);
  gvAddLibrary(gvc, &gvplugin_neato_layout_LTX_library);
  gvAddLibrary(gvc, &gvplugin_core_LTX_library);
  return gvc;
}

int wrap_gvLayout(GVC_t *gvc, graph_t *g, std::string const& engine)
{
  return gvLayout(gvc, g, const_cast<char*>(engine.c_str()));
}

std::string gvRenderString(GVC_t *gvc, graph_t *g, std::string const& format)
{
  char *result = 0;
  unsigned int length = 0;
  int rc = gvRenderData(gvc, g, const_cast<char*>(format.c_str()),
			&result, &length);
  if (rc) return "";
  std::string s (result, length);
  std::free(result);
  size_t off = s.find("<!--");
  if (off != std::string::npos) s = s.substr(off);
  return s;
}

Agraph_t* wrap_agopen(std::string const& name, Agdesc_t kind)
{
  return agopen(const_cast<char*>(name.c_str()), kind, 0);
}

Agraph_t* wrap_agread(std::string const& filename)
{
  FILE* f = std::fopen(filename.c_str(), "r");
  if (! f) return 0;
  Agraph_t* g = agread(static_cast<void*>(f), 0);
  std::fclose(f);
  return g;
}

Agraph_t* wrap_agconcat(Agraph_t* g, std::string const& filename)
{
  FILE* f = std::fopen(filename.c_str(), "r");
  if (! f) return 0;
  g = agconcat(g, static_cast<void*>(f), 0);
  std::fclose(f);
  return g;
}

Agnode_t* wrap_agnode(Agraph_t* g, std::string const& name, int createflag)
{
  return agnode(g, const_cast<char*>(name.c_str()), createflag);
}

Agedge_t* wrap_agedge(Agraph_t* g, Agnode_t* t, Agnode_t* h,
	std::string const& name, int createflag)
{
  return agedge(g, t, h, const_cast<char*>(name.c_str()), createflag);
}

std::string aggraphget(Agraph_t* obj, std::string const& name)
{
  return agget(static_cast<void*>(obj), const_cast<char*>(name.c_str()));
}

int aggraphset(Agraph_t* obj, std::string const& name, std::string const& value)
{
  return agset(static_cast<void*>(obj), const_cast<char*>(name.c_str()),
		const_cast<char*>(value.c_str()));
}

std::string agnodeget(Agnode_t* obj, std::string const& name)
{
  return agget(static_cast<void*>(obj), const_cast<char*>(name.c_str()));
}

int agnodeset(Agnode_t* obj, std::string const& name, std::string const& value)
{
  return agset(static_cast<void*>(obj), const_cast<char*>(name.c_str()),
		const_cast<char*>(value.c_str()));
}

std::string agedgeget(Agedge_t* obj, std::string const& name)
{
  return agget(static_cast<void*>(obj), const_cast<char*>(name.c_str()));
}

int agedgeset(Agedge_t* obj, std::string const& name, std::string const& value)
{
  return agset(static_cast<void*>(obj), const_cast<char*>(name.c_str()),
		const_cast<char*>(value.c_str()));
}

using namespace emscripten;

EMSCRIPTEN_BINDINGS(bindviz) {
  class_<GVC_s>("GVC_t");
  class_<Agraph_s>("Agraph_t");
  class_<Agnode_s>("Agnode_t");
  class_<Agedge_s>("Agedge_t");
  value_struct<Agdesc_s>("Agdesc_t");
  function("gvcVersion",	&wrap_gvcVersion,	allow_raw_pointers());
  function("gvcBuildDate",	&wrap_gvcBuildDate,	allow_raw_pointers());
  function("gvContext",		&wrap_gvContext,	allow_raw_pointers());
  function("gvLayout",		&wrap_gvLayout,		allow_raw_pointers());
  function("gvRenderString",	&gvRenderString,	allow_raw_pointers());
  function("gvFreeLayout",	&gvFreeLayout,		allow_raw_pointers());
  function("gvFreeContext",	&gvFreeContext,		allow_raw_pointers());
  function("agopen",		&wrap_agopen,		allow_raw_pointers());
  function("agclose",		&agclose,		allow_raw_pointers());
  function("agread",		&wrap_agread,		allow_raw_pointers());
  function("agconcat",		&wrap_agconcat,		allow_raw_pointers());
  function("agnnodes",		&agnnodes,		allow_raw_pointers());
  function("agnedges",		&agnedges,		allow_raw_pointers());
  function("agnode",		&agnode,		allow_raw_pointers());
  function("agidnode",		&agidnode,		allow_raw_pointers());
  function("agfstnode",		&agfstnode,		allow_raw_pointers());
  function("agnxtnode",		&agnxtnode,		allow_raw_pointers());
  function("agprvnode",		&agprvnode,		allow_raw_pointers());
  function("aglstnode",		&aglstnode,		allow_raw_pointers());
  function("agfirstnode",	&agfstnode,		allow_raw_pointers());
  function("agnextnode",	&agnxtnode,		allow_raw_pointers());
  function("agprevnode",	&agprvnode,		allow_raw_pointers());
  function("aglastnode",	&aglstnode,		allow_raw_pointers());
  function("agdelnode",		&agdelnode,		allow_raw_pointers());
  function("agdegree",		&agdegree,		allow_raw_pointers());
  function("agedge",		&wrap_agedge,		allow_raw_pointers());
  function("agfstedge",		&agfstedge,		allow_raw_pointers());
  function("agnxtedge",		&agnxtedge,		allow_raw_pointers());
  function("agfstin",		&agfstin,		allow_raw_pointers());
  function("agnxtin",		&agnxtin,		allow_raw_pointers());
  function("agfstout",		&agfstout,		allow_raw_pointers());
  function("agnxtout",		&agnxtout,		allow_raw_pointers());
  function("agfirstedge",	&agfstedge,		allow_raw_pointers());
  function("agnextedge",	&agnxtedge,		allow_raw_pointers());
  function("agfirstinedge",	&agfstin,		allow_raw_pointers());
  function("agnextinedge",	&agnxtin,		allow_raw_pointers());
  function("agfirstoutedge",	&agfstout,		allow_raw_pointers());
  function("agnextoutedge",	&agnxtout,		allow_raw_pointers());
  function("agdeledge",		&agdeledge,		allow_raw_pointers());
  function("agget",		&aggraphget,		allow_raw_pointers());
  function("agset",		&aggraphset,		allow_raw_pointers());
  function("agnodeget",		&agnodeget,		allow_raw_pointers());
  function("agnodeset",		&agnodeset,		allow_raw_pointers());
  function("agedgeget",		&agedgeget,		allow_raw_pointers());
  function("agedgeset",		&agedgeset,		allow_raw_pointers());
}

int main() { return 0; }