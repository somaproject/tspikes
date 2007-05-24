#include <boost/test/unit_test.hpp>
#include "clusterview.h"
#include "spikes.h"
#include <stdexcept>

using namespace boost::unit_test;

void constructors_test()

{

  ClusterView cv0(0, 0, 200, 200, 100, 100, 1, 2); 
  BOOST_CHECK_EQUAL( cv0.getVoriginX(), 0);
  BOOST_CHECK_EQUAL( cv0.getVoriginY(), 0);

  BOOST_CHECK_THROW(  new ClusterView(0, 0, 0, 200, 100, 100, 1, 2), std::logic_error); 

}

void conversion_test()
{
  PointList p(4); 
  p[0] = 10000; 
  p[1] = 20000; 
  p[2] = 30000; 
  p[3] = 40000; 
  SpikePoint sp1(1000, p, 1); 
  
  ClusterView cv0(0, 0, 200, 200, 100, 100, 1, 2); 
  BOOST_CHECK_EQUAL( cv0.getX(sp1), 200); 
  BOOST_CHECK_EQUAL( cv0.getY(sp1), 300); 

  ClusterView cv1(0, 0, 200, 200, 1000, 1000, 0, 3); 
  BOOST_CHECK_EQUAL( cv1.getX(sp1), 10); 
  BOOST_CHECK_EQUAL( cv1.getY(sp1), 40); 

  // test origin manipulation
  ClusterView cv2(10000, 10000, 200, 200, 1000, 1000, 0, 1);
  BOOST_CHECK_EQUAL( cv2.getX(sp1), 0); 
  BOOST_CHECK_EQUAL( cv2.getY(sp1), 10); 

  // non-uniform manipulations
  ClusterView cv3(1000, 2000, 1000, 1000, 100, 200, 2, 3); 
  BOOST_CHECK_EQUAL( cv3.getX(sp1), 290); 
  BOOST_CHECK_EQUAL( cv3.getY(sp1), 190); 

}
test_suite*
init_unit_test_suite( int argc, char* argv[] )
{
    test_suite* test= BOOST_TEST_SUITE( "clusterView test" );
    test->add( BOOST_TEST_CASE( &constructors_test ) );
    test->add( BOOST_TEST_CASE( &conversion_test ) ); 

    return test;
}
