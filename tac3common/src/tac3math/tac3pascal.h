
#pragma once
#include <iostream>
#include <vector>

namespace Tac
{
  //    1     (d=0, c=0)
  //   1 1    (d=1, c=0) (d=1, c=1)
  //  1 2 1
  //
  // depth starts from 0
  // col starts from 0
  class PascalTriangle
  {
  public:
    static PascalTriangle& Instance();
    unsigned GetNumber(unsigned depth, unsigned col);
    // todo: overload []
    std::ostream & Print(std::ostream & os) const;

  private:
    PascalTriangle();
    void AddRow();
    std::vector<std::vector<unsigned> > mTree;
  };
  
  std::ostream & operator << (
    std::ostream & os, 
    const PascalTriangle & tri);
} // Tac
