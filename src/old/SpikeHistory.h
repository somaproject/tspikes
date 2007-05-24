

typedef std::vector<SpikePoint> SpikeBin; 

class SpikeHistory 
{
  
 public: 
  SpikeHistory(int gran);
  ~SpikeHistory(); 
  
  void addSpike(const Spike& s ); 
  
 private:
  int granularity_; 
  std::list< SpikeBin > spikebins_;
}
