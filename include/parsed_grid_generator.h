#ifndef __dealii_parsed_grid_generator_h
#define __dealii_parsed_grid_generator_h

#include <deal.II/base/parameter_handler.h>
#include <deal.II/grid/tria.h>
#include <deal.II/distributed/tria.h>


#include "parameter_acceptor.h"

using namespace dealii;

/**
 * Parsed grid generator. Create a grid reading a parameter file. This
 * can be either generated using dealii::GridGenerator functions, or
 * read from a file.
 *
 * All Triangulations in the GridGenerator Namespace are supported, as
 * well as all formats supported by deal.II.
 */
template<int dim, int spacedim=dim>
class ParsedGridGenerator : public ParameterAcceptor
{
public:
    /**
     * Constructor. Takes a name for the section of the Parameter
     * Handler to use.
     *
     * This class is derived from ParameterAcceptor. Once you
     * constructed an object of this class, if you call
     * ParameterAcceptor::parse_all_parameters(prm), also the
     * parameters of this class will be filled with values from the
     * argument ParameterHandler.
     */
    ParsedGridGenerator (std::string section_name="");

    /**
     * Declare possible parameters of this class.
     */
    virtual void declare_parameters(ParameterHandler &prm);

    /**
     * Return a pointer to a newly created Triangulation. It will
     * throw an exception if called before any parsing has
     * occured. It's the user's responsability to destroy the created
     * grid once it is no longer needed.
     */
    Triangulation<dim, spacedim> * serial();

    /**
       * Return a pointer to a newly created Triangulation. It will
       * throw an exception if called before any parsing has
       * occured. It's the user's responsability to destroy the created
       * grid once it is no longer needed.
       */
    parallel::distributed::Triangulation<dim, spacedim> * distributed(MPI_Comm mpi_communicator);

private:
    typename Triangulation<dim,spacedim>::MeshSmoothing
    get_smoothing();
    /**
     * Mesh smoothing. Parse the type of MeshSmoothing for the
     * generated Triangulation.
     */
    std::string mesh_smoothing;

    /**
     * The grid to generate. Use the name "file" to read from a file.
     */
    std::string grid_name;

    /**
     * Grid file name.
     */
    std::string grid_file_name;
};

#endif