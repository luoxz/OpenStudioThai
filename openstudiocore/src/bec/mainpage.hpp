namespace openstudio {
namespace bec {
/** \mainpage OpenStudio BEC Translator
*
* OpenStudio BEC Translator is a conversion utility between BEC data files and
* OpenStudio Model (osm files).  The translation is supported in both
* directions.  The utility that converts from BEC format to the OpenStudio Model
* is called BEC ReverseTranslator.  Similarly, BEC ForwardTransltor is the
* utility that converts from an OpenStudio Model to BEC format.  The conversion
* from BEC to OpenStudio is comprehensive, encompasing all aspects of the
* building which relate to energy modeling, including geometry, loads, and HVAC.
* Translation from OpenStudio to BEC is partial, suporting only the geometric
* aspects of the building.
*
* The implementation of both the forward and reverse translators is a series of
* procedural functions that are called hierarchically.  Functions that translate
* high level building concepts are invoked first, and these functions in turn
* call other functions to translate lower level building concepts.  The process
* begins at the project level, to the building, spaces, zones, space loads, and
* so forth.
*
* ReverseTranslator.hpp defines the complete set of (private) translator
* functions for converting an BEC file to an OpenStudio Model.  The method
* ReverseTranslate::loadModel(), is the public interface for carrying out the
* translation.  The method simply accepts a path to the BEC file that is to be
* translated.
*
* ForwardTransltor.hpp defines the complete set of transltor functions for
* converting an OpenStudio Model to an BEC file.  Translation is invoked by
* calling ForwardTranslate::modelToBEC(), passing the model to translate and the
* path to save the BEC file.
*
*/

} // bec
} // openstudio
