cmake_minimum_required(VERSION 2.6)

project(testing_algo_import)
	
add_executable( test_algo_import 
algo_import.cpp
#AHEMImporter.cpp
#AHEMImporterDefAttr.cpp
import.cpp
importMRDAT.cpp
importObjTex.cpp
#importSvg
)	

target_link_libraries( test_algo_import
	${CGoGN_LIBS} ${CGoGN_EXT_LIBS})

qt_use_cgogn_modules(test_algo_import)
