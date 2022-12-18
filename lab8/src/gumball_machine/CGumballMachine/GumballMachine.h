#pragma once

#include <iostream>
#include <memory>

#include "../IGumballMachine.h"
#include "../state/IState.h"

namespace gumball_machine
{

class GumballMachine final : private IGumballMachine
{
public:
	using REchoStream = std::ostream&;

	explicit GumballMachine(REchoStream echoOutput = std::cout);
	explicit GumballMachine(size_t onInitBallsCount, REchoStream echoOutput = std::cout);

	void InsertQuarter();
	void EjectQuarter();
	void TurnCrank();

	size_t GetBallCount() const noexcept override;
	size_t GetQuarterCount() const noexcept override;
	size_t GetMaxQuarterCount() const noexcept override;

	std::string Description() const override;

private:
	using State = state::IStatePtr;

	void ReleaseBall() override;
	void SetHasQuarterState() override;
	void SetNoQuarterState() override;
	void SetSoldOutState() override;
	void SetSoldState() override;

	size_t m_gumCount;
	State m_currentState;

	REchoStream m_echoOutput;
};

} // namespace gumball_machine
