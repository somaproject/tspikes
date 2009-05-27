#include <boost/test/auto_unit_test.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/format.hpp>

using namespace boost;       
#include "spvectdb.h"


BOOST_AUTO_TEST_SUITE(spvectdbtest); 

BOOST_AUTO_TEST_CASE(creation)
{
  /*
    Compilation check
    
   */
  SpikePointVectDatabase spv(10, 100); 
  BOOST_CHECK_EQUAL(spv.size(), 0); 
  GLSpikePoint_t sp; 
  spv.append(sp); 

  BOOST_CHECK_EQUAL(spv.size(), 1); 

  BOOST_CHECK_EQUAL(spv.rbegin()->second->size(), 1); 
}


BOOST_AUTO_TEST_CASE(new_buffers)
{
  /*
    Add a bunch of spikes, setting the time appropriately, and
    then check the sizes are correct. 
    
   */

  float mindur = 10.0; 
  SpikePointVectDatabase spv(mindur, 100); 
  
  BOOST_CHECK_EQUAL(spv.size(), 0); 

  for (int i = 0; i < 100; i++) { 
    spv.setTime(float(i)); 

    GLSpikePoint_t sp; 
    spv.append(sp); 
  }

  BOOST_CHECK_EQUAL(spv.size(), 100); 
  
  float time = 0.0; 
  for(GLSPVectMap_t::const_iterator ci = spv.begin(); 
      ci != spv.end(); ci++) {
    
    BOOST_CHECK_EQUAL(ci->second->size(), 10); 
  }
  
  spv.reset(); 
  BOOST_CHECK_EQUAL(spv.size(), 0); 
  
  BOOST_CHECK_EQUAL(spv.rbegin()->second->size(), 0); 

}

BOOST_AUTO_TEST_CASE(new_buffer_wrap_around)
{
  /*
    Add a bunch of spikes, and see if we don't overflow
    
   */

  float mindur = 10.0; 
  SpikePointVectDatabase spv(mindur, 100); 
  
  BOOST_CHECK_EQUAL(spv.size(), 0); 

  for (int i = 0; i < 1000; i++) { 
    spv.setTime(float(i)); 

    GLSpikePoint_t sp; 
    spv.append(sp); 
  }

  BOOST_CHECK_EQUAL(spv.size(), 100); 
  
  float time = 0.0; 
  for(GLSPVectMap_t::const_iterator ci = spv.begin(); 
      ci != spv.end(); ci++) {
    
    BOOST_CHECK_EQUAL(ci->second->size(), 10); 
  }

}



BOOST_AUTO_TEST_SUITE_END(); 
