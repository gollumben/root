/*****************************************************************************
 * Project: BaBar detector at the SLAC PEP-II B-factory
 * Package: RooFitCore
 *    File: $Id: RooRealFunc1D.cc,v 1.4 2001/08/01 21:30:15 david Exp $
 * Authors:
 *   DK, David Kirkby, Stanford University, kirkby@hep.stanford.edu
 * History:
 *   29-Apr-2001 DK Created initial version
 *
 * Copyright (C) 2001 Stanford University
 *****************************************************************************/

// -- CLASS DESCRIPTION --
// This class performs a lightweight binding of a RooAbsReal object with
// one RooRealVar object that it depends on, and implements the abstract
// RooAbsFunc1D interface for a real-valued function of one real variable.

// #include "BaBar/BaBar.hh"

#include "RooFitCore/RooRealFunc1D.hh"
#include "RooFitCore/RooAbsReal.hh"
#include "RooFitCore/RooRealVar.hh"
#include "RooFitCore/RooDataSet.hh"
#include "RooFitCore/RooRealIntegral.hh"

#include "TString.h"

ClassImp(RooRealFunc1D)
;

static const char rcsid[] =
"$Id: RooRealFunc1D.cc,v 1.4 2001/08/01 21:30:15 david Exp $";

RooRealFunc1D::RooRealFunc1D(const RooAbsReal &func, RooRealVar &x, Double_t scaleFactor,
			     const RooArgSet *normVars) :
  _funcPtr(&func), _xPtr(&x), _scale(scaleFactor), _nset(0), _projected(0)
{
  // Create a new binding object. The input objects are not cloned so the
  // lifetime of the newly created object is limited by their lifetimes.

  if(0 != normVars) {
    RooArgSet vars(*normVars);
    RooAbsArg *found= vars.find(x.GetName());
    if(found) {
      // if requested, normalize ourselves wrt to the dependent x
      vars.remove(*found);
      _nset= new RooArgSet(*found,"Self-Normalization Set");
    }
    if(vars.GetSize() > 0) {
      // project out any other variables if possible
      const RooAbsPdf *pdfPtr= dynamic_cast<const RooAbsPdf*>(&func);
      if(0 == pdfPtr) {
	cout << "RooRealFunc1D::WARNING: cannot integrate non-PDF \""
	     << func.GetName() << "\" over";
	vars.Print();
	cout << "  (will calculate along a slice instead)" << endl;
      }
      else {
	cout << "RooRealFunc1D: integrating out ";
	vars.Print();
        _projected= new RooRealIntegral(TString(func.GetName()).Append("Projected"),
					TString(func.GetTitle()).Append(" (Projected)"),
					*pdfPtr,vars);
        _funcPtr= _projected;
      }
    }
  }
}

RooRealFunc1D::~RooRealFunc1D() {
  if(_nset) delete _nset;
  if(_projected) delete _projected;
}

Double_t RooRealFunc1D::operator()(Double_t x) const {
  _xPtr->setVal(x);
  return _scale*_funcPtr->getVal(_nset);
}
