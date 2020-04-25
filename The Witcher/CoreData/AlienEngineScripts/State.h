#pragma once
class PlayerController;

enum (StateType,
	IDLE,
	RUNNING,
	ATTACKING,
	JUMPING,
	ROLLING,
	CASTING,
	DEAD,
	REVIVING,
	HIT,
	ROOT,
	MAX
	);

//State Interface based on: http://gameprogrammingpatterns.com/state.html. Recommended reading.
class State
{
	friend class PlayerController;
public:
	StateType type;
	virtual ~State() {}
	virtual State* HandleInput(PlayerController* player) { return nullptr; }
	virtual void Update(PlayerController* player) {}
	virtual void OnEnter(PlayerController* player) {}
	virtual void OnExit(PlayerController* player) {}
	virtual State* OnAnimationEnd(PlayerController* player, const char* name) { return nullptr; }
};

class IdleState : public State
{
public:
	State* HandleInput(PlayerController* player) override;
	void Update(PlayerController* player) override;
	void OnEnter(PlayerController* player) override;
	void OnExit(PlayerController* player) override;
};

class RunningState : public State
{
	State* HandleInput(PlayerController* player) override;
	void Update(PlayerController* player) override;
	void OnEnter(PlayerController* player) override;
	void OnExit(PlayerController* player) override;
};

class JumpingState : public State
{
	State* HandleInput(PlayerController* player) override;
	void Update(PlayerController* player) override;
	void OnEnter(PlayerController* player) override;
	void OnExit(PlayerController* player) override;
};

class RollingState : public State
{
	//void HandleInput(PlayerController* player, float2 movement_input) override;
	void Update(PlayerController* player) override;
	State* OnAnimationEnd(PlayerController* player, const char* name) override;
	void OnEnter(PlayerController* player) override;
	void OnExit(PlayerController* player) override;
};

class AttackingState : public State
{
	State* HandleInput(PlayerController* player) override;
	void Update(PlayerController* player) override;
	State* OnAnimationEnd(PlayerController* player, const char* name) override;
	void OnEnter(PlayerController* player) override;
	void OnExit(PlayerController* player) override;
};

class CastingState : public State
{
	//State* HandleInput(PlayerController* player) override;
	void Update(PlayerController* player) override;
	State* OnAnimationEnd(PlayerController* player, const char* name) override;
	void OnEnter(PlayerController* player) override;
	void OnExit(PlayerController* player) override;
};

class DeadState : public State
{
	//void HandleInput(PlayerController* player, float2 movement_input) override;
	//void Update(PlayerController* player) override;
	void OnEnter(PlayerController* player) override;
	void OnExit(PlayerController* player) override;
};

class RevivingState : public State
{
	//void HandleInput(PlayerController* player, float2 movement_input) override;
	//void Update(PlayerController* player) override;
	State* OnAnimationEnd(PlayerController* player, const char* name) override;
	void OnEnter(PlayerController* player) override;
	void OnExit(PlayerController* player) override;
};

class HitState : public State
{
	//void HandleInput(PlayerController* player, float2 movement_input) override;
	void Update(PlayerController* player) override;
	State* OnAnimationEnd(PlayerController* player, const char* name) override;
	void OnEnter(PlayerController* player) override;
	void OnExit(PlayerController* player) override;
};


