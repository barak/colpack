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

#include "ColPackHeaders.h"

using namespace std;

namespace ColPack
{
	RecoveryCore::RecoveryCore() {
		//formatType = "UNKNOWN";

		//for ADOL-C Format (AF)
		AF_available = false;
		i_AF_rowCount = 0;
		dp2_AF_Value = NULL;

		//for Sparse Solvers Format (SSF)
		SSF_available = false;
		i_SSF_rowCount = 0;
		ip_SSF_RowIndex = NULL;
		ip_SSF_ColumnIndex = NULL;
		dp_SSF_Value = NULL;

		//for Coordinate Format (CF)
		CF_available = false;
		i_CF_rowCount = 0;
		ip_CF_RowIndex = NULL;
		ip_CF_ColumnIndex = NULL;
		dp_CF_Value = NULL;
	}

	void RecoveryCore::reset() {

		//for ADOL-C Format (AF)
		if (AF_available) {
			//free_2DMatrix(dp2_AF_Value, i_AF_rowCount);
			for( int i=0; i < i_AF_rowCount; i++ ) {
			    free( dp2_AF_Value[i] );
			}
			free( dp2_AF_Value );

			dp2_AF_Value = NULL;
			AF_available = false;
			i_AF_rowCount = 0;
		}

		//for Sparse Solvers Format (SSF)
		if (SSF_available) {
			//delete[] ip_SSF_RowIndex;
			free(ip_SSF_RowIndex);
			ip_SSF_RowIndex = NULL;
			//delete[] ip_SSF_ColumnIndex;
			free(ip_SSF_ColumnIndex);
			ip_SSF_ColumnIndex = NULL;
			//delete[] dp_SSF_Value;
			free(dp_SSF_Value);
			dp_SSF_Value = NULL;
			SSF_available = false;
			i_SSF_rowCount = 0;
		}

		//for Coordinate Format (CF)
		if (CF_available) {
			//do something
			//delete[] ip_CF_RowIndex;
			free(ip_CF_RowIndex);
			ip_CF_RowIndex = NULL;
			//delete[] ip_CF_ColumnIndex;
			free(ip_CF_ColumnIndex);
			ip_CF_ColumnIndex = NULL;
			//delete[] dp_CF_Value;
			free(dp_CF_Value);
			dp_CF_Value = NULL;
			CF_available = false;
			i_CF_rowCount = 0;
		}

		//formatType = "UNKNOWN";
	}

	RecoveryCore::~RecoveryCore() {

		//for ADOL-C Format (AF)
		if (AF_available) {
			//do something
			//free_2DMatrix(dp2_AF_Value, i_AF_rowCount);

			for( int i=0; i < i_AF_rowCount; i++ ) {
			    free( dp2_AF_Value[i] );
			}
			free( dp2_AF_Value );
		}

		//for Sparse Solvers Format (SSF)
		if (SSF_available) {
			//do something
			//delete[] ip_SSF_RowIndex;
			free(ip_SSF_RowIndex);
			//delete[] ip_SSF_ColumnIndex;
			free(ip_SSF_ColumnIndex);
			//delete[] dp_SSF_Value;
			free(dp_SSF_Value);
		}

		//for Coordinate Format (CF)
		if (CF_available) {
			//do something
			//delete[] ip_CF_RowIndex;
			free(ip_CF_RowIndex);
			//delete[] ip_CF_ColumnIndex;
			free(ip_CF_ColumnIndex);
			//delete[] dp_CF_Value;
			free(dp_CF_Value);
		}
	}
}
