#include "tac3pascal.h"
#include <iomanip>

namespace Tac
{
  PascalTriangle& PascalTriangle::Instance()
  {
    static PascalTriangle global;
    return global;
  }
  PascalTriangle::PascalTriangle()
  {
    // add the first row so the tree is never empty
    mTree.push_back(std::vector<unsigned>());
    mTree[0].push_back(1);
  }

  unsigned PascalTriangle::GetNumber(
    unsigned depth, 
    unsigned col)
  {
    while (depth >= mTree.size())
      AddRow();

    return mTree[depth][col];
  }

  void PascalTriangle::AddRow()
  {
    std::vector<unsigned> newRow;

    std::vector<unsigned> & currLastRow = mTree.back();
    newRow.push_back(1);
    for (unsigned i = 1; i < currLastRow.size(); ++i)
    {
      unsigned v0 = currLastRow[i - 1];
      unsigned v1 = currLastRow[i];
      newRow.push_back(v0 + v1);
    }
    newRow.push_back(1);

    mTree.push_back(newRow);
  }

  std::ostream & PascalTriangle::Print(std::ostream & os) const 
  {
    for (auto ic = mTree.begin(); ic != mTree.end(); ++ic)
    {
      const std::vector<unsigned> & currRow = *ic;
      for (auto ir = currRow.begin(); ir != currRow.end(); ++ir)
      {
        os << std::setw(5) << *ir;
      }
      os << std::endl;
    }
    return os;
  }

  std::ostream & operator << (
    std::ostream & os, 
    const PascalTriangle & tri)
  {
    return tri.Print(os);
  }
} // Tac
