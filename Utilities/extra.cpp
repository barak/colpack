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

#include "extra.h"
#include "Pause.h"
#include "mmio.h"

bool isValidOrdering(vector<int> & ordering, int offset) {
  vector<bool> isExist;
  int orderingNum = 0;
  isExist.resize(ordering.size(), false);
  for(int i=0; i<ordering.size(); i++) {
    orderingNum = ordering[i] - offset;
    if(orderingNum<0 || orderingNum>= ordering.size()) {
      cerr<<" This vertex # is not in the valid range [0, ordering.size()]. ordering[i]: "<<ordering[i]<<endl;
      return false;
    }
    
    if(isExist[ orderingNum ]) {
      cerr<<"This vertex has been seen before. We have duplication!"<<endl;
      return false;
    }
    
    isExist[ orderingNum ] = true;
  }
  
  return true;
}

int ReadRowCompressedFormat(string s_InputFile, unsigned int *** uip3_SparsityPattern, int& rowCount, int& columnCount) {
  string line;
  int lineCounter = 0,nz_counter = 0, nonzeros = 0, nnz_per_row = 0;
  unsigned int num = 0;
  istringstream in2;
  ifstream in (s_InputFile.c_str());
  
  if(!in) {
    cout<<s_InputFile<<" not Found!"<<endl;
    exit(1);
  }
  
  getline(in,line);
  lineCounter++;
  in2.str(line);
  in2 >> rowCount >> columnCount >> nonzeros;
  
  (*uip3_SparsityPattern) = new unsigned int*[rowCount];

  for(int i=0;i < rowCount; i++) {
		getline(in, line);
		lineCounter++;
		if(line!="")
		{
			in2.clear();
			in2.str(line);
			in2>>nnz_per_row;
			(*uip3_SparsityPattern)[i] = new unsigned int[nnz_per_row + 1];
			(*uip3_SparsityPattern)[i][0] = nnz_per_row;
			
			for(int j=1; j<nnz_per_row+1; j++) {
			  in2>>num;
			  (*uip3_SparsityPattern)[i][j] = num;
			  nz_counter++;
			}			
		}
		else
		{
			cerr<<"* WARNING: ReadRowCompressedFormat()"<<endl;
			cerr<<"*\t line == \"\" at row "<<lineCounter<<". Empty line. Wrong input format. Can't process."<<endl;
			cerr<<"\t total non-zeros so far: "<<nz_counter<<endl;
			exit( -1);
		}
  }
  
  if(nz_counter<nonzeros) { //nz_counter should be == nonzeros
		  cerr<<"* WARNING: ReadRowCompressedFormat()"<<endl;
		  cerr<<"*\t nz_counter<nonzeros+1. Wrong input format. Can't process."<<endl;
		  cerr<<"\t total non-zeros so far: "<<nz_counter<<endl;
		  exit( -1);
  }



  return 0;

}

int RowCompressedFormat_2_SparseSolversFormat_StructureOnly (unsigned int ** uip2_HessianSparsityPattern, unsigned int ui_rowCount, unsigned int** ip2_RowIndex, unsigned int** ip2_ColumnIndex) {

	//first, count the number of non-zeros in the upper triangular and also populate *ip2_RowIndex array
	unsigned int nnz = 0;
	unsigned int nnz_in1Row = 0;
	(*ip2_RowIndex) = new unsigned int[ui_rowCount + 1];
	for (unsigned int i=0; i < ui_rowCount; i++) {
	  nnz_in1Row = uip2_HessianSparsityPattern[i][0];
	  (*ip2_RowIndex)[i] = nnz;
	  for (unsigned int j = 1; j <= nnz_in1Row ; j++) {
		if (i <= uip2_HessianSparsityPattern[i][j]) nnz++;
	  }
	}
	(*ip2_RowIndex)[ui_rowCount] = nnz;
	//cout<<"nnz = "<<nnz<<endl;

	displayVector(*ip2_RowIndex,ui_rowCount+1);

	// populate *ip2_ColumnIndex array
	(*ip2_ColumnIndex) = new unsigned int[nnz];
	unsigned int count = 0;
	for (unsigned int i=0; i < ui_rowCount; i++) {
	  nnz_in1Row = uip2_HessianSparsityPattern[i][0];
	  for (unsigned int j = 1; j <= nnz_in1Row ; j++) {
		if (i <= uip2_HessianSparsityPattern[i][j]) {
		  (*ip2_ColumnIndex)[count] = uip2_HessianSparsityPattern[i][j];
		    count++;
		}
	  }
	}
	if(count != nnz) {
	  cerr<<"!!! count != nnz. count = "<<count<<endl;
	  Pause();
	}

	return nnz;
}


void ConvertDIMACSFormat2MatrixMarketFormat(string fileNameNoExt) {
	string inFileName = fileNameNoExt + ".gr";
	string outFileName = fileNameNoExt + ".mtx";
	string line, temp;
	ifstream in(inFileName.c_str());
	ofstream out(outFileName.c_str());
	istringstream iin;

	while(in) {
		getline(in, line);
		if(line=="") break;
		switch(line[0]) {
			case 'a':
				//Line has this format "a <in_node> <out_node> <edge_weight>"
				out<<line.substr(2,line.size()-2)<<endl;
				break;
			case 'c': // comment line
				break;
			default: // 'p'
				//Heading. Line has this format "p sp <num_of_node> <num_of_edges == num_of_line after this line>"
				iin.str(line);
				iin>>temp>>temp>>temp;out<<temp<<" "<<temp<<" ";
				iin>>temp;out<<temp<<endl;
				break;
		}
	}

	in.close();
	out.close();
}

void randomOrdering(vector<int>& ordering) {
	srand(time(NULL));
	int size = ordering.size();
	int ran_num = 0;
	for(int i=0; i < size-1; i++) {
		//Get a random number in range [i,  size]
		ran_num = (int)(((float) rand() / RAND_MAX) * (size -1 - i)) + i;
		swap(ordering[i],ordering[ran_num]);
	}
}

string toUpper(string input) {
	string output = input;

	for(int i = input.size() - 1; i>=0; i--) {
		if(input[i]==' ' || input[i]=='\t' || input[i]=='\n') {
			output[i] = '_';
		}
		else {
			output[i] = toupper(input[i]);
		}
	}

	return output;
}

//just manipulate the value of dp2_Values a little bit
int Times2Plus1point5(double** dp2_Values, int i_RowCount, int i_ColumnCount) {
	for(int i=0; i < i_RowCount; i++) {
		for(int j=0; j < i_ColumnCount; j++) {
			if(dp2_Values[i][j] != 0.) dp2_Values[i][j] = dp2_Values[i][j]*2 + 1.5; //for each non-zero entry in the matrix, do the manipulation.
		}

	}
	return 0;
}
int Times2(double** dp2_Values, int i_RowCount, int i_ColumnCount) {
	for(int i=0; i < i_RowCount; i++) {
		for(int j=0; j < i_ColumnCount; j++) {
			if(dp2_Values[i][j] != 0.) dp2_Values[i][j] = dp2_Values[i][j]*2;
		}

	}
	return 0;
}

int GenerateValues(unsigned int ** uip2_SparsityPattern, int rowCount, double*** dp3_Value) {
	//srand(time(NULL));
	srand(0);

	(*dp3_Value) = new double*[rowCount];
	for(unsigned int i=0; i < (unsigned int)rowCount; i++) {
		unsigned int numOfNonZeros = uip2_SparsityPattern[i][0];
		(*dp3_Value)[i] = new double[numOfNonZeros + 1];
		(*dp3_Value)[i][0] = (double)numOfNonZeros;
		for(unsigned int j=1; j <= numOfNonZeros; j++) {
			(*dp3_Value)[i][j] = (rand()%2001 - 1000)/1000.0;
			//printf("(*dp3_Value)[%d][%d] = (%d % 2001 - 1000)/1000.0 = %7.2f \n",i,j,rand(),(*dp3_Value)[i][j]);
		}
	}

	return 0;
}

int GenerateValuesForSymmetricMatrix(unsigned int ** uip2_SparsityPattern, int rowCount, double*** dp3_Value) {
	//srand(time(NULL));
	srand(0);
	
	int * nnzCount = new int[rowCount]; // keep track of the # of non-zeros in each row
	for(unsigned int i=0; i < (unsigned int)rowCount; i++) nnzCount[i] = 0;

	(*dp3_Value) = new double*[rowCount];
	for(unsigned int i=0; i < (unsigned int)rowCount; i++) {
		unsigned int numOfNonZeros = uip2_SparsityPattern[i][0];
		(*dp3_Value)[i] = new double[numOfNonZeros + 1];
		(*dp3_Value)[i][0] = (double)numOfNonZeros;
		for(unsigned int j=1; j <= numOfNonZeros; j++) {
			if (uip2_SparsityPattern[i][j] >i) break;
			(*dp3_Value)[i][j] = (rand()%2001 - 1000)/1000.0; nnzCount[i]++;
			if (uip2_SparsityPattern[i][j] <i) { // copy the value from the low triangular to the upper triangular
			  (*dp3_Value)[uip2_SparsityPattern[i][j]][nnzCount[uip2_SparsityPattern[i][j]]+1] = (*dp3_Value)[i][j]; nnzCount[uip2_SparsityPattern[i][j]]++;
			}
			//printf("(*dp3_Value)[%d][%d] = (%d % 2001 - 1000)/1000.0 = %7.2f \n",i,j,rand(),(*dp3_Value)[i][j]);
		}
	}
	
	delete[] nnzCount;

	return 0;
}

int ConvertMatrixMarketFormatToRowCompressedFormat(string s_InputFile, unsigned int *** uip3_SparsityPattern, double*** dp3_Value, int &rowCount, int &columnCount) {

	string m_s_InputFile=s_InputFile;

	//initialize local data
	int rowCounter=0, nonzeros=0, rowIndex=0, colIndex=0, nz_counter=0;
	//int num=0, numCount=0;
	float value;
	bool b_getValue, b_symmetric;
	istringstream in2;
	string line="";
	map<int,vector<int> > nodeList;
	map<int,vector<float> > valueList;

	//READ IN BANNER
	MM_typecode matcode;
	FILE *f;
	if ((f = fopen(m_s_InputFile.c_str(), "r")) == NULL)  {
	  cout<<m_s_InputFile<<" not Found!"<<endl;
	  exit(1);
	}
	else cout<<"Found file "<<m_s_InputFile<<endl;

	if (mm_read_banner(f, &matcode) != 0)
	{
	    printf("Could not process Matrix Market banner.\n");
	    exit(1);
	}

	if( mm_is_pattern(matcode) ) {
	  b_getValue = false;
	}
	else b_getValue = true;
	
	if(mm_is_symmetric(matcode)) {
	  b_symmetric = true;
	}
	else b_symmetric = false;

	//Check and make sure that the input file is supported
	char * result = mm_typecode_to_str(matcode);
	printf("Graph of Market Market type: [%s]\n", result);
	free(result);
	if (b_getValue) printf("\t Graph structure and VALUES will be read\n");
	else printf("\t Read graph struture only. Values will NOT be read\n");
	if( !( mm_is_coordinate(matcode) && (mm_is_symmetric(matcode) || mm_is_general(matcode) ) && ( mm_is_real(matcode) || mm_is_pattern(matcode) || mm_is_integer(matcode) ) ) ) {
	  printf("Sorry, this application does not support this type.");
	  exit(1);
	}

	fclose(f);
	//DONE - READ IN BANNER

	// FIND OUT THE SIZE OF THE MATRIX
	ifstream in (m_s_InputFile.c_str());
	if(!in) {
		cout<<m_s_InputFile<<" not Found!"<<endl;
		exit(1);
	}
	else {
	  //cout<<"Found file "<<m_s_InputFile<<endl;
	}

	getline(in,line);
	rowCounter++;
	while(line.size()>0&&line[0]=='%') {//ignore comment line
		getline(in,line);
	}
	in2.str(line);
	in2 >> rowCount >> columnCount >> nonzeros;
	//cout<<"rowCount="<<rowCount<<"; columnCount="<<columnCount<<"; nonzeros="<<nonzeros<<endl;
	// DONE - FIND OUT THE SIZE OF THE MATRIX

	while(!in.eof() && nz_counter<nonzeros) //there should be (nonzeros+1) lines in the input file
	{
		getline(in,line);
		rowCounter++;
		//cout<<"Line "<<rowCounter<<"="<<line<<endl;
		if(line!="")
		{
			in2.clear();
			in2.str(line);
			in2>>rowIndex>>colIndex;
			rowIndex--;
			colIndex--;
			
			if(b_symmetric) {
				if(rowIndex > colIndex) {
				
					//cout<<"\t"<<setw(4)<<rowIndex<<setw(4)<<colIndex<<setw(4)<<nz_counter<<endl;
					nodeList[rowIndex].push_back(colIndex); 
					nodeList[colIndex].push_back(rowIndex);
					nz_counter += 2;

					if(b_getValue) {
						in2>>value;
						//cout<<"Value = "<<value<<endl;
						valueList[rowIndex].push_back(value);
						valueList[colIndex].push_back(value);
					}
				}
				else if (rowIndex == colIndex) {
					//cout<<"\t"<<setw(4)<<rowIndex<<setw(4)<<colIndex<<setw(4)<<nz_counter<<endl;
					nodeList[rowIndex].push_back(rowIndex);
					nz_counter++;
					if(b_getValue) {
					  in2>>value;
					  valueList[rowIndex].push_back(value);
					}
				}
				else { //rowIndex < colIndex
				  cerr<<"* WARNING: ConvertMatrixMarketFormatToRowCompressedFormat()"<<endl;
				  cerr<<"\t Found a nonzero in the upper triangular. A symmetric Matrix Market file format should only specify the nonzeros in the lower triangular."<<endl;
				  exit( -1);
				}
			}
			else { // !b_symmetric
				cout<<"\t"<<setw(4)<<rowIndex<<setw(4)<<colIndex<<setw(4)<<nz_counter<<endl;
				nodeList[rowIndex].push_back(colIndex);
				nz_counter++;
				if(b_getValue) {
				  in2>>value;
				  cout<<"Value = "<<value<<endl;
				  valueList[rowIndex].push_back(value);
				}
			}
			
		}
		else
		{
			cerr<<"* WARNING: ConvertMatrixMarketFormatToRowCompressedFormat()"<<endl;
			cerr<<"*\t line == \"\" at row "<<rowCounter<<". Empty line. Wrong input format. Can't process."<<endl;
			cerr<<"\t total non-zeros so far: "<<nz_counter<<endl;
			exit( -1);
		}
	}
	if(nz_counter<nonzeros) { //nz_counter should be == nonzeros
			cerr<<"* WARNING: ConvertMatrixMarketFormatToRowCompressedFormat()"<<endl;
			cerr<<"*\t nz_counter<nonzeros+1. Wrong input format. Can't process."<<endl;
			cerr<<"\t total non-zeros so far: "<<nz_counter<<endl;
			exit( -1);
	}


	(*uip3_SparsityPattern) = new unsigned int*[rowCount];
	if(b_getValue)	(*dp3_Value) = new double*[rowCount];
	for(int i=0;i<rowCount; i++) {
	  unsigned int numOfNonZeros = nodeList[i].size();
//printf("row = %d \t numOfNonZeros = %d : ", i, (int)numOfNonZeros);

	  //Allocate memory for each row
	  (*uip3_SparsityPattern)[i] = new unsigned int[numOfNonZeros+1];
	  (*uip3_SparsityPattern)[i][0] = numOfNonZeros;

	  if(b_getValue) {
	    (*dp3_Value)[i] = new double[numOfNonZeros+1];
	    (*dp3_Value)[i][0] = (double)numOfNonZeros;
	  }

	  for(unsigned int j=0; j < numOfNonZeros; j++) {
	    (*uip3_SparsityPattern)[i][j+1] = nodeList[i][j];
//printf("\t %d", (int) nodeList[i][j]);
	  }	  

	  if(b_getValue)	for(unsigned int j=0; j < numOfNonZeros; j++) {
	    (*dp3_Value)[i][j+1] = valueList[i][j];
	  }	  
//printf("\n");
	}


	return(0);
}

int MatrixMultiplication_VxS(unsigned int ** uip3_SparsityPattern, double** dp3_Value, int rowCount, int columnCount, double** dp2_seed, int colorCount, double*** dp3_CompressedMatrix) {

	//Allocate memory for (*dp3_CompressedMatrix)[rowCount][colorCount]
	//cout<<"Allocate memory for (*dp3_CompressedMatrix)[rowCount][colorCount]"<<endl;
	(*dp3_CompressedMatrix) = new double*[rowCount];
	for(unsigned int i=0; i < (unsigned int)rowCount; i++) {
		(*dp3_CompressedMatrix)[i] = new double[colorCount];
		for(unsigned int j=0; j < (unsigned int)colorCount; j++) {
			(*dp3_CompressedMatrix)[i][j] = 0.;
		}
	}

	//do the multiplication
	//cout<<"Do the multiplication"<<endl;
	for(unsigned int i=0; i < (unsigned int)rowCount; i++) {
		unsigned int numOfNonZeros = uip3_SparsityPattern[i][0];
		for(unsigned int j=1; j <= numOfNonZeros; j++) {
		  for(unsigned int k=0; k < (unsigned int)colorCount; k++) {
				//printf("i=%d\tj=%d\tuip3_SparsityPattern[i][j]=%d\tk=%d\n", i, j, uip3_SparsityPattern[i][j], k);
				(*dp3_CompressedMatrix)[i][k] += dp3_Value[i][j]*dp2_seed[uip3_SparsityPattern[i][j]][k];
			}
		}
	}

	return 0;
}

int MatrixMultiplication_SxV(unsigned int ** uip3_SparsityPattern, double** dp3_Value, int rowCount, int columnCount, double** dp2_seed, int colorCount, double*** dp3_CompressedMatrix) {

	//Allocate memory for (*dp3_CompressedMatrix)[colorCount][columnCount]
	//cout<<"Allocate memory for (*dp3_CompressedMatrix)[colorCount][columnCount]"<<endl;
	(*dp3_CompressedMatrix) = new double*[colorCount];
	for(unsigned int i=0; i < (unsigned int)colorCount; i++) {
		(*dp3_CompressedMatrix)[i] = new double[columnCount];
		for(unsigned int j=0; j < (unsigned int)columnCount; j++) {
			(*dp3_CompressedMatrix)[i][j] = 0.;
		}
	}

	//do the multiplication
	//cout<<"Do the multiplication"<<endl;
	for(unsigned int i=0; i < (unsigned int)rowCount; i++) {
		unsigned int numOfNonZeros = uip3_SparsityPattern[i][0];
		for(unsigned int j=1; j <= numOfNonZeros; j++) {
		  for(unsigned int k=0; k < (unsigned int)colorCount; k++) {
				//printf("i=%d\tj=%d\tuip3_SparsityPattern[i][j]=%d\tk=%d\n", i, j, uip3_SparsityPattern[i][j], k);
				(*dp3_CompressedMatrix)[k][uip3_SparsityPattern[i][j]] += dp2_seed[k][i]*dp3_Value[i][j];
			}
		}
	}

	return 0;
}

bool CompressedRowMatricesREqual(double** dp3_Value, double** dp3_NewValue, int rowCount, bool compare_exact, bool print_all) {
	double ratio = 1.;
	int none_equal_count = 0;

	for(unsigned int i=0; i < (unsigned int)rowCount; i++) {
		unsigned int numOfNonZeros = (unsigned int)dp3_Value[i][0];
		if (numOfNonZeros != (unsigned int)dp3_NewValue[i][0]) {
			printf("Number of non-zeros in row %d are not equal. dp3_Value[i][0] = %d; dp3_NewValue[i][0] = %d; \n",i,(unsigned int)dp3_Value[i][0],(unsigned int)dp3_NewValue[i][0]);
			if (print_all) {
				none_equal_count++;
				continue;
			}
			else return false;
		}
		for(unsigned int j=0; j <= numOfNonZeros; j++) {
			if (compare_exact) {
				if (dp3_Value[i][j] != dp3_NewValue[i][j]) {
					printf("At row %d, column %d, dp3_Value[i][j](%f) != dp3_NewValue[i][j](%f) \n",i,j,dp3_Value[i][j],dp3_NewValue[i][j]);
					if (print_all) {
						none_equal_count++;
					}
					else {
						printf("You may want to set the flag \"compare_exact\" to 0 to compare the values approximately\n");
						return false;
					}
				}
			}
			else {
				if(dp3_NewValue[i][j] == 0.) {
					if(dp3_Value[i][j] != 0.) {
						printf("At row %d, column %d, dp3_Value[i][j](%f) != dp3_NewValue[i][j](0) \n",i,j,dp3_Value[i][j]);
						if (print_all) {
							none_equal_count++;
						}
						else return false;
					}
				}
				else {
					ratio = dp3_Value[i][j] / dp3_NewValue[i][j];
					if( ratio < .99 || ratio > 1.02) {
						printf("At row %d, column %d, dp3_Value[i][j](%f) != dp3_NewValue[i][j](%f) ; dp3_Value[i][j] / dp3_NewValue[i][j]=%f\n",i,j,dp3_Value[i][j],dp3_NewValue[i][j], ratio);
						if (print_all) {
							none_equal_count++;
						}
						else return false;
					}
				}
			}
		}
	}

	if(none_equal_count!=0) {
		printf("Total: %d lines. (The total # of non-equals can be greater)\n",none_equal_count);
		if (compare_exact) printf("You may want to set the flag \"compare_exact\" to 0 to compare the values approximately\n");
		return false;
	}
	else return true;
}

