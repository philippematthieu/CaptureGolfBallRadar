/*
 * EquationODE.cpp
 *
 *  Created on: 21 sept. 2016
 *  Author:
 */

#include "EquationODE.h"
#include <string>
#include <iostream>
#include <vector>

using namespace std;
EquationODE::EquationODE(){
}

EquationODE::EquationODE(std::vector<double> pParametres){
	parametres.resize(pParametres.size());
	parametres = pParametres;
}

void EquationODE::setParamEq(double value, int index)	{
	parametres[index] = value;
}

double EquationODE::getParamEq(int index) {
	return parametres[index];
}

int EquationODE::getSizeParamEq() {
	return parametres.size();
}

EquationODE::~EquationODE() {}

