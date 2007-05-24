#include <iostream>
#include <list>

int main()
{
  std::list<int> test; 
  test.push_back(7); 
  std::list<int>::iterator iter = test.begin(); 
  
  for (int i = 0; i < 10; i++)
    {
      test.push_back(i); 
      if (iter == test.end() )
	{
	  std::cout << "equal" << std::endl; 
	}

    }
}
