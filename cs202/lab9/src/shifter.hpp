#include <vector>
#include <string>
class Shifter {
	public:

		bool Read_Grid_And_Shapes(int argc, const char **argv);

		void Apply_Shape(int index, int r, int c);

		bool Find_Solution(int index);

		void Print_Solution() const;

		std::vector <std::string> Grid;                   

		std::vector < std::vector <std::string> > Shapes;     

		std::vector <int> Solution_Rows;            
		std::vector <int> Solution_Cols;            
};
