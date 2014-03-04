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

using namespace std;

#ifndef BIPARTITEGRAPHPARTIALCOLORINGINTERFACE_H
#define BIPARTITEGRAPHPARTIALCOLORINGINTERFACE_H

namespace ColPack
{
	/** @ingroup group21
	 *  @brief class BipartiteGraphPartialColoringInterface in @link group21@endlink.

	 To be completed.
	 Note that for each object, only one type of Coloring (either Row or Column) should be used.
	 The reason is because both of RowColoring and ColumnColoring will update (share) the value of m_i_VertexColorCount.
	 If RowColoring is run and then ColumnColoring is run, both PrintColumnPartialColoringMetrics() and PrintRowPartialColoringMetrics()
	 will display the result of the later run (ColumnColoring) only.
	 */
	class BipartiteGraphPartialColoringInterface : public BipartiteGraphPartialColoring
	{
	public: //DOCUMENTED

		/// Build a BipartiteGraphPartialColoringInterface object and create the bipartite graph based on the graph structure specified by the input source
		/** This function will:
		- 0. Create initial BipartiteGraphPartialColoringInterface object
		- 1. Create the bipartite graph based on the graph structure specified by the input source

		Structure of this variadic function's parameters: BipartiteGraphPartialColoringInterface(int i_type, [2 or more parameters for input source depending on the value of i_type]). Here are some examples:
		  - Just create the BipartiteGraphPartialColoringInterface object: BipartiteGraphPartialColoringInterface(SRC_WAIT);
		  - Get the input from file: BipartiteGraphPartialColoringInterface(SRC_FILE, s_InputFile.c_str() ,"AUTO_DETECTED");
		  - Get input from ADOLC: BipartiteGraphPartialColoringInterface(SRC_MEM_ADOLC,uip2_SparsityPattern, i_rowCount, i_columnCount);

		About input parameters:
		- int i_type: specified the input source. i_type can be either:
		  - -1 (SRC_WAIT): only step 0 will be done.
		  - 0 (SRC_FILE): The graph structure will be read from file. The next 2 parameters are:
		    - fileName: name of the input file. If the full path is not given, the file is assumed to be in the current directory
		    - fileType can be either:
			    - "AUTO_DETECTED"  or "". ColPack will decide the format of the file based on the file extension:
				    - ".mtx": MatrixMarket format
				    - ".hb", or any combination of ".<r, c, p><s, u, h, x, r><a, e>": HarwellBoeing format
				    - ".graph": MeTiS format
				    - ".gen": Generic Matrix format
				    - ".gens": Generic Square Matrix format
				    - If the above extensions are not found, MatrixMarket format will be assumed.
			    - "MM" for MatrixMarket format (http://math.nist.gov/MatrixMarket/formats.html#MMformat). Notes:
			      - ColPack only accepts MatrixMarket coordinate format (NO array format)
			      - List of arithmetic fields accepted by ColPack: real, pattern or integer
			      - List of symmetry structures accepted by ColPack: general or symmetric
			      - The first line of the input file should be similar to this: "%%MatrixMarket matrix coordinate real general"
			    - "HB" for HarwellBoeing format (http://math.nist.gov/MatrixMarket/formats.html#hb)
			    - "MeTiS" for MeTiS format (http://people.sc.fsu.edu/~burkardt/data/metis_graph/metis_graph.html)
			    - "GEN" for Generic Matrix format
			    - "GENS" for Generic Square Matrix format
		  - 1 (SRC_MEM_ADOLC): The graph structure will be read from Row Compressed Structure (used by ADOLC). The next 3 parameters are:
		    - unsigned int **uip2_SparsityPattern: The pattern of Jacobian matrix stored in Row Compressed Format
		    - int i_rowCount: number of rows in the Jacobian matrix. Number of rows in uip2_SparsityPattern.
		    - int i_ColumnCount: number of columns in the Jacobian matrix. Number of columns in uip2_SparsityPattern.
		  - 2 (SRC_MEM_ADIC): TO BE IMPLEMENTED so that ColPack can interface with ADIC
		//*/
		BipartiteGraphPartialColoringInterface(int i_type, ...);

		/// (Partial-Distance-Two) Color the bipartite graph based on the requested s_ColoringVariant and s_OrderingVariant
		/**	This function will
		- 1. Order the vertices based on the requested Ordering variant (s_OrderingVariant)
		- 2. Bicolor the graph based on the requested Coloring variant (s_ColoringVariant)
		- Ordering Time and Coloring Time will be recorded.

		About input parameters:
		- s_OrderingVariant can be either
			- "NATURAL" (default)
			- "LARGEST_FIRST"
			- "SMALLEST_LAST"
			- "INCIDENCE_DEGREE"
			- "RANDOM"
		- s_ColoringVariant can be either
			- "COLUMN_PARTIAL_DISTANCE_TWO" (default)
			- "ROW_PARTIAL_DISTANCE_TWO"

		Postcondition:
		- The Bipartite Graph is (Partial-Distance-Two) colored, i.e., either m_vi_LeftVertexColors or m_vi_RightVertexColors will be populated.
		*/
		int PartialDistanceTwoColoring(string s_OrderingVariant = "NATURAL", string s_ColoringVariant = "COLUMN_PARTIAL_DISTANCE_TWO");

		/// Generate and return the seed matrix
		/**	This function will
		- 1. Color the graph by (Row or Column)-Partial-Distance-2-Coloring  with the specified ordering
		- 2. Create and return the seed matrix (*dp3_seed) from the coloring information

		About input parameters:
		- s_ColoringVariant:
			- if == "COLUMN_PARTIAL_DISTANCE_TWO" (default), PartialDistanceTwoColumnColoring() will be used and the right seed matrix will be return
			- if == "ROW_PARTIAL_DISTANCE_TWO", PartialDistanceTwoRowColoring() will be used and the left seed matrix will be return
		- s_OrderingVariant can be either
			- "NATURAL" (default)
			- "LARGEST_FIRST"
			- "SMALLEST_LAST"
			- "INCIDENCE_DEGREE"
			- "RANDOM"

		Postcondition:
		- *dp3_seed: the size will be [*ip1_SeedRowCount] [*ip1_SeedColumnCount]
			- if (s_ColoringVariant == "COLUMN_PARTIAL_DISTANCE_TWO"): [num of columns of the original matrix == i_ColumnCount] [ColorCount]
			- if (s_ColoringVariant == "ROW_PARTIAL_DISTANCE_TWO"): [ColorCount] [num of rows of the original matrix == i_RowCount]
		*/
		void GenerateSeedJacobian(double*** dp3_seed, int *ip1_SeedRowCount, int *ip1_SeedColumnCount, string s_OrderingVariant="NATURAL", string s_ColoringVariant = "COLUMN_PARTIAL_DISTANCE_TWO");


		/// Same as GenerateSeedJacobian(), except that this Seed matrix is NOT managed by ColPack 
		/** Notes:
		- This Seed matrix is NOT managed by ColPack. Therefore, the user should free the Seed matrix manually when the matrix is no longer needed.
		*/
		void GenerateSeedJacobian_unmanaged(double*** dp3_seed, int *ip1_SeedRowCount, int *ip1_SeedColumnCount, string s_OrderingVariant="NATURAL", string s_ColoringVariant = "COLUMN_PARTIAL_DISTANCE_TWO");

		/// Based on m_s_VertexColoringVariant, either GetLeftSeedMatrix() or GetRightSeedMatrix() will be called.
		double** GetSeedMatrix(int* ip1_SeedRowCount, int* ip1_SeedColumnCount);

		void GetOrderedVertices(vector<int> &output);
	private:

		Timer m_T_Timer;

	public:

		//Public Destructor 2602
		~BipartiteGraphPartialColoringInterface();

		//Public Function 2603
		void Clear();

		//Public Function 2604
		void Reset();

	};
}
#endif

