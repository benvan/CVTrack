#pragma once
#include "Head.h"


class TennisWatcher
{

private:
    CRITICAL_SECTION headCriticalSection;
	Head* head;

public:
    TennisWatcher::TennisWatcher(CRITICAL_SECTION criticalSection, Head* headPar);
    ~TennisWatcher(void);
};

