#include "stdafx.h"
#include "TennisWatcher.h"


TennisWatcher::TennisWatcher(CRITICAL_SECTION criticalSection, Head* headPar)
{
	headCriticalSection = criticalSection;
	head = headPar;

    printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");



	

}

TennisWatcher::~TennisWatcher(void)
{
}