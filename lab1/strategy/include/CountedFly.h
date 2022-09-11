#ifndef COUNTEDFLY_H
#define COUNTEDFLY_H

#include "IFlyBehavior.h"

class CountedFly : public IFlyBehavior
{
public:
	CountedFly();

	void Fly() override;

protected:
	void BumpCounter();

private:
	std::size_t m_flightCount;
};

#endif // !COUNTEDFLY_H