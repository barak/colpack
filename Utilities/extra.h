/************************************************************************************
    Copyright (C) 2005-2008 Assefaw H. Gebremedhin, Arijit Tarafdar, Duc Nguyen,
    Alex Pothen

    This file is part of ColPack.

    ColPack is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published
    by the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    ColPack is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with ColPack.  If not, see <http://www.gnu.org/licenses/>.
************************************************************************************/

#ifndef EXTRA_H
#define EXTRA_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <ctime>
#include <cstdlib>
//#include <cctype> //for toupper()

#include <list>
#include <map>
#include <string>
#include <vector>

using namespace std;

/*
#include "Definitions.h"
#include "Pause.h"
*/

/// Read a Row Compressed Format file
/** Read a Row Compressed Format file
Line 1: <# of rows> <# of columns> <# of non-zeros>
Line 2-(# of non-zeros + 1): <# of non-zeros in that row> <index of the 1st non-zero> <index of the 2nd non-zero> ... <index of the (# of non-zeros in that row)th non-zero> 
*/
int ReadRowCompressedFormat(string s_InputFile, unsigned int *** uip3_SparsityPattern, int& rowCount, int& columnCount);

//Re-order the values randomly
void randomOrdering(vector<int>& ordering);

/// Convert all the characters in input to upper case, ' ', '\ t', '\ n' will be converted to '_'
string toUpper(string input);

/// Build the index struture from Row Compressed Format to Sparse Solvers Format
/**
ip2_RowIndex and ip2_ColumnIndex will be allocated memory and populated with the matrix structure in Sparse Solvers Format

Input:
- uip2_HessianSparsityPattern in Row Compressed Format
- ui_rowCount

Output:
- ip2_RowIndex[ui_rowCount + 1] for Sparse Solvers Format
- ip2_ColumnIndex[ ip2_RowIndex[ui_rowCount] - 1] for Sparse Solvers Format


*/
int RowCompressedFormat_2_SparseSolversFormat_StructureOnly(unsigned int ** uip2_HessianSparsityPattern, unsigned int ui_rowCount, unsigned int** ip2_RowIndex, unsigned int** ip2_ColumnIndex);

/// Covert file with DIMACS format to Matrix Market format
/**
DIMACS graph format: http://www.dis.uniroma1.it/~challenge9/format.shtml#graph
Note: DIMACS graph format is for directed graph => the equivalent matrix is squared and non-systemic

Read input from file "<fileNameNoExt>.gr" (DIMACS graph format)
and generate file "<fileNameNoExt>.mtx" (Matrix Market format)
*/
void ConvertDIMACSFormat2MatrixMarketFormat(string fileNameNoExt);

///Read the sparse matrix from Matrix-Market-format file and convert to compress sparse row format "uip3_SparsityPattern" & "dp3_Value"
/** Read in a matrix from matrix-market format file and create a matrix stored in compressed sparse row format
The Matrix-Market-format has 3 values in each row, the row index, column index and numerical value of each nonzero.
The last 4 parameters of this routine are output parameters (unsigned int *** uip3_SparsityPattern, double*** dp3_Value,int &rowCount, int &columnCount)
*/
int ConvertMatrixMarketFormatToRowCompressedFormat(string s_InputFile, unsigned int *** uip3_SparsityPattern, double*** dp3_Value, int &rowCount, int &columnCount);

/// Multiply the original sparse matrix (uip3_SparsityPattern,dp3_Value) (in compress sparse row format) with the seed matrix dp2_seed and store the result in "dp3_CompressedMatrix"
/** (*dp3_CompressedMatrix) = (*dp3_Value) * dp2_seed
*/
int MatrixMultiplication_VxS(unsigned int ** uip3_SparsityPattern, double** dp3_Value, int rowCount, int columnCount, double** dp2_seed, int colorCount, double*** dp3_CompressedMatrix);

/// Multiply the seed matrix dp2_seed with the original sparse matrix (uip3_SparsityPattern,dp3_Value) (in compress sparse row format) and store the result in "dp3_CompressedMatrix"
/** (*dp3_CompressedMatrix) = dp2_seed * (*dp3_Value)
*/
int MatrixMultiplication_SxV(unsigned int ** uip3_SparsityPattern, double** dp3_Value, int rowCount, int columnCount, double** dp2_seed, int colorCount, double*** dp3_CompressedMatrix);

///Compare dp3_Value with dp3_NewValue and see if all the values are equal.
/**
	If (compare_exact == 0) num1 and num2 are consider equal if 0.99 <= num1/num2 <= 1.02
	If (print_all == 1) all cases of non-equal will be print out. Normally (when print_all == 0), this rountine will stop after the first non-equal.
*/
bool CompressedRowMatricesREqual(double** dp3_Value, double** dp3_NewValue, int rowCount, bool compare_exact = 1, bool print_all = 0);

///just manipulate the value of dp2_Values a little bit. Each non-zero entry in the matrix * 2 + 1.5.
int Times2Plus1point5(double** dp2_Values, int i_RowCount, int i_ColumnCount);

///just manipulate the value of dp2_Values a little bit. Each non-zero entry in the matrix * 2.
int Times2(double** dp2_Values, int i_RowCount, int i_ColumnCount);

///Allocate memory and generate random values for dp3_Value
int GenerateValues(unsigned int ** uip2_SparsityPattern, int rowCount, double*** dp3_Value);

///Allocate memory and generate random values for dp3_Value of a Symmetric Matrix.
int GenerateValuesForSymmetricMatrix(unsigned int ** uip2_SparsityPattern, int rowCount, double*** dp3_Value);

#ifndef EXTRA_H_TEMPLATE_FUNCTIONS
#define EXTRA_H_TEMPLATE_FUNCTIONS

///Find the difference between 2 arrays. Return 0 if there is no difference, 1 if there is at least 1 difference
template<class T>
int diffArrays(T* array1, T* array2, int rowCount, bool compare_exact = 1, bool print_all = 0) {
	double ratio = 0.;
	int none_equal_count = 0;
	for(int i = 0; i < rowCount; i++) {
	  if (compare_exact) {
	    if(array1[i]!=array2[i]) { // found a difference
	      cout<<"At index i="<<i<<"\t array1[] = "<<array1[i]<";\t array2[] = "<<array2[i]<<endl;
	      none_equal_count++;
	      if(!print_all) return 1;
	    }
	  }
	  else {
	    ratio = array1[i] / array2[i];
	    if(ratio < .99 || ratio > 1.02) { // found a difference
	      cout<<"At index i="<<i<<"\t array1[] = "<<array1[i]<";\t array2[] = "<<array2[i]<<endl;
	      none_equal_count++;
	      if(!print_all) return 1;
	    }
	  }
	}
	
	return none_equal_count;
}

///Find the difference between 2 vectors. Return 0 if there is no difference, 1 if there is at least 1 difference
template<class T>
int diffVectors(vector<T> array1, vector<T> array2, bool compare_exact = 1, bool print_all = 0) {
	double ratio = 0.;
	int none_equal_count = 0;
	
	if(array1.size() != array2.size()) {
	  cout<<"array1.size() "<<array1.size()<<" != array2.size()"<<array2.size()<<endl;
	  none_equal_count++;
	}
	
	int min_array_size = (array1.size() < array2.size())?array1.size():array2.size();
	
	for(int i = 0; i < min_array_size; i++) {
	  if (compare_exact) {
	    if(array1[i]!=array2[i]) { // found a difference
	      cout<<"At index i="<<i<<"\t array1[] = "<<array1[i]<<";\t array2[] = "<<array2[i]<<endl;
	      none_equal_count++;
	      if(!print_all) return none_equal_count;
	    }
	  }
	  else {
	    ratio = array1[i] / array2[i];
	    if(ratio < .99 || ratio > 1.02) { // found a difference
	      cout<<"At index i="<<i<<"\t array1[] = "<<array1[i]<<";\t array2[] = "<<array2[i]<<endl;
	      none_equal_count++;
	      if(!print_all) return none_equal_count;
	    }
	  }
	}
	
	return none_equal_count;
}

template<class T>
int deleteMatrix(T** xp2_matrix, int rowCount) {
//cout<<"IN deleteM 2"<<endl<<flush;
//printf("* deleteMatrix rowCount=%d \n",rowCount);
//Pause();
	for(int i = 0; i < rowCount; i++) {
//printf("delete xp2_matrix[%d][0] = %7.2f \n", i, (float) xp2_matrix[i][0]);
		delete xp2_matrix[i];
	}
//cout<<"MID deleteM 2"<<endl<<flush;
	delete xp2_matrix;
//cout<<"OUT deleteM 2"<<endl<<flush;
	return 0;
}

template<class T>
int deleteMatrix(T*** xp3_matrix, int rowCount) {
//cout<<"IN deleteM 3"<<endl<<flush;
	deleteMatrix(*xp3_matrix,rowCount);
//cout<<"MID deleteM 3"<<endl<<flush;
	delete xp3_matrix;
//cout<<"OUT deleteM 3"<<endl<<flush;
	return 0;
}
template<class T>
void displayCompressedRowMatrix(T** xp2_Value, int rowCount, bool structureOnly = false) {
	unsigned int estimateColumnCount = 30;
	cout<<setw(4)<<"["<<setw(3)<<"\\"<<"]";
	if(structureOnly) {
		for(unsigned int j=0; j < estimateColumnCount; j++) cout<<setw(4)<<j;
	}
	else {
		for(unsigned int j=0; j < estimateColumnCount; j++) cout<<setw(9)<<j;
	}
	cout<<endl;

	for(unsigned int i=0; i < (unsigned int)rowCount; i++) {
		cout<<setw(4)<<"["<<setw(3)<<i<<"]";
		unsigned int numOfNonZeros = (unsigned int)xp2_Value[i][0];
		if(structureOnly) {
			for(unsigned int j=0; j <= numOfNonZeros; j++) {
			  if (j==0) printf("  (%d)",(int)xp2_Value[i][j]);
			  else printf("  %d",(int)xp2_Value[i][j]);
			}
		}
		else {
			//for(unsigned int j=0; j <= numOfNonZeros; j++) cout<<setw(8)<<xp2_Value[i][j];
			for(unsigned int j=0; j <= numOfNonZeros; j++) {
			  if(j==0) printf("  (%7.2f)",(float)xp2_Value[i][j]);
			  else printf("  %7.2f",(float)xp2_Value[i][j]);
			}
		}
		cout<<endl<<flush;
	}
	cout<<endl<<endl;
}

template<class T>
void displayMatrix(T** xp2_Value, int rowCount, int columnCount, bool structureOnly = false) {
	cout<<setw(4)<<"["<<setw(3)<<"\\"<<"]";
	if(structureOnly) {
		for(unsigned int j=0; j < (unsigned int)columnCount; j++) cout<<setw(3)<<j;
	}
	else {
		for(unsigned int j=0; j < (unsigned int)columnCount; j++) cout<<setw(9)<<j;
	}
	cout<<endl;

	for(unsigned int i=0; i < (unsigned int)rowCount; i++) {
		cout<<setw(4)<<"["<<setw(3)<<i<<"]";
		if(structureOnly) {
			for(unsigned int j=0; j < (unsigned int)columnCount; j++) cout<<setw(3)<<(bool)xp2_Value[i][j];
		}
		else {
			for(unsigned int j=0; j < (unsigned int)columnCount; j++) printf("  %7.2f",(float)xp2_Value[i][j]);
			//for(unsigned int j=0; j < (unsigned int)columnCount; j++) cout<<setw(8)<<xp2_Value[i][j];
		}
		cout<<endl<<flush;
	}
	cout<<endl<<endl;
}

template<class T>
void displayVector(T* xp2_Value, int size, bool structureOnly = false) {
	if(structureOnly) {
		for(unsigned int i=0; i < (unsigned int)size; i++) {
			cout<<setw(4)<<"["<<setw(3)<<i<<"]";
			cout<<setw(3)<<(bool)xp2_Value[i];
			cout<<endl<<flush;
		}
	}
	else {
		for(unsigned int i=0; i < (unsigned int)size; i++) {
			cout<<setw(4)<<"["<<setw(3)<<i<<"]";
			printf("  %7.2f",(float)xp2_Value[i]);
			//cout<<setw(8)<<xp2_Value[i];
			cout<<endl<<flush;
		}
	}
	cout<<endl<<endl;
}

/// Used mainly to debug GraphColoringInterface::IndirectRecover() routine
template<class T>
void displayAdjacencyMatrix(vector< vector<T> > &xp2_Value, bool structureOnly = false) {
	unsigned int estimateColumnCount = 30;
	cout<<setw(4)<<"["<<setw(3)<<"\\"<<"]";
	if(structureOnly) {
		for(unsigned int j=0; j < estimateColumnCount; j++) cout<<setw(3)<<j;
	}
	else {
		for(unsigned int j=0; j < estimateColumnCount; j++) cout<<setw(9)<<j;
	}
	cout<<endl;

	unsigned int rowCount = xp2_Value.size();
	for(unsigned int i=0; i < rowCount; i++) {
		cout<<setw(4)<<"["<<setw(3)<<i<<"]";
		unsigned int numOfNonZeros = (int)xp2_Value[i].size();
		cout<<"("<<setw(5)<<numOfNonZeros<<")";
		if(structureOnly) {
			for(unsigned int j=0; j < numOfNonZeros; j++) cout<<setw(3)<<(bool)xp2_Value[i][j];
		}
		else {
			for(unsigned int j=0; j < numOfNonZeros; j++) cout<<setw(9)<<xp2_Value[i][j];
		}
		cout<<endl<<flush;
	}
	cout<<endl<<endl;
}


#endif //EXTRA_H_TEMPLATE_FUNCTIONS

#endif //EXTRA_H



