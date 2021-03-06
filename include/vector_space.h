#ifndef VECTOR_SPACE_H
#define VECTOR_SPACE_H

#include <deal.II/base/parameter_handler.h>
#include <deal.II/base/smartpointer.h>
#include <deal.II/fe/fe_tools.h>
#include <deal.II/fe/mapping.h>
#include <deal.II/grid/persistent_tria.h>
#include <deal.II/dofs/dof_handler.h>
#include <deal.II/lac/constraint_matrix.h>
#include <deal.II/numerics/solution_transfer.h>

#include "parameter_acceptor.h"

using namespace dealii;

/**
  VectorSpace object. Anything related to the definition of the
  Hilbert space that makes up the pde problem lives in this class.
*/
template <int dim, int spacedim=dim>
class VectorSpace : public ParameterAcceptor
{
public:

  /** Set all internal pointers to invalid pointers, and set the name of this class. */
  VectorSpace (const std::string name);

  /** Destructor. Clears all created pointers. */
  ~VectorSpace ();

  /** Reinit. The state of this object after calling reinit, is
      the same as after the full constructor is used.*/
  void reinit(ParameterHandler &prm,
              Triangulation<dim, spacedim> &dd,
              const unsigned int n_mpi_processes=1,
              const unsigned int this_mpi_process=0);

  /** Reset the internal status of the current triangulation, leaving
      untouched all the rest. The status of this object is as if the
      current triangulation and dof handler had just been
      constructed. Nothing is done to the saved triangulation, nor to
      the saved dofhandler. */
  void reinit();

  /** Parse the parameter file. */
  virtual void parse_parameters(ParameterHandler &prm);

  /** Generate entries in the given parameter file. */
  virtual void declare_parameters(ParameterHandler &prm);

  /** Initialize the mesh. */
  void initialize_mesh();

  /** Refine the mesh globally. */
  void refine_globally();

  /** Flag the mesh according to the given error estimator and to the
      refinement strategy defined in the paratmeters. */
  void flag_for_refinement(Vector<float> &error);

  /** Save current vector space. */
  void save_step();

  /** Save current vector space, and reset triangulation to be pristine. */
  void save_step_and_reinit();

  /** Transfer solution from old to new grid. */
  template <typename VECTOR>
  void transfer(VECTOR &dst, const VECTOR &src);

  /** Reorder the degrees of freedom according to the
      parameters. This is done for the level specified on the
      argument. If level is -1, then the active dofs are reordered. */
  void reorder(int level = -1,
               std::vector<unsigned int> target_comps=std::vector<unsigned int>());

  /** Redistribute degrees of freedom. The optional arguments groups
      together the components. It is useful for vector valued finite elements, when one wants to sort together*/
  void redistribute_dofs(std::vector<unsigned int>
                         target_components=std::vector<unsigned int>());

  /** Compute maximum and minimum diameter of the mesh cells. */
  void measure_mesh();

  /** Interpolate Boundary Conditions. Generates the boundary
      conditions for the problem. This will be used after the assembly
      procedures. */
  void interpolate_dirichlet_bc(const Function<spacedim,double> &f,
                                std::map<unsigned int, double> &bvalues);

  /** Return reference to finite element.*/
  inline FiniteElement<dim, spacedim> &get_fe()
  {
    return *fe;
  }

  /** Return reference to current dof handler.*/
  inline DoFHandler<dim, spacedim> &get_dh()
  {
    return *dh;
  }

  /** Return reference to previous dof handler.*/
  inline DoFHandler<dim, spacedim> &get_last_dh()
  {
    return *last_dh;
  }

  /** Return reference to current triangulation..*/
  inline Triangulation<dim, spacedim> &get_tria()
  {
    return *tria;
  }

  /** Return reference to current triangulation..*/
  inline Triangulation<dim, spacedim> &get_last_tria()
  {
    return *last_tria;
  }

  /** Return reference to coarse triangulation..*/
  inline Triangulation<dim, spacedim> &get_coarse_tria()
  {
    return *coarse;
  }

  /** Return reference to hanging node constraints..*/
  inline ConstraintMatrix &get_hang()
  {
    return hang;
  }

  /** Return reference to mapping.*/
  inline Mapping<dim, spacedim> &get_mapping()
  {
    return *mapping;
  }


  /** Return constant reference to finite element.*/
  inline const FiniteElement<dim, spacedim> &get_fe() const
  {
    return *fe;
  }

  /** Return constant reference to current dof handler.*/
  inline const DoFHandler<dim, spacedim> &get_dh() const
  {
    return *dh;
  }

  /** Return constant reference to previous dof handler.*/
  inline const DoFHandler<dim, spacedim> &get_last_dh() const
  {
    return *last_dh;
  }

  /** Return reference to current triangulation..*/
  inline const Triangulation<dim, spacedim> &get_tria() const
  {
    return *tria;
  }

  /** Return reference to coarse triangulation..*/
  inline const Triangulation<dim, spacedim> &get_coarse_tria() const
  {
    return *coarse;
  }

  /** Return reference to current triangulation..*/
  inline const Triangulation<dim, spacedim> &get_last_tria() const
  {
    return *last_tria;
  }

  /** Return reference to hanging node constraints..*/
  inline const ConstraintMatrix &get_hang() const
  {
    return hang;
  }

  /** Return reference to mapping.*/
  inline const Mapping<dim, spacedim> &get_mapping() const
  {
    return *mapping;
  }

  /** Number of dofs. */
  inline unsigned int n_dofs()
  {
    return dh->n_dofs();
  }

  /** Number of dofs per process. */
  inline const std::vector<unsigned int> &n_dofs_pp() const
  {
    return local_dofs_per_process;
  }

  /** Number of dofs per block. */
  inline const std::vector<unsigned int> &n_dofs_pb() const
  {
    return dofs_per_block;
  }

  /** Number of dofs in this process. */
  inline  unsigned int n_ldofs() const
  {
    return n_local_dofs;
  }

  /** Number of dofs per block per process.  */
  inline const std::vector<std::vector<unsigned int> > &n_dofs_pb_pp() const
  {
    return dofs_per_block_per_process;
  }

  /** Number of dofs per process per block.  */
  inline const  std::vector<std::vector<unsigned int> > &n_dofs_pp_pb() const
  {
    return dofs_per_process_per_block;
  }

  /** Number of blocks. */
  inline  unsigned int n_blocks() const
  {
    return number_of_blocks;
  }
  /** Bool that checks if local refiment is enabled.*/
  bool enable_local_refinement;

  /** Initial global refinement. */
  unsigned int global_refinement;

  /** The size of this mesh. */
  double h_max;

  /** The size of this mesh. */
  double h_min;

  /** Dirichlet Boundary Indicators. */
  std::map<char, std::vector<bool> > dirichlet_bc;

  /** Neumann Boundary Indicators. */
  std::map<char, std::vector<bool> > neumann_bc;

  /** Other Boundary Indicators. */
  std::map<char, std::vector<bool> > other_bc;
private:
  const std::string name;

  /** For internal use. Counts the dofs per block, block and what not. */
  void count_dofs(std::vector<unsigned int> target_components);

  std::string fe_name;
  std::vector<std::string> ordering;
  unsigned int map_type;

  std::vector<std::string> d_bc;
  std::vector<std::string> n_bc;
  std::vector<std::string> o_bc;

  /** Pointer to the finite element used. */
  SmartPointer<FiniteElement<dim,spacedim> > fe;

  /** Pointer to the dofhandler used */
  SmartPointer<DoFHandler<dim,spacedim> > dh;

  /** Pointer to the last dofhandler used */
  SmartPointer<DoFHandler<dim,spacedim> > last_dh;

  /** Pointer to a pristine coarse triangulation. */
  SmartPointer<Triangulation<dim,spacedim> > coarse;

  /** Pointer to the current triangulation used */
  SmartPointer<Triangulation<dim,spacedim> > tria;

  /** Pointer to the previous triangulation used. */
  SmartPointer<Triangulation<dim,spacedim> > last_tria;

  /** Finite Element Mapping. Various mappings are supported. If the
      mapping parameter is 0, then cartesian mapping is used. Else Qn, with
      n the mapping parameter. */
  SmartPointer<Mapping<dim,spacedim> > mapping;

  /** Constraint Matrix. */
  ConstraintMatrix hang;

public:

  /** Number of processes. */
  unsigned int n_mpi_processes;

  /** The id of this process. */
  unsigned int this_mpi_process;

private:

  /** Local size of vectors. */
  std::vector<unsigned int> local_dofs_per_process;

  /** Local size of vectors. */
  std::vector<unsigned int> dofs_per_block;

  /** Local sizes of vectors divided by blocks. */
  std::vector<std::vector<unsigned int> > dofs_per_block_per_process;

  /** Local sizes of vectors divided by blocks. */
  std::vector<std::vector<unsigned int> > dofs_per_process_per_block;

  /** Number of local dofs. */
  unsigned int n_local_dofs;

  /** Number of Blocks. */
  unsigned int number_of_blocks;

  /** Number of cells. */
  unsigned int number_of_cells;

  /** Distortion coefficient. */
  double distortion;

  /** Refinement strategy. */
  std::string refinement_strategy;

public:

  /** Bottom fraction of refinement. */
  double bottom_fraction;

  /** Top fraction of refinement. */
  double top_fraction;

private:

  /** Maximum number of allowed cells. */
  unsigned int max_cells;

  /** The wind direction, in case we order the mesh upwind. */
  Point<spacedim> wind;

};
#endif
