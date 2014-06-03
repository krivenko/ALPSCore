/*
 * Copyright (C) 1998-2014 ALPS Collaboration. See COPYRIGHT.TXT
 * All rights reserved. Use is subject to license terms. See LICENSE.TXT
 * For use in publications, see ACKNOWLEDGE.TXT
 */

/* $Id$ */

#ifndef ALPS_MODEL_MODEL_HELPER_H
#define ALPS_MODEL_MODEL_HELPER_H

#include <alps/model/modellibrary.h>
#include <alps/lattice/graph_helper.h>

namespace alps {

template <class I=short>
class model_helper
{
public:  

  typedef BasisDescriptor<I> basis_descriptor_type;
  typedef SiteBasisDescriptor<I> site_basis_descriptor_type;
  typedef half_integer<I> half_integer_type;
  typedef QuantumNumberDescriptor<I> quantum_number_type;
  
  model_helper(alps::Parameters const& p, bool issymbolic=false)
   : model_library_(p), 
     parms_(p) 
  {
    model_ = model_library_.get_hamiltonian(p,issymbolic);
  }

  template <class G>
  model_helper(alps::graph_helper<G> const& g, alps::Parameters const& p, bool issymbolic=false)
   : model_library_(p), 
     parms_(p) 
  {
    model_ = model_library_.get_hamiltonian(g,p,issymbolic);
  }

  const ModelLibrary& model_library() const { return model_library_; }  
  HamiltonianDescriptor<I>& model() { return model_;}
  const HamiltonianDescriptor<I>& model() const { return model_;}
  basis_descriptor_type& basis() { return model().basis();}
  const basis_descriptor_type& basis() const { return model().basis();}
  const site_basis_descriptor_type& site_basis(int type=0) const { return basis().site_basis(type);}
  SiteOperator site_term(int type=0) const { return model().site_term(type);}
  BondOperator bond_term(int type=0) const { return model().bond_term(type);};
  
  bool has_site_operator(const std::string& name) const { return model_library_.has_site_operator(name);}
  bool has_bond_operator(const std::string& name) const { return model_library_.has_bond_operator(name);}
  bool has_global_operator(const std::string& name) const { return model_library_.has_global_operator(name);}
  bool has_operator(const std::string& name) const { return model_library_.has_operator(name);}
  SiteOperator get_site_operator(const std::string& name,const Parameters& p) const { return model_library_.get_site_operator(name,p);}
  BondOperator get_bond_operator(const std::string& name,const Parameters& p) const { return model_library_.get_bond_operator(name,p);}
  SiteOperator get_site_operator(const std::string& name) const { return model_library_.get_site_operator(name,parms_);}
  BondOperator get_bond_operator(const std::string& name) const { return model_library_.get_bond_operator(name,parms_);}
  GlobalOperator get_global_operator(const std::string& name) const { return model_library_.get_global_operator(name,parms_);}
  
  template <class OP>
  void substitute_operators(OP& op, const Parameters& p) const { op.substitute_operators(model_library_,p);}

  std::set<std::string> quantum_numbers(int type=0)
  {
    std::set<std::string> qns;
    site_basis_descriptor_type b = this->site_basis(type);
    for (int i=0;i<b.size();++i)
      qns.insert(b[i].name());
    return qns;
  }
  
private:
   ModelLibrary model_library_;
   HamiltonianDescriptor<I> model_;
   Parameters parms_;
};

} // end namespace

#endif
