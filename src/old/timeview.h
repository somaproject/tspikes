#ifndef TIMEVIEW_H
#define TIMEVIEW_H

class TimeView
{
 public:
  TimeView(unsigned int endTime, int rate, int granularity, int initVal); 
  TimeView(int rate, int granularity, int initVal); 
  unsigned getEndTime(); 
  int getRate(); 
  bool isEndTimeNow(); 
  int getGranularity(); 
  int getInitialValue(); 

 private:
  unsigned int endTime_; 
  bool endTimeIsNow_; 
  int rate_; 
  int granularity_; 
  int initialValue_; 

};

inline TimeView::TimeView(unsigned int endTime, int rate, 
		   int granularity, int initVal) :
  endTime_ (endTime), 
  endTimeIsNow_ (false), 
  rate_ (rate), 
  granularity_ (granularity), 
  initialValue_ (initVal)
{
}

inline TimeView::TimeView(int rate, int granularity, int initVal) :
  endTime_ (0), 
  endTimeIsNow_ (true), 
  rate_ (rate), 
  granularity_ (granularity), 
  initialValue_ (initVal)
{
}

inline
unsigned TimeView::getEndTime()
{
  return endTime_; 
}

inline
int TimeView::getRate()
{
  return rate_;
}

inline
bool TimeView::isEndTimeNow()
{
  return endTimeIsNow_; 
}

inline
int TimeView::getGranularity()
{
  return granularity_;
}

inline
int TimeView::getInitialValue()
{
  return initialValue_;
}
 
#endif // TIMEVIEW_H
