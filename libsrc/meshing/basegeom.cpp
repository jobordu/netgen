#include <mystdlib.h>
#include "meshing.hpp"

namespace netgen
{

  DLL_HEADER GeometryRegisterArray geometryregister; 
  //DLL_HEADER Array<GeometryRegister*> geometryregister;

  GeometryRegister :: ~GeometryRegister()
  { ; }

  
  shared_ptr<NetgenGeometry> GeometryRegisterArray :: LoadFromMeshFile (istream & ist) const
  {
    for (int i = 0; i < Size(); i++)
      {
        NetgenGeometry * hgeom = (*this)[i]->LoadFromMeshFile (ist);
        if (hgeom)
          return shared_ptr<NetgenGeometry>(hgeom);
      }
    return nullptr;
  }



  
  int NetgenGeometry :: GenerateMesh (shared_ptr<Mesh> & mesh, MeshingParameters & mparam)
  {
    if (!mesh) return 1;

    if (mparam.perfstepsstart <= MESHCONST_MESHVOLUME)
      {
	multithread.task = "Volume meshing";
	
	MESHING3_RESULT res =
	  MeshVolume (mparam, *mesh);
	
	if (res != MESHING3_OK) return 1;
	
	if (multithread.terminate) return 0;
	
	RemoveIllegalElements (*mesh);
	if (multithread.terminate) return 0;

	MeshQuality3d (*mesh);
      }

    
    if (multithread.terminate || mparam.perfstepsend <= MESHCONST_MESHVOLUME)
      return 0;


    if (mparam.perfstepsstart <= MESHCONST_OPTVOLUME)
      {
	multithread.task = "Volume optimization";
	
	OptimizeVolume (mparam, *mesh);
	if (multithread.terminate) return 0;
      }
    
    return 0;
  }    
  

  const Refinement & NetgenGeometry :: GetRefinement () const
  {
    return *new Refinement;;
  }


  void NetgenGeometry :: Save (string filename) const
  {
    throw NgException("Cannot save geometry - no geometry available");
  }

  static RegisterClassForArchive<NetgenGeometry> regnggeo;
}
