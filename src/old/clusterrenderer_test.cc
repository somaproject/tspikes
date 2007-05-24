#include <boost/test/unit_test.hpp>
#include "clusterview.h"
#include "spikes.h"
#include "clusterrenderer.h"
#include <stdexcept>

using namespace boost::unit_test;

void constructors_test()

{

  PointList p(4); 
  p[0] = 10000; 
  p[1] = 20000; 
  p[2] = 30000; 
  p[3] = 40000; 
  SpikePoint sp(1000, p, 1); 
  SpikePointList spl; 
  
  spl.push_back(sp); 


}

void conversion_test()
{
  PointList p(4); 
  p[0] = 10000; 
  p[1] = 20000; 
  p[2] = 30000; 
  p[3] = 40000; 
  SpikePoint sp1(1000, p, 1); 

}
test_suite*
init_unit_test_suite( int argc, char* argv[] )
{
    test_suite* test= BOOST_TEST_SUITE( "ClusterRenderer test" );
    test->add( BOOST_TEST_CASE( &constructors_test ) );
    //test->add( BOOST_TEST_CASE( &conversion_test ) ); 

    return test;
}
