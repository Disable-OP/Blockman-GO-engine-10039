#include "Util/SecTimer.h"
map<int, std::shared_ptr<SecTimer::Timer>>::type SecTimer::m_TimerList;
ui16 SecTimer::m_idacc = 1;
