#include "parsed_finite_element.h"
#include <deal.II/fe/fe_tools.h>

template <int dim, int spacedim>
ParsedFiniteElement<dim, spacedim>::ParsedFiniteElement(const std::string &name,
                                                        const std::string &default_name) :
  ParameterAcceptor(name),
  fe_name(default_name)
{}

template <int dim, int spacedim>
void ParsedFiniteElement<dim, spacedim>::declare_parameters(ParameterHandler &prm)
{
  add_parameter(prm, &fe_name,
                "Finite element space", fe_name,
                Patterns::Anything(),
                "The finite element space to use. For vector "
                "finite elements use the notation "
                "FESystem[FE_Q(2)^2-FE_DGP(1)] (e.g. Navier-Stokes). ");
}

template <int dim, int spacedim>
FiniteElement<dim,spacedim> *
ParsedFiniteElement<dim, spacedim>::operator()()
{
  return FETools::get_fe_by_name<dim,spacedim>(fe_name);
}



template class ParsedFiniteElement<1,1>;
template class ParsedFiniteElement<1,2>;
template class ParsedFiniteElement<1,3>;
template class ParsedFiniteElement<2,2>;
template class ParsedFiniteElement<2,3>;
template class ParsedFiniteElement<3,3>;
