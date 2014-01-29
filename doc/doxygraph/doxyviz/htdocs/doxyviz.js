/*
Copyright 2013 Doxygraph project

This Source Code Form is subject to the terms of the Mozilla Public License,
version 2.0. If a copy of the MPL was not distributed with this file, You can
obtain one at http://mozilla.org/MPL/2.0/.
*/

var Context;
var Graph;

function openGraph() {
    closeGraph();
    var svgarea = document.getElementById("svgarea");
    Context = Module.gvContext();
    if (! Context) {
        if (svgarea) svgarea.innerHTML = "gvContext() returned null";
    } else {
        Module.print("graphviz " + Module.gvcVersion(Context) +
                     " built on " + Module.gvcBuildDate(Context));
        Graph = Module.agread("/graph");
        if (! Graph) {
            if (svgarea) svgarea.innerHTML = "agread() returned null";
        } else {
            return true;
        }
    }
    return false;
}
function closeGraph() {
    if (Graph) { Module.agclose(Graph); Graph = null; }
    if (Context) { Module.gvFreeContext(Context); Context = null; }
}

var Module = { 'canvas':{} };
Module['preRun'] = function() {
    Module['FS_createPreloadedFile']('/', 'graph', 'graph.dot', true, false);
};
Module['postRun'] = function() {
    if (openGraph()) {
        var svgarea = document.getElementById("svgarea");
        var ctrlarea = document.getElementById("ctrlarea");
        if (! Graph) {
            svgarea.innerHTML = "agread returned null";
        } else {
            var classes = [];
            for (var n = Module.agfirstnode(Graph);
                  n; n = Module.agnextnode(Graph, n)) {
              var name = /^([\w:\.]+)/.exec(Module.agnodeget(n,"label"))[0];
              classes.push(name);
            }
            classes.sort();
            var html = "";
            for (var i = 0; i < classes.length; i++) {
              html += '<input type="checkbox" name="' + classes[i] + '">' +
                      classes[i] + "</input><br/>\n";
            }
            ctrlarea.innerHTML =
                '<form id="boxes" style="white-space:nowrap">' +
                html + "</form>";
        }
    }
    closeGraph();
};

function selectAll() {
    var boxes = document.getElementById("boxes").getElementsByTagName("input");
    for (var i = 0; i < boxes.length; i++) {
        boxes[i].checked = true;
    }
}
function selectNone() {
    var boxes = document.getElementById("boxes").getElementsByTagName("input");
    for (var i = 0; i < boxes.length; i++) {
        boxes[i].checked = false;
    }
}
function showDiagram() {
    if (openGraph()) {
        var svgarea = document.getElementById("svgarea");
        var boxes = document.getElementById("boxes").getElementsByTagName("input");
        var classes = {};
        var classcnt = 0;
        for (var i = 0; i < boxes.length; i++) {
            classes[boxes[i].name] = boxes[i].checked;
            if (boxes[i].checked) classcnt++;
        }
        if (0 == classcnt) {
            alert("You haven't selected any classes.");
        } else {
            if (classcnt < 50 || confirm("You have selected " +
                "more than 50 classes. This may take some time."))
            {
                var delnodes = [];
                for (var n = Module.agfirstnode(Graph);
                      n; n = Module.agnextnode(Graph, n)) {
                    var label = Module.agnodeget(n, "label");
                    var name = /^([\w:\.]+)/.exec(label)[0];
                    if (! classes[name]) delnodes.push(n);
                }
                for (var i = 0; i < delnodes.length; i++) {
                    Module.agdelnode(Graph, delnodes[i]);
                }
                var rc = Module.gvLayout(Context, Graph, "dot");
                if (rc) {
                    svgarea.innerHTML = "gvLayout returned " + rc;
                } else {
                    svgarea.innerHTML = Module.gvRenderString(Context, Graph, "svg");
                    var svg = svgarea.getElementsByTagName("svg")[0];
                    svg.setAttribute("width", "100%");
                    svg.setAttribute("height", "100%");
                    svg.getElementsByTagName("g")[0].setAttribute("viewBox", "0 0 0 0");
                    SVGPanReinit(svg);
                }
            }
        }
    }
    closeGraph();
}